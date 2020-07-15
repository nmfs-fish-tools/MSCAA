
//#include "nmfUtilsQt.h"
#include "nmfMSCAATab02.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"


nmfMSCAA_Tab2::nmfMSCAA_Tab2(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    MSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_currentSegmentIndex = 0;
    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfMSCAA_Tab2::nmfMSCAA_Tab2");

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/MSCAA/MSCAA_Tab02.ui");
    file.open(QFile::ReadOnly);
    MSCAA_Tab2_Widget = loader.load(&file,MSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    MSCAA_Tabs->addTab(MSCAA_Tab2_Widget, tr("2. Predator Size Preference"));

    MSCAA_Tab2_PreferredTV           = new QTableView();
    MSCAA_Tab2_VarianceLessThanTV    = new QTableView();
    MSCAA_Tab2_VarianceGreaterThanTV = new QTableView();
    MSCAA_Tab2_PrevPB                = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab2_PrevPB");
    MSCAA_Tab2_NextPB                = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab2_NextPB");
    MSCAA_Tab2_LoadPB                = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab2_LoadPB");
    MSCAA_Tab2_SavePB                = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab2_SavePB");
    MSCAA_Tab2_RatioTBW              = MSCAA_Tabs->findChild<QTabWidget  *>("MSCAA_Tab2_RatioTBW");
    MSCAA_Tab2_PreferredLT           = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab2_PreferredLT");
    MSCAA_Tab2_VarianceLessThanLT    = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab2_VarianceLessThanLT");
    MSCAA_Tab2_VarianceGreaterThanLT = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab2_VarianceGreaterThanLT");

    MSCAA_Tab2_PreferredLT->addWidget(MSCAA_Tab2_PreferredTV);
    MSCAA_Tab2_VarianceLessThanLT->addWidget(MSCAA_Tab2_VarianceLessThanTV);
    MSCAA_Tab2_VarianceGreaterThanLT->addWidget(MSCAA_Tab2_VarianceGreaterThanTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    MSCAA_Tab2_PreferredTV->setFont(noBoldFont);
    MSCAA_Tab2_VarianceLessThanTV->setFont(noBoldFont);
    MSCAA_Tab2_VarianceGreaterThanTV->setFont(noBoldFont);

    connect(MSCAA_Tab2_NextPB, SIGNAL(clicked()),
            this,              SLOT(callback_NextPB()));
    connect(MSCAA_Tab2_PrevPB, SIGNAL(clicked()),
            this,              SLOT(callback_PrevPB()));
    connect(MSCAA_Tab2_LoadPB, SIGNAL(clicked()),
            this,              SLOT(callback_LoadPB()));
    connect(MSCAA_Tab2_SavePB, SIGNAL(clicked()),
            this,              SLOT(callback_SavePB()));

    MSCAA_Tab2_PrevPB->setText("\u25C1--");
    MSCAA_Tab2_NextPB->setText("--\u25B7");

} // end constructor


nmfMSCAA_Tab2::~nmfMSCAA_Tab2()
{

}

void
nmfMSCAA_Tab2::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}


int
nmfMSCAA_Tab2::getNumSpecies()
{
    bool foundSpecies;
    std::vector<std::string> species;

    foundSpecies = m_databasePtr->getAllSpecies(m_logger, species);
    if (! foundSpecies) {
        return 0;
    } else {
        return species.size();
    }
}

QTableView*
nmfMSCAA_Tab2::getTablePreferred()
{
    return MSCAA_Tab2_PreferredTV;
}

QTableView*
nmfMSCAA_Tab2::getTableVarianceLessThan()
{
    return MSCAA_Tab2_VarianceLessThanTV;
}

QTableView*
nmfMSCAA_Tab2::getTableVarianceGreaterThan()
{
    return MSCAA_Tab2_VarianceGreaterThanTV;
}

void
nmfMSCAA_Tab2::callback_PrevPB()
{
    int prevPage = MSCAA_Tabs->currentIndex()-1;
    MSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfMSCAA_Tab2::callback_NextPB()
{
    int nextPage = MSCAA_Tabs->currentIndex()+1;
    MSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfMSCAA_Tab2::callback_LoadPB()
{
    loadWidgets();
}

void
nmfMSCAA_Tab2::loadTable(QTableView*  tableView,
                         std::string  tableName)
{
    int m;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    int NumSpecies = smodel->rowCount();
    QStandardItem *item;

    fields     = {"SystemName","PredatorName","PreyName","Value"};
    queryStr   = "SELECT SystemName,PredatorName,PreyName,Value FROM " + tableName;
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["PredatorName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"Warning: No records found in "+tableName);
        return;
    }

    m = 0;
    for (int row = 0; row < NumSpecies; ++row) {
        for (int col = 0; col < NumSpecies; ++col) {
            if (m < NumRecords) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
            } else {
                item = new QStandardItem("");
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
        }
    }
}


void
nmfMSCAA_Tab2::saveTable(QTableView*  tableView,
                         std::string  tableName)
{
    int NumSpecies;
    std::string saveCmd;
    std::string deleteCmd;
    std::string errorMsg;
    std::string value;
    std::string MohnsRhoLabel = ""; // placeholder
    std::vector<std::string> species;
    QString msg;
    QModelIndex index;
    QAbstractItemModel* smodel = tableView->model();

    getSpecies(species);
    NumSpecies = smodel->rowCount();

    // Delete the current entry here
    deleteCmd = "DELETE FROM " + tableName + " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (errorMsg != " ") {
        msg = "\nError in Save command. Couldn't delete all records from " + QString::fromStdString(tableName) + " table";
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAA_Tab2::SaveTable: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(MSCAA_Tab2_Widget, "Error", msg, QMessageBox::Ok);
        return;
    }

    saveCmd = "INSERT INTO " + tableName;
    saveCmd += " (MohnsRhoLabel,SystemName,PredatorName,PreyName,Value) VALUES ";
    for (int row = 0; row < NumSpecies; ++row) {
        for (int col = 0; col < NumSpecies; ++col) {
            index = smodel->index(row,col);
            value = index.data().toString().toStdString();
            saveCmd += "('" + MohnsRhoLabel +
                    "','" + m_ProjectSettingsConfig +
                    "','" + species[row] +
                    "','" + species[col] +
                    "', " + value + "),";
        }
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAA_Tab2::SaveTable: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return;
    }

    msg = "\n" + QString::fromStdString(tableName) + " table has been successfully updated.\n";
    QMessageBox::information(MSCAA_Tabs, QString::fromStdString(tableName) +" Updated", msg, QMessageBox::Ok);

    loadWidgets();
}


void
nmfMSCAA_Tab2::callback_SavePB()
{
    switch (MSCAA_Tab2_RatioTBW->currentIndex()) {
        case 0:
            saveTable(MSCAA_Tab2_PreferredTV,"PredatorPreyPreferredRatio");
            break;
        case 1:
            saveTable(MSCAA_Tab2_VarianceLessThanTV,"PredatorPreyVarianceLTRatio");
            break;
        case 2:
            saveTable(MSCAA_Tab2_VarianceGreaterThanTV,"PredatorPreyVarianceGTRatio");
            break;
        default:
            break;
    }
}


void
nmfMSCAA_Tab2::getSpecies(std::vector<std::string>& species)
{
    int NumSpecies=0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"SpeName","NumSurveys"};
    queryStr   = "SELECT SpeName,NumSurveys FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

    species.clear();
    for (int row = 0; row < NumSpecies; ++row) {
        species.push_back(dataMap["SpeName"][row]);
    }
}

void
nmfMSCAA_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({MSCAA_Tab2_PreferredTV,
                                MSCAA_Tab2_VarianceLessThanTV,
                                MSCAA_Tab2_VarianceGreaterThanTV});
}

bool
nmfMSCAA_Tab2::loadWidgets()
{
std::cout << "nmfMSCAA_Tab2::loadWidgets()" << std::endl;
    int NumRows;
    int NumCols;
    std::vector<std::string> species;
    QStringList HorizontalLabels;
    QStringList VerticalLabels;

    readSettings();

    clearWidgets();

    getSpecies(species);
    NumRows = species.size();
    NumCols = species.size();
    for (int i = 0; i < NumRows; ++i) {
        HorizontalLabels << QString::fromStdString(species[i]);
        VerticalLabels   << QString::fromStdString(species[i]);
    }

    QStandardItemModel* smodelPreferred   = new QStandardItemModel(NumRows,NumCols);
    QStandardItemModel* smodelLessThan    = new QStandardItemModel(NumRows,NumCols);
    QStandardItemModel* smodelGreaterThan = new QStandardItemModel(NumRows,NumCols);

    MSCAA_Tab2_PreferredTV->setModel(smodelPreferred);
    MSCAA_Tab2_VarianceLessThanTV->setModel(smodelLessThan);
    MSCAA_Tab2_VarianceGreaterThanTV->setModel(smodelGreaterThan);

    smodelPreferred->setHorizontalHeaderLabels(HorizontalLabels);
    smodelPreferred->setVerticalHeaderLabels(VerticalLabels);
    smodelLessThan->setHorizontalHeaderLabels(HorizontalLabels);
    smodelLessThan->setVerticalHeaderLabels(VerticalLabels);
    smodelGreaterThan->setHorizontalHeaderLabels(HorizontalLabels);
    smodelGreaterThan->setVerticalHeaderLabels(VerticalLabels);

    std::vector<QTableView*> tableViews = {MSCAA_Tab2_PreferredTV,
                                           MSCAA_Tab2_VarianceLessThanTV,
                                           MSCAA_Tab2_VarianceGreaterThanTV};
    std::vector<std::string> tableNames = {"PredatorPreyPreferredRatio",
                                           "PredatorPreyVarianceLTRatio",
                                           "PredatorPreyVarianceGTRatio"};

    for (int i = 0; i < tableNames.size(); ++i) {
        loadTable(tableViews[i],tableNames[i]);
    }

    return true;
}
