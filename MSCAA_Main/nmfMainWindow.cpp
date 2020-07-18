#include "nmfMainWindow.h"
#include "ui_nmfMainWindow.h"
#include "nmfUtilsQt.h"

//#include "nmfConstants.h"
//#include "nmfConstantsMSCAA.h"

#include <QIODevice>
#include <QTextStream>


nmfMainWindow::nmfMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_UI(new Ui::nmfMainWindow)
{
    bool usePreviousDatabase = false;

    m_UI->setupUi(this);

    m_ProjectDir.clear();
    m_ProjectDatabase.clear();
    m_ProjectName.clear();
    m_ProjectSettingsConfig.clear();
    m_Username.clear();
    m_Password.clear();
    m_PreferencesWidget = nullptr;
    m_PreferencesDlg    = new QDialog();

    EntityListLV = m_UI->EntityDockWidget->findChild<QListView *>("EntityListLV");

    // Check for and make if necessary hidden dirs for program usage: logs, data
    nmfUtilsQt::checkForAndCreateDirectories(nmfConstantsMSCAA::HiddenDir,
                                             nmfConstantsMSCAA::HiddenDataDir,
                                             nmfConstantsMSCAA::HiddenLogDir);

    // Check for log files
    nmfUtilsQt::checkForAndDeleteLogFiles("MSCAA",
             nmfConstantsMSCAA::HiddenLogDir,
             nmfConstantsMSCAA::LogFilter);

    m_logger = new nmfLogger();
    m_logger->initLogger("MSCAA");

    // On Windows, the following Sql code must be done in main .exe file or else
    // the program can't find the libmysql.dll driver.  Not sure why, but moving
    // the following logic from nmfDatabase.dll to here fixes the issue.
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    m_databasePtr = new nmfDatabase();
    m_databasePtr->nmfSetConnectionByName(db.connectionName());

    // Hide Progress Chart and Log widgets. Show them once user does their first MSCAA run.
    setDefaultDockWidgetsVisibility();

    // Prompt user for database login and password
    if (nmfDatabaseUtils::menu_connectToDatabase(
                this,nmfConstantsMSCAA::SettingsDirWindows,m_databasePtr,
                m_Username,m_Password))
    {
        usePreviousDatabase = queryUserPreviousDatabase();
    } else {
        m_isStartUpOK = false;
        return;
    }
    readSettings(); // Read settings again in case user has cleared settings

    initializePreferencesDlg();

    // Initialize widgets and connections
    setupLogWidget();
    initGUIs();
    initConnections();

    // Setup chart widgets and default state
    setup2dChart();
    setup3dChart();
    setChartView("2D");

    // Arrange tab widgets
    // Arrange Progress and Log dock widgets.
    QMainWindow::tabifyDockWidget(m_UI->ProgressDockWidget,m_UI->LogDockWidget);
    m_UI->LogDockWidget->raise();

    // Load widgets with data from database
    callback_ReloadWidgets();

    if (! usePreviousDatabase) {
        Setup_Tab2_ptr->clearProject();
        enableApplicationFeatures(false);
    }

}

bool
nmfMainWindow::isStartUpOK()
{
    return m_isStartUpOK;
}

void
nmfMainWindow::initializePreferencesDlg()
{
    QUiLoader loader;
    QFile file(":/forms/Main/PreferencesDlg.ui");
    file.open(QFile::ReadOnly);
    m_PreferencesWidget = loader.load(&file,this);
    file.close();

    QComboBox*   styleCMB = m_PreferencesWidget->findChild<QComboBox*>("PrefAppStyleCMB");
    QPushButton* closePB  = m_PreferencesWidget->findChild<QPushButton*>("PrefClosePB");

    QVBoxLayout* layt = new QVBoxLayout();
    layt->addWidget(m_PreferencesWidget);
    m_PreferencesDlg->setLayout(layt);
    m_PreferencesDlg->setWindowTitle("Preferences");

    connect(styleCMB,         SIGNAL(currentTextChanged(QString)),
            this,             SLOT(callback_PreferencesSetStyleSheet(QString)));
    connect(closePB,          SIGNAL(clicked()),
            m_PreferencesDlg, SLOT(close()));
}

void
nmfMainWindow::callback_PreferencesSetStyleSheet(QString style)
{
    if (style == "Dark") {
        QFile fileStyle(":qdarkstyle/style.qss");
        if (! fileStyle.exists()) {
            std::cout << "Error: Unable to set stylesheet, file not found: qdarkstyle/style.qss\n" << std::endl;;
        } else {
            fileStyle.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&fileStyle);
            qApp->setStyleSheet(ts.readAll());
        }
    } else {
            qApp->setStyleSheet("");
    }
}

void
nmfMainWindow::updateWindowTitle()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName     = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir      = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectDatabase = settings->value("ProjectDatabase","").toString().toStdString();
    m_SetupFontSize   = settings->value("FontSize",9).toInt();
    settings->endGroup();

    std::string winTitle = "MSCAA (" + m_ProjectName + " - " + m_ProjectSettingsConfig + ")";
    setWindowTitle(QString::fromStdString(winTitle));

    delete settings;
}



void
nmfMainWindow::enableApplicationFeatures(bool enable)
{
    QTreeWidgetItem* item;

    // Adjust some items in the Setup group
    item = NavigatorTree->topLevelItem(0);
    if (enable) {
        for (int j=2; j<item->childCount(); ++j) {
            item->child(j)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
            Setup_Tab2_ptr->enableSetupTabs(true);
        }
    } else {
        for (int j=2; j<item->childCount(); ++j) {
            item->child(j)->setFlags(Qt::NoItemFlags);
            Setup_Tab2_ptr->enableSetupTabs(false);
        }
    }

    // Adjust other Navigation groups
    for (int i=1; i<NavigatorTree->topLevelItemCount(); ++i) {
        item = NavigatorTree->topLevelItem(i);
        if (enable) {
            item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        } else {
            item->setFlags(Qt::NoItemFlags);
        }
    }
}

void
nmfMainWindow::setupLogWidget()
{
    nmfLogWidget* logWidget = new nmfLogWidget(m_logger,nmfConstantsMSCAA::LogDir);
    m_UI->LogWidget->setLayout(logWidget->vMainLayt);
}

bool
nmfMainWindow::queryUserPreviousDatabase()
{
    QMessageBox::StandardButton reply;
    std::string msg  = "\nLast Project worked on:  " + m_ProjectName + "\n\nContinue working with this Project?\n";
    reply = QMessageBox::question(this, tr("Open"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    return (reply == QMessageBox::Yes);
}

void
nmfMainWindow::loadDatabase()
{
    QString msg = QString::fromStdString("Loading database: "+m_ProjectDatabase);
    m_logger->logMsg(nmfConstants::Normal,msg.toStdString());
    m_databasePtr->nmfSetDatabase(m_ProjectDatabase);
    saveSettings();
}


nmfMainWindow::~nmfMainWindow()
{
    delete m_UI;
}

void
nmfMainWindow::showDockWidgets(bool show)
{
    m_UI->NavigatorDockWidget->setVisible(show);
    m_UI->ProgressDockWidget->setVisible(show);
    m_UI->LogDockWidget->setVisible(show);
    m_UI->OutputDockWidget->setVisible(show);

//    // Turn these off.
//    m_UI->SetupOutputTE->setVisible(false);

} // end showDockWidgets

void
nmfMainWindow::initializeNavigatorTree()
{
    QTreeWidgetItem *item;

    NavigatorTree->clear();
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,"Setup");
    nmfUtilsQt::addTreeItem(item, "1. Getting Started");
    nmfUtilsQt::addTreeItem(item, "2. Project Setup");
    nmfUtilsQt::addTreeItem(item, "3. Species Setup");
    nmfUtilsQt::addTreeItem(item, "4. Environmental Setup");
    nmfUtilsQt::addTreeItem(item, "5. Survey Setup");
    nmfUtilsQt::addTreeItem(item, "6. Fleet Setup");
    nmfUtilsQt::addTreeItem(item, "7. Age-Length Setup");
    item->setExpanded(true);

    // Create Single-Species Data Input navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,"SSCAA Data Input");
    nmfUtilsQt::addTreeItem(item, "1. Fishery Catch");
    nmfUtilsQt::addTreeItem(item, "2. Survey Catch");
    nmfUtilsQt::addTreeItem(item, "3. Weight-At-Age");
    nmfUtilsQt::addTreeItem(item, "4. Maturity");
    nmfUtilsQt::addTreeItem(item, "5. Mortality");
    nmfUtilsQt::addTreeItem(item, "6. Run");
    item->setExpanded(true);


    // Create Multi-Species Data Input navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,"MSCAA Data Input");
    nmfUtilsQt::addTreeItem(item, "1. Predator/Prey Configuration");
    nmfUtilsQt::addTreeItem(item, "2. Predator Size Preference");
    nmfUtilsQt::addTreeItem(item, "3. Predator Diet");
    nmfUtilsQt::addTreeItem(item, "4. Consumption:Biomass");
    nmfUtilsQt::addTreeItem(item, "5. Run");
    item->setExpanded(true);

    // RSK - TBD
    /*
    // Create Simulation Data Input navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,"Simulation Data Input");
    nmfUtilsQt::addTreeItem(item, "1. Simulation Parameters           ");
    nmfUtilsQt::addTreeItem(item, "2. Run");
    item->setExpanded(true);

    // Create Diagnostics navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,"Diagnostics");
    nmfUtilsQt::addTreeItem(item, "1. Setup");
    item->setExpanded(true);

    // Create Forecast navigator group
    item = nmfUtilsQt::addTreeRoot(NavigatorTree,"Forecast");
    nmfUtilsQt::addTreeItem(item, "1. Setup");
    item->setExpanded(true);
    */
}

void
nmfMainWindow::loadEntityList()
{
std::cout << "LoadEntityList"  << std::endl;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    EntityListLV = m_UI->EntityDockWidget->findChild<QListView *>("EntityListLV");

    // Clear entity list
    m_entityModel.removeRows(0, m_entityModel.count(), QModelIndex());

    // Get species names and load gui
    fields = {"SpeName","MinAge","MaxAge","FirstYear","LastYear"};
    queryStr   = "SELECT SpeName,MinAge,MaxAge,FirstYear,LastYear FROM Species";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        m_logger->logMsg(nmfConstants::Error,"nmfMainWindow::LoadEntityList: No Species Found");
    } else {
        m_logger->logMsg(nmfConstants::Normal,"nmfMainWindow::LoadEntityList: Num Species found: "+std::to_string(NumSpecies));
        for (int i=0; i<NumSpecies; ++i) {
            m_entityModel.append( nmfEntity{ QString::fromStdString(dataMap["SpeName"][i]) });
        }
        EntityListLV->setModel(&m_entityModel);
    }
}

QString
nmfMainWindow::getSpecies()
{
    QModelIndex index = EntityListLV->currentIndex();
    return index.data(Qt::DisplayRole).toString();
}

void
nmfMainWindow::loadGUIs()
{
    m_logger->logMsg(nmfConstants::Normal,"Loading all widgets");

    Setup_Tab1_ptr->loadWidgets();
    Setup_Tab2_ptr->loadWidgets();
    Setup_Tab3_ptr->loadWidgets();
    Setup_Tab4_ptr->loadWidgets();
    Setup_Tab5_ptr->loadWidgets();
    Setup_Tab6_ptr->loadWidgets();
    Setup_Tab7_ptr->loadWidgets();

    SSCAA_Tab1_ptr->loadWidgets();
    SSCAA_Tab2_ptr->loadWidgets();
    SSCAA_Tab3_ptr->loadWidgets();
    SSCAA_Tab4_ptr->loadWidgets();
    SSCAA_Tab5_ptr->loadWidgets();
    SSCAA_Tab6_ptr->loadWidgets();

    MSCAA_Tab1_ptr->loadWidgets();
    MSCAA_Tab2_ptr->loadWidgets();
    MSCAA_Tab3_ptr->loadWidgets();
    MSCAA_Tab4_ptr->loadWidgets();
    MSCAA_Tab5_ptr->loadWidgets();

//    Simulation_Tab1_ptr->loadWidgets();
//    Simulation_Tab2_ptr->loadWidgets();


//std::cout << "==== opening databse" << std::endl;
//    Setup_Tab2_ptr->initDatabase(QString::fromStdString(ProjectDatabase));
}

void
nmfMainWindow::initGUIs()
{
    QUiLoader loader;

    m_UI->SetupOutputTE->hide();
    m_UI->SSCAADataInputTabWidget->hide();
    m_UI->MSCAADataInputTabWidget->hide();
    m_UI->ForecastDataInputTabWidget->hide();
    m_UI->SimulationDataInputTabWidget->hide();

    // Initialize Navigator Tree
    QFile fileNav(":/forms/Main/Main_NavigatorTreeWidget.ui");
    fileNav.open(QFile::ReadOnly);
    NavigatorTreeWidget = loader.load(&fileNav,this);
    fileNav.close();
    m_UI->NavigatorDockWidget->setWidget(NavigatorTreeWidget);
    NavigatorTree = m_UI->NavigatorDockWidget->findChild<QTreeWidget *>("NavigatorTree");
    QModelIndex index = NavigatorTree->model()->index(0,0);
    NavigatorTree->setCurrentIndex(index);
    initializeNavigatorTree();

    // Initialize EntityDockWidget with Species
    QFile fileEntity(":/forms/Main/Main_EntityListWidget.ui");
    fileEntity.open(QFile::ReadOnly);
    EntityListWidget = loader.load(&fileEntity,this);
    fileEntity.close();

    m_UI->EntityDockWidget->setWidget(EntityListWidget);
    loadEntityList();
    loadDatabase();
    initInputPages();

    // Select first item in Navigator Tree
    callback_SetupTabChanged(0);
}

void
nmfMainWindow::initInputPages()
{

    // Create input pages
    Setup_Tab1_ptr = new nmfSetup_Tab1(m_UI->SetupInputTabWidget);
    Setup_Tab2_ptr = new nmfSetup_Tab2(m_UI->SetupInputTabWidget,m_logger,m_databasePtr,m_UI->SetupOutputTE);
    Setup_Tab3_ptr = new nmfSetup_Tab3(m_UI->SetupInputTabWidget,m_logger,m_databasePtr,m_UI->SetupOutputTE,m_ProjectDir);
    Setup_Tab4_ptr = new nmfSetup_Tab4(m_UI->SetupInputTabWidget,m_logger,m_databasePtr,m_UI->SetupOutputTE,m_ProjectDir);
    Setup_Tab5_ptr = new nmfSetup_Tab5(m_UI->SetupInputTabWidget,m_logger,m_databasePtr,m_UI->SetupOutputTE,m_ProjectDir);
    Setup_Tab6_ptr = new nmfSetup_Tab6(m_UI->SetupInputTabWidget,m_logger,m_databasePtr,m_UI->SetupOutputTE,m_ProjectDir);
    Setup_Tab7_ptr = new nmfSetup_Tab7(m_UI->SetupInputTabWidget,m_logger,m_databasePtr,m_UI->SetupOutputTE,m_ProjectDir);

    SSCAA_Tab1_ptr = new nmfSSCAA_Tab1(m_UI->SSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    SSCAA_Tab2_ptr = new nmfSSCAA_Tab2(m_UI->SSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    SSCAA_Tab3_ptr = new nmfSSCAA_Tab3(m_UI->SSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    SSCAA_Tab4_ptr = new nmfSSCAA_Tab4(m_UI->SSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    SSCAA_Tab5_ptr = new nmfSSCAA_Tab5(m_UI->SSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    SSCAA_Tab6_ptr = new nmfSSCAA_Tab6(m_UI->SSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir,m_ProjectName);

    MSCAA_Tab1_ptr = new nmfMSCAA_Tab1(m_UI->MSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    MSCAA_Tab2_ptr = new nmfMSCAA_Tab2(m_UI->MSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    MSCAA_Tab3_ptr = new nmfMSCAA_Tab3(m_UI->MSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    MSCAA_Tab4_ptr = new nmfMSCAA_Tab4(m_UI->MSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
    MSCAA_Tab5_ptr = new nmfMSCAA_Tab5(m_UI->MSCAADataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir,m_ProjectName);

//    Simulation_Tab1_ptr = new nmfSimulation_Tab1(m_UI->SimulationDataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);
//    Simulation_Tab2_ptr = new nmfSimulation_Tab2(m_UI->SimulationDataInputTabWidget,m_logger,m_databasePtr,m_ProjectDir);

//    Diagnostic_Tab1_ptr = new nmfDiagnostic_Tab1(ui->DiagnosticsDataInputTabWidget,logger,databasePtr,ProjectDir);
//    Diagnostic_Tab2_ptr = new nmfDiagnostic_Tab2(ui->DiagnosticsDataInputTabWidget,logger,databasePtr,ProjectDir);

//    Forecast_Tab1_ptr   = new nmfForecast_Tab1(ui->ForecastDataInputTabWidget,logger,databasePtr,ProjectDir);
//    Forecast_Tab2_ptr   = new nmfForecast_Tab2(ui->ForecastDataInputTabWidget,logger,databasePtr,ProjectDir);
//    Forecast_Tab3_ptr   = new nmfForecast_Tab3(ui->ForecastDataInputTabWidget,logger,databasePtr,ProjectDir);
//    Forecast_Tab4_ptr   = new nmfForecast_Tab4(ui->ForecastDataInputTabWidget,logger,databasePtr,ProjectDir);

      Output_Controls_ptr = new MSCAA_GuiOutputControls(m_UI->MSCAAOutputControlsGB,m_logger,m_databasePtr,m_ProjectDir);
}


void
nmfMainWindow::initConnections()
{

    // Make GUI connections
    connect(m_UI->SetupInputTabWidget,     SIGNAL(currentChanged(int)),
            this,                          SLOT(callback_SetupTabChanged(int)));
    connect(m_UI->SSCAADataInputTabWidget, SIGNAL(currentChanged(int)),
            this,                          SLOT(callback_SSCAATabChanged(int)));
    connect(m_UI->MSCAADataInputTabWidget, SIGNAL(currentChanged(int)),
            this,                          SLOT(callback_MSCAATabChanged(int)));
    connect(m_UI->SimulationDataInputTabWidget, SIGNAL(currentChanged(int)),
            this,                               SLOT(callback_SimulationTabChanged(int)));
    connect(NavigatorTree,               SIGNAL(itemSelectionChanged()),
            this,                        SLOT(callback_NavigatorSelectionChanged()));
    connect(Setup_Tab2_ptr,              SIGNAL(ReloadWidgets()),
            this,                        SLOT(callback_ReloadWidgets()));
    connect(Setup_Tab2_ptr,              SIGNAL(SavedProject()),
            this,                        SLOT(callback_ProjectSaved()));
    connect(Setup_Tab3_ptr,              SIGNAL(LoadedSpecies()),
            this,                        SLOT(callback_RefreshEntityList()));
    connect(EntityListLV,                SIGNAL(clicked(QModelIndex)),
            this,                        SLOT(callback_EntityList(QModelIndex)));
    connect(Setup_Tab6_ptr,              SIGNAL(LoadedSystemData()),
            MSCAA_Tab1_ptr,              SLOT(callback_LoadPB()));
    connect(MSCAA_Tab1_ptr,              SIGNAL(LoadedSystemData()),
            Setup_Tab6_ptr,              SLOT(callback_LoadSystemDataNoEmit()));

    // Make menu connections
    connect(m_UI->actionAbout,          SIGNAL(triggered()),
            this,                       SLOT(menu_about()));
    connect(m_UI->actionWhatsThis,      SIGNAL(triggered()),
            this,                       SLOT(menu_whatsThis()));
    connect(m_UI->actionCreateTables,   SIGNAL(triggered()),
            this,                       SLOT(menu_createTables()));
    connect(m_UI->actionQuit,           SIGNAL(triggered()),
            this,                       SLOT(menu_quit()));
    connect(m_UI->actionCopy,           SIGNAL(triggered()),
            this,                       SLOT(menu_copy()));
    connect(m_UI->actionPaste,          SIGNAL(triggered()),
            this,                       SLOT(menu_paste()));
    connect(m_UI->actionClear,          SIGNAL(triggered()),
            this,                       SLOT(menu_clear()));
    connect(m_UI->actionPasteAll,       SIGNAL(triggered()),
            this,                       SLOT(menu_pasteAll()));
    connect(m_UI->actionSelectAll,      SIGNAL(triggered()),
            this,                       SLOT(menu_selectAll()));
    connect(m_UI->actionDeselectAll,    SIGNAL(triggered()),
            this,                       SLOT(menu_deselectAll()));
    connect(m_UI->actionImportDatabase, SIGNAL(triggered()),
            this,                       SLOT(menu_importDatabase()));
    connect(m_UI->actionExportDatabase, SIGNAL(triggered()),
            this,                       SLOT(menu_exportDatabase()));
    connect(m_UI->actionShowLastADMBRun,SIGNAL(triggered()),
            this,                       SLOT(menu_showLastADMBRun()));
    connect(m_UI->actionLayoutOutput,   SIGNAL(triggered()),
            this,                       SLOT(menu_layoutOutput()));
    connect(m_UI->actionLayoutDefault,  SIGNAL(triggered()),
            this,                       SLOT(menu_layoutDefault()));
    connect(m_UI->actionShowTableNames, SIGNAL(triggered()),
            this,                       SLOT(menu_showTableNames()));
    connect(m_UI->actionPreferences,    SIGNAL(triggered()),
            this,                       SLOT(menu_preferences()));
    connect(m_UI->actionScreenShot,     SIGNAL(triggered()),
            this,                       SLOT(menu_screenShot()));

    // Make Output Control connections
    connect(Output_Controls_ptr, SIGNAL(ShowChart(QString,QString)),
            this,                SLOT(callback_ShowChart(QString,QString)));
    connect(Output_Controls_ptr, SIGNAL(UpdateAgeList()),
            this,                SLOT(callback_UpdateAgeList()));
    connect(Output_Controls_ptr, SIGNAL(AbundanceAgeGroupsSelected(QString,QModelIndexList)),
            this,                SLOT(callback_AbundanceAgeGroupsSelected(QString,QModelIndexList)));
    connect(Output_Controls_ptr, SIGNAL(MortalityAgeGroupsSelected(QModelIndexList,QModelIndexList)),
            this,                SLOT(callback_MortalityAgeGroupsSelected(QModelIndexList,QModelIndexList)));
//    connect(Simulation_Tab2_ptr, SIGNAL(UpdateOutputCharts(QString)),
//            this,                SLOT(callback_ShowChart(QString)));
//    connect(Simulation_Tab2_ptr, SIGNAL(UpdateSuitabilityNuOther(double)),
//            Simulation_Tab1_ptr, SLOT(callback_UpdateSuitabilityNuOther(double)));
//    connect(Simulation_Tab2_ptr, SIGNAL(LoadUserSuppliedNuOther()),
//            this,                SLOT(callback_LoadUserSuppliedNuOther()));
//    connect(Simulation_Tab1_ptr, SIGNAL(RunSimulation()),
//            Simulation_Tab2_ptr, SLOT(callback_RunPB()));
    connect(SSCAA_Tab6_ptr,      SIGNAL(UpdateOutputCharts(QString)),
            this,                SLOT(callback_ShowChart(QString)));
    connect(MSCAA_Tab5_ptr,      SIGNAL(SelectDefaultSpecies()),
            this,                SLOT(callback_SelectDefaultSpecies()));
    connect(MSCAA_Tab5_ptr,      SIGNAL(UpdateOutputCharts(QString)),
            this,                SLOT(callback_ShowChart(QString)));
    connect(SSCAA_Tab6_ptr,      SIGNAL(RunADMB(int)),
            MSCAA_Tab5_ptr,      SLOT(callback_RunPB(int)));

}



void
nmfMainWindow::callback_NavigatorSelectionChanged()
{
    QString parentStr;
    QString itemSelected;
    int pageNum;

    //updateMainWindowTitle("");

    // Handle the case if the user hasn't selected anything yet.
    QList<QTreeWidgetItem *> selectedItems = NavigatorTree->selectedItems();

    if (selectedItems.count() > 0) {
        itemSelected = selectedItems[0]->text(0);
        pageNum = itemSelected.at(0).digitValue();
        if (pageNum < 1)
            pageNum = 1;

        parentStr.clear();
        if (selectedItems[0]->parent()) {
            parentStr = selectedItems[0]->parent()->text(0);
        }

        m_UI->SetupInputTabWidget->hide();
        m_UI->SSCAADataInputTabWidget->hide();
        m_UI->MSCAADataInputTabWidget->hide();
        m_UI->ForecastDataInputTabWidget->hide();
        m_UI->SimulationDataInputTabWidget->hide();

        if ((itemSelected == "Setup") ||
            (parentStr    == "Setup")) {
            m_UI->EntityDockWidget->setEnabled(true);
            m_UI->SetupInputTabWidget->show();
            if (pageNum > 0) {
                m_UI->SetupInputTabWidget->blockSignals(true);
                m_UI->SetupInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->SetupInputTabWidget->blockSignals(false);
            }
        } else if ((itemSelected == "SSCAA Data Input") ||
                   (parentStr    == "SSCAA Data Input")) {
            m_UI->EntityDockWidget->setEnabled(true);
            m_UI->SSCAADataInputTabWidget->show();
            // Select appropriate tab
            if (pageNum > 0) {
                m_UI->SSCAADataInputTabWidget->blockSignals(true);
                m_UI->SSCAADataInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->SSCAADataInputTabWidget->blockSignals(false);
            }
            // Select first species by default (if there are any species);
//            if (EntityListLV->model()->rowCount() > 0) {
//                EntityListLV->setCurrentIndex(EntityListLV->model()->index(0,0));
//                callback_EntityList(EntityListLV->model()->index(0,0));
//            }
        } else if ((itemSelected == "MSCAA Data Input") ||
                   (parentStr    == "MSCAA Data Input")) {
            m_UI->EntityDockWidget->setEnabled(pageNum==4);
            m_UI->MSCAADataInputTabWidget->show();
            // Select appropriate tab
            if (pageNum > 0) {
                m_UI->MSCAADataInputTabWidget->blockSignals(true);
                m_UI->MSCAADataInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->MSCAADataInputTabWidget->blockSignals(false);
            }
        }  else if ((itemSelected == "Simulation Data Input") ||
                    (parentStr    == "Simulation Data Input")) {
            m_UI->EntityDockWidget->setEnabled(true);
            m_UI->SimulationDataInputTabWidget->show();
            // Select appropriate tab
            if (pageNum > 0) {
                m_UI->SimulationDataInputTabWidget->blockSignals(true);
                m_UI->SimulationDataInputTabWidget->setCurrentIndex(pageNum-1);
                m_UI->SimulationDataInputTabWidget->blockSignals(false);
            }
        }
    }

}

void
nmfMainWindow::menu_screenShot()
{
    QPixmap pm;
    QString outputFile;
    if (m_ChartView2d->isVisible() || m_ChartView3d->isVisible())
    {
        QApplication::sync();

        // Get output filename
        nmfStructsQt::ChartSaveDlg *dlg = new nmfStructsQt::ChartSaveDlg(this);
        if (dlg->exec()) {
            outputFile = dlg->getFilename();
        }
        delete dlg;
        if (outputFile.isEmpty())
            return;

        // Grab the image and store in a pixmap
        if (m_ChartView2d->isVisible()) {
            m_ChartView2d->update();
            m_ChartView2d->repaint();
            pm = m_ChartView2d->grab();
        } else if (m_ChartView3d->isVisible()) {
            m_ChartView3d->update();
            m_ChartView3d->repaint();
            pm.convertFromImage(m_Graph3D->renderToImage());
        }
        saveScreenshot(outputFile,pm);
    } else {
        QMessageBox::information(this,
                                 tr("No Chart"),
                                 tr("\nChart must be visible prior to capturing image.\n"),
                                 QMessageBox::Ok);
    }

}

bool
nmfMainWindow::saveScreenshot(QString &outputFile, QPixmap &pm)
{
    QString msg;
    QString path = QDir(QString::fromStdString(m_ProjectDir)).filePath(QString::fromStdString(nmfConstantsMSSPM::OutputImagesDir));
    QMessageBox::StandardButton reply;
    QString outputFileWithPath;

    // If path doesn't exist make it
    QDir pathDir(path);
    if (! pathDir.exists()) {
        pathDir.mkpath(path);
    }
    outputFileWithPath = QDir(path).filePath(outputFile);
    if (QFileInfo(outputFileWithPath).exists()) {
        msg   = "\nFile exists. OK to overwrite?\n";
        reply = QMessageBox::question(this, tr("File Exists"), tr(msg.toLatin1()),
                                      QMessageBox::No|QMessageBox::Yes,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::No)
            return false;
    }

    // Save the image
    pm.save(outputFileWithPath);

    // Notify user image has been saved
    msg = "\nCapture image saved to file:\n\n" + outputFileWithPath;
    QMessageBox::information(this,
                             tr("Image Saved"),
                             tr(msg.toLatin1()),
                             QMessageBox::Ok);

    m_logger->logMsg(nmfConstants::Normal,"menu_screenshot: Image saved: "+ outputFile.toStdString());

    return true;
}

void
nmfMainWindow::menu_preferences()
{
    m_PreferencesDlg->show();
}

QString
nmfMainWindow::getADMBVersion()
{
    int i=0;
    int versionPos = -1;
    QString text;
    QString admbVersion;

    // Run ADMB and read the default output which contains the version.
    std::system("admb > .admbVersion");

    QFile file(".admbVersion");
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "?";
    QTextStream in(&file);
    text = in.readAll();
    file.close();

    // Split the output and get the text just after "Version:" which is the version number.
    QStringList parts = text.split(QRegExp("\\s+"),QString::SkipEmptyParts);
    if ((parts.size() > 1) && (parts[0] == "Builds")) {
        for (int i=0; i<parts.size(); ++i) {
            if (parts[i] == "Version:") {
                versionPos = i+1;
                break;
            }
        }
        if ((versionPos > 0) && (versionPos < parts.size()-1)) {
            admbVersion = parts[versionPos];
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText("\nCouldn't find admb exectuable. Please make sure it's installed and the PATH environment variable properly set.\n");
        msgBox.exec();
        return "?";
    }
    return admbVersion;

}

void
nmfMainWindow::menu_about()
{
    QString name    = "Multi-Species Statistical Catch-At-Age";
    QString version = "MSCAA v0.9.1 (beta)";
    QString specialAcknowledgement = "";
    QString cppVersion   = "C++??";
    QString mysqlVersion = "?";
    QString boostVersion = "?";
    QString admbVersion = "?";
//  QString nloptVersion = "?";
    QString beesLink;
    QString qdarkLink;
    QString linuxDeployLink;
//  QString nloptLink;
    QString boostLink;
    QString mysqlLink;
    QString qtLink;
    QString admbLink;
    QString msg = "";
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString os = QString::fromStdString(nmfUtils::getOS());

    // Define Qt link
    qtLink = QString("<a href='https://www.qt.io'>https://www.qt.io</a>");

    // Find C++ version in case you want it later
    if (__cplusplus == 201103L)
        cppVersion = "C++11";
    else if (__cplusplus == 201402L)
        cppVersion = "C++14";
    else if (__cplusplus == 201703L)
        cppVersion = "C++17";

    // MySQL version and link
    fields   = {"version()"};
    queryStr = "SELECT version()";
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["version()"].size() > 0) {
        mysqlVersion = QString::fromStdString(dataMap["version()"][0]);
    }
    mysqlLink = QString("<a href='https://www.mysql.com'>https://www.mysql.com</a>");

    // Boost version and link
    boostVersion = QString::number(BOOST_VERSION / 100000) + "." +
                   QString::number(BOOST_VERSION / 100 % 1000) + "." +
                   QString::number(BOOST_VERSION / 100);
    boostLink = QString("<a href='https://www.boost.org'>https://www.boost.org</a>");

//  // NLopt version and link
//  int major,minor,bugfix;
//  nlopt::version(major,minor,bugfix);
//  nloptVersion = QString::number(major)+"."+QString::number(minor)+"."+QString::number(bugfix);
//  nloptLink = QString("<a href='https://nlopt.readthedocs.io'>https://nlopt.readthedocs.io</a>");

    // Bees link
    beesLink = QString("<a href='http://beesalgorithmsite.altervista.org'>http://beesalgorithmsite.altervista.org</a>");

    // QDarkStyle link
    qdarkLink = QString("<a href='https://github.com/ColinDuquesnoy/QDarkStyleSheet'>https://github.com/ColinDuquesnoy/QDarkStyleSheet</a>");

    // linuxdeployqt link
    linuxDeployLink = QString("<a href='https://github.com/probonopd/linuxdeployqt'>https://github.com/probonopd/linuxdeployqt</a>");

    // ADMB link
    admbLink = QString("<a href='http://www.admb-project.org'>http://www.admb-project.org</a>");
    admbVersion = getADMBVersion();

    // Build About message
//  msg += QString("<li>")+cppVersion+QString("</li>");
    msg += QString("<li>")+QString("Qt ")+QString::fromUtf8(qVersion())+QString("<br>")+qtLink+QString("</li>");
    msg += QString("<li>")+QString("MySQL ")+mysqlVersion+QString("<br>")+mysqlLink+QString("</li>");
    msg += QString("<li>")+QString("Boost ")+boostVersion+QString("<br>")+boostLink+QString("</li>");
    msg += QString("<li>")+QString("ADMB ")+admbVersion+QString("<br>")+admbLink+QString("</li>");
//  msg += QString("<li>")+QString("NLopt ")+nloptVersion+QString("<br>")+nloptLink+QString("</li>");
//  msg += QString("<li>")+QString("Bees Algorithm (August 4, 2011) - M. Castellani<br>")+beesLink+QString("</li>");
    msg += QString("<li>")+QString("QDarkStyleSheet 2.6.5 - Colin Duquesnoy (MIT License)<br>")+qdarkLink+QString("</li>");
    msg += QString("<li>")+QString("linuxdeployqt 6 (January 27, 2019)<br>")+linuxDeployLink+QString("</li>");
    msg += QString("</ul>");

    nmfUtilsQt::showAboutWidget(this,name,os,version,specialAcknowledgement,msg);
}


void
nmfMainWindow::menu_createTables()
{
    if (m_ProjectDatabase.empty()) {
        m_logger->logMsg(nmfConstants::Error,"Please Create/Select a Database");
        return;
    }

    Setup_Tab2_ptr->createTables(QString::fromStdString(m_ProjectDatabase));

    QMessageBox::information(this, "Information",
                         "\nTables created. Existing tables not modified.\n",
                         QMessageBox::Ok);
//    completeInit();
}

void
nmfMainWindow::menu_whatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void
nmfMainWindow::menu_quit() {
    close(); // emits closeEvent
}

void
nmfMainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
//    menu_stopRun();
}

void
nmfMainWindow::setDefaultDockWidgetsVisibility()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("MainWindow");
    m_UI->OutputDockWidget->setVisible(settings->value("OutputDockWidgetIsVisible",false).toBool());
    m_UI->ProgressDockWidget->setVisible(settings->value("ProgressDockWidgetIsVisible",false).toBool());
    m_UI->LogDockWidget->setVisible(settings->value("LogDockWidgetIsVisible",false).toBool());
    settings->endGroup();

    delete settings;

    // Turn these off
    m_UI->SetupOutputTE->setVisible(false);
}

void
nmfMainWindow::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("MainWindow");
    settings->setValue("pos", pos());
    settings->setValue("size", size());
//  settings->setValue("style",getCurrentStyle());
    settings->setValue("NavigatorDockWidgetWidth",   m_UI->NavigatorDockWidget->width());
    settings->setValue("OutputDockWidgetWidth",      m_UI->OutputDockWidget->width());
    settings->setValue("LogDockWidgetIsVisible",     m_UI->LogDockWidget->isVisible());
    settings->setValue("ProgressDockWidgetIsVisible",m_UI->ProgressDockWidget->isVisible());
    settings->setValue("ProgressDockWidgetWidth",    m_UI->ProgressDockWidget->width());
    settings->setValue("OutputDockWidgetIsVisible",  m_UI->OutputDockWidget->isVisible());
    settings->setValue("OutputDockWidgetIsFloating", m_UI->OutputDockWidget->isFloating());
    settings->setValue("OutputDockWidgetPos",        m_UI->OutputDockWidget->pos());
    settings->setValue("OutputDockWidgetSize",       m_UI->OutputDockWidget->size());
    settings->endGroup();

    settings->beginGroup("SetupTab");
    settings->setValue("ProjectName",               QString::fromStdString(m_ProjectName));
    settings->setValue("ProjectDir",                QString::fromStdString(m_ProjectDir));
    settings->setValue("ProjectDatabase",           QString::fromStdString(m_ProjectDatabase));
//  settings->setValue("ProjectAuthor",             ProjectAuthor);
//  settings->setValue("ProjectDescription",        ProjectDescription);
    settings->endGroup();

    delete settings;


//    settings->beginGroup("SetupTab");
//    settings->setValue("FontSize", Setup_Tab4_ptr->getFontSize());
//    settings->endGroup();

//    Output_Controls_ptr->SaveSettings();
}


void
nmfMainWindow::readProjectSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName     = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir      = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectDatabase = settings->value("ProjectDatabase","").toString().toStdString();
    m_SetupFontSize   = settings->value("FontSize",9).toInt();
    settings->endGroup();
std::cout << "readProjectSettings - db name: " << m_ProjectDatabase << std::endl;

    delete settings;
}

void
nmfMainWindow::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("MainWindow");
    resize(settings->value("size", QSize(400, 400)).toSize());
    move(settings->value("pos", QPoint(200, 200)).toPoint());
    // Resize the dock widgets
    int NavDockWidth      = settings->value("NavigatorDockWidgetWidth",200).toInt();
    int OutputDockWidth   = settings->value("OutputDockWidgetWidth",200).toInt();
    int ProgressDockWidth = settings->value("ProgressDockWidgetWidth",200).toInt();
    this->resizeDocks({m_UI->NavigatorDockWidget,m_UI->OutputDockWidget,m_UI->ProgressDockWidget},
                      {NavDockWidth,OutputDockWidth,ProgressDockWidth},
                      Qt::Horizontal);
    settings->endGroup();

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    settings->beginGroup("SetupTab");
    m_ProjectName     = settings->value("ProjectName","").toString().toStdString();
    m_ProjectDir      = settings->value("ProjectDir","").toString().toStdString();
    m_ProjectDatabase = settings->value("ProjectDatabase","").toString().toStdString();
    m_SetupFontSize   = settings->value("FontSize",9).toInt();
    settings->endGroup();
std::cout << "readSettings: db name: " << m_ProjectDatabase << std::endl;

    delete settings;
}



QTableView*
nmfMainWindow::findTableInFocus()
{
    QTableView *retv = NULL;

    if (Setup_Tab3_ptr->getTableSpecies()->hasFocus()) {
        return Setup_Tab3_ptr->getTableSpecies();
    } else if (Setup_Tab4_ptr->getTableEnvCovariates()->hasFocus()) {
        return Setup_Tab4_ptr->getTableEnvCovariates();
    } else if (Setup_Tab5_ptr->getTableTotalAnnualSurveyCatch()->hasFocus()) {
        return Setup_Tab5_ptr->getTableTotalAnnualSurveyCatch();
    } else if (Setup_Tab5_ptr->getTableSurveyCatchProportions()->hasFocus()) {
        return Setup_Tab5_ptr->getTableSurveyCatchProportions();
    } else if (Setup_Tab6_ptr->getTableFleetData()->hasFocus()) {
        return Setup_Tab6_ptr->getTableFleetData();
    } else if (Setup_Tab7_ptr->getTableAgeLengthKey()->hasFocus()) {
        return Setup_Tab7_ptr->getTableAgeLengthKey();

    } else if (SSCAA_Tab1_ptr->getTableCatchAtAge()->hasFocus()) {
        return SSCAA_Tab1_ptr->getTableCatchAtAge();
    } else if (SSCAA_Tab1_ptr->getTableCatchAtLength()->hasFocus()) {
        return SSCAA_Tab1_ptr->getTableCatchAtLength();
    } else if (SSCAA_Tab1_ptr->getTableTotalWeight()->hasFocus()) {
        return SSCAA_Tab1_ptr->getTableTotalWeight();
    } else if (SSCAA_Tab2_ptr->getTableSurveyCatchAtAge()->hasFocus()) {
        return SSCAA_Tab2_ptr->getTableSurveyCatchAtAge();
    } else if (SSCAA_Tab2_ptr->getTableSurveyTotal()->hasFocus()) {
        return SSCAA_Tab2_ptr->getTableSurveyTotal();
    } else if (SSCAA_Tab3_ptr->getTable()->hasFocus()) {
        return SSCAA_Tab3_ptr->getTable();
    } else if (SSCAA_Tab4_ptr->getTable()->hasFocus()) {
        return SSCAA_Tab4_ptr->getTable();
    } else if (SSCAA_Tab5_ptr->getNaturalMortalityTable()->hasFocus()) {
        return SSCAA_Tab5_ptr->getNaturalMortalityTable();
    } else if (SSCAA_Tab5_ptr->getFishingMortalityTable()->hasFocus()) {
        return SSCAA_Tab5_ptr->getFishingMortalityTable();
    } else if (SSCAA_Tab6_ptr->getTable1()->hasFocus()) {
        return SSCAA_Tab6_ptr->getTable1();
    } else if (SSCAA_Tab6_ptr->getTable2()->hasFocus()) {
        return SSCAA_Tab6_ptr->getTable2();

    } else if (MSCAA_Tab1_ptr->getTableInteractions()->hasFocus()) {
        return MSCAA_Tab1_ptr->getTableInteractions();
    } else if (MSCAA_Tab1_ptr->getTableVulnerability()->hasFocus()) {
        return MSCAA_Tab1_ptr->getTableVulnerability();
    } else if (MSCAA_Tab2_ptr->getTablePreferred()->hasFocus()) {
        return MSCAA_Tab2_ptr->getTablePreferred();
    } else if (MSCAA_Tab2_ptr->getTableVarianceLessThan()->hasFocus()) {
        return MSCAA_Tab2_ptr->getTableVarianceLessThan();
    } else if (MSCAA_Tab2_ptr->getTableVarianceGreaterThan()->hasFocus()) {
        return MSCAA_Tab2_ptr->getTableVarianceGreaterThan();
    } else if (MSCAA_Tab3_ptr->getTableDiet()->hasFocus()) {
        return MSCAA_Tab3_ptr->getTableDiet();
    } else if (MSCAA_Tab4_ptr->getTableConsumptionBiomass()->hasFocus()) {
        return MSCAA_Tab4_ptr->getTableConsumptionBiomass();
    } else if (MSCAA_Tab5_ptr->getTableFoodHabitsProportions()->hasFocus()) {
        return MSCAA_Tab5_ptr->getTableFoodHabitsProportions();
    } else if (MSCAA_Tab5_ptr->getTablePhase()->hasFocus()) {
        return MSCAA_Tab5_ptr->getTablePhase();

//    } else if (Simulation_Tab1_ptr->getYearlyParametersTable()->hasFocus()) {
//        return Simulation_Tab1_ptr->getYearlyParametersTable();
//    } else if (Simulation_Tab2_ptr->getAbundanceTable()->hasFocus()) {
//        return Simulation_Tab2_ptr->getAbundanceTable();
//    } else if (Simulation_Tab2_ptr->getParameterTable()->hasFocus()) {
//        return Simulation_Tab2_ptr->getParameterTable();

    } else {
        QMessageBox::information(this, "Information",
                             "\nTo copy from text box, please use right-click menu option.\n",
                             QMessageBox::Ok);
//      std::cout << "Error: No table found to cut, copy, or paste." << std::endl;
        return retv;
    }
}


void
nmfMainWindow::callback_RefreshEntityList()
{
std::cout << "callback_RefreshEntityList"  << std::endl;
    loadEntityList();

    Setup_Tab5_ptr->loadWidgets();
    Setup_Tab6_ptr->loadWidgets();
    Setup_Tab7_ptr->loadWidgets();
    SSCAA_Tab6_ptr->loadWidgets();
}

void
nmfMainWindow::callback_SetupTabChanged(int tab)
{
    m_UI->EntityDockWidget->setEnabled(true);

    QModelIndex topLevelndex = NavigatorTree->model()->index(0,0); // 0 is Setupgroup in NavigatorTree
    QModelIndex childIndex   = topLevelndex.child(tab,0);

    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);
}

void
nmfMainWindow::callback_SSCAATabChanged(int tab)
{
    m_UI->EntityDockWidget->setEnabled(true);

    QModelIndex topLevelndex = NavigatorTree->model()->index(1,0); // 1 is SSCAA group in NavigatorTree
    QModelIndex childIndex   = topLevelndex.child(tab,0);

    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);
}

void
nmfMainWindow::setup2dChart()
{
    QVBoxLayout* Layt   = new QVBoxLayout();
    m_ChartWidget       = new QChart();
    m_ChartView2d       = new QChartView(m_ChartWidget);
    Layt->addWidget(m_ChartView2d);
    m_UI->MSCAAOutputChartTab->setLayout(Layt);
}


void
nmfMainWindow::setup3dChart()
{
    Q3DTheme *myTheme;
    QVBoxLayout* layt;

    m_Graph3D = new Q3DSurface();

//    if (m_Graph3D != NULL) {
//        ChartView3d->show();
//        return;
//    }
//    m_Graph3D = new Q3DSurface();

    myTheme = m_Graph3D->activeTheme();
    myTheme->setLabelBorderEnabled(false);

    m_ChartView3d = QWidget::createWindowContainer(m_Graph3D);
    if (! m_Graph3D->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Error nmfMainWindow::Setup3dChart: Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return;
    }

    m_ChartView3d->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    layt = qobject_cast<QVBoxLayout*>(m_UI->MSCAAOutputChartTab->layout());
    layt->insertWidget(0,m_ChartView3d);
//    ui->MSCAAOutputChartTab->setLayout(Layt);

}



void
nmfMainWindow::setChartView(QString type)
{
    if ((m_ChartView2d == NULL) || (m_ChartView3d == NULL)) {
        return;
    }

    if (type == "2D") {
        m_ChartView3d->hide();
        m_ChartView2d->show();
    } else if (type == "3D") {
        m_ChartView2d->hide();
        m_ChartView3d->show();
    }
}


QString
nmfMainWindow::getNavigationGroup()
{
    QModelIndex currentIndex = NavigatorTree->currentIndex();
    if (currentIndex.isValid() && currentIndex.parent().isValid())
        return currentIndex.parent().data().toString();
    else
        return "";
}

void
nmfMainWindow::callback_ShowChart(const QString& mode)
{
    callback_ShowChart(mode,
                       Output_Controls_ptr->getOutputType(),
                       Output_Controls_ptr->getOutputScale());
}

void
nmfMainWindow::callback_ShowChart(const QString& type,
                                  const QString& scale)
{
    // Get the current mode
    QString mode = getNavigationGroup();
    if (! mode.isEmpty()) {
        callback_ShowChart(mode,
                           Output_Controls_ptr->getOutputType(),
                           Output_Controls_ptr->getOutputScale());
    } else {
        QMessageBox::warning(this,tr("Warning"),tr("\nPlease run a Model Estimation or Simulation."));
    }
}

void
nmfMainWindow::callback_ReloadWidgets()
{
    // Read the New Project settings
    readProjectSettings();

    // Load the New Project database
    loadDatabase();

    // Load the Species
    loadEntityList();

    // Load all the widgets with the new database tables
    loadGUIs();
}

void
nmfMainWindow::callback_SelectDefaultSpecies()
{
    EntityListLV->setCurrentIndex(EntityListLV->model()->index(0,0));
    m_UI->EntityDockWidget->setEnabled(true);
}

void
nmfMainWindow::callback_ShowChart(const QString& modeDELETE,
                                  const QString& type,
                                  const QString& scale)
{
    bool showNaturalLogOfData;
    int MinAge,MaxAge,FirstYear,LastYear,NumLengthBins;
    float MinLength,MaxLength;
    boost::numeric::ublas::matrix<double> *Abundance;
    boost::numeric::ublas::matrix<double> AbundanceScaled;
    boost::numeric::ublas::matrix<double> Data;
    std::vector<double>* SpawningStockBiomass;
    std::vector<double>* Recruitment;
    double sf = 1.0;
    std::string Species = getSpecies().toStdString();

    QString mode = getNavigationGroup();
    if (Species.empty()) {
        QMessageBox::critical(this,tr("Warning"),
             tr("\nPlease select a species."),QMessageBox::Ok);
        return;
    }
    if (mode.isEmpty() || (mode == "Setup")) {
        QMessageBox::critical(this,tr("Warning"),
             tr("\nPlease run either a SingleSpecies, MultiSpecies, or Simulation"),QMessageBox::Ok);
        return;
    }
//std::cout << "mode2: " << mode.toStdString() << std::endl;

    if (scale == "000")
        sf = 1e-3;
    else if (scale == "000 000")
        sf = 1e-6;
    else if (scale == "000 000 000")
        sf = 1e-9;

    m_databasePtr->getSpeciesData(m_logger,Species,
                                MinAge,MaxAge,FirstYear,LastYear,
                                MinLength,MaxLength,NumLengthBins);

    if (mode == "Simulation Data Input") {
//        Abundance            = Simulation_Tab2_ptr->getAbundance();
//        SpawningStockBiomass = Simulation_Tab2_ptr->getSpawningStockBiomass();
//        Recruitment          = Simulation_Tab2_ptr->getRecruitment();
    } else if (mode == "SSCAA Data Input") {
        Abundance            = SSCAA_Tab6_ptr->getAbundance();
    } else if (mode == "MSCAA Data Input") {
        Abundance            = MSCAA_Tab5_ptr->getAbundance(Species);
    }

    // RSK - Rework this logic...put Species combo box in output controls area
    if (Abundance == nullptr) {
        return;
    }

    if ((Abundance->size1() == 0) || (Abundance->size2() == 0)) {
        m_logger->logMsg(nmfConstants::Error,"No Abundance data found. Please run: " +
                       mode.split(" ")[0].toStdString());
        return;
    }

    // Update Abundance data in Output widget
    populateOutputDataTable(MinAge,FirstYear,Abundance);

    // Configure and then draw chart
    if (type == "Abundance (3D)") {
        setChartView("3D");
        QString XTitle = "Year";
        QString YTitle = "Abundance";
        QString ZTitle = "Age Groups";
        showNaturalLogOfData = Output_Controls_ptr->showNaturalLogOfData();
        Output_Controls_ptr->enableScaleWidgets(! showNaturalLogOfData);
        if (mode == "Simulation Data Input") {
//            Simulation_Tab2_ptr->showChartAbundance3d(
//                        showNaturalLogOfData,MinAge,FirstYear,
//                        m_Graph3D,XTitle,YTitle,ZTitle,scale,sf);
        } else if (mode == "SSCAA Data Input") {
            SSCAA_Tab6_ptr->showChartAbundance3d(
                        showNaturalLogOfData,MinAge,FirstYear,
                        m_Graph3D,XTitle,YTitle,ZTitle,scale,sf);
        } else if (mode == "MSCAA Data Input") {
            MSCAA_Tab5_ptr->showChartAbundance3d(
                        Species,
                        showNaturalLogOfData,MinAge,FirstYear,
                        m_Graph3D,XTitle,YTitle,ZTitle,scale,sf);
        }
    } else if (type == "Abundance vs Time") {
        setChartView("2D");
        if (! Output_Controls_ptr->thereAreSelections()) {
            Output_Controls_ptr->selectFirstAgeGroup();
        } else {
            Output_Controls_ptr->callback_OutputScaleCMB(scale);
        }
    } else if (type == "Mortality vs Time") {
        setChartView("2D");
        if (! Output_Controls_ptr->thereAreSelections()) {
            Output_Controls_ptr->selectFirstAgeGroup();
        } else {
            Output_Controls_ptr->callback_OutputScaleCMB(scale);
        }
    } else if (type == "Recruitment vs SSB") {
//        if (mode == "Simulation Data Input") {
//            int NumPointsSSB = SpawningStockBiomass->size();
//            int NumPointsRec = Recruitment->size();
//            if (NumPointsRec == NumPointsSSB) {
//                setChartView("2D");
//                nmfUtils::initialize(Data,2,NumPointsSSB);
//                for (int i = 0; i < NumPointsRec; ++i) {
//                    Data(0,i) =    (*SpawningStockBiomass)[i];
//                    Data(1,i) = sf*(*Recruitment)[i];
//                }
//                Simulation_Tab2_ptr->showChartRecruitmentVsSSB(
//                            m_ChartWidget,Species,Data,scale);
//            } else {
//                m_logger->logMsg(nmfConstants::Error,"Number of Recruitment values not the same as the number of Spawning Stock Biomass values");
//                return;
//            }
//        }
    } else if (type == "SSB vs Time") {
//        setChartView("2D");
//        if (mode == "Simulation Data Input") {
//            int NumPointsSSB = SpawningStockBiomass->size();
//            nmfUtils::initialize(Data,2,NumPointsSSB);
//            for (int i = 0; i < NumPointsSSB; ++i) {
//                Data(0,i) = FirstYear+i;
//                Data(1,i) = sf*(*SpawningStockBiomass)[i];
//            }
//            Simulation_Tab2_ptr->showChartSSBvsTime(
//                        m_ChartWidget,Species,Data,scale);
//        }
    }
}

void
nmfMainWindow::populateOutputDataTable(
        int& MinAge,
        int& FirstYear,
        boost::numeric::ublas::matrix<double> *Abundance)
{
    QStandardItem*      item;
    QStringList         ageNames;
    QStringList         yearNames;
    QStandardItemModel* smodel;

    int NumAges  = Abundance->size2();
    int NumYears = Abundance->size1();
    smodel = new QStandardItemModel(NumYears,NumAges);
    for (unsigned year=0; year<NumYears; ++year) {
        yearNames << QString::number(FirstYear+year);
        for (unsigned age=0; age<NumAges; ++age) {
            if (year == 0) {
                ageNames << "Age " + QString::number(MinAge+age);
            }
            item = new QStandardItem(QString::number((*Abundance)(year,age)));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(year, age, item);
        }
    }
    smodel->setHorizontalHeaderLabels(ageNames);
    smodel->setVerticalHeaderLabels(yearNames);

    m_UI->MSCAAOutputDataTV->setModel(smodel);
    m_UI->MSCAAOutputDataTV->resizeColumnsToContents();
}

void
nmfMainWindow::callback_AbundanceAgeGroupsSelected(
        QString mode,
        QModelIndexList ageIndexList)
{
    int value;
    int NumYears;
    int NumSelectedAges;
    double sf = 1.0;
    QString scale = Output_Controls_ptr->getOutputScale();
    boost::numeric::ublas::matrix<double> *Abundance;
    boost::numeric::ublas::matrix<double> AbundanceScaled;
    QString Species = getSpecies();
    QStringList AgeGroupNames;
    QString AgeGroupName;
    std::vector<int> AgeVec;
    int MinAge,MaxAge,FirstYear,LastYear,NumLengthBins;
    float MinLength,MaxLength;

    mode = (mode == "tbd") ? getNavigationGroup() : mode;

    setChartView("2D");

    m_databasePtr->getSpeciesData(m_logger,Species.toStdString(),
                                MinAge,MaxAge,FirstYear,LastYear,
                                MinLength,MaxLength,NumLengthBins);

    if (scale == "000")
        sf = 1e-3;
    else if (scale == "000 000")
        sf = 1e-6;
    else if (scale == "000 000 000")
        sf = 1e-9;

    for (QModelIndex index : ageIndexList) {
        AgeGroupName = index.data().toString();
        AgeGroupNames.append(AgeGroupName);
        value = AgeGroupName.split(" ")[1].toInt();
        AgeVec.push_back(value-MinAge);
    }

    if (mode == "Simulation Data Input") {
//        Abundance = Simulation_Tab2_ptr->getAbundance();
    } else if (mode == "SSCAA Data Input") {
        Abundance = SSCAA_Tab6_ptr->getAbundance();
    } else if (mode == "MSCAA Data Input") {
        Abundance = MSCAA_Tab5_ptr->getAbundance(Species.toStdString());
    }
    if (Abundance == nullptr) {
        return;
    }
    NumYears        = Abundance->size1();
    NumSelectedAges = AgeVec.size();
    nmfUtils::initialize(AbundanceScaled,NumYears,NumSelectedAges);
    for (int i=0;i<NumYears;++i) {
        for (int j=0;j<NumSelectedAges;++j) {
            AbundanceScaled(i,j) = sf*(*Abundance)(i,AgeVec[j]);
        }
    }

    if (mode == "Simulation Data Input") {
//        Simulation_Tab2_ptr->showChartAbundanceVsTime(
//                    m_ChartWidget,Species.toStdString(),
//                    FirstYear,AgeGroupNames,AbundanceScaled,scale);
    } else if (mode == "SSCAA Data Input") {
        SSCAA_Tab6_ptr->showChartAbundanceVsTime(
                    m_ChartWidget,Species.toStdString(),
                    FirstYear,AgeGroupNames,AbundanceScaled,scale);
    } else if (mode == "MSCAA Data Input") {
        MSCAA_Tab5_ptr->showChartAbundanceVsTime(
                    m_ChartWidget,Species.toStdString(),
                    FirstYear,AgeGroupNames,AbundanceScaled,scale);
    }
}

void
nmfMainWindow::callback_MortalityAgeGroupsSelected(QModelIndexList ageIndexList,
                                                   QModelIndexList mortalityTypeIndexList)
{
    bool clearChart;
    int value;
    int NumYears;
    int NumAges;
    int NumSelectedAges;
    int NaturalMortalitySize1;
    int FishingMortalitySize1;
    int PredationMortalitySize1;
    int MinAge,MaxAge,FirstYear,LastYear,NumLengthBins;
    float MinLength,MaxLength;
    boost::numeric::ublas::matrix<double> NaturalMortality;
    boost::numeric::ublas::matrix<double> FishingMortality;
    boost::numeric::ublas::matrix<double> PredationMortality;
    boost::numeric::ublas::matrix<double> NaturalMortalityWithAges;
    boost::numeric::ublas::matrix<double> FishingMortalityWithAges;
    boost::numeric::ublas::matrix<double> PredationMortalityWithAges;
    QStringList AgeGroupNames;
    QStringList MortalityTypes;
    QString AgeGroupName;
    QString MortalityType;
    QString Species = getSpecies();
    QString scale = Output_Controls_ptr->getOutputScale();
    std::vector<int> ageVec;
    QString mode = getNavigationGroup();

    setChartView("2D");

    m_databasePtr->getSpeciesData(m_logger,Species.toStdString(),
                                MinAge,MaxAge,FirstYear,LastYear,
                                MinLength,MaxLength,NumLengthBins);
    NumYears = LastYear - FirstYear + 1;
    NumAges  = MaxAge   - MinAge    + 1;

    if (mode == "Simulation Data Input") {
//        NaturalMortality   = *Simulation_Tab2_ptr->getNaturalMortality();
//        FishingMortality   = *Simulation_Tab2_ptr->getFishingMortality();
//        PredationMortality = *Simulation_Tab2_ptr->getPredationMortality();
    } else if (mode == "SSCAA Data Input") {
        if (! m_databasePtr->getMortalityData(
                    m_logger,m_ProjectSettingsConfig,Species.toStdString(),
                    NumYears,NumAges,"MortalityNatural",NaturalMortality)) {
            m_logger->logMsg(nmfConstants::Warning,"No Natural Mortality data found.");
            return;
        }
        if (! m_databasePtr->getMortalityData(
                    m_logger,m_ProjectSettingsConfig,Species.toStdString(),
                    NumYears,NumAges,"MortalityFishing",FishingMortality)) {
            m_logger->logMsg(nmfConstants::Warning,"No Fishing Mortality data found.");
            return;
        }
    }

    // See what's selected in the Mortality list view
    for (QModelIndex index : mortalityTypeIndexList) {
        MortalityType = index.data().toString();
        MortalityTypes.append(MortalityType);
    }

    // See whats selected in the Age Groups list view
    for (QModelIndex index : ageIndexList) {
        AgeGroupName = index.data().toString();
        AgeGroupNames.append(AgeGroupName);
        value = AgeGroupName.split(" ")[1].toInt();
        ageVec.push_back(value-MinAge);
    }

    clearChart = true;
    if (MortalityTypes.contains("Natural (solid line)")) {
        NaturalMortalitySize1 = NaturalMortality.size1(); // Years
        NumSelectedAges = ageVec.size();
        nmfUtils::initialize(NaturalMortalityWithAges,NaturalMortalitySize1,NumSelectedAges);
        for (int i=0;i<NaturalMortalitySize1;++i) {
            for (int j=0;j<NumSelectedAges;++j) {
                NaturalMortalityWithAges(i,j) = NaturalMortality(i,ageVec[j]);
            }
        }
        if (mode == "Simulation Data Input") {
//            Simulation_Tab2_ptr->showChartMortalityVsTime(
//                        clearChart, "SolidLine",
//                        m_ChartWidget,Species.toStdString(),
//                        FirstYear,AgeGroupNames,
//                        NaturalMortalityWithAges,scale);
        } else if (mode == "SSCAA Data Input") {
            SSCAA_Tab6_ptr->showChartMortalityVsTime(
                        clearChart, "SolidLine",
                        m_ChartWidget,Species.toStdString(),
                        FirstYear,AgeGroupNames,
                        NaturalMortalityWithAges,scale);
        }
        clearChart = false;
    }

    if (MortalityTypes.contains("Fishing (dashed line)")) {
        FishingMortalitySize1 = FishingMortality.size1(); // Years
        NumSelectedAges = ageVec.size();
        nmfUtils::initialize(FishingMortalityWithAges,FishingMortalitySize1,NumSelectedAges);
        for (int i=0;i<FishingMortalitySize1;++i) {
            for (int j=0;j<NumSelectedAges;++j) {
                FishingMortalityWithAges(i,j) = FishingMortality(i,ageVec[j]);
            }
        }
        if (mode == "Simulation Data Input") {
//            Simulation_Tab2_ptr->showChartMortalityVsTime(
//                        clearChart, "DashedLine",
//                        m_ChartWidget,Species.toStdString(),
//                        FirstYear,AgeGroupNames,
//                        FishingMortalityWithAges,scale);
        } else if (mode == "SSCAA Data Input") {
            SSCAA_Tab6_ptr->showChartMortalityVsTime(
                        clearChart, "DashedLine",
                        m_ChartWidget,Species.toStdString(),
                        FirstYear,AgeGroupNames,
                        FishingMortalityWithAges,scale);
        }
        clearChart = false;
    }

    if (MortalityTypes.contains("Predation (dotted line)")) {
        PredationMortalitySize1 = PredationMortality.size1(); // Years
        NumSelectedAges = ageVec.size();
        nmfUtils::initialize(PredationMortalityWithAges,PredationMortalitySize1,NumSelectedAges);
        for (int i=0;i<PredationMortalitySize1;++i) {
            for (int j=0;j<NumSelectedAges;++j) {
                PredationMortalityWithAges(i,j) = PredationMortality(i,ageVec[j]);
            }
        }
        if (mode == "Simulation Data Input") {
//            Simulation_Tab2_ptr->showChartMortalityVsTime(
//                        clearChart, "DottedLine",
//                        m_ChartWidget,Species.toStdString(),
//                        FirstYear,AgeGroupNames,PredationMortalityWithAges,scale);
        } else if (mode == "SSCAA Data Input") {
            SSCAA_Tab6_ptr->showChartMortalityVsTime(
                        clearChart, "DottedLine",
                        m_ChartWidget,Species.toStdString(),
                        FirstYear,AgeGroupNames,PredationMortalityWithAges,scale);
        }
        clearChart = false;
    }

}

void
nmfMainWindow::callback_LoadUserSuppliedNuOther()
{
//    Simulation_Tab2_ptr->setUseUserNuOther(Simulation_Tab1_ptr->useNuOther());
//    Simulation_Tab2_ptr->setUserNuOther(Simulation_Tab1_ptr->getNuOther());
}

void
nmfMainWindow::callback_UpdateAgeList()
{
    int MinAge,MaxAge,FirstYear,LastYear,NumLengthBins;
    float MinLength,MaxLength;
    std::string Species = getSpecies().toStdString();

    m_databasePtr->getSpeciesData(m_logger,Species,
                                MinAge,MaxAge,FirstYear,LastYear,
                                MinLength,MaxLength,NumLengthBins);
    QStringList ages;
    for (int age=MinAge; age<=MaxAge; ++age) {
        ages.append("Age " + QString::number(age));
    }
    Output_Controls_ptr->loadAgeLV(ages);

}


void
nmfMainWindow::callback_MSCAATabChanged(int tab)
{
    QModelIndex topLevelndex = NavigatorTree->model()->index(2,0); // 2 is MSCAA group in NavigatorTree
    QModelIndex childIndex   = topLevelndex.child(tab,0);

    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);

    m_UI->EntityDockWidget->setEnabled(tab==3);
}

void
nmfMainWindow::callback_SimulationTabChanged(int tab)
{
    m_UI->EntityDockWidget->setEnabled(true);

    QModelIndex topLevelndex = NavigatorTree->model()->index(3,0); // 3 is Simulation group in NavigatorTree
    QModelIndex childIndex   = topLevelndex.child(tab,0);

    NavigatorTree->blockSignals(true);
    NavigatorTree->setCurrentIndex(childIndex);
    NavigatorTree->blockSignals(false);
}

void
nmfMainWindow::callback_EntityList(QModelIndex index)
{
//  std::cout << "Clicked: " << index.data().toString().toStdString() << std::endl;

    this->setCursor(Qt::WaitCursor);

    QString mode = getNavigationGroup();

    Setup_Tab4_ptr->setSpecies(index.data().toString());
    Setup_Tab6_ptr->setSpecies(index.data().toString());
    Setup_Tab7_ptr->setSpecies(index.data().toString());

    SSCAA_Tab1_ptr->setSpecies(index.data().toString());
    SSCAA_Tab2_ptr->setSpecies(index.data().toString());
    SSCAA_Tab3_ptr->speciesChanged(index.data().toString());
    SSCAA_Tab4_ptr->speciesChanged(index.data().toString());
    SSCAA_Tab5_ptr->speciesChanged(index.data().toString());
    SSCAA_Tab6_ptr->speciesChanged(index.data().toString());

    // Necessary so when user changes the species, the correct age groups get loaded.
    callback_UpdateAgeList();

    //  MSCAA_Tab1_ptr->speciesChanged(index.data().toString());
    MSCAA_Tab4_ptr->changeSpecies(index.data().toString());
//std::cout << "mode: " << mode.toStdString() << std::endl;

    if (mode == "SSCAA Data Input")
        callback_ShowChart("SSCAA_Data_Input");
    else if (mode == "MSCAA Data Input")
        callback_ShowChart("MSCAA_Data_Input");

//    Simulation_Tab1_ptr->speciesChanged(index.data().toString());
//    Simulation_Tab2_ptr->speciesChanged(index.data().toString());

    this->setCursor(Qt::ArrowCursor);
}

void
nmfMainWindow::callback_ProjectSaved()
{
    updateWindowTitle();
    enableApplicationFeatures(true);
}

void
nmfMainWindow::menu_importDatabase()
{
    QMessageBox::StandardButton reply;

    // Go to project data page
    NavigatorTree->setCurrentIndex(NavigatorTree->model()->index(0,0));
    m_UI->SetupInputTabWidget->setCurrentIndex(1);

    // Ask if user wants to clear the Project meta data
    std::string msg  = "\nDo you want to overwrite current Project data with imported database information?";
    msg += "\n\nYes: Overwrites Project data\nNo: Clears Project data, user enters new Project data\n";
    reply = QMessageBox::question(this, tr("Import Database"), tr(msg.c_str()),
                                  QMessageBox::No|QMessageBox::Yes|QMessageBox::Cancel,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    }

    // Do the import
    this->setCursor(Qt::WaitCursor);
    QString dbName = m_databasePtr->importDatabase(this,
                                                   m_logger,
                                                   m_ProjectDir,
                                                   m_Username,
                                                   m_Password);
    if (!dbName.isEmpty()) {
        Setup_Tab2_ptr->loadWidgets();
        Setup_Tab3_ptr->loadWidgets();
        Setup_Tab4_ptr->loadWidgets();
        Setup_Tab5_ptr->loadWidgets();
        Setup_Tab6_ptr->loadWidgets();
        Setup_Tab7_ptr->loadWidgets();
        if (reply == QMessageBox::No) {
            Setup_Tab2_ptr->clearProject();
            QMessageBox::information(this, tr("Project"),
                                     tr("\nPlease fill in Project data fields before continuing."),
                                     QMessageBox::Ok);
            Setup_Tab2_ptr->enableProjectData();
        }
        Setup_Tab2_ptr->setProjectDatabase(dbName);
        Setup_Tab2_ptr->callback_Setup_Tab2_SaveProject();
    }
    this->setCursor(Qt::ArrowCursor);

}


void
nmfMainWindow::menu_exportDatabase()
{
    m_databasePtr->exportDatabase(this,
                                  m_ProjectDir,
                                  m_Username,
                                  m_Password,
                                  m_ProjectDatabase);
}

void
nmfMainWindow::menu_showTableNames()
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg = "";
    int NumTables=0;

    fields    = {"table_name"};
    queryStr  = "SELECT table_name FROM information_schema.tables WHERE ";
    queryStr += "table_schema = '" + m_ProjectDatabase + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumTables = dataMap["table_name"].size();
    if (NumTables <= 0) {
        msg = "\nNo tables found in database: " + m_ProjectDatabase;
        QMessageBox::information(this,
                                 tr("Database Tables"),
                                 tr(msg.c_str()),
                                 QMessageBox::Ok);
    } else {
        for (int i=0; i<NumTables; ++i) {
            msg += std::to_string(i+1) + ". " + dataMap["table_name"][i] + "\n";
        }
        msg = "\nTables in database: " + m_ProjectDatabase + "\n\n" + msg;
        QMessageBox::information(this,
                                 tr("Database Tables"),
                                 tr(msg.c_str()),
                                 QMessageBox::Ok);
    }

}


void
nmfMainWindow::menu_showLastADMBRun()
{
    callback_SelectDefaultSpecies();

    QString mode = getNavigationGroup();
    if (mode == "SSCAA Data Input") {
        SSCAA_Tab6_ptr->readLastReportFile("SingleSpecies");
    } else if (mode == "MSCAA Data Input") {
        MSCAA_Tab5_ptr->readLastReportFile("MultiSpecies");
    } else {
        QMessageBox::information(this,tr("Info"),"\nPlease select either the SSCAA or MSCAA Run tab.",QMessageBox::Ok);
    }

//    Output_Controls_ptr->

}


//
// The following are necessary so the user can use the main menu items
// to edit the individual table views.  I've implemented the table views
// by implementing an nmfTableView class.
//
void
nmfMainWindow::menu_copy()
{
    //QTest::keyClick(ui->SSCAADataInputTabWidget,
//    QTest::keyClick(SSCAA_Tab1_ptr->getCatchTV(),
//                    Qt::Key_C,Qt::ControlModifier);
//    QTest::keyClick(SSCAA_Tab1_ptr->getTotalTV(),
//                    Qt::Key_C,Qt::ControlModifier);

    QString retv = nmfUtilsQt::copy(qApp,findTableInFocus());
    if (! retv.isEmpty()) {
        m_logger->logMsg(nmfConstants::Error,retv.trimmed().toStdString());
        //QMessageBox::question(this,tr("Copy"),retv,QMessageBox::Ok);
    }

}

void
nmfMainWindow::menu_layoutOutput()
{
    m_UI->OutputDockWidget->setFloating(true);
    m_UI->OutputDockWidget->setGeometry(this->x()+this->width()*(2.0/3.0),
                                      this->y(),
                                      1000,350);
    m_UI->OutputDockWidget->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    m_UI->OutputDockWidget->show();
    m_UI->OutputDockWidget->raise();
}

void
nmfMainWindow::menu_layoutDefault()
{
    this->addDockWidget(Qt::LeftDockWidgetArea,  m_UI->NavigatorDockWidget);
    this->addDockWidget(Qt::LeftDockWidgetArea,  m_UI->EntityDockWidget);
    this->addDockWidget(Qt::RightDockWidgetArea, m_UI->OutputDockWidget);
    m_UI->NavigatorDockWidget->show();
    m_UI->EntityDockWidget->show();
    m_UI->OutputDockWidget->setFloating(false);
    m_UI->OutputDockWidget->show();
    m_UI->ProgressDockWidget->setFloating(false);
    m_UI->ProgressDockWidget->show();
    m_UI->LogDockWidget->setFloating(false);
    m_UI->LogDockWidget->show();
    this->addDockWidget(Qt::BottomDockWidgetArea, m_UI->ProgressDockWidget);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_UI->LogDockWidget);
    this->tabifyDockWidget(m_UI->LogDockWidget, m_UI->ProgressDockWidget);
    m_UI->LogDockWidget->raise();

    // Not currently implemented for this tool
    m_UI->ProgressDockWidget->hide();

}

void
nmfMainWindow::menu_paste()
{
    //    QTest::keyClick(ui->SSCAADataInputTabWidget,
//    QTest::keyClick(SSCAA_Tab1_ptr->getCatchTV(),
//                    Qt::Key_V,Qt::ControlModifier);
//    QTest::keyClick(SSCAA_Tab1_ptr->getTotalTV(),
//                    Qt::Key_V,Qt::ControlModifier);

    QString retv = nmfUtilsQt::paste(qApp,findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Paste"),retv,QMessageBox::Ok);
    }
}


void
nmfMainWindow::menu_clear()
{
//    QTest::keyClick(ui->SSCAADataInputTabWidget,
//                    Qt::Key_X,Qt::ControlModifier);

    QString retv = nmfUtilsQt::clear(qApp,findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Clear All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_pasteAll()
{
//    QTest::keyClick(ui->SSCAADataInputTabWidget,
//    QTest::keyClick(SSCAA_Tab1_ptr->getCatchTV(),
//                    Qt::Key_V,Qt::ShiftModifier|Qt::ControlModifier);

    QString retv = nmfUtilsQt::pasteAll(qApp,findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Paste All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_selectAll()
{
//    QTest::keyClick(ui->SSCAADataInputTabWidget,
//                    Qt::Key_A,Qt::ControlModifier);

    QString retv = nmfUtilsQt::selectAll(findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Select All"),retv,QMessageBox::Ok);
    }
}

void
nmfMainWindow::menu_deselectAll()
{
//    QTest::keyClick(ui->SSCAADataInputTabWidget,
//                    Qt::Key_A,Qt::ShiftModifier|Qt::ControlModifier);

    QString retv = nmfUtilsQt::deselectAll(findTableInFocus());
    if (! retv.isEmpty()) {
        QMessageBox::question(this,tr("Deselect All"),retv,QMessageBox::Ok);
    }
}
