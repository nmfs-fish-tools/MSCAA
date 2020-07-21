
#include "nmfSetupTab05.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"


nmfSetup_Tab5::nmfSetup_Tab5(QTabWidget*  tabs,
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
    QFile file(":/forms/Setup/Setup_Tab05.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab5_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab5_Widget->setObjectName("Setup_Tab5_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab5_Widget, tr("5. Survey Setup"));

    Setup_Tab5_SurveyMonthTV = Setup_Tabs->findChild<QTableView  *>("Setup_Tab5_SurveyMonthTV");
    Setup_Tab5_TSwtTV        = Setup_Tabs->findChild<QTableView  *>("Setup_Tab5_TSwtTV");
    Setup_Tab5_SPwtTV        = Setup_Tabs->findChild<QTableView  *>("Setup_Tab5_SPwtTV");
    Setup_Tab5_LoadPB        = Setup_Tabs->findChild<QPushButton *>("Setup_Tab5_LoadPB");
    Setup_Tab5_SavePB        = Setup_Tabs->findChild<QPushButton *>("Setup_Tab5_SavePB");
    Setup_Tab5_PrevPB        = Setup_Tabs->findChild<QPushButton *>("Setup_Tab5_PrevPB");
    Setup_Tab5_NextPB        = Setup_Tabs->findChild<QPushButton *>("Setup_Tab5_NextPB");

    // Make connections
    connect(Setup_Tab5_LoadPB, SIGNAL(clicked()),
            this,              SLOT(callback_LoadPB()));
    connect(Setup_Tab5_SavePB, SIGNAL(clicked()),
            this,              SLOT(callback_SavePB()));
    connect(Setup_Tab5_PrevPB, SIGNAL(clicked()),
            this,              SLOT(callback_PrevPB()));
    connect(Setup_Tab5_NextPB, SIGNAL(clicked()),
            this,              SLOT(callback_NextPB()));
}

nmfSetup_Tab5::~nmfSetup_Tab5()
{
}

void
nmfSetup_Tab5::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSetup_Tab5::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    settings->setValue("Name", QString::fromStdString(m_ProjectSettingsConfig));
    settings->endGroup();
    delete settings;
}

void
nmfSetup_Tab5::callback_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}


void
nmfSetup_Tab5::callback_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}

void
nmfSetup_Tab5::loadWtTables()
{
    QStandardItemModel* smodelTS = qobject_cast<QStandardItemModel*>(Setup_Tab5_TSwtTV->model());
    QStandardItemModel* smodelSP = qobject_cast<QStandardItemModel*>(Setup_Tab5_SPwtTV->model());
    int NumRows = smodelTS->rowCount();
    int NumCols = smodelTS->columnCount();
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QModelIndex index;

    fields     = {"SystemName","SpeName","Survey","TSwtValue","SPwtValue"};
    queryStr   = "SELECT SystemName,SpeName,Survey,TSwtValue,SPwtValue FROM SurveyWeights";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"Warning: No records found in SurveyWeights");
        return;
    }

    int m = 0;
    for (int row = 0; row < NumRows; ++row) {
        for (int col = 0; col < NumCols; ++col) {
            if (m < NumRecords) {
                index = smodelTS->index(row,col);
                smodelTS->setData(index,QString::fromStdString(dataMap["TSwtValue"][m]));
                index = smodelSP->index(row,col);
                smodelSP->setData(index,QString::fromStdString(dataMap["SPwtValue"][m++]));
            }
        }
    }
}

void
nmfSetup_Tab5::loadMonthTable()
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Setup_Tab5_SurveyMonthTV->model());
    int m;
    int month;
    int NumRows = smodel->rowCount();
    int NumCols = smodel->columnCount();
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QModelIndex index;
    QComboBox* cbox;

    fields     = {"SystemName","SpeName","Survey","Value"};
    queryStr   = "SELECT SystemName,SpeName,Survey,Value FROM SurveyMonth";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"Warning: No records found in SurveyMonth");
        return;
    }

    m = 0;
    for (int row = 0; row < NumRows; ++row) {
        for (int col = 0; col < NumCols; ++col) {
            if (m < NumRecords) {
                month = std::stoi(dataMap["Value"][m++]);
                index = smodel->index(row,col);
                cbox  = qobject_cast<QComboBox *>(Setup_Tab5_SurveyMonthTV->indexWidget(index));
                cbox->setCurrentIndex(month-1);
            }
        }
    }
}

void
nmfSetup_Tab5::callback_LoadPB()
{
    loadMonthTable();
    loadWtTables();
}

void
nmfSetup_Tab5::callback_SavePB()
{
    QString msg = "\n";

    if (saveMonthTable()) {
        msg += "SurveyWeights table has been successfully updated.\n";
    }
    if (saveWeightTables()) {
        msg += "SurveyMonth table has been successfully updated.\n";
    }

    QMessageBox::information(Setup_Tab5_Widget, "Survey Table(s) Updated", msg, QMessageBox::Ok);
}

QTableView*
nmfSetup_Tab5::getTableTotalAnnualSurveyCatch()
{
    return Setup_Tab5_TSwtTV;
}

QTableView*
nmfSetup_Tab5::getTableSurveyCatchProportions()
{
    return Setup_Tab5_SPwtTV;
}

bool
nmfSetup_Tab5::saveWeightTables()
{
    QStandardItemModel* smodelTS = qobject_cast<QStandardItemModel*>(Setup_Tab5_TSwtTV->model());
    QStandardItemModel* smodelSP = qobject_cast<QStandardItemModel*>(Setup_Tab5_SPwtTV->model());
    int NumRows = smodelTS->rowCount();
    int NumCols = smodelTS->columnCount();
    QModelIndex index;
    std::string deleteCmd;
    std::string saveCmd;
    std::string errorMsg;
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    QStandardItem* SpeciesLabel;
    QStandardItem* SurveyLabel;
    float TSwtValue;
    float SPwtValue;

    // Delete the current Species entry here
    deleteCmd = "DELETE FROM SurveyWeights WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (errorMsg != " ") {
        msg = "\nError in Save command. Couldn't delete all records from SurveyMonth table";
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab5::saveWeightTables: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Setup_Tab5_Widget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Build insert command from the model data
    saveCmd = "INSERT INTO SurveyWeights ";
    saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Survey,TSwtValue,SPwtValue) VALUES ";
    for (int row = 0; row < NumRows; ++row) {
        SpeciesLabel = smodelTS->verticalHeaderItem(row);
        for (int col = 0; col < NumCols; ++col) {
            SurveyLabel = smodelTS->horizontalHeaderItem(col);
            index       = smodelTS->index(row,col);
            TSwtValue   = index.data().toFloat();
            index       = smodelSP->index(row,col);
            SPwtValue   = index.data().toFloat();
            saveCmd += "('"  + MohnsRhoLabel +
                       "','" + m_ProjectSettingsConfig +
                       "','" + SpeciesLabel->text().toStdString() +
                       "', " + SurveyLabel->text().split(" ")[1].toStdString() +
                       " , " + std::to_string(TSwtValue) +
                       " , " + std::to_string(SPwtValue) + "),";
        }
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab5::saveWeightTables: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Setup_Tab5_Widget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

bool
nmfSetup_Tab5::saveMonthTable()
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Setup_Tab5_SurveyMonthTV->model());
    int month;
    int NumRows = smodel->rowCount();
    int NumCols = smodel->columnCount();
    QModelIndex index;
    QComboBox* cbox;
    std::string deleteCmd;
    std::string saveCmd;
    std::string errorMsg;
    std::map<std::string,int> monthMap = {{"Jan",1},{"Feb",2},{"Mar",3},{"Apr",4}, {"May",5}, {"Jun",6},
                                          {"Jul",7},{"Aug",8},{"Sep",9},{"Oct",10},{"Nov",11},{"Dec",12}};
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    QStandardItem* SpeciesLabel;
    QStandardItem* SurveyLabel;

    // Delete the current Species entry here
    deleteCmd = "DELETE FROM SurveyMonth WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (errorMsg != " ") {
        msg = "\nError in Save command. Couldn't delete all records from SurveyMonth table";
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab5::callback_Setup_Tab5_SavePB: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Setup_Tab5_Widget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Build insert command from the model data
    saveCmd = "INSERT INTO SurveyMonth ";
    saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Survey,Value) VALUES ";
    for (int row = 0; row < NumRows; ++row) {
        SpeciesLabel = smodel->verticalHeaderItem(row);
        for (int col = 0; col < NumCols; ++col) {
            SurveyLabel = smodel->horizontalHeaderItem(col);
            index = smodel->index(row,col);
            cbox  = qobject_cast<QComboBox *>(Setup_Tab5_SurveyMonthTV->indexWidget(index));
            month = monthMap[cbox->currentText().toStdString()];
            saveCmd += "('"  + MohnsRhoLabel +
                       "','" + m_ProjectSettingsConfig +
                       "','" + SpeciesLabel->text().toStdString() +
                       "', " + SurveyLabel->text().split(" ")[1].toStdString() +
                       " , " + std::to_string(month) + "),";
        }
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab5::callback_Setup_Tab5_SavePB: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Setup_Tab5_Widget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

void
nmfSetup_Tab5::getSpecies(std::vector<std::string>& species,
                          std::map<std::string,int>& numSurveyMap,
                          int& MaxNumSurveys)
{
    int NumSpecies=0;
    int NumSurveys;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    MaxNumSurveys = 0;
    species.clear();
    numSurveyMap.clear();

    fields     = {"SpeName","NumSurveys"};
    queryStr   = "SELECT SpeName,NumSurveys FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

    for (int row = 0; row < NumSpecies; ++row) {
        species.push_back(dataMap["SpeName"][row]);
        NumSurveys = std::stoi(dataMap["NumSurveys"][row]);
        MaxNumSurveys = (NumSurveys > MaxNumSurveys) ? NumSurveys : MaxNumSurveys;
        numSurveyMap[species[row]] = NumSurveys;
    }
}

void
nmfSetup_Tab5::clearWidgets()
{
    nmfUtilsQt::clearTableView({Setup_Tab5_SurveyMonthTV,
                                Setup_Tab5_TSwtTV,
                                Setup_Tab5_SPwtTV});
}

void
nmfSetup_Tab5::loadWidgets()
{
    int NumSpecies;
    int MaxNumSurveys;
    QComboBox *cbox;
    QStandardItemModel* smodelMonth;
    QStandardItemModel* smodelTSwt;
    QStandardItemModel* smodelSPwt;
    QModelIndex index;
    QStringList strList = QStringList() << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" <<
                                           "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";
    std::vector<std::string> Species;
    QStringList ColLabels;
    QStringList RowLabels;
    std::map<std::string,int> numSurveyMap;

    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab5::loadWidgets()");

    clearWidgets();

    getSpecies(Species,numSurveyMap,MaxNumSurveys);
    NumSpecies = Species.size();

    smodelMonth = new QStandardItemModel(NumSpecies,MaxNumSurveys);
    smodelTSwt  = new QStandardItemModel(NumSpecies,MaxNumSurveys);
    smodelSPwt  = new QStandardItemModel(NumSpecies,MaxNumSurveys);
    Setup_Tab5_SurveyMonthTV->setModel(smodelMonth);
    Setup_Tab5_TSwtTV->setModel(smodelTSwt);
    Setup_Tab5_SPwtTV->setModel(smodelSPwt);

    for (int row = 0; row < NumSpecies; ++row) {
        RowLabels.append(QString::fromStdString(Species[row]));
        for (int col = 0; col < MaxNumSurveys; ++col) {
            if (row == 0) {
                ColLabels.append("Survey "+QString::number(col+1));
            }
            cbox = new QComboBox();
            if (col < numSurveyMap[Species[row]]) {
                cbox->addItems(strList);
            } else {
                cbox->setEnabled(false);
            }
            index = Setup_Tab5_SurveyMonthTV->model()->index(row,col);
            Setup_Tab5_SurveyMonthTV->setIndexWidget(index,cbox);
        }
        Setup_Tab5_SurveyMonthTV->resizeColumnsToContents();
    }

    smodelMonth->setHorizontalHeaderLabels(ColLabels);
    smodelMonth->setVerticalHeaderLabels(RowLabels);
    smodelTSwt->setHorizontalHeaderLabels(ColLabels);
    smodelTSwt->setVerticalHeaderLabels(RowLabels);
    smodelSPwt->setHorizontalHeaderLabels(ColLabels);
    smodelSPwt->setVerticalHeaderLabels(RowLabels);

    // Now load the widgets with data from the table
    loadMonthTable();
    loadWtTables();

}

