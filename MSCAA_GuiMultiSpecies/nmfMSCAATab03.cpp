
//#include "nmfUtilsQt.h"
#include "nmfMSCAATab03.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"


nmfMSCAA_Tab3::nmfMSCAA_Tab3(QTabWidget  *tabs,
                             nmfLogger   *logger,
                             nmfDatabase *databasePtr,
                             std::string &projectDir)
{
    QUiLoader loader;

    MSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_currentSegmentIndex = 0;

    m_logger->logMsg(nmfConstants::Normal,"nmfMSCAA_Tab3::nmfMSCAA_Tab3");

    readSettings();

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/MSCAA/MSCAA_Tab03.ui");
    file.open(QFile::ReadOnly);
    MSCAA_Tab3_Widget = loader.load(&file,MSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    MSCAA_Tabs->addTab(MSCAA_Tab3_Widget, tr("3. Predator Diet"));

    MSCAA_Tab3_DietTV             = new QTableView();
    MSCAA_Tab3_NextPB             = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab3_NextPB");
    MSCAA_Tab3_PrevPB             = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab3_PrevPB");
    MSCAA_Tab3_LoadPB             = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab3_LoadPB");
    MSCAA_Tab3_SavePB             = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab3_SavePB");
    MSCAA_Tab3_PredatorSpeciesCMB = MSCAA_Tabs->findChild<QComboBox   *>("MSCAA_Tab3_PredatorSpeciesCMB");
    MSCAA_Tab3_BinTypeCMB         = MSCAA_Tabs->findChild<QComboBox   *>("MSCAA_Tab3_BinTypeCMB");
    MSCAA_Tab3_PredatorAgeCMB     = MSCAA_Tabs->findChild<QComboBox   *>("MSCAA_Tab3_PredatorAgeCMB");
    MSCAA_Tab3_UnitsCMB           = MSCAA_Tabs->findChild<QComboBox   *>("MSCAA_Tab3_UnitsCMB");
    MSCAA_Tab3_NumSegCMB          = MSCAA_Tabs->findChild<QComboBox   *>("MSCAA_Tab3_NumSegCMB");
    MSCAA_Tab3_AutoFillSegCB      = MSCAA_Tabs->findChild<QCheckBox   *>("MSCAA_Tab3_AutoFillSegCB");
    MSCAA_Tab3_TVLayoutLT         = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab3_TVLayoutLT");
    MSCAA_Tab3_TVLayoutLT->addWidget(MSCAA_Tab3_DietTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    MSCAA_Tab3_DietTV->setFont(noBoldFont);

    // Hide the units combobox for now as the table is unitless
    MSCAA_Tab3_UnitsCMB->hide();

    // Load combo boxes
    nmfMSCAAUtils::loadComboBox(m_databasePtr,m_logger,MSCAA_Tab3_PredatorSpeciesCMB,"Name","");
    callback_PredatorSpeciesCMB(MSCAA_Tab3_PredatorSpeciesCMB->currentText());


    connect(MSCAA_Tab3_NextPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_NextPB()));
    connect(MSCAA_Tab3_PrevPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_PrevPB()));
    connect(MSCAA_Tab3_LoadPB,             SIGNAL(clicked()),
            this,                          SLOT(callback_LoadPB()));
    connect(MSCAA_Tab3_SavePB,             SIGNAL(clicked()),
            this,                          SLOT(callback_SavePB()));
    connect(MSCAA_Tab3_PredatorSpeciesCMB, SIGNAL(activated(QString)),
            this,                          SLOT(callback_PredatorSpeciesCMB(QString)));
    connect(MSCAA_Tab3_BinTypeCMB,         SIGNAL(activated(QString)),
            this,                          SLOT(callback_BinTypeCMB(QString)));
    connect(MSCAA_Tab3_PredatorAgeCMB,     SIGNAL(activated(QString)),
            this,                          SLOT(callback_PredatorAgeCMB(QString)));
    connect(MSCAA_Tab3_AutoFillSegCB,      SIGNAL(clicked(bool)),
            this,                          SLOT(callback_AutoFillSegCB(bool)));
    connect(MSCAA_Tab3_NumSegCMB,          SIGNAL(activated(int)),
            this,                          SLOT(callback_NumSegCMB(int)));

    MSCAA_Tab3_NextPB->setText("--\u25B7");
    MSCAA_Tab3_PrevPB->setText("\u25C1--");

    // Set default states
    MSCAA_Tab3_AutoFillSegCB->setChecked(true);

} // end constructor


nmfMSCAA_Tab3::~nmfMSCAA_Tab3()
{

}

bool
nmfMSCAA_Tab3::isAutoFillChecked()
{
    return MSCAA_Tab3_AutoFillSegCB->isChecked();
}

int
nmfMSCAA_Tab3::getNumBins()
{
    return MSCAA_Tab3_NumSegCMB->currentText().toInt();
}

void
nmfMSCAA_Tab3::setNumBins(int numBins)
{
    MSCAA_Tab3_NumSegCMB->setCurrentText(QString::number(numBins));
}

int
nmfMSCAA_Tab3::getNumSpecies()
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
nmfMSCAA_Tab3::getTableDiet()
{
    return MSCAA_Tab3_DietTV;
}

void
nmfMSCAA_Tab3::setUnits(std::string units)
{
    MSCAA_Tab3_UnitsCMB->setCurrentText(QString::fromStdString(units));
}

std::string
nmfMSCAA_Tab3::getUnits()
{
    return MSCAA_Tab3_UnitsCMB->currentText().toStdString();
}
QString
nmfMSCAA_Tab3::getPredatorSpecies()
{
    return MSCAA_Tab3_PredatorSpeciesCMB->currentText();
}

QString
nmfMSCAA_Tab3::getPredatorAge()
{
    return MSCAA_Tab3_PredatorAgeCMB->currentText().split(" ")[1];
}


int
nmfMSCAA_Tab3::loadNumBins(QString species)
{
    int MinAge,MaxAge,FirstYear,LastYear;
    float MinLength,MaxLength;
    int NumLengthBins;

    if (! m_databasePtr->getSpeciesData(m_logger,species.toStdString(),
                                      MinAge,MaxAge,FirstYear,LastYear,
                                      MinLength,MaxLength,NumLengthBins)) {
        return 0;
    }

    // Set up combo box
    MSCAA_Tab3_NumSegCMB->clear();
    for (int seg = 1; seg <= LastYear-FirstYear+1; ++seg) {
        MSCAA_Tab3_NumSegCMB->addItem(QString::number(seg));
    }

    if (m_currentSegmentIndex < MSCAA_Tab3_NumSegCMB->count()) {
        MSCAA_Tab3_NumSegCMB->setCurrentIndex(m_currentSegmentIndex);
    }
    return MSCAA_Tab3_NumSegCMB->count();

}


void
nmfMSCAA_Tab3::callback_BinTypeCMB(QString binType)
{
    MSCAA_Tab3_AutoFillSegCB->setEnabled(true);
    MSCAA_Tab3_AutoFillSegCB->setChecked(false);
    if (binType == "Years per Bin") {
        MSCAA_Tab3_NumSegCMB->setToolTip("Number of Years per Bin");
        MSCAA_Tab3_NumSegCMB->setStatusTip("Number of Years per Bin");
        MSCAA_Tab3_AutoFillSegCB->setChecked(true);
        MSCAA_Tab3_AutoFillSegCB->setEnabled(false);
    } else if (binType == "Number of Bins") {
        MSCAA_Tab3_NumSegCMB->setToolTip("Number of Bins");
        MSCAA_Tab3_NumSegCMB->setStatusTip("Number of Bins");
    }
    callback_NumSegCMB(MSCAA_Tab3_NumSegCMB->currentText().toInt());
}

void
nmfMSCAA_Tab3::callback_PredatorSpeciesCMB(QString currentSpecies)
{
    int numBins = loadNumBins(currentSpecies);
    m_logger->logMsg(nmfConstants::Normal,"nmfMSCAA_Tab3::callback_PredatorSpeciesCMB numBins = "+std::to_string(numBins));
    if (numBins == 0) {
        return;
    }

    nmfMSCAAUtils::loadComboBox(m_databasePtr,m_logger,
                                MSCAA_Tab3_PredatorAgeCMB,
                                "Age",currentSpecies);
    loadWidgets();
}

void
nmfMSCAA_Tab3::callback_PredatorAgeCMB(QString age)
{
    loadWidgets();
}

void
nmfMSCAA_Tab3::callback_NextPB()
{
    int nextPage = MSCAA_Tabs->currentIndex()+1;
    MSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfMSCAA_Tab3::callback_PrevPB()
{
    int prevPage = MSCAA_Tabs->currentIndex()-1;
    MSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfMSCAA_Tab3::callback_LoadPB()
{
    loadWidgets();
}

QString
nmfMSCAA_Tab3::getBinType()
{
    return MSCAA_Tab3_BinTypeCMB->currentText();
}

void
nmfMSCAA_Tab3::callback_SavePB()
{
    bool ok;
    QString msg;
    QString binType = getBinType();

    if (nmfUtilsQt::allCellsArePopulated(MSCAA_Tabs,MSCAA_Tab3_DietTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveBinnedTable(MSCAA_Tabs,
                                       m_databasePtr,
                                       m_logger,
                                       MSCAA_Tab3_DietTV,
                                       m_ProjectSettingsConfig,
                                       "Diet", getNumBins(),
                                       binType,
                                       getPredatorSpecies(),
                                       getPredatorAge());
        if (ok) {
            msg = "\nDiet table has been successfully updated.\n";
            QMessageBox::information(MSCAA_Tabs, "Diet Updated", msg, QMessageBox::Ok);
        }
    }
}

void
nmfMSCAA_Tab3::callback_NumSegCMB(int value)
{
    QString binType = getBinType();
    QString Species = getPredatorSpecies();
    m_currentSegmentIndex = value;

    if (binType == "Number of Bins") {
        nmfMSCAAUtils::createNewNumberOfBinsTable(m_databasePtr,m_logger,
                                                  m_ProjectSettingsConfig,
                                                  MSCAA_Tab3_DietTV,
                                                  "Diet", getNumBins(),
                                                  Species,
                                                  getPredatorAge(),
                                                  isAutoFillChecked());
    } else if (binType == "Years per Bin") {
        nmfMSCAAUtils::createNewYearsPerBinTable(m_databasePtr,m_logger,
                                                 m_ProjectSettingsConfig,
                                                 MSCAA_Tab3_DietTV,
                                                 "Diet", getNumBins(),
                                                 Species,
                                                 getPredatorAge(),
                                                 isAutoFillChecked());
    }
}

void
nmfMSCAA_Tab3::callback_AutoFillSegCB(bool isAutoFillChecked)
{
    QString binType = getBinType();
    QString Species = getPredatorSpecies();

    if (binType == "Number of Bins") {
        nmfMSCAAUtils::createNewNumberOfBinsTable(m_databasePtr,m_logger,
                                                  m_ProjectSettingsConfig,
                                                  MSCAA_Tab3_DietTV,
                                                  "Diet", getNumBins(),
                                                  Species,
                                                  getPredatorAge(),
                                                  isAutoFillChecked);
    } else if (binType == "Years per Bin") {
        nmfMSCAAUtils::createNewYearsPerBinTable(m_databasePtr,m_logger,
                                                 m_ProjectSettingsConfig,
                                                 MSCAA_Tab3_DietTV,
                                                 "Diet", getNumBins(),
                                                 Species,
                                                 getPredatorAge(),
                                                 isAutoFillChecked);
    }
}

void
nmfMSCAA_Tab3::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfMSCAA_Tab3::clearWidgets()
{
    nmfUtilsQt::clearTableView({MSCAA_Tab3_DietTV});
}

bool
nmfMSCAA_Tab3::loadWidgets()
{
std::cout << "nmfMSCAA_Tab3::loadWidgets()" << std::endl;
    int numBins;
    QString binType;

    readSettings();

    clearWidgets();

    QString predatorSpecies = getPredatorSpecies();
    if (getNumSpecies() == 0)
        return false;

    if (loadNumBins(predatorSpecies) == 0) {
        return false;
    }

    // Get num bins from saved file and set combobox appropriately
    numBins = nmfMSCAAUtils::getNumBins(m_databasePtr,m_logger,
                                        m_ProjectSettingsConfig,
                                        "Diet", binType,
                                        predatorSpecies,
                                        getPredatorAge());

    if (numBins == 0) {
        callback_NumSegCMB(0);
    } else {
        setNumBins(numBins);
        nmfMSCAAUtils::loadBinnedTable(m_databasePtr,m_logger,
                                       m_ProjectSettingsConfig,
                                       MSCAA_Tab3_DietTV,
                                       "Diet", numBins, binType,
                                       predatorSpecies,
                                       getPredatorAge());
    }

    // Adjust GUI elements based upon bin type
    MSCAA_Tab3_BinTypeCMB->blockSignals(true);
    MSCAA_Tab3_BinTypeCMB->setCurrentText(binType);
    MSCAA_Tab3_BinTypeCMB->blockSignals(false);
    MSCAA_Tab3_AutoFillSegCB->blockSignals(true);
    MSCAA_Tab3_AutoFillSegCB->setChecked(true);
    MSCAA_Tab3_AutoFillSegCB->blockSignals(false);
    if (binType == "Years per Bin") {
        MSCAA_Tab3_AutoFillSegCB->setEnabled(false);
    }

    return true;
}
