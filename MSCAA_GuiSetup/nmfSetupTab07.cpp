
#include "nmfSetupTab07.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"


nmfSetup_Tab7::nmfSetup_Tab7(QTabWidget*  tabs,
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
    QFile file(":/forms/Setup/Setup_Tab07.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab7_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab7_Widget->setObjectName("Setup_Tab7_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab7_Widget, tr("7. Age-Length Setup"));

    Setup_Tab7_AgeLengthTV = new QTableView(Setup_Tabs);
    Setup_Tab7_SpeciesLBL  = Setup_Tabs->findChild<QLabel      *>("Setup_Tab7_SpeciesLBL");
    Setup_Tab7_MinMaxPB    = Setup_Tabs->findChild<QPushButton *>("Setup_Tab7_MinMaxPB");
    Setup_Tab7_LoadPB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab7_LoadPB");
    Setup_Tab7_SavePB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab7_SavePB");
    Setup_Tab7_PrevPB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab7_PrevPB");
    Setup_Tab7_AveLengthLT = Setup_Tabs->findChild<QVBoxLayout *>("Setup_Tab7_AveLengthLT");
    Setup_Tab7_AveLengthLT->addWidget(Setup_Tab7_AgeLengthTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    Setup_Tab7_AgeLengthTV->setFont(noBoldFont);
    Setup_Tab7_AgeLengthTV->setModel(nullptr);

    // Make connections
    connect(Setup_Tab7_LoadPB, SIGNAL(clicked()),
            this,              SLOT(callback_LoadPB()));
    connect(Setup_Tab7_SavePB, SIGNAL(clicked()),
            this,              SLOT(callback_SavePB()));
    connect(Setup_Tab7_PrevPB, SIGNAL(clicked()),
            this,              SLOT(callback_PrevPB()));
    connect(Setup_Tab7_MinMaxPB, SIGNAL(clicked()),
            this,                SLOT(callback_MinMaxPB()));
}

nmfSetup_Tab7::~nmfSetup_Tab7()
{
}

void
nmfSetup_Tab7::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSetup_Tab7::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    settings->setValue("Name", QString::fromStdString(m_ProjectSettingsConfig));
    settings->endGroup();
    delete settings;
}

void
nmfSetup_Tab7::callback_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}


void
nmfSetup_Tab7::callback_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}

void
nmfSetup_Tab7::callback_MinMaxPB()
{
    int MinAge;
    int MaxAge;
    int FirstYear;
    int LastYear;
    int MinLength;
    int MaxLength;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Setup_Tab7_AgeLengthTV->model());
    int NumRows = smodel->rowCount();
    int NumCols = smodel->columnCount();
    QString currentSpecies = getSpecies();
    QModelIndex index;

    getSpeciesData(currentSpecies,MinAge,MaxAge,FirstYear,LastYear,
                   MinLength,MaxLength);

    // Set first and last Age values as specified in Species matrix
    for (int row = 0; row < NumRows; ++row) {
        index = smodel->index(row,0);
        smodel->setData(index,QString::number(MinLength));
        index = smodel->index(row,NumCols-1);
        smodel->setData(index,QString::number(MaxLength));
    }
}

void
nmfSetup_Tab7::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSetup_Tab7::callback_SavePB()
{
    if (Setup_Tab7_AgeLengthTV->model() == nullptr) {
        return;
    }
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(Setup_Tab7_AgeLengthTV->model());
    int NumRows = smodel->rowCount();
    int NumCols = smodel->columnCount();
    QString Species = getSpecies();
    QModelIndex index;
    std::string deleteCmd;
    std::string saveCmd;
    std::string errorMsg;
    std::string Value;
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    QStandardItem* YearLabel;
    QStandardItem* AgeLabel;

    // Delete the current Species entry here
    deleteCmd = "DELETE FROM AgeLengthKey WHERE SystemName = '" + m_ProjectSettingsConfig + "'" +
                " AND SpeName = '" + Species.toStdString() + "'";
    errorMsg = m_databasePtr->nmfUpdateDatabase(deleteCmd);
    if (errorMsg != " ") {
        msg = "\nError in Save command. Couldn't delete all records from AgeLengthKey table";
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab7::callback_SavePB: DELETE error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(Setup_Tab7_Widget, "Error", msg, QMessageBox::Ok);
        return;
    }

    // Build insert command from the model data
    saveCmd = "INSERT INTO AgeLengthKey ";
    saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Year,Age,Value) VALUES ";
    for (int row = 0; row < NumRows; ++row) {
        YearLabel = smodel->verticalHeaderItem(row);
        for (int col = 0; col < NumCols; ++col) {
            AgeLabel  = smodel->horizontalHeaderItem(col);
            index     = smodel->index(row,col);
            Value     = index.data().toString().toStdString();
            saveCmd  += "('"  + MohnsRhoLabel +
                        "','" + m_ProjectSettingsConfig +
                        "','" + Species.toStdString() +
                        "', " + YearLabel->text().toStdString() +
                        " , " + AgeLabel->text().split(" ")[1].toStdString() +
                        " , " + Value + " ),";
        }
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab7::callback_SavePB: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(Setup_Tab7_Widget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return;
    }

    // Reload widgets so table is centered
    loadWidgets();

    msg = "\nAgeLengthKey table has been successfully updated.\n";
    QMessageBox::information(Setup_Tab7_Widget, "AgeLengthKey Updated", msg, QMessageBox::Ok);

}

QString
nmfSetup_Tab7::getSpecies()
{
    return Setup_Tab7_SpeciesLBL->text();
}

void
nmfSetup_Tab7::getSpeciesData(QString& Species,
                              int& MinAge,
                              int& MaxAge,
                              int& FirstYear,
                              int& LastYear,
                              int& MinLength,
                              int& MaxLength)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    if (Species.isEmpty())
        return;

    fields     = {"SpeName","MinAge","MaxAge","FirstYear","LastYear","MinLength","MaxLength"};
    queryStr   = "SELECT SpeName,MinAge,MaxAge,FirstYear,LastYear,MinLength,MaxLength FROM Species";
    queryStr  += " WHERE SpeName = '" + Species.toStdString() + "'";

    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["MinAge"].size();
    if (NumRecords == 0) {
        return;
    }

    // Get values to send back
    MinAge    = std::stoi(dataMap["MinAge"][0]);
    MaxAge    = std::stoi(dataMap["MaxAge"][0]);
    FirstYear = std::stoi(dataMap["FirstYear"][0]);
    LastYear  = std::stoi(dataMap["LastYear"][0]);
    MinLength = std::stoi(dataMap["MinLength"][0]);
    MaxLength = std::stoi(dataMap["MaxLength"][0]);
}


QTableView*
nmfSetup_Tab7::getTableAgeLengthKey()
{
    return Setup_Tab7_AgeLengthTV;
}

void
nmfSetup_Tab7::setSpecies(QString species)
{
    clearWidgets();
    Setup_Tab7_SpeciesLBL->setText(species);
    loadWidgets();
}

void
nmfSetup_Tab7::clearWidgets()
{
    QStandardItemModel *smodel;
    QList<QTableView*> tables = {Setup_Tab7_AgeLengthTV};

    for (QTableView* table : tables) {
        smodel = qobject_cast<QStandardItemModel*>(table->model());
        if (smodel) {
            smodel->clear();
        }
    }

    Setup_Tab7_SpeciesLBL->clear();
}

void
nmfSetup_Tab7::loadWidgets()
{
std::cout << "nmfSetup_Tab7::loadWidgets()" << std::endl;

    int m;
    int MinAge;
    int MaxAge;
    int NumYears;
    int NumAges;
    int FirstYear;
    int LastYear;
    int MinLength;
    int MaxLength;
    int NumRecords;
    QStandardItemModel* smodel;
    std::string theSpecies;
    std::vector<std::string> Species;
    QStringList RowLabels;
    QStringList ColLabels;
    std::map<std::string,int> SpeciesFleetsMap;
    QStandardItem *item;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString currentSpecies;

//  clearWidgets();

    currentSpecies = getSpecies();
    if (currentSpecies.isEmpty())
        return;

    // Read AgeLengthKey data
    fields     = {"SystemName","SpeName","Year","Age","Value"};
    queryStr   = "SELECT SystemName,SpeName,Year,Age,Value FROM AgeLengthKey";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " AND SpeName = '" + currentSpecies.toStdString() + "'";
    queryStr  += " ORDER BY Year,Age";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"Warning: No records found in table: AgeLengthKey");
    }

    getSpeciesData(currentSpecies,MinAge,MaxAge,FirstYear,LastYear,
                   MinLength,MaxLength);
    NumAges  = MaxAge - MinAge + 1;
    NumYears = LastYear - FirstYear + 1;

    smodel = new QStandardItemModel(NumYears,NumAges);
    m = 0;
    for (int row = 0; row < NumYears; ++row) {
        RowLabels << QString::number(FirstYear+row);
        for (int col = 0; col < NumAges; ++col) {
            if (row == 0) {
                ColLabels << "Age "+QString::number(col);
            }
            if (m < NumRecords) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m]));
            } else {
                item = new QStandardItem("");
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
            ++m;
        }
    }

    smodel->setVerticalHeaderLabels(RowLabels);
    smodel->setHorizontalHeaderLabels(ColLabels);
    Setup_Tab7_AgeLengthTV->setModel(smodel);
    Setup_Tab7_AgeLengthTV->resizeColumnsToContents();

/*
    // Read fleet data
    fields     = {"SystemName","SpeName","FleetNumber","FleetName"};
    queryStr   = "SELECT SystemName,SpeName,FleetNumber,FleetName FROM Fleets";
    queryStr  += " WHERE SystemName = '" + ProjectSettingsConfig + "'";
    queryStr  += " ORDER BY SpeName,FleetNumber";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        logger->logMsg(nmfConstants::Warning,"Warning: No records found in table: Fleets");
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
    Setup_Tab7_FleetNameTV->setModel(smodel);
    Setup_Tab7_FleetNameTV->resizeColumnsToContents();
    */
}

