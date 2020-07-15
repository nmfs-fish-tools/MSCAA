
#include "nmfUtilsQt.h"
#include "nmfSimulationTab01.h"
#include "nmfUtils.h"
//#include "nmfConstants.h"


nmfSimulation_Tab1::nmfSimulation_Tab1(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;
    QString msg;

    Simulation_Tabs = tabs;
    m_logger        = logger;
    m_databasePtr   = databasePtr;

    m_logger->logMsg(nmfConstants::Normal,"nmfSimulation_Tab1::nmfSimulation_Tab1");

    ReadSettings();

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/Simulation/Simulation_Tab01.ui");
    file.open(QFile::ReadOnly);
    Simulation_Tab1_Widget = loader.load(&file,Simulation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Simulation_Tabs->addTab(Simulation_Tab1_Widget, tr("1. Simulation Parameters"));

    Simulation_Tab1_YearlyParametersTV  = new QTableView(Simulation_Tabs);
    Simulation_Tab1_NextPB              = Simulation_Tabs->findChild<QPushButton    *>("Simulation_Tab1_NextPB");
    Simulation_Tab1_LoadPB              = Simulation_Tabs->findChild<QPushButton    *>("Simulation_Tab1_LoadPB");
    Simulation_Tab1_SavePB              = Simulation_Tabs->findChild<QPushButton    *>("Simulation_Tab1_SavePB");
    Simulation_Tab1_SpeciesLBL          = Simulation_Tabs->findChild<QLabel         *>("Simulation_Tab1_SpeciesLBL");
    Simulation_Tab1_YearlyParametersLT  = Simulation_Tabs->findChild<QVBoxLayout    *>("Simulation_Tab1_YearlyParametersLT");
    Simulation_Tab1_YearlyParametersLT->addWidget(Simulation_Tab1_YearlyParametersTV);
    Simulation_Tab1_NuOtherDSB          = Simulation_Tabs->findChild<QDoubleSpinBox *>("Simulation_Tab1_NuOtherDSB");
    Simulation_Tab1_NuOtherCB           = Simulation_Tabs->findChild<QCheckBox      *>("Simulation_Tab1_NuOtherCB");

    // Set tooltips
    msg  = "<html>&#x3B6;<sub>y</sub> is the fluctuation about the expected recruitment for year y, ";
    msg += "which is assumed to be normally distributed with &#x3C3;<sub>R</sub></html>";
    Simulation_Tab1_YearlyParametersTV->setToolTip(msg);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    Simulation_Tab1_YearlyParametersTV->setFont(noBoldFont);

    connect(Simulation_Tab1_NextPB, SIGNAL(clicked()),
            this,                   SLOT(callback_NextPB()));
    connect(Simulation_Tab1_LoadPB, SIGNAL(clicked()),
            this,                   SLOT(callback_LoadPB()));
    connect(Simulation_Tab1_SavePB, SIGNAL(clicked()),
            this,                   SLOT(callback_SavePB()));
    connect(Simulation_Tab1_NuOtherCB, SIGNAL(stateChanged(int)),
            this,                   SLOT(callback_NuOtherCB(int)));
    connect(Simulation_Tab1_NuOtherDSB, SIGNAL(valueChanged(double)),
            this,                       SLOT(callback_RunSimulation(double)));
    Simulation_Tab1_NextPB->setText("--\u25B7");
    Simulation_Tab1_NuOtherDSB->setValue(0);
    Simulation_Tab1_NuOtherDSB->setEnabled(false);

} // end constructor


nmfSimulation_Tab1::~nmfSimulation_Tab1()
{
}


void
nmfSimulation_Tab1::speciesChanged(QString species)
{
    Simulation_Tab1_SpeciesLBL->clear();
    setSpecies(species);
    loadWidgets();
}

void
nmfSimulation_Tab1::setSpecies(QString species)
{
    Simulation_Tab1_SpeciesLBL->setText(species);
}

QString
nmfSimulation_Tab1::getSpecies()
{
    return Simulation_Tab1_SpeciesLBL->text();
}

QTableView*
nmfSimulation_Tab1::getYearlyParametersTable()
{
    return Simulation_Tab1_YearlyParametersTV;
}

void
nmfSimulation_Tab1::callback_NuOtherCB(int state)
{
    Simulation_Tab1_NuOtherDSB->setEnabled(state);
}

bool
nmfSimulation_Tab1::useNuOther()
{
    return Simulation_Tab1_NuOtherCB->isChecked();
}

double
nmfSimulation_Tab1::getNuOther()
{
    return Simulation_Tab1_NuOtherDSB->value();
}

void
nmfSimulation_Tab1::callback_NextPB()
{
    int nextPage = Simulation_Tabs->currentIndex()+1;
    Simulation_Tabs->setCurrentIndex(nextPage);
}

void
nmfSimulation_Tab1::callback_LoadPB()
{
    loadWidgets();
}


void
nmfSimulation_Tab1::callback_UpdateSuitabilityNuOther(double nuOther)
{
    Simulation_Tab1_NuOtherDSB->setValue(nuOther);
}

void
nmfSimulation_Tab1::callback_RunSimulation(double value)
{
    emit RunSimulation();
}

bool
nmfSimulation_Tab1::saveYearlyParameters()
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Simulation_Tab1_YearlyParametersTV->model());
    std::string MohnsRhoLabel = ""; // placeholder
    std::string value;
    std::string deleteCmd="";
    std::string saveCmd="";
    std::string errorMsg;
    QStringList ParameterNames = {"zeta","sigma"};
    QModelIndex index;
    QString msg;
    QString year;
    QString Species = getSpecies();

    saveCmd = "INSERT INTO SimulationParametersYearly";
    saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Year,ParameterName,Value) VALUES ";
    for (int col = 0; col < smodel->columnCount(); ++col) {
        for (int row=0; row<smodel->rowCount(); ++row) {
            index = smodel->index(row,col);
            value = index.data().toString().toStdString();
            year = smodel->verticalHeaderItem(row)->text();
            saveCmd += "('"  + MohnsRhoLabel +
                       "','" + m_ProjectSettingsConfig +
                       "','" + Species.toStdString() +
                       "','" + year.toStdString() +
                       "','" + ParameterNames[col].toStdString() +
                       "', " + value + "),";
        }
    }

    // Delete the current Species entry here
    deleteCmd  = "DELETE FROM SimulationParametersYearly ";
    deleteCmd += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    deleteCmd += " AND SpeName = '" + Species.toStdString() + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (errorMsg != " ") {
        msg = "\nError in Save command. Couldn't delete all records from SimulationParametersYearly table";
        m_logger->logMsg(nmfConstants::Error,"nmfSimulation_Tab1::saveYearlyParameters: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Simulation_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSimulation_Tab1::saveYearlyParameters: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Simulation_Tabs, "Error",
                             "\nError in Save command. Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Reload the table
    loadYearlyParameters();

    return true;
}

void
nmfSimulation_Tab1::callback_SavePB()
{
    QString msg = "\n";
    bool ok;

    ok = saveYearlyParameters();
    if (ok) {
        msg += "SimulationParametersYearly saved OK\n";
    }

    QMessageBox::information(Simulation_Tabs, "Table(s) Updated", msg, QMessageBox::Ok);
}

bool
nmfSimulation_Tab1::loadYearlyParameters()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel;
    QChar sigma = QChar(0xC3, 0x03);
    QChar zeta  = QChar(0xB6, 0x03);
    QStringList ColLabels = {zeta,sigma};
    QStringList RowLabels;
    int NumCols = ColLabels.size();
    int NumRows;
    int m = 0;
    int NumRecords;
    int FirstYear=0;
    QStandardItem *item;
    std::vector<std::string> Years;
    QString Species = getSpecies();

    // Get all years and load row labels
    nmfMSCAAUtils::getAllYears(m_databasePtr,m_logger,Species,FirstYear,Years);
    NumRows = Years.size();
    for (std::string Year : Years) {
        RowLabels << QString::number(FirstYear+std::stoi(Year)-1);
    }

    smodel = new QStandardItemModel(NumRows,NumCols);

    // Read data from table
    fields     = {"SystemName","SpeName","Year","ParameterName","Value"};
    queryStr   = "SELECT SystemName,SpeName,Year,ParameterName,Value FROM SimulationParametersYearly ";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " AND SpeName = '" + Species.toStdString() + "'";
    queryStr  += " ORDER BY Year,ParameterName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Year"].size();

    for (int row=0; row<NumRows; ++row) {
        for (int col=NumCols-1; col>=0; col--) {
            if (m < NumRecords) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m]));
            } else {
                item = new QStandardItem("");
            }
            ++m;
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
        }
    }

    smodel->setHorizontalHeaderLabels(ColLabels);
    smodel->setVerticalHeaderLabels(RowLabels);
    Simulation_Tab1_YearlyParametersTV->setModel(smodel);
    Simulation_Tab1_YearlyParametersTV->resizeColumnsToContents();
}

void
nmfSimulation_Tab1::clearWidgets()
{
    nmfUtilsQt::clearTableView({Simulation_Tab1_YearlyParametersTV});
}

bool
nmfSimulation_Tab1::loadWidgets()
{
std::cout << "nmfSimulation_Tab1::loadWidgets()" << std::endl;
    ReadSettings();

    clearWidgets();

    loadYearlyParameters();

    return true;
}

void
nmfSimulation_Tab1::ReadSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

