
//#include "nmfUtilsQt.h"
//#include "nmfSSCAATab04.h"
//#include "nmfUtils.h"
//#include "nmfConstants.h"

#include "nmfSSCAATab04.h"
#include "nmfMSCAATableIO.h"


nmfSSCAA_Tab4::nmfSSCAA_Tab4(QTabWidget*  tabs,
                             nmfLogger*   logger,
                             nmfDatabase* databasePtr,
                             std::string& projectDir)
{
    QUiLoader loader;

    SSCAA_Tabs    = tabs;
    m_logger      = logger;
    m_databasePtr = databasePtr;
    ReadSettings();

    m_logger->logMsg(nmfConstants::Normal,"nmfSSCAA_Tab4::nmfSSCAA_Tab4");

    m_ProjectDir = projectDir;

    // Load ui as a widget from disk
    QFile file(":/forms/SSCAA/SSCAA_Tab04.ui");
    file.open(QFile::ReadOnly);
    SSCAA_Tab4_Widget = loader.load(&file,SSCAA_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    SSCAA_Tabs->addTab(SSCAA_Tab4_Widget, tr("4. Maturity"));

    SSCAA_Tab4_MaturityTV = new QTableView(SSCAA_Tabs);
    SSCAA_Tab4_SpeciesLBL = SSCAA_Tabs->findChild<QLabel      *>("SSCAA_Tab4_SpeciesLBL");
    SSCAA_Tab4_PrevPB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab4_PrevPB");
    SSCAA_Tab4_NextPB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab4_NextPB");
    SSCAA_Tab4_LoadPB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab4_LoadPB");
    SSCAA_Tab4_SavePB     = SSCAA_Tabs->findChild<QPushButton *>("SSCAA_Tab4_SavePB");
    SSCAA_Tab4_MaturityLT = SSCAA_Tabs->findChild<QVBoxLayout *>("SSCAA_Tab4_MaturityLT");
    SSCAA_Tab4_MaturityLT->addWidget(SSCAA_Tab4_MaturityTV);

    QFont noBoldFont;
    noBoldFont.setBold(false);
    SSCAA_Tab4_MaturityTV->setFont(noBoldFont);

    connect(SSCAA_Tab4_PrevPB,   SIGNAL(clicked()),
            this,                SLOT(callback_PrevPB()));
    connect(SSCAA_Tab4_NextPB,   SIGNAL(clicked()),
            this,                SLOT(callback_NextPB()));
    connect(SSCAA_Tab4_LoadPB,   SIGNAL(clicked()),
            this,                SLOT(callback_LoadPB()));
    connect(SSCAA_Tab4_SavePB,   SIGNAL(clicked()),
            this,                SLOT(callback_SavePB()));

    SSCAA_Tab4_PrevPB->setText("\u25C1--");
    SSCAA_Tab4_NextPB->setText("--\u25B7");

} // end constructor


nmfSSCAA_Tab4::~nmfSSCAA_Tab4()
{

}


QTableView*
nmfSSCAA_Tab4::getTable()
{
    return SSCAA_Tab4_MaturityTV;
}


std::string
nmfSSCAA_Tab4::getSpecies()
{
    return SSCAA_Tab4_SpeciesLBL->text().toStdString();
}

void
nmfSSCAA_Tab4::speciesChanged(QString species)
{
    std::string units = "";

    SSCAA_Tab4_SpeciesLBL->setText(species);
    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             SSCAA_Tab4_MaturityTV,
                             nmfConstantsMSCAA::TableMaturity,species,
                             "0",units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);
}

void
nmfSSCAA_Tab4::callback_PrevPB()
{
    int prevPage = SSCAA_Tabs->currentIndex()-1;
    SSCAA_Tabs->setCurrentIndex(prevPage);
}

void
nmfSSCAA_Tab4::callback_NextPB()
{
    int nextPage = SSCAA_Tabs->currentIndex()+1;
    SSCAA_Tabs->setCurrentIndex(nextPage);
}

void
nmfSSCAA_Tab4::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSSCAA_Tab4::callback_SavePB()
{
    bool ok = false;
    QString msg;

    if (nmfUtilsQt::allCellsArePopulated(SSCAA_Tabs,SSCAA_Tab4_MaturityTV,
                                         nmfConstants::ShowError))
    {
         ok = nmfMSCAAUtils::saveTable(SSCAA_Tabs,m_databasePtr,m_logger,
                                 SSCAA_Tab4_MaturityTV,
                                 m_ProjectSettingsConfig,
                                 nmfConstantsMSCAA::TableMaturity,getSpecies(),
                                 "0","",
                                 nmfConstants::IsProportion,
                                 nmfConstants::dontIncludeTotalColumn,
                                 nmfConstants::AllYears);
         if (ok) {
             msg = "\nMaturity table has been successfully updated.\n";
             QMessageBox::information(SSCAA_Tabs, "Maturity Updated", msg, QMessageBox::Ok);
         }
    }
}

void
nmfSSCAA_Tab4::ReadSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSSCAA_Tab4::clearWidgets()
{
    nmfUtilsQt::clearTableView({SSCAA_Tab4_MaturityTV});
}


bool
nmfSSCAA_Tab4::loadWidgets()
{
    std::string units = "";
std::cout << "nmfSSCAA_Tab4::loadWidgets()" << std::endl;

    ReadSettings();

    clearWidgets();

    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             SSCAA_Tab4_MaturityTV,
                             nmfConstantsMSCAA::TableMaturity,
                             QString::fromStdString(getSpecies()),
                             std::to_string(0),units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::AllYears);

    return true;
}

