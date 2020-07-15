
#include "nmfSetupTab04.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
//#include "nmfConstants.h"


nmfSetup_Tab4::nmfSetup_Tab4(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             QTextEdit*   setupOutputTE,
                             std::string& projectDir)
{
    QUiLoader loader;

    Setup_Tabs    = tabs;
    m_databasePtr = databasePtr;
    m_logger      = logger;
    m_ProjectDir  = projectDir;
    m_ProjectSettingsConfig.clear();

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab04.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab4_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab4_Widget->setObjectName("Setup_Tab4_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab4_Widget, tr("4. Environmental Setup"));

    Setup_Tab4_CovariatesTV = new QTableView(Setup_Tabs);
    Setup_Tab4_CovariatesLT = Setup_Tabs->findChild<QVBoxLayout *>("Setup_Tab4_CovariatesLT");
    Setup_Tab4_CovariatesSB = Setup_Tabs->findChild<QSpinBox    *>("Setup_Tab4_CovariatesSB");
    Setup_Tab4_SpeciesLBL   = Setup_Tabs->findChild<QLabel      *>("Setup_Tab4_SpeciesLBL");
    Setup_Tab4_LoadPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_LoadPB");
    Setup_Tab4_SavePB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_SavePB");
    Setup_Tab4_PrevPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_PrevPB");
    Setup_Tab4_NextPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab4_NextPB");
    Setup_Tab4_CovariatesLT->addWidget(Setup_Tab4_CovariatesTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    Setup_Tab4_CovariatesTV->setFont(noBoldFont);

    // Set up connections
    connect(Setup_Tab4_LoadPB,       SIGNAL(clicked()),
            this,                    SLOT(callback_LoadPB()));
    connect(Setup_Tab4_SavePB,       SIGNAL(clicked()),
            this,                    SLOT(callback_SavePB()));
    connect(Setup_Tab4_PrevPB,       SIGNAL(clicked()),
            this,                    SLOT(callback_PrevPB()));
    connect(Setup_Tab4_NextPB,       SIGNAL(clicked()),
            this,                    SLOT(callback_NextPB()));
    connect(Setup_Tab4_CovariatesSB, SIGNAL(valueChanged(int)),
            this,                    SLOT(callback_CovariatesSB(int)));

}

nmfSetup_Tab4::~nmfSetup_Tab4()
{
}


void
nmfSetup_Tab4::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSetup_Tab4::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    settings->setValue("Name", QString::fromStdString(m_ProjectSettingsConfig));
    settings->endGroup();
    delete settings;

}

void
nmfSetup_Tab4::callback_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}


void
nmfSetup_Tab4::callback_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}


void
nmfSetup_Tab4::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSetup_Tab4::callback_SavePB()
{
    std::string deleteCmd;
    std::string saveCmd;
    std::string errorMsg;
    QString msg;
    std::string MohnsRhoLabel = ""; // placeholder
    QString Species = getSpecies();
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Setup_Tab4_CovariatesTV->model());
    std::string covariateName;
    std::string year;
    std::string value;
    std::string covariateNumber;
    QModelIndex index;

    if (! nmfUtilsQt::allCellsArePopulated(Setup_Tabs,Setup_Tab4_CovariatesTV,nmfConstants::ShowError))
        return;

    // Delete the current Species entry here
    deleteCmd  = "DELETE FROM Covariates ";
    deleteCmd += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    deleteCmd += " AND SpeName = '" + Species.toStdString() + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (errorMsg != " ") {
        msg = "\nError in Save command. Couldn't delete records from Covariates table";
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab4::callback_SavePB: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Setup_Tab4_Widget, "Error", msg, QMessageBox::Ok);
        return;
    }

    // Build insert command from the model data
    saveCmd  = "INSERT INTO Covariates ";
    saveCmd += " (MohnsRhoLabel,SystemName,SpeName,CovariateNumber,CovariateName,Year,Value) VALUES ";
    for (int col = 0; col < smodel->columnCount(); ++col) {
        covariateNumber = std::to_string(col);
        index = smodel->index(0,col);
        covariateName = index.data().toString().toStdString();
        for (int row = 1; row < smodel->rowCount(); ++row) {
            year  = smodel->verticalHeaderItem(row)->text().toStdString();
            index = smodel->index(row,col);
            value = index.data().toString().toStdString();
            saveCmd += "('"   + MohnsRhoLabel +
                        "','" + m_ProjectSettingsConfig +
                        "','" + Species.toStdString() +
                        "',"  + covariateNumber +
                        " ,'" + covariateName +
                        "', " + year +
                        " , " + value + "),";
        }
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab4::callback_SavePB: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Setup_Tab4_Widget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return;
    }

    msg = "\nCovariates table has been successfully updated.\n";
    QMessageBox::information(Setup_Tab4_Widget, "Covariates Updated", msg, QMessageBox::Ok);
}

QString
nmfSetup_Tab4::getSpecies()
{
    return Setup_Tab4_SpeciesLBL->text();
}

void
nmfSetup_Tab4::callback_CovariatesSB(int value)
{
    QStringList verticalList;
    QStandardItemModel* smodel;
    int FirstYear;
    int LastYear;
    int MinAge;
    int MaxAge;
    int NumRows;
    int NumCols = value;
    float MinLength;
    float MaxLength;
    int NumLengthBins;

    QString species = getSpecies();
    if (species.isEmpty()) {
        m_logger->logMsg(nmfConstants::Error,"No Species selected.");
        return;
    }

    m_databasePtr->getSpeciesData(m_logger,species.toStdString(),
                                MinAge,MaxAge,FirstYear,LastYear,
                                MinLength,MaxLength,NumLengthBins);
    NumRows = LastYear - FirstYear + 1 + 1; // +1 additional row for name of covariate

    verticalList << "Name";
    for (int row = 0; row < NumRows-1; ++row) {
        verticalList << QString::number(FirstYear+row);
    }

    smodel = new QStandardItemModel(NumRows,NumCols);
    smodel->setVerticalHeaderLabels(verticalList);
    Setup_Tab4_CovariatesTV->setModel(smodel);

}

void
nmfSetup_Tab4::setSpecies(QString species)
{
    Setup_Tab4_SpeciesLBL->setText(species);
    refreshTable();
    loadWidgets();
}

void
nmfSetup_Tab4::refreshTable()
{
    callback_CovariatesSB(Setup_Tab4_CovariatesSB->value());
}

QTableView*
nmfSetup_Tab4::getTableEnvCovariates()
{
    return Setup_Tab4_CovariatesTV;
}

void
nmfSetup_Tab4::clearWidgets()
{
    nmfUtilsQt::clearTableView({Setup_Tab4_CovariatesTV});
    Setup_Tab4_SpeciesLBL->clear();
}

bool
nmfSetup_Tab4::loadWidgets()
{
    int NumRows;
    int NumCols;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::map<std::string, std::vector<std::string> > dataMap2;
    std::string queryStr;
    QString Species;
    QStandardItem *item;
    QStringList VerticalList;
    QStandardItemModel* smodel;
    QString msg;

    clearWidgets();

    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab4::loadWidgets()");

    Species = getSpecies();
    if (Species.isEmpty()) {
        m_logger->logMsg(nmfConstants::Warning,"Please select a Species to load.");
        return false;
    }

    fields     = {"MohnsRhoLabel","SystemName","SpeName","CovariateNumber",
                  "CovariateName","Year","Value"};
    queryStr   = "SELECT MohnsRhoLabel,SystemName,SpeName,CovariateNumber,CovariateName,Year,Value FROM Covariates ";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " AND SpeName = '" + Species.toStdString() + "'";
    queryStr  += " ORDER BY CovariateNumber,Year";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["CovariateNumber"].size();
    if (NumRecords == 0) {
        msg = "Warning: Didn't find any records in Covariates table for Species: "+Species;
        m_logger->logMsg(nmfConstants::Warning,msg.toStdString());
        //QMessageBox::warning(Setup_Tab4_Widget, "Error", "\n"+msg+"\n", QMessageBox::Ok);
        return false;
    }

    fields    = {"COUNT(DISTINCT(CovariateNumber))"};
    queryStr  = "SELECT COUNT(DISTINCT(CovariateNumber)) FROM Covariates ";
    queryStr += " WHERE SpeName='" + Species.toStdString() + "'";
    dataMap2  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumCols   = std::stoi(dataMap2["COUNT(DISTINCT(CovariateNumber))"][0]);

    Setup_Tab4_CovariatesSB->setValue(NumCols);

    fields    = {"COUNT(DISTINCT(Year))"};
    queryStr  = "SELECT COUNT(DISTINCT(Year)) FROM Covariates ";
    queryStr += " WHERE SpeName='" + Species.toStdString() + "'";
    dataMap2  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRows   = std::stoi(dataMap2["COUNT(DISTINCT(Year))"][0]);

    // Load model
    int m = 0;
    VerticalList << "Name";
    smodel = new QStandardItemModel(NumRows+1,NumCols);
    for (int col = 0; col < NumCols; ++col) {
        item = new QStandardItem(QString::fromStdString(dataMap["CovariateName"][m]));
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(0, col, item);
        for (int row = 1; row < NumRows+1; ++row) {
            if (col == 0) {
                VerticalList << QString::fromStdString(dataMap["Year"][m]);
            }
            item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
        }
    }
    smodel->setVerticalHeaderLabels(VerticalList);
    Setup_Tab4_CovariatesTV->setModel(smodel);
    Setup_Tab4_CovariatesTV->resizeColumnsToContents();

    return true;
}


