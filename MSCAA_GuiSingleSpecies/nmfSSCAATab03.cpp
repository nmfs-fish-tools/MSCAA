
//#include "nmfUtilsQt.h"
//#include "nmfSSCAATab03.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"

#include "nmfSSCAATab03.h"
#include "nmfMSCAATableIO.h"

nmfSSCAA_Tab3::nmfSSCAA_Tab3(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    SSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;
    ReadSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfSSCAA_Tab3::nmfSSCAA_Tab3");

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/SSCAA/SSCAA_Tab03.ui");
    file.open(QFile::ReadOnly);
    SSCAA_Tab3_Widget = loader.load(&file,SSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    SSCAA_Tabs->addTab(SSCAA_Tab3_Widget, tr("3. Weight-At-Age"));

    SSCAA_Tab3_WeightTV   = new QTableView(SSCAA_Tabs);
    SSCAA_Tab3_SpeciesLBL = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab3_SpeciesLBL");
    SSCAA_Tab3_PrevPB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab3_PrevPB");
    SSCAA_Tab3_NextPB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab3_NextPB");
    SSCAA_Tab3_WeightGB   = SSCAA_Tabs->findChild<QGroupBox   *>("SSCAA_Tab3_WeightGB");
    SSCAA_Tab3_LoadPB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab3_ReloadPB");
    SSCAA_Tab3_SavePB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab3_SavePB");
    SSCAA_Tab3_UnitsCMB   = SSCAA_Tabs->findChild<QComboBox   *>("SSCAA_Tab3_UnitsCMB");
    SSCAA_Tab3_TVLayoutLT = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab3_TVLayoutLT");
    SSCAA_Tab3_TVLayoutLT->addWidget(SSCAA_Tab3_WeightTV);

    // Load units
    SSCAA_Tab3_UnitsCMB->addItem("Kilograms");
    SSCAA_Tab3_UnitsCMB->addItem("Grams");
    //SSCAA_Tab3_UnitsCMB->addItem("Pounds");
    m_originalUnits = "Kilograms";

    QFont noBoldFont;
    noBoldFont.setBold(false);
    SSCAA_Tab3_WeightTV->setFont(noBoldFont);

    connect(SSCAA_Tab3_PrevPB,   SIGNAL(clicked()),
            this,                SLOT(callback_PrevPB()));
    connect(SSCAA_Tab3_NextPB,   SIGNAL(clicked()),
            this,                SLOT(callback_NextPB()));
    connect(SSCAA_Tab3_LoadPB,   SIGNAL(clicked()),
            this,                SLOT(callback_LoadPB()));
    connect(SSCAA_Tab3_SavePB,   SIGNAL(clicked()),
            this,                SLOT(callback_SavePB()));
    connect(SSCAA_Tab3_UnitsCMB, SIGNAL(currentTextChanged(QString)),
            this,                SLOT(callback_UnitsChanged(QString)));

    SSCAA_Tab3_PrevPB->setText("\u25C1--");
    SSCAA_Tab3_NextPB->setText("--\u25B7");

} // end constructor


nmfSSCAA_Tab3::~nmfSSCAA_Tab3()
{

}


QTableView*
nmfSSCAA_Tab3::getTable()
{
    return SSCAA_Tab3_WeightTV;
}

void
nmfSSCAA_Tab3::setUnits(std::string units)
{
    if (units.empty()) {
        units = "Kilograms";
    }
    SSCAA_Tab3_UnitsCMB->blockSignals(true);
    SSCAA_Tab3_UnitsCMB->setCurrentText(QString::fromStdString(units));
    SSCAA_Tab3_UnitsCMB->blockSignals(false);
    m_originalUnits = QString::fromStdString(units);
}

std::string
nmfSSCAA_Tab3::getUnits()
{
    return SSCAA_Tab3_UnitsCMB->currentText().toStdString();
}

std::string
nmfSSCAA_Tab3::getSpecies()
{
    return SSCAA_Tab3_SpeciesLBL->text().toStdString();
}

void
nmfSSCAA_Tab3::speciesChanged(QString species)
{

    std::string units;

    SSCAA_Tab3_SpeciesLBL->setText(species);
    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             SSCAA_Tab3_WeightTV,
                             nmfConstantsMSCAA::TableWeight,
                             species,std::to_string(0),
                             units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
    setUnits(units);
}

void
nmfSSCAA_Tab3::callback_PrevPB()
{
    int prevPage = SSCAA_Tabs->currentIndex()-1;
    SSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfSSCAA_Tab3::callback_NextPB()
{
    int nextPage = SSCAA_Tabs->currentIndex()+1;
    SSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfSSCAA_Tab3::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSSCAA_Tab3::callback_SavePB()
{
    bool ok = false;
    QString msg;

    if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab3_WeightTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                 SSCAA_Tab3_WeightTV,
                                 m_ProjectSettingsConfig,
                                 nmfConstantsMSCAA::TableWeight,getSpecies(),
                                 "0",getUnits(),
                                 nmfConstants::IsNotProportion,
                                 nmfConstants::dontIncludeTotalColumn,
                                 nmfConstants::AllYears);
        if (ok) {
            msg = "\nWeight table has been successfully updated.\n";
            QMessageBox::information(SSCAA_Tabs, "Weight Updated", msg, QMessageBox::Ok);
        }
    }
}

void
nmfSSCAA_Tab3::ReadSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSSCAA_Tab3::clearWidgets()
{
    nmfUtilsQt::clearTableView({SSCAA_Tab3_WeightTV});
}

bool
nmfSSCAA_Tab3::loadWidgets()
{
std::cout << "nmfSSCAA_Tab3::loadWidgets()" << std::endl;
    std::string units;
    ReadSettings();

    clearWidgets();

    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             SSCAA_Tab3_WeightTV,
                             nmfConstantsMSCAA::TableWeight,
                             QString::fromStdString(getSpecies()),
                             std::to_string(0),units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
std::cout << "Setting units to: " << units << std::endl;
    setUnits(units);

    return true;
}

void
nmfSSCAA_Tab3::callback_UnitsChanged(QString newUnits)
{
    double unitsSF = 1.0;
    if (newUnits == "Kilograms") {
        if (m_originalUnits == "Grams") {
            unitsSF = 1000;
        }
    } else if (newUnits == "Grams") {
        if (m_originalUnits == "Kilograms") {
            unitsSF = 0.001;
        }
    }
    /*
    if (newUnits == "Kilograms") {
        if (originalUnits == "Pounds") {
            unitsSF = nmfConstants::KgToLbs;
        } else if (originalUnits == "000 Pounds") {
            unitsSF = nmfConstants::KgToLbs/1000.0;
        }
    } else if (newUnits == "Pounds") {
        if (originalUnits == "Kilograms") {
            unitsSF = 1.0/nmfConstants::KgToLbs;
        } else if (originalUnits == "000 Pounds") {
            unitsSF = 1.0/1000.0;
        }
    } else if (newUnits == "000 Pounds") {
        if (originalUnits == "Kilograms") {
            unitsSF = 1000.0/nmfConstants::KgToLbs;
        } else if (originalUnits == "Pounds") {
            unitsSF = 1000.0;
        }
    }
    */
    m_originalUnits = newUnits;

    nmfMSCAAUtils::rescaleModel(SSCAA_Tab3_WeightTV,unitsSF);
}
