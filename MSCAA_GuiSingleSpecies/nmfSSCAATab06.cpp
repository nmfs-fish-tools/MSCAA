
//#include "nmfUtilsQt.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"
//#include "nmfConstantsMSCAA.h"

#include "nmfSSCAATab06.h"
#include "nmfMSCAATableIO.h"


nmfSSCAA_Tab6::nmfSSCAA_Tab6(QTabWidget  *tabs,
                             nmfLogger   *theLogger,
                             nmfDatabase *theDatabasePtr,
                             std::string &theProjectDir,
                             std::string &ProjectName)
{
    QString msg;
    QUiLoader loader;

    m_logger      = theLogger;
    m_databasePtr = theDatabasePtr;
    m_ProjectDir  = theProjectDir;
    m_ProjectName = ProjectName;

    SSCAA_Tabs = tabs;
    ReadSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfSSCAA_Tab6::nmfSSCAA_Tab6");

    // Load ui as a widget from disk
    QFile file(":/forms/SSCAA/SSCAA_Tab06.ui");
    file.open(QFile::ReadOnly);
    SSCAA_Tab6_Widget = loader.load(&file,SSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    SSCAA_Tabs->addTab(SSCAA_Tab6_Widget, tr("6. Run"));
    SSCAA_Tab6_EstimationsTV = new QTableView(SSCAA_Tabs);
    SSCAA_Tab6_WeightingsTV  = new QTableView(SSCAA_Tabs);
    SSCAA_Tab6_PrevPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab6_PrevPB");
    SSCAA_Tab6_LoadPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab6_LoadPB");
    SSCAA_Tab6_SavePB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab6_SavePB");
    SSCAA_Tab6_RunPB         = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab6_RunPB");
    SSCAA_Tab6_DebugPB       = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab6_DebugPB");
    SSCAA_Tab6_OptFilesPB    = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab6_OptFilesPB");
    SSCAA_Tab6_EstimationLT  = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab6_EstimationLT");
    SSCAA_Tab6_EstimationLT->addWidget(SSCAA_Tab6_EstimationsTV);
    SSCAA_Tab6_WeightingsLT  = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab6_WeightingsLT");
    SSCAA_Tab6_WeightingsLT->addWidget(SSCAA_Tab6_WeightingsTV);
    SSCAA_Tab6_SpeciesLBL    = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab6_SpeciesLBL");
    SSCAA_Tab6_FleetsCMB     = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab6_FleetsCMB");
    SSCAA_Tab6_SummaryTE     = SSCAA_Tabs->findChild<QTextEdit   *>("SSCAA_Tab6_SummaryTE");
    SSCAA_Tab6_DebugLE       = SSCAA_Tabs->findChild<QLineEdit   *>("SSCAA_Tab6_DebugLE");


    msg  = "<strong>The columns are defined as follows:</strong><br><br>";
    msg += "aAge1 estimation phase - ...<br>";
    msg += "aFt estimation phase - ...<br>";
    msg += "dAge1 estimation phase - ...<br>";
    msg += "dFtph estimation phase - ...<br>";
    msg += "ficph - Survey selectivity, FICsel, estimation phase<br>";
    msg += "fishph - Fishery selectivity, agesel, estimation phase<br>";
    msg += "Yr1ph estimation phase - ...<br>";
//  msg += "Rhoph - Phase in which each rho is estimated<br>";
    SSCAA_Tab6_EstimationsTV->setWhatsThis(msg);

    msg  = "<strong>The columns are defined as follows:</strong><br><br>";
    msg += "TCwt - Total annual commercial catch in weight<br>";
    msg += "CPwt - Commercial catch proportions at age<br>";
    msg += "Bwt - Weight for Biomass Penalty Term, Bpen<br>";
    msg += "Ywt - Weight for Yr1 Penalty Term, Ypen)<br>";
    msg += "Rwt - Weight for Recruitment Penalty Term, Rpen<br>";
//  msg += "FHwt - Food Habits proportions by weight<br>";
    msg += "Bthres - Biomass threshold used in the penalty function to avoid B equal to 0 (which would cause M2 calc to crash)<br>";
    msg += "Rthres- Threshold for the coefficient of variation of recruitment<br>";
    SSCAA_Tab6_WeightingsTV->setWhatsThis(msg);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    SSCAA_Tab6_EstimationsTV->setFont(noBoldFont);
    SSCAA_Tab6_WeightingsTV->setFont(noBoldFont);

    connect(SSCAA_Tab6_PrevPB, SIGNAL(clicked()),
            this,              SLOT(callback_PrevPB()));
    connect(SSCAA_Tab6_LoadPB, SIGNAL(clicked()),
            this,              SLOT(callback_LoadPB()));
    connect(SSCAA_Tab6_SavePB, SIGNAL(clicked()),
            this,              SLOT(callback_SavePB()));
    connect(SSCAA_Tab6_RunPB,  SIGNAL(clicked()),
            this,              SLOT(callback_RunPB()));
    connect(SSCAA_Tab6_OptFilesPB, SIGNAL(clicked()),
            this,                  SLOT(callback_OptFilesPB()));
    connect(SSCAA_Tab6_DebugPB,    SIGNAL(clicked()),
            this,                  SLOT(callback_DebugPB()));

    SSCAA_Tab6_PrevPB->setText("\u25C1--");

    QString Species = getSpecies();
    if (! Species.isEmpty()) {
        nmfMSCAAUtils::loadFleetComboBox(m_databasePtr,m_logger,m_ProjectSettingsConfig,
                                         Species,SSCAA_Tab6_FleetsCMB);
    }

} // end constructor


nmfSSCAA_Tab6::~nmfSSCAA_Tab6()
{

}

QTableView*
nmfSSCAA_Tab6::getTable1()
{
    return SSCAA_Tab6_EstimationsTV;
}

QTableView*
nmfSSCAA_Tab6::getTable2()
{
    return SSCAA_Tab6_WeightingsTV;
}

void
nmfSSCAA_Tab6::callback_PrevPB()
{
    int prevPage = SSCAA_Tabs->currentIndex()-1;
    SSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfSSCAA_Tab6::callback_NextPB()
{
    int nextPage = SSCAA_Tabs->currentIndex()+1;
    SSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfSSCAA_Tab6::callback_LoadPB()
{
    loadWidgets();
}

bool
nmfSSCAA_Tab6::callback_SavePB()
{
    bool okEst;
    bool okWt;
    QString msg;

    okEst = saveToSpeciesTable(nmfConstantsMSCAA::ColLabelsEstimations,SSCAA_Tab6_EstimationsTV);
    okWt  = saveToSpeciesTable(nmfConstantsMSCAA::ColLabelsWeightings, SSCAA_Tab6_WeightingsTV);

    if (okEst || okWt) {
        msg = "\nSpecies table has been successfully updated.\n";
        QMessageBox::information(SSCAA_Tabs, "Table(s) Updated", msg, QMessageBox::Ok);
    }

    return (okEst || okWt);
}

void
nmfSSCAA_Tab6::speciesChanged(QString species)
{
    SSCAA_Tab6_SpeciesLBL->setText(species);
    nmfMSCAAUtils::loadFleetComboBox(m_databasePtr,m_logger,m_ProjectSettingsConfig,
                                     species,SSCAA_Tab6_FleetsCMB);
}

QString
nmfSSCAA_Tab6::getSpecies()
{
    return SSCAA_Tab6_SpeciesLBL->text();
}

QString
nmfSSCAA_Tab6::getFleet()
{
    return SSCAA_Tab6_FleetsCMB->currentText();
}

void
nmfSSCAA_Tab6::clearSummaryTE()
{
    SSCAA_Tab6_SummaryTE->clear();
}

void
nmfSSCAA_Tab6::appendSummaryTE(QString& msg)
{
    SSCAA_Tab6_SummaryTE->append(msg);
    SSCAA_Tab6_SummaryTE->update();
}

void
nmfSSCAA_Tab6::readLastReportFile(const QString& type)
{
    QString loadStr = nmfMSCAAUtils::loadLastADMB(
                m_databasePtr,m_logger,
                m_ProjectDir,m_ProjectName,m_ProjectSettingsConfig,
                type,SSCAA_Tabs,SSCAA_Tab6_SummaryTE,m_AbundanceAll);
    if (loadStr.isEmpty()) {
        SSCAA_Tab6_SummaryTE->append("Loaded last saved data.");

        // Emit signal that will update the Output charts
        emit UpdateOutputCharts("SSCAA Data Input");
    }
}

int
nmfSSCAA_Tab6::getVerboseDebugLevel()
{
    return SSCAA_Tab6_DebugLE->text().toInt();
}

void
nmfSSCAA_Tab6::callback_RunPB()
{
    int Trophic = 0;
    int VerboseDebugLevel = getVerboseDebugLevel();
    QMessageBox::StandardButton reply;
    QStringList filters;

    QString msg = "Please select a Species prior to running in Single Species mode";

    if (getSpecies().isEmpty()) {
        m_logger->logMsg(nmfConstants::Error,msg.toStdString());
        QMessageBox::warning(SSCAA_Tabs, "Error","\n"+msg+"\n",
                             QMessageBox::Ok);
        return;
    }

    // Remove previous run's data files
    QString admbDir = nmfMSCAAUtils::getADMBDir(
       m_databasePtr,m_logger,m_ProjectDir,m_ProjectName,
       m_ProjectSettingsConfig,"SingleSpecies",SSCAA_Tab6_SummaryTE);
    msg = "\nOK to remove previous run's output data files? ";
    msg += "This will remove all files in directory: \n\n";
    msg += admbDir + "\n";
    reply = QMessageBox::question(SSCAA_Tabs, tr("Remove Previous Run's Files"),
                                  msg,
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return;
    }
    QDir dir(admbDir);
    filters << "MSCAA*" << "*.cxx";
    dir.setNameFilters(filters);
    for(const QString & filename: dir.entryList()){
        dir.remove(filename);
        msg = "Removing: " + filename;
        m_logger->logMsg(nmfConstants::Normal,msg.toStdString());
    }

    // Clear chart and data
    emit ClearOutput();

    // ///////////////////
    //                  //
    //    Run ADMB      //
    //                  //
    // ///////////////////
    bool runOK = nmfMSCAAUtils::runOptimizerADMB(
                m_databasePtr,
                m_logger,
                m_ProjectDir,
                m_ProjectName,
                m_ProjectSettingsConfig,
                Trophic,
                VerboseDebugLevel,
                "SingleSpecies",
                SSCAA_Tabs,
                SSCAA_Tab6_SummaryTE,
                m_AbundanceAll);

    if (runOK) {
        // Emit signal that will update the Output charts
        emit UpdateOutputCharts("SSCAA Data Input");
    }
}

void
nmfSSCAA_Tab6::callback_DebugPB()
{
    VerboseDebugLevelDialog* dlg = new VerboseDebugLevelDialog(SSCAA_Tabs,SSCAA_Tab6_DebugLE);
    dlg->show();
}

void
nmfSSCAA_Tab6::callback_OptFilesPB()
{
    // Open file dialog and allow user to choose an ADMB file to view.
    QString fileName = QFileDialog::getOpenFileName(SSCAA_Tabs,
        tr("Open ADMB File"),
        nmfMSCAAUtils::getADMBDir(
           m_databasePtr,m_logger,m_ProjectDir,m_ProjectName,
           m_ProjectSettingsConfig,"SingleSpecies",SSCAA_Tab6_SummaryTE),
        tr(""));
    if (fileName.isEmpty())
        return;

    // Open file viewer with font resize button
    nmfFileViewer *dlg = new nmfFileViewer(SSCAA_Tabs,fileName.toLatin1());
    dlg->show();
}


boost::numeric::ublas::matrix<double>*
nmfSSCAA_Tab6::getAbundance()
{
    return &m_AbundanceAll[getSpecies().toStdString()];
}


void
nmfSSCAA_Tab6::showChartAbundance3d(const bool     showLogData,
                                    const int&     MinAge,
                                    const int&     FirstYear,
                                    Q3DSurface*    Graph3D,
                                    const QString& XTitle,
                                    const QString& YTitle,
                                    const QString& ZTitle,
                                    const QString& Scale,
                                    const double&  sf)
{
    QString ScaleStr="";
    QString YTitleWithSuffix;
    boost::numeric::ublas::matrix<double> RowValues;
    boost::numeric::ublas::matrix<double> ColumnValues;
    boost::numeric::ublas::matrix<double> AbundanceScaled;
    std::string Species = getSpecies().toStdString();
    int NumYears = int(m_AbundanceAll[Species].size1());
    int NumAges  = int(m_AbundanceAll[Species].size2());
    QString xLabelFormat = "%d";
    QString zLabelFormat = "Age %d";
    double val = 0;

    nmfUtils::initialize(AbundanceScaled,NumYears,NumAges);
    nmfUtils::initialize(RowValues,      NumYears,NumAges);
    nmfUtils::initialize(ColumnValues,   NumYears,NumAges);
    for (unsigned i = 0; i < unsigned(NumYears); ++i) {
        for (unsigned j = 0; j < unsigned(NumAges); ++j) {
            RowValues(i,j)    = FirstYear + int(i);
            ColumnValues(i,j) = MinAge + int(j);
            if (showLogData) {
                val = sf * std::log(m_AbundanceAll[Species](i,j));
                AbundanceScaled(i,j) = (val <= 0) ? 0 : val;
            } else {
                AbundanceScaled(i,j) = sf * m_AbundanceAll[Species](i,j);
            }
        }
    }
    ScaleStr = (Scale == "Default") ? "" : Scale + " ";
    YTitleWithSuffix = (showLogData) ?
                YTitle + QString(" (ln (Num Fish) )") :
                YTitle + QString(" (") + ScaleStr + "Fish)";

    // Draw 3d surface with abundance data
    nmfChartSurface surface(Graph3D, XTitle, YTitleWithSuffix, ZTitle,
                            xLabelFormat, zLabelFormat,
                            nmfConstants::DontReverseAxis,
                            nmfConstants::ReverseAxis,
                            RowValues, ColumnValues, AbundanceScaled,
                            nmfConstants::ShowShadow);
}

void
nmfSSCAA_Tab6::showChartAbundanceVsTime(
        QChart* ChartWidget,
        const std::string& Species,
        const int& StartYear,
        const QStringList& AgeGroupNames,
        const boost::numeric::ublas::matrix<double>& Data,
        const QString& Scale)
{
    nmfChartLine lineChart;
    std::string ChartType = "Line";
    std::string LineStyle = "SolidLine";
    bool xAxisIsInteger = true;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend = AgeGroupNames;
    std::string MainTitle = "Abundance vs Time: "+Species;
    std::string XLabel    = "Time (Years)";
    std::string YLabel    = "Abundance";
    std::vector<bool> GridLines = {true,true};
    int Theme = 0;
    double YMinSliderVal = 0;
    QColor dashedLineColor = Qt::black;
    QList<QColor> LineColors = {Qt::blue,
                                Qt::red,
                                Qt::darkGreen,
                                Qt::black,
                                Qt::gray};
    std::string ScaleStr = (Scale == "Default") ? "" : Scale.toStdString()+" ";
    YLabel += " (" + ScaleStr + "Fish)";

    ChartWidget->removeAllSeries();
    lineChart.populateChart(ChartWidget,
                            ChartType,
                            LineStyle,
                            nmfConstantsMSCAA::ShowFirstPoint,
                            StartYear,
                            xAxisIsInteger,
                            YMinSliderVal,
                            Data,
                            RowLabelsForBars,
                            ColumnLabelsForLegend,
                            MainTitle,
                            XLabel,
                            YLabel,
                            GridLines,
                            Theme,
                            dashedLineColor,
                            "",//LineColors,
                            1.0);
}

void
nmfSSCAA_Tab6::showChartMortalityVsTime(
        bool ClearChart,
        std::string LineStyle,
        QChart* ChartWidget,
        const std::string& Species,
        const int& StartYear,
        const QStringList& AgeGroupNames,
        const boost::numeric::ublas::matrix<double>& Data,
        const QString& Scale)
{
    nmfChartLine lineChart;
    std::string ChartType = "Line";
    bool xAxisIsInteger = true;
    QStringList RowLabelsForBars;
    QStringList ColumnLabelsForLegend = AgeGroupNames;
    std::string MainTitle = "Mortality vs Time: "+Species;
    std::string XLabel    = "Time (Years)";
    std::string YLabel    = "Mortality";
    std::vector<bool> GridLines = {true,true};
    int Theme = 0;
    double YMinSliderVal = 0;
    QColor dashedLineColor = QColor(0,0,1); // Qt::black;
    QList<QColor> LineColors = {Qt::blue,
                                Qt::red,
                                Qt::darkGreen,
                                Qt::black,
                                Qt::gray};
//    std::string ScaleStr = (Scale == "Default") ? "" : Scale.toStdString()+" ";
//    YLabel += " (" + ScaleStr + "Fish)";

    if (ClearChart) {
        ChartWidget->removeAllSeries();
    }
    lineChart.populateChart(ChartWidget,
                            ChartType,
                            LineStyle,
                            nmfConstantsMSCAA::ShowFirstPoint,
                            StartYear,
                            xAxisIsInteger,
                            YMinSliderVal,
                            Data,
                            RowLabelsForBars,
                            ColumnLabelsForLegend,
                            MainTitle,
                            XLabel,
                            YLabel,
                            GridLines,
                            Theme,
                            dashedLineColor,
                            "",//LineColors,
                            1.0);
}

/*
bool
nmfSSCAA_Tab6::runADMB(const QString& tplFile,
                       const QString& buildOutputFile,
                       const QString& runOutputFile)
{
    std::string cmd;
    QFileInfo fileInfo(tplFile);
    QString fileBase = fileInfo.baseName();
    QString filePath = fileInfo.path();
    QString msg;

    // 1. Build .cpp file
    cmd = "~/admb/admb " + tplFile.toStdString() + " > " + buildOutputFile.toStdString();
    std::system(cmd.c_str());
    msg = "<strong><br>Build command:</strong><br><br>" + QString::fromStdString(cmd);
    appendSummaryTE(msg);

    // 2. Check that .cpp file was built correctly
    QString text;
    QFile file(buildOutputFile);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&file);
    text = in.readAll().trimmed().replace("\n","<br>");
    file.close();
    msg = "<strong><br>Build output:</strong><br><br>" + text + "<br>";
    appendSummaryTE(msg);
    if (! text.contains("Successfully built executable.")) {
        m_logger->logMsg(nmfConstants::Error,"Error building admb executable.");
        return false;
    }

    // 3. Run admb generated exe
    cmd = QDir(filePath).filePath(fileBase).toStdString();
    cmd += " > " + runOutputFile.toStdString() + " 2>&1"; // This last part redirects stderr as well
    std::system(cmd.c_str());
    msg = "<strong>Run command:</strong><br><br>" + QString::fromStdString(cmd);
    appendSummaryTE(msg);

    return true;
}
*/
bool
nmfSSCAA_Tab6::writeADMBDataFile(const QString& datFile,
                                 const QString& Species,
                                 const QString& Fleet,
                                 const int& FirstYear,
                                 const int& LastYear,
                                 const int& MinAge,
                                 const int& MaxAge)
{
    int NumYears = LastYear-FirstYear+1;
    int NumAges  = MaxAge-MinAge+1;
    bool retv = false;
    QFile file(datFile);
    boost::numeric::ublas::matrix<double> NaturalMortality;
    boost::numeric::ublas::matrix<double> FishingMortality;
    boost::numeric::ublas::matrix<double> Catch;
    boost::numeric::ublas::matrix<double> Weight;
    std::vector<boost::numeric::ublas::matrix<double> > Tables = {Catch, Weight};
    std::vector<std::string> TableNames = {"CatchFishery","Weight"};
    std::vector<boost::numeric::ublas::matrix<double> > MortalityTables = {NaturalMortality, FishingMortality};
    std::vector<std::string> MortalityTableNames = {"MortalityNatural","MortalityFishing"};
    QString msg;

    // Read Catch table
    for (int i = 0; i < TableNames.size(); ++i) {
        if (! nmfMSCAAUtils::getDatabaseData(m_databasePtr,m_logger,
                m_ProjectSettingsConfig,Species.toStdString(),Fleet.toStdString(),
                NumYears,NumAges,TableNames[i],Tables[i])) {
            m_logger->logMsg(nmfConstants::Warning,
                           "No " + TableNames[i] + " Year-Age data found.");
            return false;
        }
    }

    // Read Mortality tables
    for (int i = 0; i < MortalityTableNames.size(); ++i) {
        if (! nmfMSCAAUtils::getMortalityData(m_databasePtr,m_logger,
                 m_ProjectSettingsConfig,Species.toStdString(),NumYears,
                 MinAge,MaxAge,MortalityTableNames[i],MortalityTables[i])) {
            m_logger->logMsg(nmfConstants::Warning,
                           "No " + MortalityTableNames[i] + " Year-Age data found.");
            return false;
        }
    }

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);
        fileStream << "#\n";
        fileStream << "# " << Species << "\n";
        fileStream << "#\n";
        fileStream << "# NumYears NumAges\n";
        fileStream << " " << NumYears << " " << NumAges << "\n";
        fileStream << "#\n";
        fileStream << "# Year Range: " << FirstYear << " to " << LastYear << "\n";
        fileStream << "#\n";
        fileStream << "# Observed Catch\n";
        writeToFileStream(fileStream,Tables[0]);
        fileStream << "# Observed Weight\n";
        writeToFileStream(fileStream,Tables[1]);
        fileStream << "# Natural Mortality\n";
        writeToFileStream(fileStream,MortalityTables[0]);
        fileStream << "# Fishing Mortality\n";
        writeToFileStream(fileStream,MortalityTables[1]);
        file.close();
        retv = true;
        msg = "<strong>Wrote data file: </strong>" + datFile;
        appendSummaryTE(msg);
    }
    return retv;
}

void
nmfSSCAA_Tab6::writeToFileStream(QTextStream& fileStream,
                                 boost::numeric::ublas::matrix<double>& matrix)
{
    for (int i = 0; i < matrix.size1(); ++i) {
        for (int j = 0; j < matrix.size2(); ++j) {
            fileStream << " " << matrix(i,j);
        }
        fileStream << "\n";
    }
}

bool
nmfSSCAA_Tab6::writeADMBParameterFile(const QString& parameterFile,
                                      const int& NumYears,
                                      const int& NumAges)
{
    bool retv = false;
    QFile file(parameterFile);
    QString msg;

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);
        fileStream << "# Number of parameters = 38\n";
        fileStream << "# log_q:\n";
        fileStream << "-1.0\n";
        fileStream << "# log_popscale:\n";
        fileStream << "8.31261120238\n";       // RSK change this
        fileStream << "# log_sel_coff:\n";
        for (int i = 0; i < NumAges-2; ++i) {
            fileStream << " " << 0;
        }
        fileStream << "\n";
        fileStream << "# log_relpop:\n";
        for (int i = 0; i < NumYears+NumAges-1; ++i) {
            fileStream << " " << nmfUtils::getRandomNumber(-1,-15.0,15.0);
        }
        fileStream << "\n";
        fileStream << "# effort_devs:\n";
        for (int i = 0; i < NumYears; ++i) {
            fileStream << " " << 0;
        }
        fileStream << "\n";
        file.close();

        msg = "<strong>Wrote parameter file: </strong>" + parameterFile;
        appendSummaryTE(msg);
        retv = true;
    }
    return retv;
}

bool
nmfSSCAA_Tab6::writeADMBTPLFile(const QString& tplFile)
{
    bool retv = false;
    QFile file(tplFile);
    QString msg;

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);
        fileStream << "DATA_SECTION\n";
        fileStream << "  init_int nyrs\n";
        fileStream << "  init_int nages\n";
        fileStream << "  init_matrix obs_Catch(1,nyrs,1,nages)\n";
        fileStream << "  init_matrix obs_Weight(1,nyrs,1,nages)\n";
        fileStream << "  init_matrix obs_F(1,nyrs,1,nages)\n";
        fileStream << "  init_matrix obs_M(1,nyrs,1,nages)\n";
        fileStream << "  init_vector effort(1,nyrs)\n";
        fileStream << "  vector relwt(2,nages);\n";
        fileStream << "\n";
        fileStream << "INITIALIZATION_SECTION\n";
        fileStream << "  log_q -1\n";
        fileStream << "  log_popscale 5\n";
        fileStream << "\n";
        fileStream << "PARAMETER_SECTION\n";
        fileStream << "  init_number log_q(1)\n";
        fileStream << "  init_number log_popscale(1)\n";
        fileStream << "  init_bounded_dev_vector log_sel_coff(1,nages-1,-15.,15.,2)\n";
        fileStream << "  init_bounded_dev_vector log_relpop(1,nyrs+nages-1,-15.,15.,2)\n";
        fileStream << "  init_bounded_dev_vector effort_devs(1,nyrs,-5.,5.,3)\n";
        fileStream << "  vector log_sel(1,nages)\n";
        fileStream << "  vector log_initpop(1,nyrs+nages-1);\n";
        fileStream << "  matrix F(1,nyrs,1,nages)\n";
        fileStream << "  matrix Z(1,nyrs,1,nages)\n";
        fileStream << "  matrix S(1,nyrs,1,nages)\n";
        fileStream << "  matrix N(1,nyrs,1,nages)\n";
        fileStream << "  matrix C(1,nyrs,1,nages)\n";
        fileStream << "  objective_function_value f\n";
        fileStream << "  number recsum\n";
        fileStream << "  number initsum\n";
        fileStream << "  sdreport_number avg_F\n";
        fileStream << "  sdreport_vector predicted_N(2,nages)\n";
        fileStream << "  sdreport_vector ratio_N(2,nages)\n";
        fileStream << "  // changed from the manual because adjusted likelihood routine doesn't\n";
        fileStream << "  // work\n";
        fileStream << "  likeprof_number pred_B\n";
        fileStream << "\n";
        fileStream << "PRELIMINARY_CALCS_SECTION\n";
        fileStream << "  // this is just to ``invent'' some relative average\n";
        fileStream << "  // weight at age numbers\n";
        fileStream << "  relwt.fill_seqadd(1.,1.);\n";
        fileStream << "  relwt=pow(relwt,.5);\n";
        fileStream << "  relwt/=max(relwt);\n";
        fileStream << "\n";
        fileStream << "PROCEDURE_SECTION\n";
        fileStream << "  get_mortality_and_survival_rates();\n";
        fileStream << "  get_numbers_at_age();\n";
        fileStream << "  get_catch_at_age();\n";
        fileStream << "  evaluate_the_objective_function();\n";
        fileStream << "\n";
        fileStream << "FUNCTION get_mortality_and_survival_rates\n";
        fileStream << "  int i, j;\n";
        fileStream << "  for (i=1;i<=nyrs;i++) {\n";
        fileStream << "    for (j=1;j<=nages;j++) {\n";
        fileStream << "      F(i,j) = obs_F(i,j);\n";
        fileStream << "    }\n";
        fileStream << "  }\n";
        fileStream << "  for (i=1;i<=nyrs;i++) {\n";
        fileStream << "      Z(i) = obs_F(i) + obs_M(i);\n";
        fileStream << "  }\n";
        fileStream << "  // get the survival rate\n";
        fileStream << "  S = mfexp(-Z);\n";
        fileStream << "\n";
        fileStream << "FUNCTION get_numbers_at_age\n";
        fileStream << "  int i, j;\n";
        fileStream << "  log_initpop = log_relpop + log_popscale;\n";
        fileStream << "  for (i=1;i<=nyrs;i++)\n";
        fileStream << "  {\n";
        fileStream << "    N(i,1) = mfexp(log_initpop(i));\n";
        fileStream << "  }\n";
        fileStream << "  for (j=2;j<=nages;j++)\n";
        fileStream << "  {\n";
        fileStream << "    N(1,j) = mfexp(log_initpop(nyrs+j-1));\n";
        fileStream << "  }\n";
        fileStream << "  for (i=1;i<nyrs;i++)\n";
        fileStream << "  {\n";
        fileStream << "    for (j=1;j<nages;j++)\n";
        fileStream << "    {\n";
        fileStream << "      N(i+1,j+1) = N(i,j)*S(i,j);\n";
        fileStream << "    }\n";
        fileStream << "  }\n";
        fileStream << "  // calculated predicted numbers at age for next year\n";
        fileStream << "  for (j=1;j<nages;j++)\n";
        fileStream << "  {\n";
        fileStream << "    predicted_N(j+1) = N(nyrs,j)*S(nyrs,j);\n";
        fileStream << "    ratio_N(j+1) = predicted_N(j+1)/N(1,j+1);\n";
        fileStream << "  }\n";
        fileStream << "  // calculated predicted Biomass for next year for\n";
        fileStream << "  // adjusted profile likelihood\n";
        fileStream << "  pred_B = (predicted_N * relwt);\n";
        fileStream << "\n";
        fileStream << "FUNCTION get_catch_at_age\n";
        fileStream << "  C = elem_prod(elem_div(F,Z),elem_prod(1.-S,N));\n";
        fileStream << "\n";
        fileStream << "FUNCTION evaluate_the_objective_function\n";
        fileStream << "  // penalty functions to ``regularize '' the solution\n";
        fileStream << "  f += .01*norm2(log_relpop);\n";
        fileStream << "  avg_F = sum(F)/double(size_count(F));\n";
        fileStream << "  if (last_phase()) {\n";
        fileStream << "    // a very small penalty on the average fishing mortality\n";
        fileStream << "    f += .001*square(log(avg_F/.2));\n";
        fileStream << "  } else {\n";
        fileStream << "    f += 1000.*square(log(avg_F/.2));\n";
        fileStream << "  }\n";
        fileStream << "  f += 0.5*double(size_count(C)+size_count(effort_devs))\n";
        fileStream << "       * log( sum(elem_div(square(C-obs_Catch),.01+C))\n";
        fileStream << "       + 0.1*norm2(effort_devs));\n";
        fileStream << "\n";
        fileStream << "REPORT_SECTION\n";
        fileStream << "  report << \"Estimated Abundance\" << endl;\n";
        fileStream << "  report << N << endl;\n";
        fileStream << "  report << \"Estimated Catch\" << endl;\n";
        fileStream << "  report << C << endl;\n";
        fileStream << "  report << \"Observed Catch\" << endl;\n";
        fileStream << "  report << obs_Catch << endl;\n";
        fileStream << "  report << \"Observed Weight\" << endl;\n";
        fileStream << "  report << obs_Weight << endl;\n";
        fileStream << "  report << \"Estimated Fishing Mortality\" << endl;\n";
        fileStream << "  report << F << endl;\n";
        fileStream << "  report << \"Observed Fishing Mortality\" << endl;\n";
        fileStream << "  report << obs_F << endl;\n";
        fileStream << "  report << \"Observed Natural Mortality\" << endl;\n";
        fileStream << "  report << obs_M << endl;\n";
        file.close();

        msg = "<strong>Wrote tpl file: </strong>" + tplFile;
        appendSummaryTE(msg);
        retv = true;
    }
    return retv;
}

//bool
//nmfSSCAA_Tab6::readADMBReportFile(
//        const QString& reportFile)
//{
//    bool retv = false;
//    int NumYears = m_Abundance.size1();
//    int NumAges  = m_Abundance.size2();
//    QFile file(reportFile);
//    QString line;
//    QStringList slist;

//    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
//          return retv;
//    QTextStream in(&file);
//    bool readingAbundance = false;
//    int year = 0;
//    std::string msg;
//    while (in.readLineInto(&line)) {
//        if (line.contains("Estimated Abundance")) {
//            readingAbundance = true;
//            continue;
//        } else if (line.contains("Estimated") || (line.contains("Observed"))) {
//            readingAbundance = false;
//        }
//        if (readingAbundance) {
////std::cout << "-> " << line.toStdString() << std::endl;
//            slist = line.trimmed().split(" ");
//            if (slist.size() != NumAges) {
//                msg = "nmfSSCAA_Tab6::readADMBReportFile: Expecting " + std::to_string(NumAges) +
//                      " age groups, found: " + std::to_string(slist.size());
//                m_logger->logMsg(nmfConstants::Error,msg);
//                return false;
//            }
//            for (int age = 0; age < NumAges; ++age) {
//                m_Abundance(year,age) = slist[age].toDouble();
//            }
//            ++year;
//            retv = true;
//        }

//    }

//    return retv;
//}

bool
nmfSSCAA_Tab6::saveToSpeciesTable(const std::vector<std::string>& ColLabels,
                                  QTableView* tableView)
{
    QStandardItemModel *smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    int NumSpecies;
    std::string value;
    std::string saveCmd="";
    std::string errorMsg;
    std::vector<std::string> Species;
    QModelIndex index;

    m_databasePtr->getAllSpecies(m_logger, Species);
    NumSpecies = Species.size();

    for (int row = 0; row < NumSpecies; ++row) {
        saveCmd += "UPDATE Species SET ";
        for (int col = 0; col < smodel->columnCount(); ++col) {
            index = smodel->index(row,col);
            value = index.data().toString().toStdString();
            saveCmd += ColLabels[col] + " = " + value + ",";
        }
        saveCmd = saveCmd.substr(0,saveCmd.size()-1);
        saveCmd += " WHERE SpeName = '" + Species[row] + "'; ";
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (errorMsg != " ") {
        m_logger->logMsg(nmfConstants::Error,"nmfSSCAA_Tab6::saveWeightings: Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(SSCAA_Tabs, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Reload to reset columns
    loadFromSpeciesTable(ColLabels,tableView);

    return true;
}

void
nmfSSCAA_Tab6::ReadSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSSCAA_Tab6::clearWidgets()
{
    nmfUtilsQt::clearTableView({SSCAA_Tab6_EstimationsTV,
                                SSCAA_Tab6_WeightingsTV});
}

bool
nmfSSCAA_Tab6::loadWidgets()
{
std::cout << "nmfSSCAA_Tab6::loadWidgets()" << std::endl;
    ReadSettings();

    clearWidgets();

    loadFromSpeciesTable(nmfConstantsMSCAA::ColLabelsEstimations,SSCAA_Tab6_EstimationsTV);
    loadFromSpeciesTable(nmfConstantsMSCAA::ColLabelsWeightings, SSCAA_Tab6_WeightingsTV);

    return true;
}

bool
nmfSSCAA_Tab6::loadFromSpeciesTable(const std::vector<std::string>& ColLabels,
                                    QTableView* tableView)
{
    int col;
    int NumSpecies;
    int NumCols;
    int NumRecords;
    QStringList SpeciesLabels;
    std::vector<std::string> Species;
    QStandardItemModel *smodel;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string value;
    QStandardItem *item;
    QStringList StrList;

    NumCols = ColLabels.size();

    m_databasePtr->getAllSpecies(m_logger, Species);
    NumSpecies = Species.size();
    smodel = new QStandardItemModel(NumSpecies,NumCols);

    // Get data from Species table
    fields.push_back("SpeName");
    for (std::string label : ColLabels) {
        fields.push_back(label);
    }
    queryStr = "SELECT SpeName";
    for (std::string label : ColLabels) {
        queryStr += "," + label;
    }
    queryStr  += " FROM Species ORDER BY SpeName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"No Species found in database.");
        return false;
    }

    // Load data into model
    for (int row = 0; row < NumSpecies; ++row) {
        SpeciesLabels.append(QString::fromStdString(Species[row]));
        col = 0;
        for (std::string colLabel : ColLabels) {
            value = dataMap[colLabel][row];
            item  = new QStandardItem(QString::fromStdString(value));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row,col++,item);
        }
    }

    // Label columns and assign model to tableview
    nmfUtilsQt::convertVectorToStrList(ColLabels,StrList);
    smodel->setHorizontalHeaderLabels(StrList);
    smodel->setVerticalHeaderLabels(SpeciesLabels);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();

    return true;
}



