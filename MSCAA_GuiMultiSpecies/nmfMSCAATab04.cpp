
//#include "nmfUtilsQt.h"
#include "nmfMSCAATab04.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"


nmfMSCAA_Tab4::nmfMSCAA_Tab4(QTabWidget  *tabs,
                             nmfLogger   *logger,
                             nmfDatabase *databasePtr,
                             std::string &projectDir)
{
    QUiLoader loader;

    MSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;
    readSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfMSCAA_Tab4::nmfMSCAA_Tab4");

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/MSCAA/MSCAA_Tab04.ui");
    file.open(QFile::ReadOnly);
    MSCAA_Tab4_Widget = loader.load(&file,MSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    MSCAA_Tabs->addTab(MSCAA_Tab4_Widget, tr("4. Consumption:Biomass"));

    MSCAA_Tab4_ConsumptionTV = new QTableView(MSCAA_Tabs);
    MSCAA_Tab4_SpeciesLBL    = MSCAA_Tabs->findChild<QLabel      *>("MSCAA_Tab4_SpeciesLBL");
    MSCAA_Tab4_PrevPB        = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab4_PrevPB");
    MSCAA_Tab4_NextPB        = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab4_NextPB");
    MSCAA_Tab4_LoadPB        = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab4_ReloadPB");
    MSCAA_Tab4_SavePB        = MSCAA_Tabs->findChild<QPushButton *>("MSCAA_Tab4_SavePB");
    MSCAA_Tab4_UnitsCMB      = MSCAA_Tabs->findChild<QComboBox   *>("MSCAA_Tab4_UnitsCMB");
    MSCAA_Tab4_TVLayoutLT    = MSCAA_Tabs->findChild<QVBoxLayout *>("MSCAA_Tab4_TVLayoutLT");
    MSCAA_Tab4_TVLayoutLT->addWidget(MSCAA_Tab4_ConsumptionTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    MSCAA_Tab4_ConsumptionTV->setFont(noBoldFont);

    // Hide the units combobox for now as the table is unitless
    MSCAA_Tab4_UnitsCMB->hide();

    connect(MSCAA_Tab4_PrevPB, SIGNAL(clicked()),
            this,                   SLOT(callback_PrevPB()));
    connect(MSCAA_Tab4_NextPB, SIGNAL(clicked()),
            this,                   SLOT(callback_NextPB()));
    connect(MSCAA_Tab4_LoadPB, SIGNAL(clicked()),
            this,                   SLOT(callback_LoadPB()));
    connect(MSCAA_Tab4_SavePB, SIGNAL(clicked()),
            this,                   SLOT(callback_SavePB()));

    MSCAA_Tab4_PrevPB->setText("\u25C1--");
    MSCAA_Tab4_NextPB->setText("--\u25B7");

} // end constructor


nmfMSCAA_Tab4::~nmfMSCAA_Tab4()
{

}


QTableView*
nmfMSCAA_Tab4::getTableConsumptionBiomass()
{
    return MSCAA_Tab4_ConsumptionTV;
}

void
nmfMSCAA_Tab4::setUnits(std::string units)
{
    if (! units.empty()) {
        MSCAA_Tab4_UnitsCMB->setCurrentText(QString::fromStdString(units));
    }
}

std::string
nmfMSCAA_Tab4::getUnits()
{
    return MSCAA_Tab4_UnitsCMB->currentText().toStdString();
}

std::string
nmfMSCAA_Tab4::getSpecies()
{
    return MSCAA_Tab4_SpeciesLBL->text().toStdString();
}

void
nmfMSCAA_Tab4::changeSpecies(QString species)
{
    std::string units;

    MSCAA_Tab4_SpeciesLBL->setText(species);
    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             MSCAA_Tab4_ConsumptionTV,
                             nmfConstantsMSCAA::TableConsumption,species,
                             std::to_string(0),units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
    setUnits(units);
}

void
nmfMSCAA_Tab4::callback_PrevPB()
{
    int prevPage = MSCAA_Tabs->currentIndex()-1;
    MSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfMSCAA_Tab4::callback_NextPB()
{
    int nextPage = MSCAA_Tabs->currentIndex()+1;
    MSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfMSCAA_Tab4::callback_LoadPB()
{
    loadWidgets();
}

void
nmfMSCAA_Tab4::callback_SavePB()
{
    bool ok;
    QString msg;

    if (nmfUtilsQt::allCellsArePopulated(MSCAA_Tabs,MSCAA_Tab4_ConsumptionTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveTable(MSCAA_Tabs,m_databasePtr,m_logger,
                                 MSCAA_Tab4_ConsumptionTV,
                                 m_ProjectSettingsConfig,
                                 nmfConstantsMSCAA::TableConsumption,getSpecies(),
                                 "0",getUnits(),
                                 nmfConstants::IsProportion,
                                 nmfConstants::dontIncludeTotalColumn,
                                 nmfConstants::AllYears);
        if (ok) {
            msg = "\nConsumption table has been successfully updated.\n";
            QMessageBox::information(MSCAA_Tabs, "Consumption Updated", msg, QMessageBox::Ok);
        }
    }
}

void
nmfMSCAA_Tab4::readSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfMSCAA_Tab4::clearWidgets()
{
    nmfUtilsQt::clearTableView({MSCAA_Tab4_ConsumptionTV});
}

bool
nmfMSCAA_Tab4::loadWidgets()
{
    std::string units="";
std::cout << "nmfMSCAA_Tab4::loadWidgets()" << std::endl;

    clearWidgets();

    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             MSCAA_Tab4_ConsumptionTV,
                             nmfConstantsMSCAA::TableConsumption,
                             QString::fromStdString(getSpecies()),
                             std::to_string(0),units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
    setUnits(units);

    return true;
}
