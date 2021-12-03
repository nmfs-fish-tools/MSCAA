
//#include "nmfUtilsQt.h"
//#include "nmfMSCAATab05.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"

#include "nmfMSCAATab05.h"
#include "nmfMSCAATableIO.h"


nmfMSCAA_Tab5::nmfMSCAA_Tab5(QTabWidget  *tabs,
                             nmfLogger   *logger,
                             nmfDatabase *databasePtr,
                             std::string &projectDir,
                             std::string &projectName)
{
    QUiLoader loader;

    MSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_ProjectDir  = projectDir;
    m_ProjectName = projectName;
    m_Abundance.clear();
    m_Trophic     = -1;

    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfMSCAA_Tab5::nmfMSCAA_Tab5");

    // Load ui as a widget from disk
    QFile file(":/forms/MSCAA/MSCAA_Tab05.ui");
    file.open(QFile::ReadOnly);
    MSCAA_Tab5_Widget = loader.load(&file,MSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    MSCAA_Tabs->addTab(MSCAA_Tab5_Widget, tr("5. Run"));

    MSCAA_Tab5_ProportionsTV = new QTableView(MSCAA_Tabs);
    MSCAA_Tab5_PhaseTV       = new QTableView(MSCAA_Tabs);
    MSCAA_Tab5_PrevPB        = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab5_PrevPB");
    MSCAA_Tab5_LoadPB        = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab5_LoadPB");
    MSCAA_Tab5_SavePB        = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab5_SavePB");
    MSCAA_Tab5_RunPB         = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab5_RunPB");
    MSCAA_Tab5_OptFilesPB    = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab5_OptFilesPB");
    MSCAA_Tab5_DebugPB       = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab5_DebugPB");
    MSCAA_Tab5_ProportionsLT = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab5_ProportionsLT");
    MSCAA_Tab5_PhaseLT       = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab5_PhaseLT");
    MSCAA_Tab5_ProportionsLT->addWidget(MSCAA_Tab5_ProportionsTV);
    MSCAA_Tab5_PhaseLT->addWidget(MSCAA_Tab5_PhaseTV);
    MSCAA_Tab5_SummaryTE     = MSCAA_Tabs->findChild<QTextEdit *>("MSCAA_Tab5_SummaryTE");
    MSCAA_Tab5_LogNormLE     = MSCAA_Tabs->findChild<QLineEdit *>("MSCAA_Tab5_LogNormLE");
    MSCAA_Tab5_MultiResidLE  = MSCAA_Tabs->findChild<QLineEdit *>("MSCAA_Tab5_MultiResidLE");
    MSCAA_Tab5_DebugLE       = MSCAA_Tabs->findChild<QLineEdit *>("MSCAA_Tab5_DebugLE");

    QFont noBoldFont;
    noBoldFont.setBold(false);
    MSCAA_Tab5_ProportionsTV->setFont(noBoldFont);
    MSCAA_Tab5_PhaseTV->setFont(noBoldFont);
    MSCAA_Tab5_ProportionsTV->setFixedHeight(75);
    MSCAA_Tab5_PhaseTV->setFixedHeight(75);

    connect(MSCAA_Tab5_PrevPB,     SIGNAL(clicked()),
            this,                  SLOT(callback_PrevPB()));
    connect(MSCAA_Tab5_LoadPB,     SIGNAL(clicked()),
            this,                  SLOT(callback_LoadPB()));
    connect(MSCAA_Tab5_SavePB,     SIGNAL(clicked()),
            this,                  SLOT(callback_SavePB()));
    connect(MSCAA_Tab5_RunPB,      SIGNAL(clicked()),
            this,                  SLOT(callback_RunPB()));
    connect(MSCAA_Tab5_OptFilesPB, SIGNAL(clicked()),
            this,                  SLOT(callback_OptFilesPB()));
    connect(MSCAA_Tab5_DebugPB,    SIGNAL(clicked()),
            this,                  SLOT(callback_DebugPB()));

    MSCAA_Tab5_PrevPB->setText("\u25C1--");

} // end constructor


nmfMSCAA_Tab5::~nmfMSCAA_Tab5()
{

}

QTableView*
nmfMSCAA_Tab5::getTableFoodHabitsProportions()
{
    return MSCAA_Tab5_ProportionsTV;
}


QTableView*
nmfMSCAA_Tab5::getTablePhase()
{
    return MSCAA_Tab5_PhaseTV;
}


void
nmfMSCAA_Tab5::callback_PrevPB()
{
    int prevPage = MSCAA_Tabs->currentIndex()-1;
    MSCAA_Tabs->setCurrentIndex(prevPage);
}


void
nmfMSCAA_Tab5::callback_LoadPB()
{
    loadWidgets();
}


void
nmfMSCAA_Tab5::callback_SavePB()
{
    QString msg = "\n";

    if (saveSystemData()) {
        msg += "System table updated.\n";
    }
    if (saveSpeciesData()) {
        msg += "Species table updated.\n";
    }

    QMessageBox::information(MSCAA_Tabs, "Table(s) Updated", msg, QMessageBox::Ok);
}

bool
nmfMSCAA_Tab5::saveSpeciesData()
{
    std::string saveCmd;
    std::string errorMsg;
    std::vector<std::string> SpeciesList;
    QStandardItemModel *smodelFH  = qobject_cast<QStandardItemModel*>(MSCAA_Tab5_ProportionsTV->model());
    QStandardItemModel *smodelRho = qobject_cast<QStandardItemModel*>(MSCAA_Tab5_PhaseTV->model());
    QModelIndex indexFH;
    QModelIndex indexRho;
    std::string FHstr;
    std::string Rhostr;

    m_databasePtr->getSpecies(m_logger, SpeciesList);

    for (unsigned long col=0; col<SpeciesList.size(); ++col) {
        indexFH  = smodelFH->index(0,col);
        indexRho = smodelRho->index(0,col);
        FHstr    = smodelFH->data(indexFH).toString().toStdString();
        Rhostr   = smodelRho->data(indexRho).toString().toStdString();

        saveCmd  = "UPDATE " + nmfConstantsMSCAA::TableSpecies +
                   " SET Rhoph = " + Rhostr +
                   ", FHwt = " + FHstr +
                   " WHERE SpeName = '" + SpeciesList[col] + "'";

        // Save the new data
        errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
        if (nmfUtilsQt::isAnError(errorMsg)) {
            m_logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveSystemData: Write table error: " + errorMsg);
            m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
            QMessageBox::warning(MSCAA_Tabs, "Error",
                                 "\nError in Save command.  Check that all cells are populated.\n",
                                 QMessageBox::Ok);
            return false;
        }
    }
    return true;
}

bool
nmfMSCAA_Tab5::saveSystemData()
{
    bool SystemDataExists;
    std::string saveCmd;
    std::string errorMsg;

    SystemDataExists = nmfMSCAAUtils::systemTableExists(
                         m_databasePtr,m_logger,m_ProjectSettingsConfig);

    if (SystemDataExists) {
        saveCmd  = "UPDATE " + nmfConstantsMSCAA::TableModels +
                   " SET LogNorm = " +
                    MSCAA_Tab5_LogNormLE->text().toStdString() +
                   ", MultiResid = "  + MSCAA_Tab5_MultiResidLE->text().toStdString() +
                   " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    } else {
        m_logger->logMsg(nmfConstants::Error,"Please save data from MSCAA Data Input - Tab 1 prior to saving here.");
        return false;
    }
    // Save the new data
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveSystemData: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(MSCAA_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }
    return true;
}

void
nmfMSCAA_Tab5::callback_RunPB(int trophic)
{
    m_Trophic = trophic;
    callback_RunPB();
}

void
nmfMSCAA_Tab5::callback_DebugPB()
{
    VerboseDebugLevelDialog* dlg = new VerboseDebugLevelDialog(MSCAA_Tabs,MSCAA_Tab5_DebugLE);
    dlg->show();
}

void
nmfMSCAA_Tab5::callback_OptFilesPB()
{
    // Open file dialog and allow user to choose an ADMB file to view.
    QString fileName = QFileDialog::getOpenFileName(MSCAA_Tabs,
        tr("Open ADMB File"),
        nmfMSCAAUtils::getADMBDir(
            m_databasePtr,m_logger,m_ProjectDir,m_ProjectName,
            m_ProjectSettingsConfig,"MultiSpecies",MSCAA_Tab5_SummaryTE),
        tr(""));
    if (fileName.isEmpty())
        return;

    // Open file viewer with font resize button
    nmfFileViewer *dlg = new nmfFileViewer(MSCAA_Tabs,fileName.toLatin1());
    dlg->show();
}


void
nmfMSCAA_Tab5::readLastReportFile(const QString& type)
{
    QString loadOK = nmfMSCAAUtils::loadLastADMB(
                m_databasePtr,m_logger,
                m_ProjectDir,m_ProjectName,m_ProjectSettingsConfig,
                type,MSCAA_Tabs, MSCAA_Tab5_SummaryTE,m_Abundance);
    if (loadOK.isEmpty()) {
        MSCAA_Tab5_SummaryTE->append("Loaded last saved data.");

        // Select first Species in list
        emit SelectDefaultSpecies();

        // Emit signal that will update the Output charts
        emit UpdateOutputCharts("MSCAA Data Input");
    }
}

int
nmfMSCAA_Tab5::getVerboseDebugLevel()
{
    return MSCAA_Tab5_DebugLE->text().toInt();
}

void
nmfMSCAA_Tab5::callback_RunPB()
{
    int Trophic = 1;
    int VerboseDebugLevel = getVerboseDebugLevel();

    if (! nmfMSCAAUtils::removePreviousRunsData(MSCAA_Tabs,
                m_databasePtr,m_logger,m_ProjectDir,m_ProjectName,
                m_ProjectSettingsConfig,"MultiSpecies",MSCAA_Tab5_SummaryTE)) {
        return;
    }

    // Clear chart and data
    emit ClearOutput();

    // ///////////////////
    //    Run ADMB      //
    // ///////////////////
    bool runOK = nmfMSCAAUtils::runOptimizerADMB(
                m_databasePtr,
                m_logger,
                m_ProjectDir,
                m_ProjectName,
                m_ProjectSettingsConfig,
                Trophic,
                VerboseDebugLevel,
                "MultiSpecies",
                MSCAA_Tabs,
                MSCAA_Tab5_SummaryTE,
                m_Abundance);

    if (runOK) {
        // Select first Species in list
        emit SelectDefaultSpecies();

        // Emit signal that will update the Output charts
        emit UpdateOutputCharts("MSCAA Data Input");
    }
}

boost::numeric::ublas::matrix<double>*
nmfMSCAA_Tab5::getAbundance(std::string Species)
{
    if (m_Abundance.size() == 0) {
        return nullptr;
    }
    return &m_Abundance[Species];
}

bool
nmfMSCAA_Tab5::loadSystemData()
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    fields     = {"SystemName","LogNorm","MultiResid"};
    queryStr   = "SELECT SystemName,LogNorm,MultiResid FROM " + nmfConstantsMSCAA::TableModels +
                 " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        msg  = "nmfMSCAA_Tab5::loadSystemData: No System data found for SystemName = '";
        msg += m_ProjectSettingsConfig + "'";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    MSCAA_Tab5_LogNormLE->setText(QString::fromStdString(dataMap["LogNorm"][0]));
    MSCAA_Tab5_MultiResidLE->setText(QString::fromStdString(dataMap["MultiResid"][0]));

    return true;
}

bool
nmfMSCAA_Tab5::loadProportions()
{
    int NumSpecies;
    int NumRecords;
    std::vector<std::string> Species;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QModelIndex index;
    std::string msg;
    QStringList SpeciesLabels;
    QStringList ProportionLabel = {"Proportions"};
    QStandardItemModel *smodel;

    m_databasePtr->getSpecies(m_logger, Species);
    NumSpecies = Species.size();
    smodel = new QStandardItemModel(1,NumSpecies);

    fields     = {"SpeName","FHwt"};
    queryStr   = "SELECT SpeName,FHwt FROM " + nmfConstantsMSCAA::TableSpecies +
                 " ORDER BY SpeName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        msg  = "nmfMSCAA_Tab5::loadProportions: No Species data found.";
        msg += m_ProjectSettingsConfig + "'";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int col = 0; col < NumSpecies; ++col) {
        SpeciesLabels.append(QString::fromStdString(Species[col]));
        index = smodel->index(0,col);
        smodel->setData(index,QString::fromStdString(dataMap["FHwt"][col]));
    }

    smodel->setHorizontalHeaderLabels(SpeciesLabels);
    smodel->setVerticalHeaderLabels(ProportionLabel);
    MSCAA_Tab5_ProportionsTV->setModel(smodel);
    MSCAA_Tab5_ProportionsTV->resizeColumnsToContents();

    return true;
}

bool
nmfMSCAA_Tab5::loadPhases()
{
    int NumNRhos;
    int NumRecords;
    std::vector<std::string> Species;
    QStringList RhoLabels;
    QStringList PhaseLabel = {"Phase"};
    QString label;
    QStandardItemModel *smodel;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    QModelIndex index;

    fields     = {"SpeName","Rhoph"};
    queryStr   = "SELECT SpeName,Rhoph FROM " + nmfConstantsMSCAA::TableSpecies +
                 " ORDER BY SpeName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        msg  = "nmfMSCAA_Tab5::loadPhases: No Species data found.";
        msg += m_ProjectSettingsConfig + "'";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    NumNRhos = NumRecords;

    smodel = new QStandardItemModel(1,NumNRhos);
    for (int col = 0; col < NumNRhos; ++col) {
        label = "Rho " + QString::number(col);
        RhoLabels.append(label);
        index = smodel->index(0,col);
        smodel->setData(index,QString::fromStdString(dataMap["Rhoph"][col]));
    }

    smodel->setHorizontalHeaderLabels(RhoLabels);
    smodel->setVerticalHeaderLabels(PhaseLabel);
    MSCAA_Tab5_PhaseTV->setModel(smodel);
    MSCAA_Tab5_PhaseTV->resizeColumnsToContents();

    return true;
}


void
nmfMSCAA_Tab5::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfMSCAA_Tab5::clearWidgets()
{
    nmfUtilsQt::clearTableView({MSCAA_Tab5_ProportionsTV,
                                MSCAA_Tab5_PhaseTV});
}


bool
nmfMSCAA_Tab5::loadWidgets()
{
std::cout << "nmfMSCAA_Tab5::loadWidgets()" << std::endl;

    readSettings();

    clearWidgets();

    loadProportions();
    loadPhases();
    loadSystemData();

    return true;
}


void
nmfMSCAA_Tab5::showChartAbundance3d(
        const std::string& species,
        const bool         showLogData,
        const int&         minAge,
        const int&         firstYear,
        Q3DSurface*        graph3D,
        const QString&     xTitle,
        const QString&     yTitle,
        const QString&     zTitle,
        const QString&     scale,
        const double&      sf)
{
    QString ScaleStr="";
    QString YTitleWithSuffix;
    boost::numeric::ublas::matrix<double> RowValues;
    boost::numeric::ublas::matrix<double> ColumnValues;
    boost::numeric::ublas::matrix<double> AbundanceScaled;
    int NumRows = m_Abundance[species].size1();
    int NumCols = m_Abundance[species].size2();
    QString xLabelFormat = "%d";
    QString zLabelFormat = "Age %d";

    nmfUtils::initialize(AbundanceScaled,NumRows,NumCols);
    nmfUtils::initialize(RowValues,      NumRows,NumCols);
    nmfUtils::initialize(ColumnValues,   NumRows,NumCols);
    for (int i = 0; i < NumRows; ++i) {
        for (int j = 0; j < NumCols; ++j) {
            RowValues(i,j)    = firstYear + i;
            ColumnValues(i,j) = minAge + j;
            if (showLogData) {
                AbundanceScaled(i,j) = sf * std::log(m_Abundance[species](i,j));
            } else {
                AbundanceScaled(i,j) = sf * m_Abundance[species](i,j);
            }
        }
    }
    ScaleStr = (scale == "Default") ? "" : scale + " ";
    YTitleWithSuffix = (showLogData) ?
                yTitle + QString(" (ln (Num Fish) )") :
                yTitle + QString(" (") + ScaleStr + "Fish)";

    // Draw 3d surface with abundance data
    nmfChartSurface surface(graph3D, xTitle, YTitleWithSuffix, zTitle,
                            xLabelFormat, zLabelFormat,
                            nmfConstants::DontReverseAxis,
                            nmfConstants::ReverseAxis,
                            RowValues, ColumnValues, AbundanceScaled,
                            nmfConstants::ShowShadow,
                            nmfConstants::DontManuallySetYMinMax,0,0);

}


void
nmfMSCAA_Tab5::showChartAbundanceVsTime(
        QChart* chartWidget,
        const std::string& species,
        const int& startYear,
        const QStringList& ageGroupNames,
        const boost::numeric::ublas::matrix<double>& data,
        const QString& scale)
{
    nmfChartLine lineChart;
    std::string ChartType = "Line";
    std::string LineStyle = "SolidLine";
    bool xAxisIsInteger = true;
    QStringList RowLabelsForBars;
    QStringList HoverLabels = {};
    QStringList ColumnLabelsForLegend = ageGroupNames;
    std::string MainTitle = "Abundance vs Time: "+species;
    std::string XLabel    = "Time (Years)";
    std::string YLabel    = "Abundance";
    std::vector<bool> GridLines = {true,true};
    int Theme = 0;
    double YMinSliderVal = 0;
    double YMaxVal  = nmfConstants::NoValueDouble;
    QColor dashedLineColor = Qt::black;
    std::string ScaleStr = (scale == "Default") ? "" : scale.toStdString()+" ";

    YLabel += " (" + ScaleStr + "Fish)";

    chartWidget->removeAllSeries();
    lineChart.populateChart(chartWidget,
                            ChartType,
                            LineStyle,
                            nmfConstantsMSCAA::ShowFirstPoint,
                            nmfConstants::DontShowLegend,
                            startYear,
                            xAxisIsInteger,
                            YMinSliderVal,YMaxVal,
                            nmfConstantsMSCAA::DontLeaveGapsWhereNegative,
                            data,
                            RowLabelsForBars,
                            ColumnLabelsForLegend,
                            HoverLabels,
                            MainTitle,
                            XLabel,
                            YLabel,
                            GridLines,
                            Theme,
                            dashedLineColor,
                            "",
                            1.0);
}
