
//#include "nmfUtilsQt.h"
//#include "nmfSSCAATab05.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"

#include "nmfSSCAATab05.h"
#include "nmfMSCAATableIO.h"


nmfSSCAA_Tab5::nmfSSCAA_Tab5(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    SSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_currentSegmentIndex = 0;

    ReadSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfSSCAA_Tab5::nmfSSCAA_Tab5");

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/SSCAA/SSCAA_Tab05.ui");
    file.open(QFile::ReadOnly);
    SSCAA_Tab5_Widget = loader.load(&file,SSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    SSCAA_Tabs->addTab(SSCAA_Tab5_Widget, tr("5. Mortality"));
    SSCAA_Tab5_NaturalMortalityTV   = new QTableView(SSCAA_Tabs);
    SSCAA_Tab5_FishingMortalityTV   = new QTableView(SSCAA_Tabs);
    SSCAA_Tab5_SpeciesLBL    = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab5_SpeciesLBL");
    SSCAA_Tab5_PrevPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab5_PrevPB");
    SSCAA_Tab5_NextPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab5_NextPB");
    SSCAA_Tab5_LoadPB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab5_ReloadPB");
    SSCAA_Tab5_SavePB        = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab5_SavePB");
    SSCAA_Tab5_NumSegCMB     = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab5_NumSegCMB");
    SSCAA_Tab5_AutoFillSegCB = SSCAA_Tabs->findChild<QCheckBox   *>("SSCAA_Tab5_AutoFillSegCB");
    SSCAA_Tab5_YearlyFCB     = SSCAA_Tabs->findChild<QCheckBox   *>("SSCAA_Tab5_YearlyFCB");
    SSCAA_Tab5_TVLayoutLT    = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab5_TVLayoutLT");
    SSCAA_Tab5_TVLayoutLT2   = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab5_TVLayoutLT2");
    SSCAA_Tab5_TVLayoutLT->addWidget(SSCAA_Tab5_NaturalMortalityTV);
    SSCAA_Tab5_TVLayoutLT2->addWidget(SSCAA_Tab5_FishingMortalityTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    SSCAA_Tab5_NaturalMortalityTV->setFont(noBoldFont);
    SSCAA_Tab5_FishingMortalityTV->setFont(noBoldFont);

    connect(SSCAA_Tab5_PrevPB, SIGNAL(clicked()),
            this,              SLOT(callback_PrevPB()));
    connect(SSCAA_Tab5_NextPB, SIGNAL(clicked()),
            this,              SLOT(callback_NextPB()));
    connect(SSCAA_Tab5_LoadPB, SIGNAL(clicked()),
            this,              SLOT(callback_LoadPB()));
    connect(SSCAA_Tab5_SavePB, SIGNAL(clicked()),
            this,              SLOT(callback_SavePB()));
    connect(SSCAA_Tab5_AutoFillSegCB, SIGNAL(clicked(bool)),
            this,                     SLOT(callback_AutoFillSegPB(bool)));
    connect(SSCAA_Tab5_YearlyFCB, SIGNAL(clicked(bool)),
            this,                 SLOT(callback_YearlyFPB(bool)));
    connect(SSCAA_Tab5_NumSegCMB, SIGNAL(activated(int)),
            this,                 SLOT(callback_NumSegCMB(int)));

    SSCAA_Tab5_PrevPB->setText("\u25C1--");
    SSCAA_Tab5_NextPB->setText("--\u25B7");

} // end constructor


nmfSSCAA_Tab5::~nmfSSCAA_Tab5()
{

}


QTableView*
nmfSSCAA_Tab5::getNaturalMortalityTable()
{
    return SSCAA_Tab5_NaturalMortalityTV;
}

QTableView*
nmfSSCAA_Tab5::getFishingMortalityTable()
{
    return SSCAA_Tab5_FishingMortalityTV;
}

std::string
nmfSSCAA_Tab5::getSpecies()
{
    return SSCAA_Tab5_SpeciesLBL->text().toStdString();
}

bool
nmfSSCAA_Tab5::isAutoFillChecked()
{
    return SSCAA_Tab5_AutoFillSegCB->isChecked();
}

int
nmfSSCAA_Tab5::getNumSegments()
{
    return SSCAA_Tab5_NumSegCMB->currentText().toInt();
}

void
nmfSSCAA_Tab5::setNumSegments(int numSegments)
{
    SSCAA_Tab5_NumSegCMB->setCurrentText(QString::number(numSegments));
}


void
nmfSSCAA_Tab5::speciesChanged(QString species)
{
    SSCAA_Tab5_SpeciesLBL->clear();
    if (loadNumSegments(species) > 0) {
        SSCAA_Tab5_SpeciesLBL->setText(species);
        callback_NumSegCMB(m_currentSegmentIndex);
        callback_LoadPB();
    }
}

int
nmfSSCAA_Tab5::loadNumSegments(QString Species)
{
    int MinAge,MaxAge,FirstYear,LastYear;
    float MinLength,MaxLength;
    int NumLengthBins;

    if (! m_databasePtr->getSpeciesData(m_logger,Species.toStdString(),
                                      MinAge,MaxAge,FirstYear,LastYear,
                                      MinLength,MaxLength,NumLengthBins))
    {
        return 0;
    }

    // Set up combo box
    SSCAA_Tab5_NumSegCMB->clear();
    for (int seg = 1; seg <= LastYear-FirstYear+1; ++seg) {
        SSCAA_Tab5_NumSegCMB->addItem(QString::number(seg));
    }

    if (m_currentSegmentIndex < SSCAA_Tab5_NumSegCMB->count()) {
        SSCAA_Tab5_NumSegCMB->setCurrentIndex(m_currentSegmentIndex);
    }
    return SSCAA_Tab5_NumSegCMB->count();
}

void
nmfSSCAA_Tab5::callback_PrevPB()
{
    int prevPage = SSCAA_Tabs->currentIndex()-1;
    SSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfSSCAA_Tab5::callback_NextPB()
{
    int nextPage = SSCAA_Tabs->currentIndex()+1;
    SSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfSSCAA_Tab5::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSSCAA_Tab5::callback_SavePB()
{
    bool ok = false;
    QString msg = "\n";

    if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab5_NaturalMortalityTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveBinnedTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                       SSCAA_Tab5_NaturalMortalityTV,
                                       m_ProjectSettingsConfig,
                                       "MortalityNatural",
                                       getNumSegments(), "",
                                       QString::fromStdString(getSpecies()),
                                       QString(""));
        if (ok) {
            msg += "Natural Mortality table has been successfully updated.\n";
        }
    }

    if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab5_FishingMortalityTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveBinnedTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                       SSCAA_Tab5_FishingMortalityTV,
                                       m_ProjectSettingsConfig,
                                       "MortalityFishing",
                                       getNumSegments(), "",
                                       QString::fromStdString(getSpecies()),
                                       QString(""));
        if (ok) {
            msg += "Fishing Mortality table has been successfully updated.\n";
        }
    }

    if (msg != "\n") {
        QMessageBox::information(SSCAA_Tabs, "Mortality Table(s) Updated", msg, QMessageBox::Ok);
    }
}


void
nmfSSCAA_Tab5::callback_NumSegCMB(int value)
{
    m_currentSegmentIndex = value;
    nmfMSCAAUtils::createNewNumberOfBinsTable(m_databasePtr,m_logger,
                                        m_ProjectSettingsConfig,
                                        SSCAA_Tab5_NaturalMortalityTV,
                                        "MortalityNatural",
                                        getNumSegments(),
                                        QString::fromStdString(getSpecies()),
                                        "",
                                        isAutoFillChecked());
    nmfMSCAAUtils::createNewNumberOfBinsTable(m_databasePtr,m_logger,
                                        m_ProjectSettingsConfig,
                                        SSCAA_Tab5_FishingMortalityTV,
                                        "MortalityFishing",
                                        getNumSegments(),
                                        QString::fromStdString(getSpecies()),
                                        "",
                                        isAutoFillChecked());
}

void
nmfSSCAA_Tab5::callback_AutoFillSegPB(bool isAutoFillChecked)
{
    nmfMSCAAUtils::createNewNumberOfBinsTable(m_databasePtr,m_logger,
                                        m_ProjectSettingsConfig,
                                        SSCAA_Tab5_NaturalMortalityTV,
                                        "MortalityNatural",
                                        getNumSegments(),
                                        QString::fromStdString(getSpecies()),
                                        "",
                                        isAutoFillChecked);
    nmfMSCAAUtils::createNewNumberOfBinsTable(m_databasePtr,m_logger,
                                        m_ProjectSettingsConfig,
                                        SSCAA_Tab5_FishingMortalityTV,
                                        "MortalityFishing",
                                        getNumSegments(),
                                        QString::fromStdString(getSpecies()),
                                        "",
                                        isAutoFillChecked);
}


// Set all age group values to the first age group's
void
nmfSSCAA_Tab5::callback_YearlyFPB(bool isChecked)
{
    if (isChecked) {
        nmfMSCAAUtils::setColumnsToFirst(SSCAA_Tab5_FishingMortalityTV);
    } else {
        loadWidgets();
    }
}

void
nmfSSCAA_Tab5::ReadSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSSCAA_Tab5::clearWidgets()
{
    nmfUtilsQt::clearTableView({SSCAA_Tab5_NaturalMortalityTV,
                                SSCAA_Tab5_FishingMortalityTV});
}


bool
nmfSSCAA_Tab5::loadWidgets()
{
std::cout << "nmfSSCAA_Tab5::loadWidgets()" << std::endl;
    int numSegments;
    QString binType;

    ReadSettings();

    clearWidgets();

    std::string species = getSpecies();
    if (species.empty())
        return false;

    if (loadNumSegments(QString::fromStdString(species)) <= 0) {
        return false;
    }

    // Get num bins from saved file and set combobox appropriately
    numSegments = nmfMSCAAUtils::getNumBins(m_databasePtr,m_logger,
                                            m_ProjectSettingsConfig,
                                            "MortalityNatural", binType,
                                            QString::fromStdString(species),
                                            "");

    if (numSegments == 0) {
        callback_NumSegCMB(0);
    } else {
        setNumSegments(numSegments); // +1 since segment counting starts with 0
        nmfMSCAAUtils::loadBinnedTable(m_databasePtr,m_logger,
                                       m_ProjectSettingsConfig,
                                       SSCAA_Tab5_NaturalMortalityTV,
                                       "MortalityNatural",
                                       numSegments,"",
                                       QString::fromStdString(species),
                                       "");
        nmfMSCAAUtils::loadBinnedTable(m_databasePtr,m_logger,
                                       m_ProjectSettingsConfig,
                                       SSCAA_Tab5_FishingMortalityTV,
                                       "MortalityFishing",
                                       numSegments,"",
                                       QString::fromStdString(species),
                                       "");
    }

    return true;
}



