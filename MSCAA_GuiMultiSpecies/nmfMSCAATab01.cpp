
//#include "nmfUtilsQt.h"
#include "nmfMSCAATab01.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"


nmfMSCAA_Tab1::nmfMSCAA_Tab1(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QString msg;
    QUiLoader loader;

    MSCAA_Tabs            = tabs;
    m_logger              = logger;
    m_databasePtr         = databasePtr;
    m_currentSegmentIndex = 0;

    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfMSCAA_Tab1::nmfMSCAA_Tab1");

    m_ProjectDir = projectDir;
    m_OriginalUnits.clear();

    // Load ui as a widget from disk
    QFile file(":/forms/MSCAA/MSCAA_Tab01.ui");
    file.open(QFile::ReadOnly);
    MSCAA_Tab1_Widget = loader.load(&file,MSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    MSCAA_Tabs->addTab(MSCAA_Tab1_Widget, tr("1. Predator/Prey Configuration"));

    MSCAA_Tab1_InteractionsTV  = new QTableView();
    MSCAA_Tab1_VulnerabilityTV = new QTableView();
    MSCAA_Tab1_NextPB          = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab1_NextPB");
    MSCAA_Tab1_LoadPB          = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab1_LoadPB");
    MSCAA_Tab1_SavePB          = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab1_SavePB");
    MSCAA_Tab1_TotalBiomassLE  = MSCAA_Tabs->findChild<QLineEdit   *>("MSCAA_Tab1_TotalBiomassLE");
    MSCAA_Tab1_FirstYearLE     = MSCAA_Tabs->findChild<QLineEdit   *>("MSCAA_Tab1_FirstYearLE");
    MSCAA_Tab1_LastYearLE      = MSCAA_Tabs->findChild<QLineEdit   *>("MSCAA_Tab1_LastYearLE");
    MSCAA_Tab1_UnitsCMB        = MSCAA_Tabs->findChild<QComboBox   *>("MSCAA_Tab1_UnitsCMB");
    MSCAA_Tab1_LayoutLT        = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab1_LayoutLT");
    MSCAA_Tab1_LayoutLT2       = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab1_LayoutLT2");
    MSCAA_Tab1_OtherFoodWtLE   = MSCAA_Tabs->findChild<QLineEdit   *>("MSCAA_Tab1_OtherFoodWtLE");
    MSCAA_Tab1_LayoutLT->addWidget(MSCAA_Tab1_InteractionsTV);
    MSCAA_Tab1_LayoutLT2->addWidget(MSCAA_Tab1_VulnerabilityTV);
    MSCAA_Tab1_PredPreyInteractionTV = MSCAA_Tabs->findChild<QTableView *>("MSCAA_Tab1_PredPreyInteractionTV");
    MSCAA_Tab1_PredPreyInteractionTV->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    MSCAA_Tab1_InteractionsTV->setFont(noBoldFont);
    MSCAA_Tab1_VulnerabilityTV->setFont(noBoldFont);
    MSCAA_Tab1_PredPreyInteractionTV->setFont(noBoldFont);
    MSCAA_Tab1_OtherFoodWtLE->setAlignment(Qt::AlignRight);
    MSCAA_Tab1_FirstYearLE->setAlignment(Qt::AlignRight);
    MSCAA_Tab1_LastYearLE->setAlignment(Qt::AlignRight);


    msg  = "<strong><center>Predator Prey Interaction Vectors</center></strong><br>";
    msg += "This table is read-only. It is auto-generated after the<br>";
    msg += "user saves the Species Interactions table above.<br><br>";
    msg += "The two rows are defined as: <br>";
    msg += "&nbsp;&nbsp;&nbsp;&nbsp;1. Predator number for each prey<br>";
    msg += "&nbsp;&nbsp;&nbsp;&nbsp;2. Prey number for each predator<br>";
    MSCAA_Tab1_PredPreyInteractionTV->setWhatsThis(msg);

    connect(MSCAA_Tab1_NextPB, SIGNAL(clicked()),
            this,              SLOT(callback_NextPB()));
    connect(MSCAA_Tab1_LoadPB, SIGNAL(clicked()),
            this,              SLOT(callback_LoadPB()));
    connect(MSCAA_Tab1_SavePB, SIGNAL(clicked()),
            this,              SLOT(callback_SavePB()));
    connect(MSCAA_Tab1_UnitsCMB, SIGNAL(currentIndexChanged(QString)),
            this,                SLOT(callback_UnitsCMB(QString)));


    MSCAA_Tab1_NextPB->setText("--\u25B7");

} // end constructor


nmfMSCAA_Tab1::~nmfMSCAA_Tab1()
{

}


void
nmfMSCAA_Tab1::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

int
nmfMSCAA_Tab1::getNumSpecies()
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
nmfMSCAA_Tab1::getTableInteractions()
{
    return MSCAA_Tab1_InteractionsTV;
}

QTableView*
nmfMSCAA_Tab1::getTableVulnerability()
{
    return MSCAA_Tab1_VulnerabilityTV;
}

void
nmfMSCAA_Tab1::setUnits(std::string units)
{
    MSCAA_Tab1_UnitsCMB->setCurrentText(QString::fromStdString(units));
}

std::string
nmfMSCAA_Tab1::getUnits()
{
    return MSCAA_Tab1_UnitsCMB->currentText().toStdString();
}



void
nmfMSCAA_Tab1::callback_NextPB()
{
    int nextPage = MSCAA_Tabs->currentIndex()+1;
    MSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfMSCAA_Tab1::callback_LoadPB()
{
    loadWidgets();
}

void
nmfMSCAA_Tab1::callback_SavePB()
{
    QString msg = "\n";

    if (saveSystemData()) {
        msg += "System table has been successfully updated.\n";
    }

    if (saveInteractionsData()) {
        msg += "PredatorPreyInteractions table has been successfully updated.\n";
    }

    if (saveVulnerabilityData()) {
        msg += "PredatorPreyVulnerability table has been successfully updated.\n";
    }

    // Update calculated PredPreyInteractions tableview
    updatePredPreyInteractionTable();
    if (saveInteractionsVecData()) {
        msg += "PredatorPreyInteractionsVec table has been successfully updated.\n";
    }
    saveSystemData(); // Resave System data to include recently calculated value for NumSpIter

    // Notify user
    if (msg != "\n") {
        QMessageBox::information(MSCAA_Tabs, "Table(s) Updated", msg, QMessageBox::Ok);
    }

    // Refresh widgets
    loadWidgets();
}

int
nmfMSCAA_Tab1::getNumInteractions()
{
    int NumInteractions=0;
    QAbstractItemModel *smodelMain;
    QModelIndex index;

    smodelMain = qobject_cast<QAbstractItemModel*>(MSCAA_Tab1_InteractionsTV->model());
    for (int row = 0; row < smodelMain->rowCount(); ++row) {
        for (int col = 0; col < smodelMain->columnCount(); ++col) {
            index = smodelMain->index(row,col);
            if (index.data().toInt() == 1) {
                ++NumInteractions;
            }
        }
    }

    return NumInteractions;
}


void
nmfMSCAA_Tab1::updatePredPreyInteractionTable()
{
//  int NumSpecies;
    int NumInteractions=0;
    QStandardItemModel *smodel;
    QAbstractItemModel *smodelMain;
    QStandardItem *item;
    std::vector<std::string> Species;
    QStringList RowLabels = {"Predator Interactions:","Prey Interactions:"};
    QModelIndex index;
    std::vector<int> PredVec;
    std::vector<int> PreyVec;

    m_databasePtr->getAllSpecies(m_logger, Species);
//  NumSpecies = Species.size();

    smodelMain = qobject_cast<QAbstractItemModel*>(MSCAA_Tab1_InteractionsTV->model());
    for (int row = 0; row < smodelMain->rowCount(); ++row) {
        for (int col = 0; col < smodelMain->columnCount(); ++col) {
            index = smodelMain->index(row,col);
            if (index.data().toInt() == 1) {
                ++NumInteractions;

                // Load Predator interaction values, i.e., predator # for each prey
                PredVec.push_back(row+1);

                // Load Prey interaction values; i.e., prey # for each predator
                PreyVec.push_back(col+1);
            }
        }
    }
    smodel = new QStandardItemModel(2,NumInteractions); // Only 2 rows in this matrix (it's really just two 1-dimensional vectors shown in a matrix)

    for (int col = 0; col < smodel->columnCount(); ++col) {

        // Load Predator interaction values
        item = new QStandardItem(QString::number(PredVec[col]));
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(0, col, item);

        // Load Prey interaction values
        item = new QStandardItem(QString::number(PreyVec[col]));
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(1, col, item);
    }

    // RSK - tbd Save this to a database table for easy use when sending to Run routine

    smodel->setHorizontalHeaderLabels({});
    smodel->setVerticalHeaderLabels(RowLabels);
    MSCAA_Tab1_PredPreyInteractionTV->setModel(smodel);

    // Save NumInteractions to System table

}

bool
nmfMSCAA_Tab1::saveInteractionsVecData()
{
    std::string MohnsRhoLabel = ""; // placeholder
    std::string saveCmd;
    std::string deleteCmd;
    std::string errorMsg;
    QString msg;
    QAbstractItemModel *smodel = qobject_cast<QAbstractItemModel*>(MSCAA_Tab1_PredPreyInteractionTV->model());

    // Delete the previous data in the table
    deleteCmd = "DELETE FROM PredatorPreyInteractionsVec WHERE SystemName = '" +
                m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from PredatorPreyInteractionsVec table";
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAA_Tab1::saveInteractionsVecData: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }


    // There are only 2 rows. The 1st is the Predator vec and the 2nd is the Prey vec.
    saveCmd = "REPLACE INTO PredatorPreyInteractionsVec";
    saveCmd += " (MohnsRhoLabel,SystemName,PredValue,PreyValue) VALUES ";
    for (int col = 0; col < smodel->columnCount(); ++col) {
        saveCmd += "('" + MohnsRhoLabel +
                "','" + m_ProjectSettingsConfig +
                "', " + smodel->index(0,col).data().toString().toStdString() +
                " , " + smodel->index(1,col).data().toString().toStdString() + "),";
    }
    saveCmd = saveCmd.substr(0, saveCmd.size()-1); // trim last "," character

    // Save the new data
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAA_Tab1::saveInteractionsVecData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

void
nmfMSCAA_Tab1::callback_UnitsCMB(QString units)
{
    double currentValue = MSCAA_Tab1_TotalBiomassLE->text().toDouble();

    if (m_OriginalUnits == "Metric Tons") {
        if (units == "000 Metric Tons") {
            currentValue /= 1000.0;
        } else if (units == "000 000 Metric Tons") {
            currentValue /= 1000000.0;
        }
    } else if (m_OriginalUnits == "000 Metric Tons") {
        if (units == "Metric Tons") {
            currentValue *= 1000.0;
        } else if (units == "000 000 Metric Tons") {
            currentValue /= 1000.0;
        }
    } else if (m_OriginalUnits == "000 000 Metric Tons") {
        if (units == "Metric Tons") {
            currentValue *= 1000000.0;
        } else if (units == "000 Metric Tons") {
            currentValue *= 1000.0;
        }
    }
    m_OriginalUnits = units;
    MSCAA_Tab1_TotalBiomassLE->setText(QString::number(currentValue));
}

bool
nmfMSCAA_Tab1::saveSystemData()
{
    bool SystemDataExists;
    std::string saveCmd;
    std::string errorMsg;
    std::string MohnsRhoLabel = ""; // placeholder
    QString TotalBiomass;

    SystemDataExists = nmfMSCAAUtils::systemTableExists(
                         m_databasePtr,m_logger,m_ProjectSettingsConfig);

    TotalBiomass  = MSCAA_Tab1_TotalBiomassLE->text().trimmed();
    TotalBiomass += "_" + MSCAA_Tab1_UnitsCMB->currentText();

    if (SystemDataExists) {
        saveCmd  = "UPDATE System SET TotalBiomass = '" + TotalBiomass.toStdString() +
                "', FH_FirstYear = "  + MSCAA_Tab1_FirstYearLE->text().toStdString() +
                " , FH_LastYear  = "  + MSCAA_Tab1_LastYearLE->text().toStdString() +
                " , NumSpInter   = "  + std::to_string(getNumInteractions()) +
                " , Owt          = "  + MSCAA_Tab1_OtherFoodWtLE->text().toStdString() +
                " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    } else {
        saveCmd = "REPLACE INTO System";
        saveCmd += " (MohnsRhoLabel,SystemName,TotalBiomass,FH_FirstYear,FH_LastYear,NumSpInter,Owt,AbundanceDriver) VALUES ";
        saveCmd += "('" + MohnsRhoLabel +
                "','" + m_ProjectSettingsConfig +
                "','" + TotalBiomass.toStdString() +
                "', " + MSCAA_Tab1_FirstYearLE->text().toStdString() +
                " , " + MSCAA_Tab1_LastYearLE->text().toStdString() +
                " , " + std::to_string(getNumInteractions()) +
                " , " + MSCAA_Tab1_OtherFoodWtLE->text().toStdString() +
                ",'Fishing Mortality')";
    }
    // Save the new data
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAA_Tab1::saveSystemData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

bool
nmfMSCAA_Tab1::saveInteractionsData()
{
    bool foundOutOfRangeValue = false;
    int NumSpecies;
    std::string value;
    std::string saveCmd;
    std::string deleteCmd;
    std::string errorMsg;
    std::vector<std::string> Species;
    QStandardItemModel *smodel = qobject_cast<QStandardItemModel*>(MSCAA_Tab1_InteractionsTV->model());
    std::string MohnsRhoLabel = ""; // placeholder
    QModelIndex index;
    QString msg;

    m_databasePtr->getAllSpecies(m_logger, Species);
    NumSpecies = Species.size();

    saveCmd = "INSERT INTO PredatorPreyInteractions";
    saveCmd += " (MohnsRhoLabel,SystemName,PredatorName,PreyName,Value) VALUES ";
    for (int row = 0; row < NumSpecies; ++row) {
        for (int col = 0; col < NumSpecies; ++col) {
            index = smodel->index(row,col);
            value = index.data().toString().toStdString();
            if ((value != "0") && (value != "1")) {
                foundOutOfRangeValue = true;
                value = "0";
            }
            saveCmd += "('" + MohnsRhoLabel +
                    "','" + m_ProjectSettingsConfig +
                    "','" + Species[row] +
                    "','" + Species[col] +
                    "', " + value + "),";
        }
    }

    if (foundOutOfRangeValue) {
        m_logger->logMsg(nmfConstants::Warning,"nmfMSCAA_Tab1::saveInteractionsData: Set out of range value(s) to 0.");
    }
    // Delete the previous data in the table
    deleteCmd = "DELETE FROM PredatorPreyInteractions WHERE SystemName = '" +
                m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from PredatorPreyInteractions table";
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAA_Tab1::saveInteractionsData: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveInteractionsData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

bool
nmfMSCAA_Tab1::saveVulnerabilityData()
{
    bool foundOutOfRangeValue = false;
    int NumSpecies;
    std::string valueVulnerability;
    std::string valueInteractions;
    std::string saveCmd;
    std::string deleteCmd;
    std::string errorMsg;
    std::vector<std::string> Species;
    QStandardItemModel *smodelVulnerability = qobject_cast<QStandardItemModel*>(MSCAA_Tab1_VulnerabilityTV->model());
    QStandardItemModel *smodelInteractions  = qobject_cast<QStandardItemModel*>(MSCAA_Tab1_InteractionsTV->model());
    QStandardItem *item;
    std::string MohnsRhoLabel = ""; // placeholder
    QModelIndex indexInteractions;
    QModelIndex indexVulnerability;
    QString msg;

    m_databasePtr->getAllSpecies(m_logger, Species);
    NumSpecies = Species.size();

    saveCmd = "INSERT INTO PredatorPreyVulnerability";
    saveCmd += " (MohnsRhoLabel,SystemName,PredatorName,PreyName,Value) VALUES ";
    for (int row = 0; row < NumSpecies; ++row) {
        for (int col = 0; col < NumSpecies; ++col) {
            indexInteractions  = smodelInteractions->index(row,col);
            indexVulnerability = smodelVulnerability->index(row,col);
            valueInteractions  = indexInteractions.data().toString().toStdString();
            valueVulnerability = indexVulnerability.data().toString().toStdString();
            // Check that values exist only where there are 1's in the Interactions table
            if ((valueVulnerability != "0") && (valueInteractions == "0")) {
                foundOutOfRangeValue = true;
                valueVulnerability = "0";
                // Immediately show a 0 on the table where there's no Interaction
                item  = new QStandardItem(QString::fromStdString(valueVulnerability));
                item->setTextAlignment(Qt::AlignCenter);
                smodelVulnerability->setItem(row,col,item);
            }
            saveCmd += "('" + MohnsRhoLabel +
                    "','" + m_ProjectSettingsConfig +
                    "','" + Species[row] +
                    "','" + Species[col] +
                    "', " + valueVulnerability + "),";
        }
    }

    if (foundOutOfRangeValue) {
        m_logger->logMsg(nmfConstants::Warning,"nmfMSCAA_Tab1::saveVulnerabilityData: Set value(s) to 0 where no Interaction data exist.");
    }
    // Delete the previous data in the table
    deleteCmd = "DELETE FROM PredatorPreyVulnerability WHERE SystemName = '" +
                m_ProjectSettingsConfig + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from PredatorPreyVulnerability table";
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAA_Tab1::saveVulnerabilityData: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveVulnerabilityData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

bool
nmfMSCAA_Tab1::loadSystemData()
{
//  int NumSpecies;
    int NumRecords;
    std::vector<std::string> Species;
//  QStandardItemModel *smodel;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString totalBiomassWithUnits;
    QString totalBiomass;
    QString units;

    m_databasePtr->getAllSpecies(m_logger, Species);
//  NumSpecies = Species.size();
//  smodel = new QStandardItemModel(NumSpecies,NumSpecies);

    // Get data from database
    fields     = {"SystemName","TotalBiomass","FH_FirstYear","FH_LastYear","Owt"};
    queryStr   = "SELECT SystemName,TotalBiomass,FH_FirstYear,FH_LastYear,Owt FROM `System` ";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["TotalBiomass"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Error,"No data found in table: System");
        return false;
    }

    // Load total biomass and units
    totalBiomassWithUnits = QString::fromStdString(dataMap["TotalBiomass"][0]);
    totalBiomass = totalBiomassWithUnits.split("_")[0];
    units = totalBiomassWithUnits.split("_")[1];
    MSCAA_Tab1_TotalBiomassLE->setText(totalBiomass);
    MSCAA_Tab1_UnitsCMB->setCurrentText(units);
    MSCAA_Tab1_OtherFoodWtLE->setText(QString::fromStdString(dataMap["Owt"][0]));
    m_OriginalUnits = units;

    // Load food habit years
    MSCAA_Tab1_FirstYearLE->setText(QString::fromStdString(dataMap["FH_FirstYear"][0]));
    MSCAA_Tab1_LastYearLE->setText(QString::fromStdString(dataMap["FH_LastYear"][0]));

    emit LoadedSystemData();

    return true;
}

bool
nmfMSCAA_Tab1::loadInteractionsData()
{
    int m;
    int NumSpecies;
    int NumRecords;
    std::string value;
    std::vector<std::string> Species;
    QStandardItemModel *smodel;
    QStringList SpeciesLabels;
    QStandardItem *item;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    m_databasePtr->getAllSpecies(m_logger, Species);
    NumSpecies = Species.size();
    smodel = new QStandardItemModel(NumSpecies,NumSpecies);

    // Get data from database
    fields     = {"SystemName","PredatorName","PreyName","Value"};
    queryStr   = "SELECT SystemName,PredatorName,PreyName,Value FROM PredatorPreyInteractions ";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " ORDER BY PredatorName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["PredatorName"].size();

    // Load data into model
    m = 0;
    for (int row = 0; row < NumSpecies; ++row) {
        SpeciesLabels.append(QString::fromStdString(Species[row]));
        for (int col = 0; col < NumSpecies; ++col) {
            value = (m < NumRecords) ? dataMap["Value"][m] : "0";
            item  = new QStandardItem(QString::fromStdString(value));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row,col,item);
            ++m;
        }
    }

    // Label columns and assign model to tableview
    smodel->setHorizontalHeaderLabels(SpeciesLabels);
    smodel->setVerticalHeaderLabels(SpeciesLabels);
    MSCAA_Tab1_InteractionsTV->setModel(smodel);
    MSCAA_Tab1_InteractionsTV->resizeColumnsToContents();

    return true;
}


bool
nmfMSCAA_Tab1::loadInteractionsVecData()
{
    int m;
    int NumRows = 2; // Always only 2 rows...one for pred values other for
    int NumCols;
    int NumRecords;
    QStandardItemModel *smodel;
    QStringList VerticalLabels = {"Predator Interactions:",
                                  "Prey Interactions:"};
    QStandardItem *item;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get data from database
    fields     = {"SystemName","PredValue","PreyValue"};
    queryStr   = "SELECT SystemName,PredValue,PreyValue FROM PredatorPreyInteractionsVec ";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["PredValue"].size();

    NumCols = NumRecords;
    smodel  = new QStandardItemModel(NumRows,NumCols);

    // Load data into model
    m = 0;
    for (int col = 0; col < NumCols; ++col) {
        item  = new QStandardItem(QString::fromStdString(dataMap["PredValue"][col]));
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(0,col,item);
        item  = new QStandardItem(QString::fromStdString(dataMap["PreyValue"][col]));
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(1,col,item);
        ++m;
    }

    // Label columns and assign model to tableview
    smodel->setVerticalHeaderLabels(VerticalLabels);
    MSCAA_Tab1_PredPreyInteractionTV->setModel(smodel);
    MSCAA_Tab1_PredPreyInteractionTV->resizeColumnsToContents();

    return true;
}

bool
nmfMSCAA_Tab1::loadVulnerabilityData()
{
    int m;
    int NumSpecies;
    int NumRecords;
    std::string value;
    std::vector<std::string> Species;
    QStandardItemModel *smodel;
    QStringList SpeciesLabels;
    QStandardItem *item;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    m_databasePtr->getAllSpecies(m_logger, Species);
    NumSpecies = Species.size();
    smodel = new QStandardItemModel(NumSpecies,NumSpecies);

    // Get data from database
    fields     = {"SystemName","PredatorName","PreyName","Value"};
    queryStr   = "SELECT SystemName,PredatorName,PreyName,Value FROM PredatorPreyVulnerability ";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " ORDER BY PredatorName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["PredatorName"].size();

    // Load data into model
    m = 0;
    for (int row = 0; row < NumSpecies; ++row) {
        SpeciesLabels.append(QString::fromStdString(Species[row]));
        for (int col = 0; col < NumSpecies; ++col) {
            value = (m < NumRecords) ? dataMap["Value"][m] : "0";
            item  = new QStandardItem(QString::fromStdString(value));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row,col,item);
            ++m;
        }
    }

    // Label columns and assign model to tableview
    smodel->setHorizontalHeaderLabels(SpeciesLabels);
    smodel->setVerticalHeaderLabels(SpeciesLabels);
    MSCAA_Tab1_VulnerabilityTV->setModel(smodel);
    MSCAA_Tab1_VulnerabilityTV->resizeColumnsToContents();

    return true;
}

void
nmfMSCAA_Tab1::clearWidgets()
{
    nmfUtilsQt::clearTableView({MSCAA_Tab1_InteractionsTV,
                                MSCAA_Tab1_VulnerabilityTV,
                                MSCAA_Tab1_PredPreyInteractionTV});
}

bool
nmfMSCAA_Tab1::loadWidgets()
{
    readSettings();

    clearWidgets();

std::cout << "nmfMSCAA_Tab1::loadWidgets()" << std::endl;
    loadSystemData();
    loadInteractionsData();
    loadInteractionsVecData();
    loadVulnerabilityData();

    return true;
}

