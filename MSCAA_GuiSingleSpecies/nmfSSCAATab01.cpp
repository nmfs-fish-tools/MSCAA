
#include "nmfSSCAATab01.h"
#include "nmfMSCAATableIO.h"


nmfSSCAA_Tab1::nmfSSCAA_Tab1(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_projectDir  = projectDir;

    m_logger->logMsg(nmfConstants::Normal,"nmfSSCAA_Tab1::nmfSSCAA_Tab1");

    SSCAA_Tabs = tabs;

    readSettings();

    // Load ui as a widget from disk
    QFile file(":/forms/SSCAA/SSCAA_Tab01.ui");
    file.open(QFile::ReadOnly);
    SSCAA_Tab1_Widget = loader.load(&file,SSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    SSCAA_Tabs->addTab(SSCAA_Tab1_Widget, tr("1. Fishery Catch-At-Age"));

    SSCAA_Tab1_CatchAtAgeTV      = new QTableView(SSCAA_Tabs);
    SSCAA_Tab1_CatchAtLengthTV   = new QTableView(SSCAA_Tabs);
//  SSCAA_Tab1_CatchTV           = new nmfTableView(SSCAA_Tabs);
//  SSCAA_Tab1_TotalTV           = new nmfTableView2(SSCAA_Tabs);
    SSCAA_Tab1_TotalTV           = new QTableView(SSCAA_Tabs);
    SSCAA_Tab1_AbundanceTitleLBL = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab1_AbundanceTitleLBL");
    SSCAA_Tab1_AbundanceUnitsCMB = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab1_AbundanceUnitsCMB");
    SSCAA_Tab1_WeightUnitsCMB    = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab1_WeightUnitsCMB");
    SSCAA_Tab1_FleetCMB          = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab1_FleetCMB");
    SSCAA_Tab1_NextPB            = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab1_NextPB");
    SSCAA_Tab1_SpeciesCatchLBL   = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab1_SpeciesCatchLBL");
    SSCAA_Tab1_SpeciesTotalLBL   = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab1_SpeciesTotalLBL");
    SSCAA_Tab1_LoadPB            = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab1_ReloadPB");
    SSCAA_Tab1_SavePB            = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab1_SavePB");
    SSCAA_Tab1_AbundanceLT       = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab1_AbundanceLT");
    SSCAA_Tab1_WeightLT          = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab1_WeightLT");
//  SSCAA_Tab1_AbundanceLT->addWidget(SSCAA_Tab1_CatchAtAgeTV);
    SSCAA_Tab1_WeightLT->addWidget(SSCAA_Tab1_TotalTV);

    SSCAA_Tab1_CatchAtAgeTV->hide();
    SSCAA_Tab1_CatchAtLengthTV->hide();

    // Load units
    SSCAA_Tab1_AbundanceUnitsCMB->addItem("Fish");
    SSCAA_Tab1_AbundanceUnitsCMB->addItem("000 Fish");
    SSCAA_Tab1_AbundanceUnitsCMB->addItem("000 000 Fish");
    SSCAA_Tab1_WeightUnitsCMB->addItem("Metric Tons");
    SSCAA_Tab1_WeightUnitsCMB->addItem("000 Metric Tons");
    SSCAA_Tab1_WeightUnitsCMB->addItem("000 000 Metric Tons");
    m_previousAbundanceUnits = "Fish";
    m_previousWeightUnits    = "Metric Tons";

    QFont noBoldFont;
    noBoldFont.setBold(false);
    SSCAA_Tab1_CatchAtLengthTV->setFont(noBoldFont);
    SSCAA_Tab1_CatchAtAgeTV->setFont(noBoldFont);
    SSCAA_Tab1_TotalTV->setFont(noBoldFont);

    connect(SSCAA_Tab1_NextPB,   SIGNAL(clicked()),
            this,                SLOT(callback_NextPB()));
    connect(SSCAA_Tab1_LoadPB,   SIGNAL(clicked()),
            this,                SLOT(callback_LoadPB()));
    connect(SSCAA_Tab1_SavePB,   SIGNAL(clicked()),
            this,                SLOT(callback_SavePB()));
    connect(SSCAA_Tab1_AbundanceUnitsCMB, SIGNAL(currentTextChanged(QString)),
            this,                SLOT(callback_AbundanceUnitsChanged(QString)));
    connect(SSCAA_Tab1_WeightUnitsCMB, SIGNAL(currentTextChanged(QString)),
            this,                      SLOT(callback_WeightUnitsCMB(QString)));
    connect(SSCAA_Tab1_FleetCMB, SIGNAL(currentTextChanged(QString)),
            this,                      SLOT(callback_FleetCMB(QString)));
    connect(SSCAA_Tab1_CatchAtAgeTV->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this,                      SLOT(callback_Slider1Moved(int)));
    connect(SSCAA_Tab1_TotalTV->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this,                                    SLOT(callback_Slider2Moved(int)));

    SSCAA_Tab1_NextPB->setText("--\u25B7");

} // end constructor


nmfSSCAA_Tab1::~nmfSSCAA_Tab1()
{
}


void
nmfSSCAA_Tab1::callback_Slider1Moved(int val)
{
    SSCAA_Tab1_TotalTV->verticalScrollBar()->setSliderPosition(val);
}

void
nmfSSCAA_Tab1::callback_Slider2Moved(int val)
{
    SSCAA_Tab1_CatchAtAgeTV->blockSignals(true);
    SSCAA_Tab1_CatchAtAgeTV->verticalScrollBar()->setSliderPosition(val);
    SSCAA_Tab1_CatchAtAgeTV->blockSignals(false);
}


QTableView*
nmfSSCAA_Tab1::getTableCatchAtAge()
{
    return SSCAA_Tab1_CatchAtAgeTV;
}


QTableView*
nmfSSCAA_Tab1::getTableCatchAtLength()
{
    return SSCAA_Tab1_CatchAtLengthTV;
}


QTableView*
nmfSSCAA_Tab1::getTableTotalWeight()
{
    return SSCAA_Tab1_TotalTV;
}

void
nmfSSCAA_Tab1::setAbundanceUnits(std::string units)
{
    if (units.empty()) {
        units = "Fish";
    }

    m_previousAbundanceUnits = QString::fromStdString(units);
    SSCAA_Tab1_AbundanceUnitsCMB->blockSignals(true);
    SSCAA_Tab1_AbundanceUnitsCMB->setCurrentText(QString::fromStdString(units));
    SSCAA_Tab1_AbundanceUnitsCMB->blockSignals(false);
}

void
nmfSSCAA_Tab1::setWeightUnits(std::string units)
{
    if (units.empty()) {
        units = "Metric Tons";
    }
    SSCAA_Tab1_WeightUnitsCMB->blockSignals(true);
    SSCAA_Tab1_WeightUnitsCMB->setCurrentText(QString::fromStdString(units));
    SSCAA_Tab1_WeightUnitsCMB->blockSignals(false);
    m_previousWeightUnits = QString::fromStdString(units);
}

std::string
nmfSSCAA_Tab1::getAbundanceUnits()
{
    return SSCAA_Tab1_AbundanceUnitsCMB->currentText().toStdString();
}

std::string
nmfSSCAA_Tab1::getWeightUnits()
{
    return SSCAA_Tab1_WeightUnitsCMB->currentText().toStdString();
}

std::string
nmfSSCAA_Tab1::getSpecies()
{
    return SSCAA_Tab1_SpeciesCatchLBL->text().toStdString();
}

std::string
nmfSSCAA_Tab1::getFleet()
{
    return SSCAA_Tab1_FleetCMB->currentText().toStdString();
}


bool
nmfSSCAA_Tab1::isCatchAtAge(QString species)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Read Species data
    fields     = {"SpeName","CatchAtAge"};
    queryStr   = "SELECT SpeName,CatchAtAge FROM Species";
    queryStr  += " WHERE SpeName = '" + species.toStdString() + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        m_logger->logMsg(nmfConstants::Warning,"Warning: No records found in table: Species");
        return false;
    }

    return (dataMap["CatchAtAge"][0] == "1");
}

void
nmfSSCAA_Tab1::setSpecies(QString species)
{
    if (species.isEmpty())
        return;

    clearWidgets();

    SSCAA_Tab1_SpeciesCatchLBL->setText(species);
    SSCAA_Tab1_SpeciesTotalLBL->setText(species);
/*
    std::string abundanceUnits;
    std::string weightUnits;

    // Load Catch-At-Age table
    nmfMSCAAUtils::loadTable(m_database,m_logger,
                             m_projectSettingsConfig,
                             SSCAA_Tab1_CatchAtAgeTV,
                             "CatchFishery",species,
                             std::to_string(0),
                             abundanceUnits,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
    setAbundanceUnits(abundanceUnits);

    // Load Catch-At-Length table
    nmfMSCAAUtils::loadCatchAtLengthTable(m_database,m_logger,
                             m_projectSettingsConfig,
                             SSCAA_Tab1_CatchAtLengthTV,
                             "CatchAtLengthFishery",species,
                             std::to_string(0),
                             abundanceUnits);

    // Load total weight table
    nmfMSCAAUtils::loadTheTotalTable(m_database,m_logger,
                                     m_projectSettingsConfig,
                                     SSCAA_Tab1_TotalTV,
                                     "CatchFisheryTotal",species,
                                     std::to_string(0),
                                     weightUnits);

    setWeightUnits(weightUnits);
*/
    nmfMSCAAUtils::loadFleetComboBox(
                m_databasePtr, m_logger, m_projectSettingsConfig,
                species, SSCAA_Tab1_FleetCMB);

    // Connect the appropriate table
    SSCAA_Tab1_CatchAtAgeTV->show();
    SSCAA_Tab1_CatchAtLengthTV->show();
    if (isCatchAtAge(species)) {
        SSCAA_Tab1_AbundanceLT->addWidget(SSCAA_Tab1_CatchAtAgeTV);
        SSCAA_Tab1_CatchAtLengthTV->hide();
        SSCAA_Tab1_AbundanceTitleLBL->setText("Fishery Catch-At-Age:");
    } else {
        SSCAA_Tab1_AbundanceLT->addWidget(SSCAA_Tab1_CatchAtLengthTV);
        SSCAA_Tab1_CatchAtAgeTV->hide();
        SSCAA_Tab1_AbundanceTitleLBL->setText("Fishery Length-At-Age:");
    }

    loadWidgets();
}

void
nmfSSCAA_Tab1::callback_NextPB()
{
    int nextPage = SSCAA_Tabs->currentIndex()+1;
    SSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfSSCAA_Tab1::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSSCAA_Tab1::callback_SavePB()
{
    bool ok;
    QString msg = "\n";
    std::string species = getSpecies();
    std::string fleet = getFleet();
    std::string noVal = "-1";

    if (isCatchAtAge(QString::fromStdString(species))) {
        if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab1_CatchAtAgeTV,
                                             nmfConstants::ShowError))
        {
            ok = nmfMSCAAUtils::saveTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                     SSCAA_Tab1_CatchAtAgeTV,
                                     m_projectSettingsConfig,
                                     "CatchFishery",species,fleet,
                                     getAbundanceUnits(),
                                     nmfConstants::IsNotProportion,
                                     nmfConstants::dontIncludeTotalColumn,
                                     nmfConstants::AllYears);
            if (ok) {
                msg += "CatchFishery table has been successfully updated.\n";
            }

//            ok = nmfMSCAAUtils::saveProportionTable(SSCAA_Tabs,databasePtr,logger,
//                                               SSCAA_Tab1_CatchAtAgeTV,
//                                               ProjectSettingsConfig,
//                                               "CatchFisheryProportion",
//                                               species,noVal);
//            if (ok) {
//                msg += "CatchFisheryProportion table has been successfully updated.\n";
//            }
        }
    } else {
        if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab1_CatchAtLengthTV,
                                             nmfConstants::ShowError))
        {
            ok = nmfMSCAAUtils::saveCatchAtLengthTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                                       SSCAA_Tab1_CatchAtLengthTV,
                                                       m_projectSettingsConfig,
                                                       "CatchAtLengthFishery",species,fleet,
                                                       getAbundanceUnits());
            if (ok) {
                msg += "CatchFisheryLength table has been successfully updated.\n";
            }
        }
    }

    if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab1_TotalTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveTheTotalTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                         SSCAA_Tab1_TotalTV,
                                         m_projectSettingsConfig,
                                         "CatchFisheryTotal",species,fleet,
                                         getWeightUnits());
        if (ok) {
            msg += "CatchFisheryTotal table has been successfully updated.\n";
        }
    }

    QMessageBox::information(SSCAA_Tabs, "Table(s) Updated", msg, QMessageBox::Ok);
}

void
nmfSSCAA_Tab1::clearWidgets()
{
    nmfUtilsQt::clearTableView({SSCAA_Tab1_CatchAtLengthTV,
                                SSCAA_Tab1_CatchAtAgeTV,
                                SSCAA_Tab1_TotalTV});
}

bool
nmfSSCAA_Tab1::loadWidgets()
{
    readSettings();

    std::string abundanceUnits;
    std::string weightUnits;
    QString species = QString::fromStdString(getSpecies());
    std::string fleet = getFleet();

    clearWidgets();

std::cout << "nmfSSCAA_Tab1::loadWidgets() Species: " << species.toStdString() << std::endl;

    if (! isCatchAtAge(species)) {
        nmfMSCAAUtils::loadCatchAtLengthTable(m_databasePtr,m_logger,
                                 m_projectSettingsConfig,
                                 SSCAA_Tab1_CatchAtLengthTV,
                                 "CatchAtLengthFishery",
                                 species,fleet);
    }

    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_projectSettingsConfig,
                             SSCAA_Tab1_CatchAtAgeTV,
                             "CatchFishery",
                             species,fleet,
                             abundanceUnits,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
    setAbundanceUnits(abundanceUnits);

    nmfMSCAAUtils::loadTheTotalTable(m_databasePtr,m_logger,
                             m_projectSettingsConfig,
                             SSCAA_Tab1_TotalTV,
                             "CatchFisheryTotal",
                             species,fleet,
                             weightUnits);
    setWeightUnits(weightUnits);

    return true;
}

void
nmfSSCAA_Tab1::readSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_projectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSSCAA_Tab1::callback_AbundanceUnitsChanged(QString newUnits)
{
    double unitsSF = 1.0;

    if (newUnits == "Fish") {
        if (m_previousAbundanceUnits == "000 Fish") {
            unitsSF = 0.001;
        } else if (m_previousAbundanceUnits == "000 000 Fish") {
            unitsSF = 0.000001;
        }
    } else if (newUnits == "000 Fish") {
        if (m_previousAbundanceUnits == "Fish") {
            unitsSF = 1000.0;
        } else if (m_previousAbundanceUnits == "000 000 Fish") {
            unitsSF = 0.001;
        }
    } else if (newUnits == "000 000 Fish") {
        if (m_previousAbundanceUnits == "Fish") {
            unitsSF = 1000000.0;
        } else if (m_previousAbundanceUnits == "000 Fish") {
            unitsSF = 1000.0;
        }
    }
    m_previousAbundanceUnits = newUnits;

    nmfMSCAAUtils::rescaleModel(SSCAA_Tab1_CatchAtAgeTV,unitsSF);
}

void
nmfSSCAA_Tab1::callback_FleetCMB(QString fleet)
{
    loadWidgets();
}

void
nmfSSCAA_Tab1::callback_WeightUnitsCMB(QString newUnits)
{
    double unitsSF = 1.0;

    if (newUnits == "Metric Tons") {
        if (m_previousWeightUnits == "000 Metric Tons") {
            unitsSF = 0.001;
        } else if (m_previousWeightUnits == "000 000 Metric Tons") {
            unitsSF = 0.000001;
        }
    } else if (newUnits == "000 Metric Tons") {
        if (m_previousWeightUnits == "Metric Tons") {
            unitsSF = 1000.0;
        } else if (m_previousWeightUnits == "000 000 Metric Tons") {
            unitsSF = 0.001;
        }
    } else if (newUnits == "000 000 Metric Tons") {
        if (m_previousWeightUnits == "Metric Tons") {
            unitsSF = 1000000.0;
        } else if (m_previousWeightUnits == "000 Metric Tons") {
            unitsSF = 1000.0;
        }
    }
    m_previousWeightUnits = newUnits;

    nmfMSCAAUtils::rescaleModel(SSCAA_Tab1_TotalTV,unitsSF);
}
