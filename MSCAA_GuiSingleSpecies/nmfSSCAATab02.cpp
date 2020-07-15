
//#include "nmfUtilsQt.h"
//#include "nmfSSCAATab02.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"

#include "nmfSSCAATab02.h"
#include "nmfMSCAATableIO.h"


nmfSSCAA_Tab2::nmfSSCAA_Tab2(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    SSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;

    m_logger->logMsg(nmfConstants::Normal,"nmfSSCAA_Tab2::nmfSSCAA_Tab2");
    readSettings();

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/SSCAA/SSCAA_Tab02.ui");
    file.open(QFile::ReadOnly);
    SSCAA_Tab2_Widget = loader.load(&file,SSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    SSCAA_Tabs->addTab(SSCAA_Tab2_Widget, tr("2. Survey Catch"));

    SSCAA_Tab2_CatchTV       = new QTableView(SSCAA_Tabs);
    SSCAA_Tab2_TotalTV       = new QTableView(SSCAA_Tabs);
    SSCAA_Tab2_SpeciesLBL    = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab2_SpeciesLBL");
    SSCAA_Tab2_PrevPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab2_PrevPB");
    SSCAA_Tab2_NextPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab2_NextPB");
    SSCAA_Tab2_LoadPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab2_ReloadPB");
    SSCAA_Tab2_SavePB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab2_SavePB");
    SSCAA_Tab2_UnitsCMB      = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab2_UnitsCMB");
    SSCAA_Tab2_TotalUnitsCMB = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab2_TotalUnitsCMB");
    SSCAA_Tab2_SurveyNumCMB  = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab2_SurveyNumCMB");
    SSCAA_Tab2_TVLT          = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab2_TVLT");
    SSCAA_Tab2_TotalTVLT     = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab2_TotalTVLT");
    SSCAA_Tab2_TVLT->addWidget(SSCAA_Tab2_CatchTV);
    SSCAA_Tab2_TotalTVLT->addWidget(SSCAA_Tab2_TotalTV);

    // Load units
    SSCAA_Tab2_UnitsCMB->addItem("Number per Unit Effort");
    SSCAA_Tab2_TotalUnitsCMB->addItem("Number per Unit Effort");
    m_originalUnits = "Metric Tons";

    QFont noBoldFont;
    noBoldFont.setBold(false);
    SSCAA_Tab2_CatchTV->setFont(noBoldFont);
    SSCAA_Tab2_TotalTV->setFont(noBoldFont);

    connect(SSCAA_Tab2_PrevPB,   SIGNAL(clicked()),
            this,                SLOT(callback_PrevPB()));
    connect(SSCAA_Tab2_NextPB,   SIGNAL(clicked()),
            this,                SLOT(callback_NextPB()));
    connect(SSCAA_Tab2_LoadPB,   SIGNAL(clicked()),
            this,                SLOT(callback_LoadPB()));
    connect(SSCAA_Tab2_SavePB,   SIGNAL(clicked()),
            this,                SLOT(callback_SavePB()));
    connect(SSCAA_Tab2_UnitsCMB, SIGNAL(currentTextChanged(QString)),
            this,                SLOT(callback_UnitsChangedCMB(QString)));
    connect(SSCAA_Tab2_TotalUnitsCMB, SIGNAL(currentTextChanged(QString)),
            this,                     SLOT(callback_TotalUnitsCMB(QString)));
    connect(SSCAA_Tab2_SurveyNumCMB, SIGNAL(currentTextChanged(QString)),
            this,                    SLOT(callback_SurveyNumCMB(QString)));
    connect(SSCAA_Tab2_CatchTV->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this,                                    SLOT(callback_Slider1Moved(int)));
    connect(SSCAA_Tab2_TotalTV->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this,                                    SLOT(callback_Slider2Moved(int)));
    SSCAA_Tab2_PrevPB->setText("\u25C1--");
    SSCAA_Tab2_NextPB->setText("--\u25B7");

} // end constructor


nmfSSCAA_Tab2::~nmfSSCAA_Tab2()
{

}


QTableView*
nmfSSCAA_Tab2::getTableSurveyCatchAtAge()
{
    return SSCAA_Tab2_CatchTV;
}

QTableView*
nmfSSCAA_Tab2::getTableSurveyTotal()
{
    return SSCAA_Tab2_TotalTV;
}

void
nmfSSCAA_Tab2::setUnits(std::string units)
{
//    if (units.empty()) {
//        units = "Metric Tons";
//    }
//    SSCAA_Tab2_UnitsCMB->blockSignals(true);
//    SSCAA_Tab2_UnitsCMB->setCurrentText(QString::fromStdString(units));
//    SSCAA_Tab2_UnitsCMB->blockSignals(false);
//    originalUnits = QString::fromStdString(units);
}

int
nmfSSCAA_Tab2::getSurveyNum()
{
    return SSCAA_Tab2_SurveyNumCMB->currentText().toInt();
}

std::string
nmfSSCAA_Tab2::getUnits()
{
    return SSCAA_Tab2_UnitsCMB->currentText().toStdString();
}

std::string
nmfSSCAA_Tab2::getTotalUnits()
{
    return SSCAA_Tab2_TotalUnitsCMB->currentText().toStdString();
}

std::string
nmfSSCAA_Tab2::getSpecies()
{
    return SSCAA_Tab2_SpeciesLBL->text().toStdString();
}

void
nmfSSCAA_Tab2::setSpecies(QString species)
{
    int survey;


    std::string units;
    std::string totalUnits;

    SSCAA_Tab2_SpeciesLBL->setText(species);
    loadNumSurveys(species);
    survey = getSurveyNum();
    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             SSCAA_Tab2_CatchTV,
                             "CatchSurvey",species,
                             std::to_string(survey),units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
//  setUnits(units);

    nmfMSCAAUtils::loadTheTotalTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             SSCAA_Tab2_TotalTV,
                             "CatchSurveyTotal",species,
                             std::to_string(survey),totalUnits);
}

void
nmfSSCAA_Tab2::callback_PrevPB()
{
    int prevPage = SSCAA_Tabs->currentIndex()-1;
    SSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfSSCAA_Tab2::callback_NextPB()
{
    int nextPage = SSCAA_Tabs->currentIndex()+1;
    SSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfSSCAA_Tab2::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSSCAA_Tab2::callback_SavePB()
{
    bool ok;
    QString msg = "\n";
    std::string species = getSpecies();
    int surveyNum       = getSurveyNum();

    if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab2_CatchTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                 SSCAA_Tab2_CatchTV,
                                 m_ProjectSettingsConfig,
                                 "CatchSurvey",species,
                                 std::to_string(surveyNum),
                                 getUnits(),
                                 nmfConstants::IsNotProportion,
                                 nmfConstants::dontIncludeTotalColumn,
                                 nmfConstants::AllYears);
        if (ok) {
            msg += "CatchSurvey table has been successfully updated.\n";
        }

        ok = nmfMSCAAUtils::saveProportionTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                           SSCAA_Tab2_CatchTV,
                                           m_ProjectSettingsConfig,
                                           "CatchSurveyProportion",
                                           species,std::to_string(surveyNum));
        if (ok) {
            msg += "CatchSurveyProportion table has been successfully updated.\n";
        }
    }
    if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab2_TotalTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveTheTotalTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                              SSCAA_Tab2_TotalTV,
                                              m_ProjectSettingsConfig,
                                              "CatchSurveyTotal",species,
                                              std::to_string(surveyNum),
                                              getTotalUnits());
        if (ok) {
            msg += "CatchSurveyTotal table has been successfully updated.\n";
        }
    }

    QMessageBox::information(SSCAA_Tabs, "Table(s) Updated", msg, QMessageBox::Ok);
}

void
nmfSSCAA_Tab2::loadNumSurveys(QString species)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"SpeName","NumSurveys"};
    queryStr   = "SELECT SpeName,NumSurveys FROM Species";
    queryStr  += " WHERE SpeName = '" + species.toStdString() + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        // logger->logMsg(nmfConstants::Error,"Found 0 records in Species");
        return;
    }

    // Load Num Survey widget
    SSCAA_Tab2_SurveyNumCMB->clear();
    SSCAA_Tab2_SurveyNumCMB->blockSignals(true);
    for (int survey = 1; survey <= std::stoi(dataMap["NumSurveys"][0]); ++survey) {
        SSCAA_Tab2_SurveyNumCMB->addItem(QString::number(survey));
    }
    SSCAA_Tab2_SurveyNumCMB->setCurrentIndex(0);
    SSCAA_Tab2_SurveyNumCMB->blockSignals(false);
}

void
nmfSSCAA_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({SSCAA_Tab2_CatchTV,
                                SSCAA_Tab2_TotalTV});
}

bool
nmfSSCAA_Tab2::loadWidgets()
{
std::cout << "nmfSSCAA_Tab2::loadWidgets()" << std::endl;
    std::string units;
    QString species = QString::fromStdString(getSpecies());

    readSettings();

    clearWidgets();
//  loadNumSurveys(species);

    int survey = getSurveyNum();
    if (survey == 0) {
        survey = 1;
    }

    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             SSCAA_Tab2_CatchTV,
                             "CatchSurvey",
                             species, std::to_string(survey), units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
    setUnits(units);

    nmfMSCAAUtils::loadTheTotalTable(m_databasePtr,m_logger,
                                     m_ProjectSettingsConfig,
                                     SSCAA_Tab2_TotalTV,
                                     "CatchSurveyTotal",
                                     species, std::to_string(survey),
                                     units);

    return true;
}

void
nmfSSCAA_Tab2::readSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSSCAA_Tab2::callback_TotalUnitsCMB(QString newUnits)
{

}


void
nmfSSCAA_Tab2::callback_SurveyNumCMB(QString survey)
{
    loadWidgets();
}



void
nmfSSCAA_Tab2::callback_UnitsChangedCMB(QString newUnits)
{
//    double unitsSF = 1.0;
//    if (newUnits == "Metric Tons") {
//        if (originalUnits == "000 Metric Tons") {
//            unitsSF = 0.001;
//        } else if (originalUnits == "000 000 Metric Tons") {
//            unitsSF = 0.000001;
//        }
//    } else if (newUnits == "000 Metric Tons") {
//        if (originalUnits == "Metric Tons") {
//            unitsSF = 1000.0;
//        } else if (originalUnits == "000 000 Metric Tons") {
//            unitsSF = 0.001;
//        }
//    } else if (newUnits == "000 000 Metric Tons") {
//        if (originalUnits == "Metric Tons") {
//            unitsSF = 1000000.0;
//        } else if (originalUnits == "000 Metric Tons") {
//            unitsSF = 1000.0;
//        }
//    }
//    originalUnits = newUnits;
//    nmfMSCAAUtils::rescaleModel(SSCAA_Tab2_CatchTV,unitsSF);
}


void
nmfSSCAA_Tab2::callback_Slider1Moved(int val)
{
    SSCAA_Tab2_TotalTV->verticalScrollBar()->setSliderPosition(val);
}

void
nmfSSCAA_Tab2::callback_Slider2Moved(int val)
{
    SSCAA_Tab2_CatchTV->blockSignals(true);
    SSCAA_Tab2_CatchTV->verticalScrollBar()->setSliderPosition(val);
    SSCAA_Tab2_CatchTV->blockSignals(false);
}



