
#include "nmfSetupTab06.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
//#include "nmfConstants.h"


nmfSetup_Tab6::nmfSetup_Tab6(QTabWidget*  tabs,
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
    QFile file(":/forms/Setup/Setup_Tab06.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab6_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab6_Widget->setObjectName("Setup_Tab6_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab6_Widget, tr("6. Fleet Setup"));

    Setup_Tab6_FleetNameTV        = new QTableView(Setup_Tabs);
    Setup_Tab6_LoadPB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab6_LoadPB");
    Setup_Tab6_SavePB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab6_SavePB");
    Setup_Tab6_PrevPB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab6_PrevPB");
    Setup_Tab6_NextPB             = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab6_NextPB");
    Setup_Tab6_FleetNameLT        = Setup_Tabs->findChild<QVBoxLayout  *>("Setup_Tab6_FleetNameLT");
    Setup_Tab6_AbundanceDriverFRB = Setup_Tabs->findChild<QRadioButton *>("Setup_Tab6_AbundanceDriverFRB");
    Setup_Tab6_AbundanceDriverCRB = Setup_Tabs->findChild<QRadioButton *>("Setup_Tab6_AbundanceDriverCRB");
    Setup_Tab6_FleetNameLT->addWidget(Setup_Tab6_FleetNameTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    Setup_Tab6_FleetNameTV->setFont(noBoldFont);

    // Make connections
    connect(Setup_Tab6_LoadPB, SIGNAL(clicked()),
            this,              SLOT(callback_LoadPB()));
    connect(Setup_Tab6_SavePB, SIGNAL(clicked()),
            this,              SLOT(callback_SavePB()));
    connect(Setup_Tab6_PrevPB, SIGNAL(clicked()),
            this,              SLOT(callback_PrevPB()));
    connect(Setup_Tab6_NextPB, SIGNAL(clicked()),
            this,              SLOT(callback_NextPB()));
}

nmfSetup_Tab6::~nmfSetup_Tab6()
{
}

void
nmfSetup_Tab6::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSetup_Tab6::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    settings->setValue("Name", QString::fromStdString(m_ProjectSettingsConfig));
    settings->endGroup();
    delete settings;
}

void
nmfSetup_Tab6::callback_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}


void
nmfSetup_Tab6::callback_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}



void
nmfSetup_Tab6::callback_LoadPB()
{
    loadWidgets();
}

bool
nmfSetup_Tab6::saveFleetData()
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Setup_Tab6_FleetNameTV->model());
    int NumRows = smodel->rowCount();
    int NumCols = smodel->columnCount();
    QModelIndex index;
    std::string deleteCmd;
    std::string saveCmd;
    std::string errorMsg;
    std::string fleetName;
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    QStandardItem* SpeciesLabel;

    // Delete the current Species entry here
    deleteCmd = "DELETE FROM " + nmfConstantsMSCAA::TableFleets +
                " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from TableFleets table";
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab6::saveFleetData: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Setup_Tab6_Widget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Build insert command from the model data
    saveCmd = "INSERT INTO " + nmfConstantsMSCAA::TableFleets;
    saveCmd += " (MohnsRhoLabel,SystemName,SpeName,FleetNumber,FleetName) VALUES ";
    for (int row = 0; row < NumRows; ++row) {
        SpeciesLabel = smodel->verticalHeaderItem(row);
        for (int col = 0; col < NumCols; ++col) {
            index     = smodel->index(row,col);
            fleetName = index.data().toString().toStdString();
            saveCmd  += "('"  + MohnsRhoLabel +
                        "','" + m_ProjectSettingsConfig +
                        "','" + SpeciesLabel->text().toStdString() +
                        "', " + std::to_string(col) +
                        " ,'" + fleetName + "'),";
        }
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab6::saveFleetData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Setup_Tab6_Widget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

bool
nmfSetup_Tab6::saveSystemData()
{
    bool SystemDataExists = false;
    std::string saveCmd;
    std::string errorMsg;
    std::string MohnsRhoLabel = ""; // placeholder
    QString AbundanceDriver = (Setup_Tab6_AbundanceDriverFRB->isChecked()) ?
                               Setup_Tab6_AbundanceDriverFRB->text() :
                               Setup_Tab6_AbundanceDriverCRB->text();

    SystemDataExists = nmfMSCAAUtils::systemTableExists(
                         m_databasePtr,m_logger,
                         m_ProjectSettingsConfig);

    // Build insert command from the model data
    if (SystemDataExists) {
        saveCmd  = "UPDATE " + nmfConstantsMSCAA::TableModels +
                   " SET AbundanceDriver = '" +
                    AbundanceDriver.toStdString() + "' " +
                   "WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    } else {
        saveCmd = "INSERT INTO " + nmfConstantsMSCAA::TableModels;
        saveCmd += " (MohnsRhoLabel,SystemName,TotalBiomass,FH_FirstYear,FH_LastYear,NumSpInter,AbundanceDriver) VALUES ";
        saveCmd += "('"  + MohnsRhoLabel +
                   "','" + m_ProjectSettingsConfig +
                   "','0_Metric Tons',1900,1920,0,'" +
                   AbundanceDriver.toStdString() + "')";
    }
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab6::saveSystemData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Setup_Tab6_Widget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

void
nmfSetup_Tab6::callback_SavePB()
{
    QString msg = "\n";

    if (saveFleetData()) {
        msg += "Fleets table has been successfully updated.\n";
    }
    if (saveSystemData()) {
        msg += "Abundance Driver data has been successfully saved.\n";
    }
    if (msg != "\n") {
        QMessageBox::information(Setup_Tab6_Widget, "Table(s) Updated", msg, QMessageBox::Ok);
    }

    loadWidgets();
}

void
nmfSetup_Tab6::getSpecies(std::vector<std::string>& SpeciesVec,
                          std::map<std::string,int>& SpeciesFleetsMap,
                          int& MaxNumFleets)
{
    int NumFleets=0;
    int NumSpecies=0;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string Species;

    fields     = {"SpeName","NumFleets"};
    queryStr   = "SELECT SpeName,NumFleets FROM " + nmfConstantsMSCAA::TableSpecies;
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

    SpeciesVec.clear();
    MaxNumFleets = 0;
    for (int row = 0; row < NumSpecies; ++row) {
        Species = dataMap["SpeName"][row];
        SpeciesVec.push_back(Species);
        NumFleets = std::stoi(dataMap["NumFleets"][row]);
        SpeciesFleetsMap[Species] = NumFleets;
        MaxNumFleets = (NumFleets > MaxNumFleets) ? NumFleets : MaxNumFleets;
    }
}


QTableView*
nmfSetup_Tab6::getTableFleetData()
{
    return Setup_Tab6_FleetNameTV;
}

void
nmfSetup_Tab6::setSpecies(QString species)
{
    loadWidgets();
}

void
nmfSetup_Tab6::clearWidgets()
{
    nmfUtilsQt::clearTableView({Setup_Tab6_FleetNameTV});
    Setup_Tab6_AbundanceDriverFRB->setChecked(false);
    Setup_Tab6_AbundanceDriverCRB->setChecked(false);
}

void
nmfSetup_Tab6::loadFleetData()
{
    int m;
    int NumSpecies;
    int MaxNumFleets;
    int NumRecords;
    QStandardItemModel* smodel;
    std::string theSpecies;
    std::vector<std::string> Species;
    QStringList RowLabels;
    std::map<std::string,int> SpeciesFleetsMap;
    QStandardItem *item;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    clearWidgets();

    // Read fleet data
    fields     = {"SystemName","SpeName","FleetNumber","FleetName"};
    queryStr   = "SELECT SystemName,SpeName,FleetNumber,FleetName FROM " + nmfConstantsMSCAA::TableFleets;
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " ORDER BY SpeName,FleetNumber";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"nmfSetup_Tab6::loadFleetData: No records found in table: Fleets");
    }

    getSpecies(Species,SpeciesFleetsMap,MaxNumFleets);
    NumSpecies = Species.size();

    smodel = new QStandardItemModel(NumSpecies,MaxNumFleets);
    m = 0;
    for (int row = 0; row < NumSpecies; ++row) {
        theSpecies = Species[row];
        RowLabels.append(QString::fromStdString(theSpecies));
        for (int col = 0; col < MaxNumFleets; ++col) {
            if (col < SpeciesFleetsMap[theSpecies]) {
                if (m < NumRecords) {
                    item = new QStandardItem(QString::fromStdString(dataMap["FleetName"][m]));
                } else {
                    item = new QStandardItem("");
                }
            } else {
                item = new QStandardItem(QString::fromStdString("X"));
                item->setEnabled(false);
            }
            ++m;
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
        }
    }

    smodel->setVerticalHeaderLabels(RowLabels);
    Setup_Tab6_FleetNameTV->setModel(smodel);
    Setup_Tab6_FleetNameTV->resizeColumnsToContents();
}

void
nmfSetup_Tab6::loadSystemData(bool emitSignal)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Read System data
    fields     = {"SystemName","AbundanceDriver"};
    queryStr   = "SELECT SystemName,AbundanceDriver FROM " + nmfConstantsMSCAA::TableModels;
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"nmfSetup_Tab6::loadSystemData: No records found in table: System");
        Setup_Tab6_AbundanceDriverFRB->setChecked(false);
        Setup_Tab6_AbundanceDriverCRB->setChecked(false);
        return;
    }

    if (dataMap["AbundanceDriver"][0] == "Fishing Mortality") {
        Setup_Tab6_AbundanceDriverFRB->setChecked(true);
    } else {
        Setup_Tab6_AbundanceDriverCRB->setChecked(true);
    }

    if (emitSignal) {
        emit LoadedSystemData();
    }
}

void
nmfSetup_Tab6::loadWidgets()
{
std::cout << "nmfSetup_Tab6::loadWidgets()" << std::endl;
    loadFleetData();
    loadSystemData(true);
}

void
nmfSetup_Tab6::callback_LoadSystemDataNoEmit()
{
    loadSystemData(false);
}
