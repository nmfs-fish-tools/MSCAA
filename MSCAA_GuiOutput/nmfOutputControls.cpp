
#include "nmfOutputControls.h"
#include "nmfConstantsMSCAA.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include "nmfConstants.h"

MSCAA_GuiOutputControls::MSCAA_GuiOutputControls(
        QGroupBox*   theControlsGroupBox,
        nmfLogger*   theLogger,
        nmfDatabase* theDatabasePtr,
        std::string& theProjectDir)
{
    m_ControlsGroupBox = theControlsGroupBox;
    m_logger           = theLogger;
    m_databasePtr      = theDatabasePtr;
    m_ProjectDir       = theProjectDir;
    m_ProjectSettingsConfig.clear();

    readSettings();

    initWidgets();
    initConnections();
    loadWidgets();
}

MSCAA_GuiOutputControls::~MSCAA_GuiOutputControls()
{

}


void
MSCAA_GuiOutputControls::initWidgets()
{
    QVBoxLayout* controlLayt = new QVBoxLayout();
    QHBoxLayout* typeLayt    = new QHBoxLayout();

    OutputTypeLBL    = new QLabel("Chart Type:");
    OutputSpeciesLBL = new QLabel("Species:");
    OutputAgeListLBL = new QLabel("Age Groups:");
    OutputScaleLBL   = new QLabel("Scale Factor:");
    OutputMortalityTypeLBL = new QLabel("Mortality Type:");
    OutputChartTypeCMB     = new QComboBox();
    OutputSpeciesCMB = new QComboBox();
    OutputScaleCMB   = new QComboBox();
    OutputLogCB      = new QCheckBox();
//    OutputNaturalMortalityCB   = new QCheckBox("Natural");
//    OutputFishingMortalityCB   = new QCheckBox("Fishing");
//    OutputPredationMortalityCB = new QCheckBox("Predation");
    OutputAgeListLV  = new QListView();
    OutputMortalityListLV = new QListView();

    controlLayt->addWidget(OutputTypeLBL);
    typeLayt->addWidget(OutputChartTypeCMB);
    typeLayt->addWidget(OutputLogCB);
    OutputChartTypeCMB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    controlLayt->addLayout(typeLayt);
    controlLayt->addWidget(OutputSpeciesLBL);
    controlLayt->addWidget(OutputSpeciesCMB);
    controlLayt->addWidget(OutputMortalityTypeLBL);
    controlLayt->addWidget(OutputMortalityListLV);
//    controlLayt->addWidget(OutputNaturalMortalityCB);
//    controlLayt->addWidget(OutputFishingMortalityCB);
//    controlLayt->addWidget(OutputPredationMortalityCB);
    controlLayt->addWidget(OutputAgeListLBL);
    controlLayt->addWidget(OutputAgeListLV);
    controlLayt->addWidget(OutputScaleLBL);
    controlLayt->addWidget(OutputScaleCMB);
    if (m_ControlsGroupBox->layout() == nullptr) {
        m_ControlsGroupBox->setLayout(controlLayt);
    }

    OutputAgeListLBL->setEnabled(false);
    OutputAgeListLV->setEnabled(false);
    OutputAgeListLV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    OutputChartTypeCMB->addItem("Abundance (3D)");
    OutputChartTypeCMB->addItem("Abundance vs Time");
    OutputChartTypeCMB->addItem("Mortality vs Time");
    OutputChartTypeCMB->addItem("Recruitment vs SSB");
    OutputChartTypeCMB->addItem("SSB vs Time");
    OutputChartTypeCMB->setItemData(0, "3D Surface Plot of Abundance vs Time and Age Groups",  Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(1, "2D Line Plot of Abundance vs Time (for one or more Age Groups)",  Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(2, "2D Line Plot of Spawning Stock Biomass vs Time",  Qt::ToolTipRole);
    OutputChartTypeCMB->setItemData(3, "2D Line Plot of Recruitment vs Spawning Stock Biomass",  Qt::ToolTipRole);
    OutputScaleCMB->addItem("Default");
    OutputScaleCMB->addItem("000");
    OutputScaleCMB->addItem("000 000");
    OutputScaleCMB->addItem("000 000 000");
    OutputLogCB->setToolTip("Show natural log of data (if < 0, clamp to 0)");

    // Deselect menu items that aren't completely implemented
    QVariant v(0);
    for (int i=3; i<=4; ++i) {
        OutputChartTypeCMB->model()->setData(
                    OutputChartTypeCMB->model()->index(i,0),
                    v,Qt::UserRole-1);
    }

    // Load mortality list view
    QStringListModel* MortalityModel = new QStringListModel();
    QStringList mortalityList = {"Natural (solid line)",
                                 "Fishing (dashed line)",
                                 "Predation (dotted line)"};
    MortalityModel->setStringList(mortalityList);
    OutputMortalityListLV->setModel(MortalityModel);
    OutputMortalityListLV->setSelectionMode(QAbstractItemView::ExtendedSelection);
    OutputMortalityListLV->setFixedHeight(65);
    OutputMortalityListLV->setEditTriggers(QAbstractItemView::NoEditTriggers);

    OutputAgeListLV->setEnabled(false);
    OutputAgeListLBL->setEnabled(false);
    OutputAgeListLV->setEditTriggers(QAbstractItemView::NoEditTriggers);
    OutputMortalityListLV->setEnabled(false);
    OutputMortalityTypeLBL->setEnabled(false);


    loadSpeciesControlWidget();

    readSettings();

    m_ControlsGroupBox->setMinimumHeight(100);

    // Hide these for now...
    OutputSpeciesLBL->hide();
    OutputSpeciesCMB->hide();
}


void
MSCAA_GuiOutputControls::initConnections()
{
    connect(OutputChartTypeCMB,     SIGNAL(currentTextChanged(QString)),
            this,                   SLOT(callback_OutputChartTypeCMB(QString)));
    connect(OutputSpeciesCMB,       SIGNAL(currentTextChanged(QString)),
            this,                   SLOT(callback_OutputSpeciesCMB(QString)));
    connect(OutputScaleCMB,         SIGNAL(currentTextChanged(QString)),
            this,                   SLOT(callback_OutputScaleCMB(QString)));
    connect(OutputLogCB,            SIGNAL(clicked(bool)),
            this,                   SLOT(callback_OutputLogCB(bool)));

    connect(OutputMortalityListLV->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,   SLOT(callback_MortalityTypeSelectionChanged(QItemSelection,QItemSelection)));


}

void
MSCAA_GuiOutputControls::loadWidgets()
{
    loadSpeciesControlWidget();

}

void
MSCAA_GuiOutputControls::loadSpeciesControlWidget()
{
//    int NumSpecies;
//    int NumGuilds;
//    QStringList SpeciesList;
//    QStringList GuildList;
////    QStringListModel*  SpeciesOrGuildModel;
//    std::string Algorithm;
//    std::string Minimizer;
//    std::string ObjectiveCriterion;
//    std::string Scaling;
//    std::string CompetitionForm;

//    ReadSettings();

//    AlgorithmIdentifiers(Algorithm,Minimizer,ObjectiveCriterion,Scaling,CompetitionForm,true);

//    if (! getGuilds(NumGuilds,GuildList)) {
//        m_logger->logMsg(nmfConstants::Warning,"[Warning] MSCAA_GuiOutputControls::loadSpeciesControlWidget: No records found in table Guilds, Name = "+m_ProjectSettingsConfig);
//        return;
//    }
//    if (CompetitionForm == "AGG-PROD") {
//       NumSpecies  = NumGuilds;
//       SpeciesList = GuildList;
//    } else {
//        if (! getSpecies(NumSpecies,SpeciesList)) {
//            m_logger->logMsg(nmfConstants::Error,"[Error 2] loadSpeciesControlWidget: No records found in table Species, Name = "+m_ProjectSettingsConfig);
//            return;
//        }
//    }

//    // Load Species controls combo box
//    OutputSpeciesCMB->blockSignals(true);
//    OutputAgeListLV->blockSignals(true);

//    OutputSpeciesCMB->clear();
//    m_SpeciesOrGuildModel->setStringList(QStringList({}));
//    SpeciesHash.clear();
//    for (int species=0; species<NumSpecies; ++species) {
//        SpeciesHash[SpeciesList[species]] = species;
//    }
//    // Note: using same model for 2 widgets.  Now if we change the model,
//    // we won't have to edit the widgets.
//    m_SpeciesOrGuildModel->setStringList(SpeciesList);

//    // RSK - Why doesn't this work?
////    OutputSpeciesCMB->setModel(m_SpeciesOrGuildModel);

//    OutputAgeListLV->setModel(m_SpeciesOrGuildModel);

//    for (QString Species : SpeciesList) {
//        OutputSpeciesCMB->addItem(Species);
//    }

//    OutputSpeciesCMB->blockSignals(false);
//    OutputAgeListLV->blockSignals(false);

}

bool
MSCAA_GuiOutputControls::showNaturalLogOfData()
{
    return OutputLogCB->isChecked();
}

void
MSCAA_GuiOutputControls::loadAgeLV(QStringList& ageList)
{
    QStringListModel* SpeciesModel = new QStringListModel();

    SpeciesModel->setStringList(ageList);
    OutputAgeListLV->setModel(SpeciesModel);

    disconnect(OutputAgeListLV->selectionModel(),nullptr,nullptr,nullptr);
//    connect(OutputAgeListLV->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
//            this,                              SLOT(callback_RowChanged(QModelIndex,QModelIndex)));

    connect(OutputAgeListLV->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,                              SLOT(callback_AgeGroupSelectionChanged(QItemSelection,QItemSelection)));



}

int
MSCAA_GuiOutputControls::getSpeciesNumFromName(QString SpeciesName)
{
    return SpeciesHash[SpeciesName];
}


bool
MSCAA_GuiOutputControls::getSpecies(int&         NumSpecies,
                                    QStringList& SpeciesList)
{
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    SpeciesList.clear();

    fields     = {"SpeName"};
    queryStr   = "SELECT SpeName from Species ORDER BY SpeName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = unsigned(dataMap["SpeName"].size());
    if (NumSpecies == 0) {
        m_logger->logMsg(nmfConstants::Warning,"[Warning] MSCAA_GuiOutputControls::getSpecies: No species found in table Species");
        return false;
    }

    for (unsigned species=0; species<unsigned(NumSpecies); ++species) {
        SpeciesList << QString::fromStdString(dataMap["SpeName"][species]);
    }

    return true;
}

void
MSCAA_GuiOutputControls::selectFirstAgeGroup()
{
    QModelIndex index = OutputAgeListLV->model()->index(0,0);
    OutputAgeListLV->selectionModel()->select(index,QItemSelectionModel::Select);
}

void
MSCAA_GuiOutputControls::selectFirstMortalityGroup(bool isMortality)
{
    if (isMortality) {
        QModelIndex index = OutputMortalityListLV->model()->index(0,0);
        if (OutputMortalityListLV->selectionModel()->selectedIndexes().size() > 0) {
            OutputMortalityListLV->selectionModel()->select(index,QItemSelectionModel::Select);
        }
    } else {
        // RSK - this line causes the Abundance (3D) plot not to be display after
        // the user selected the Mortality plot.  Don't know why. Maybe a Qt bug.
        // OutputMortalityListLV->selectionModel()->clearSelection();
    }
}

void
MSCAA_GuiOutputControls::callback_MortalityTypeSelectionChanged(
        QItemSelection sel,
        QItemSelection desel)
{
    emit MortalityAgeGroupsSelected(
                OutputAgeListLV->selectionModel()->selectedIndexes(),
                OutputMortalityListLV->selectionModel()->selectedIndexes());
}

void
MSCAA_GuiOutputControls::callback_AgeGroupSelectionChanged(
        QItemSelection sel,
        QItemSelection desel)
{
    int NumSelectedRows     = OutputAgeListLV->selectionModel()->selectedIndexes().size();
    QString OutputChartType = getOutputType();
    QString mode = "tbd";

    if (NumSelectedRows > 0) {
        if (OutputChartType == "Abundance vs Time") {
            emit AbundanceAgeGroupsSelected(mode,
               OutputAgeListLV->selectionModel()->selectedIndexes());
        } else if (OutputChartType == "Mortality vs Time") {
            emit MortalityAgeGroupsSelected(
                        OutputAgeListLV->selectionModel()->selectedIndexes(),
                        OutputMortalityListLV->selectionModel()->selectedIndexes());
        }
    }

    OutputScaleLBL->setFocus();
}

void
MSCAA_GuiOutputControls::callback_OutputChartTypeCMB(QString outputType)
{

    emit UpdateAgeList();
    emit ShowChart(outputType,getOutputScale());

    bool isAbundance3d     = (outputType == "Abundance (3D)");
    bool isAbundanceVsTime = (outputType == "Abundance vs Time");
    bool isMortality       = (outputType == "Mortality vs Time");
    OutputAgeListLV->setEnabled(isAbundanceVsTime  || isMortality);
    OutputAgeListLBL->setEnabled(isAbundanceVsTime || isMortality);
    OutputMortalityListLV->setEnabled(isMortality);
    OutputMortalityTypeLBL->setEnabled(isMortality);
    selectFirstMortalityGroup(isMortality);
    OutputLogCB->setVisible(isAbundance3d);
    enableScaleWidgets(! isAbundance3d);

}

void
MSCAA_GuiOutputControls::callback_OutputLogCB(bool isChecked)
{
    OutputScaleCMB->setCurrentIndex(0);
    callback_OutputChartTypeCMB("Abundance (3D)");
}

void
MSCAA_GuiOutputControls::enableScaleWidgets(bool enable)
{
    OutputScaleLBL->setEnabled(enable);
    OutputScaleCMB->setEnabled(enable);
}

void
MSCAA_GuiOutputControls::refresh()
{
//    int currentIndex = OutputTypeCMB->currentIndex();

//    OutputTypeCMB->setCurrentIndex(0);
//    OutputTypeCMB->setCurrentIndex(currentIndex);

//    refreshScenarios();
}

void
MSCAA_GuiOutputControls::callback_OutputSpeciesCMB(QString outputSpecies)
{
    /*
    QString scenario  = getOutputScenario();
    QString chartType = getOutputType();
    QString method    = getOutputDiagnostics();

    if (chartType == "Multi-Scenario Plots") {
        emit ShowChartMultiScenario(m_SortedForecastLabelsMap[scenario]);
    } else if ((chartType == "Diagnostics") && (method == "Retrospective Analysis")) {
        emit ShowChartMohnsRho();
    } else {
        emit ShowChart("",outputSpecies);
    }
    emit UpdateSummaryStatistics();
    */
}

void
MSCAA_GuiOutputControls::callback_OutputScaleCMB(QString scale)
{
    QString mode = "tbd";
    QString OutputType = getOutputType();

    if (OutputType == "Abundance vs Time") {
        emit AbundanceAgeGroupsSelected(mode,
                    OutputAgeListLV->selectionModel()->selectedIndexes());
    } else if (OutputType == "Mortality vs Time") {
        emit MortalityAgeGroupsSelected(
                    OutputAgeListLV->selectionModel()->selectedIndexes(),
                    OutputMortalityListLV->selectionModel()->selectedIndexes());
    } else {
        emit ShowChart(getOutputType(),scale);
    }
}

bool
MSCAA_GuiOutputControls::thereAreSelections()
{
    return (OutputAgeListLV->selectionModel()->selectedIndexes().size() > 0);
}

QString
MSCAA_GuiOutputControls::getOutputSpecies()
{
    return OutputSpeciesCMB->currentText();
}

int
MSCAA_GuiOutputControls::getOutputSpeciesIndex()
{
    return OutputSpeciesCMB->currentIndex();
}

void
MSCAA_GuiOutputControls::setOutputSpeciesIndex(int index)
{
    OutputSpeciesCMB->setCurrentIndex(index);
}

QString
MSCAA_GuiOutputControls::getOutputType()
{
    return OutputChartTypeCMB->currentText();
}

void
MSCAA_GuiOutputControls::setOutputType(QString type)
{
    // This forces an update if the user needs to
    // refresh the same setting.
    OutputChartTypeCMB->setCurrentIndex(0);
    OutputChartTypeCMB->setCurrentText(type);

    return;
}

QStringList
MSCAA_GuiOutputControls::getSelectedAges()
{
    QStringList ageList;
    QModelIndexList selIndexes = OutputAgeListLV->selectionModel()->selectedIndexes();

    foreach (const QModelIndex &index, selIndexes){
        ageList.append(index.data(Qt::DisplayRole ).toString());
    }

    return ageList;
}

QString
MSCAA_GuiOutputControls::getOutputScale()
{
    return OutputScaleCMB->currentText();
}


QWidget*
MSCAA_GuiOutputControls::getListViewViewport()
{
    return OutputAgeListLV->viewport();
}

QModelIndexList
MSCAA_GuiOutputControls::getListViewSelectedIndexes()
{
    return OutputAgeListLV->selectionModel()->selectedIndexes();
}



void
MSCAA_GuiOutputControls::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();

    delete settings;
}

void
MSCAA_GuiOutputControls::saveSettings()
{

}
