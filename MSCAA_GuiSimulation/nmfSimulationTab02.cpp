
#include "nmfUtilsQt.h"
#include "nmfSimulationTab02.h"
#include "nmfUtils.h"
//#include "nmfConstants.h"
//#include "nmfRicker.h"
//#include "nmfBevertonHolt.h"
#include "nmfChartSurface.h"


nmfSimulation_Tab2::nmfSimulation_Tab2(QTabWidget*  tabs,
                                       nmfLogger*   logger,
                                       nmfDatabase* databasePtr,
                                       std::string& projectDir)
{
    QUiLoader loader;

    Simulation_Tabs = tabs;

    m_logger      = logger;
    m_databasePtr = databasePtr;
    m_ProjectDir  = projectDir;
    m_Abundance.clear();
    m_NaturalMortality.clear();
    m_FishingMortality.clear();
    m_Recruitment.clear();
    m_SpawningStockBiomass.clear();
    m_logger->logMsg(nmfConstants::Normal,"nmfSimulation_Tab2::nmfSimulation_Tab2");
    m_useUserSuppliedNuOther = false;
    m_userSuppliedNuOther = 0;

    ReadSettings();

    // Load ui as a widget from disk
    QFile file(":/forms/Simulation/Simulation_Tab02.ui");
    file.open(QFile::ReadOnly);
    Simulation_Tab2_Widget = loader.load(&file,Simulation_Tabs);
    file.close();

    // Add the loaded widget as the new tabbed page
    Simulation_Tabs->addTab(Simulation_Tab2_Widget, tr("5. Run"));
    Simulation_Tab2_PrevPB                  = Simulation_Tabs->findChild<QPushButton *>("Simulation_Tab2_PrevPB");
    Simulation_Tab2_LoadPB                  = Simulation_Tabs->findChild<QPushButton *>("Simulation_Tab2_LoadPB");
    Simulation_Tab2_SavePB                  = Simulation_Tabs->findChild<QPushButton *>("Simulation_Tab2_SavePB");
    Simulation_Tab2_RunPB                   = Simulation_Tabs->findChild<QPushButton *>("Simulation_Tab2_RunPB");
    Simulation_Tab2_RecruitmentCMB          = Simulation_Tabs->findChild<QComboBox   *>("Simulation_Tab2_RecruitmentCMB");
    Simulation_Tab2_SpeciesLBL              = Simulation_Tabs->findChild<QLabel      *>("Simulation_Tab2_SpeciesLBL");
    Simulation_Tab2_InitialAbundanceTV      = Simulation_Tabs->findChild<QTableView  *>("Simulation_Tab2_InitialAbundanceTV");
    Simulation_Tab2_RecruitmentParametersTV = Simulation_Tabs->findChild<QTableView  *>("Simulation_Tab2_RecruitmentParametersTV");
    Simulation_Tab2_SummaryTE               = Simulation_Tabs->findChild<QTextEdit   *>("Simulation_Tab2_SummaryTE");
    Simulation_Tab2_FontSizeCMB             = Simulation_Tabs->findChild<QComboBox   *>("Simulation_Tab2_FontSizeCMB");
    Simulation_Tab2_ClearCB                 = Simulation_Tabs->findChild<QCheckBox   *>("Simulation_Tab2_ClearCB");
    Simulation_Tab2_AutoRunCB               = Simulation_Tabs->findChild<QCheckBox   *>("Simulation_Tab2_AutoRunCB");

    connect(Simulation_Tab2_PrevPB, SIGNAL(clicked()),
            this,                   SLOT(callback_PrevPB()));
    connect(Simulation_Tab2_LoadPB, SIGNAL(clicked()),
            this,                   SLOT(callback_LoadPB()));
    connect(Simulation_Tab2_SavePB, SIGNAL(clicked()),
            this,                   SLOT(callback_SavePB()));
    connect(Simulation_Tab2_RunPB,  SIGNAL(clicked()),
            this,                   SLOT(callback_RunPB()));
    connect(Simulation_Tab2_FontSizeCMB, SIGNAL(activated(QString)),
            this,                        SLOT(callback_FontSizeCMB(QString)));
    connect(Simulation_Tab2_RecruitmentCMB, SIGNAL(activated(QString)),
            this,                           SLOT(callback_RecruitmentCMB(QString)));

    Simulation_Tab2_PrevPB->setText("\u25C1--");

    Simulation_Tab2_RunPB->setEnabled(!getSpecies().isEmpty());

    // Set mono font
    QFont monoFont("Monospace", 10, QFont::Normal, false);
    monoFont.setStyleHint(QFont::TypeWriter);
    Simulation_Tab2_SummaryTE->setFont(monoFont);
    Simulation_Tab2_FontSizeCMB->setCurrentText("10");
    Simulation_Tab2_ClearCB->setChecked(true);

    setParametersTooltips();

} // end constructor


nmfSimulation_Tab2::~nmfSimulation_Tab2()
{

}

void
nmfSimulation_Tab2::setParametersTooltips()
{
    if (getRecruitmentType() == "Ricker") {
        Simulation_Tab2_RecruitmentParametersTV->setToolTip(QApplication::translate("Recruitment Parameters",
        "    <html>\n"
        "    <table width=\"400\">"
        "    <tr><td width=\"400\">alpha - Maximum number of recruits per spawner at low stock level</td></tr>"
        "    <tr><td width=\"400\">beta - Rate of decrease of recruits per unit spawner</td></tr>"
        "    <tr><td width=\"400\">gamma - Biomass exponent in denominator of Ricker expression</td></tr>"
        "    </table>\n"
        "    </html>", 0));
    } else if (getRecruitmentType() == "Beverton-Holt") {
        Simulation_Tab2_RecruitmentParametersTV->setToolTip(QApplication::translate("Recruitment Parameters",
        "    <html>\n"
        "    <table width=\"425\">"
        "    <tr><td width=\"425\">alpha - Maximum number of recruits produced</td></tr>"
        "    <tr><td width=\"425\">beta - Spawning stock size needed to produce half of maximum recruits</td></tr>"
        "    <tr><td width=\"425\">gamma - (unused)</td></tr>"
        "    </table>\n"
        "    </html>", 0));
    }
}

void
nmfSimulation_Tab2::speciesChanged(QString species)
{
    Simulation_Tab2_SpeciesLBL->clear();
    setSpecies(species);
    loadWidgets();
}

void
nmfSimulation_Tab2::setSpecies(QString species)
{
    Simulation_Tab2_SpeciesLBL->setText(species);
    Simulation_Tab2_RunPB->setEnabled(true);
}

QString
nmfSimulation_Tab2::getSpecies()
{
    return Simulation_Tab2_SpeciesLBL->text();
}

boost::numeric::ublas::matrix<double>*
nmfSimulation_Tab2::getAbundance()
{
    return &m_Abundance;
}

boost::numeric::ublas::matrix<double>*
nmfSimulation_Tab2::getNaturalMortality()
{
    return &m_NaturalMortality;
}

boost::numeric::ublas::matrix<double>*
nmfSimulation_Tab2::getFishingMortality()
{
    return &m_FishingMortality;
}

boost::numeric::ublas::matrix<double>*
nmfSimulation_Tab2::getPredationMortality()
{
    return &m_PredationMortality;
}

std::vector<double>*
nmfSimulation_Tab2::getRecruitment()
{
    return &m_Recruitment;
}

std::vector<double>*
nmfSimulation_Tab2::getSpawningStockBiomass()
{
    return &m_SpawningStockBiomass;
}


QTableView*
nmfSimulation_Tab2::getAbundanceTable()
{
    return Simulation_Tab2_InitialAbundanceTV;
}

QTableView*
nmfSimulation_Tab2::getParameterTable()
{
    return Simulation_Tab2_RecruitmentParametersTV;
}

void
nmfSimulation_Tab2::callback_PrevPB()
{
    int prevPage = Simulation_Tabs->currentIndex()-1;
    Simulation_Tabs->setCurrentIndex(prevPage);
}

bool
nmfSimulation_Tab2::autoRun()
{
    return Simulation_Tab2_AutoRunCB->isChecked();
}

void
nmfSimulation_Tab2::callback_LoadPB()
{
    loadWidgets();
}

void
nmfSimulation_Tab2::callback_SavePB()
{
    bool ok = false;
    QString msg = "\n";

    if (nmfUtilsQt::allCellsArePopulated(Simulation_Tabs,
                                         Simulation_Tab2_RecruitmentParametersTV,
                                         nmfConstants::ShowError))
    {
        ok = nmfMSCAAUtils::saveParameterTable(Simulation_Tabs,m_databasePtr,m_logger,
                                               Simulation_Tab2_RecruitmentParametersTV,
                                               m_ProjectSettingsConfig,
                                               "SimulationParametersSpecies",
                                               getSpecies().toStdString(),
                                               getRecruitmentType().toStdString());
        if (ok) {
             msg += "SimulationParametersSpecies table has been successfully updated.\n";
         }
    }

    if (nmfUtilsQt::allCellsArePopulated(Simulation_Tabs,
                                         Simulation_Tab2_InitialAbundanceTV,
                                         nmfConstants::ShowError))
    {
         ok = nmfMSCAAUtils::saveTable(Simulation_Tabs,m_databasePtr,m_logger,
                                       Simulation_Tab2_InitialAbundanceTV,
                                       m_ProjectSettingsConfig,
                                       "InitialAbundance",
                                       getSpecies().toStdString(),"0","",
                                       nmfConstants::IsNotProportion,
                                       nmfConstants::dontIncludeTotalColumn,
                                       nmfConstants::FirstYearOnly);
         if (ok) {
             msg += "InitialAbundance table has been successfully updated.\n";

         }
    }

    if (ok) {
        QMessageBox::information(Simulation_Tabs, "Table(s) Updated", msg, QMessageBox::Ok);
    }
}


void
nmfSimulation_Tab2::callback_FontSizeCMB(QString fontSize)
{
    QFont monoFont("Monospace", fontSize.toInt(), QFont::Normal, false);
    monoFont.setStyleHint(QFont::TypeWriter);
    Simulation_Tab2_SummaryTE->setFont(monoFont);
}

void
nmfSimulation_Tab2::callback_RecruitmentCMB(QString algorithm)
{
    loadWidgets();
    if (autoRun()) {
        callback_RunPB();
    }
    setParametersTooltips();
}


QString
nmfSimulation_Tab2::getRecruitmentType()
{
    return Simulation_Tab2_RecruitmentCMB->currentText();
}

std::string
nmfSimulation_Tab2::getAbundanceDriver()
{
    int NumRecords;
    std::string msg;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields     = {"SystemName","AbundanceDriver"};
    queryStr   = "SELECT SystemName,AbundanceDriver FROM `System`";
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        msg = "getAbundanceDriver: No records found in table: System";
        m_logger->logMsg(nmfConstants::Error,msg);
        return "";
    }

    return dataMap["AbundanceDriver"][0];
}

bool
nmfSimulation_Tab2::getSpeciesParameters(
        const std::string &TableName,
        std::map<std::string,double> &Alpha,
        std::map<std::string,double> &Beta,
        std::map<std::string,double> &Gamma)
{
    int m;
    int NumRecords;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    std::string species;
    std::vector<std::string> ParameterNames = {"alpha","beta","gamma"};
    int NumParameters = ParameterNames.size();

    fields     = {"SystemName","Algorithm","SpeName","ParameterName","Value"};
    queryStr   = "SELECT SystemName,Algorithm,SpeName,ParameterName,Value FROM " + TableName;
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " AND SpeName = '" + getSpecies().toStdString() + "'";
    queryStr  += " AND Algorithm = '" + getRecruitmentType().toStdString() + "'";
    queryStr  += " ORDER BY ParameterName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        msg = "getSpeciesParameters: No records found in table: " + TableName;
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    NumSpecies = NumRecords/NumParameters;

    m = 0;
    for (int i=0; i<NumSpecies; ++i) {
        species = dataMap["SpeName"][m];
        Alpha[species] = std::stod(dataMap["Value"][m++]);
    }
    for (int i=0; i<NumSpecies; ++i) {
        species = dataMap["SpeName"][m];
        Beta[species] = std::stod(dataMap["Value"][m++]);
    }
    for (int i=0; i<NumSpecies; ++i) {
        species = dataMap["SpeName"][m];
        Gamma[species] = std::stod(dataMap["Value"][m++]);
    }

    return true;
}

bool
nmfSimulation_Tab2::getYearlyParameters(
        const std::string &Species,
        const int &NumYears,
        const std::string &TableName,
        std::vector<double> &Sigma,
        std::vector<double> &Zeta)
{
    int m;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    std::string species;
    std::vector<std::string> ParameterNames = {"sigma","zeta"};
    int NumParameters = ParameterNames.size();

    fields     = {"SystemName","SpeName","Year","ParameterName","Value"};
    queryStr   = "SELECT SystemName,SpeName,Year,ParameterName,Value FROM " + TableName;
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " AND SpeName = '" + Species + "'";
    queryStr  += " ORDER BY ParameterName,Year";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        msg = "getYearlyParameters: No records found in table: " + TableName;
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    if (NumRecords != NumParameters*NumYears) {
        msg = "getYearlyParameters: Incorrect number of records found in table: " + TableName + "\n";
        msg += "Found " + std::to_string(NumRecords) + " records.\n";
        msg += "Calculated NumParameters*NumYears (" + std::to_string(NumParameters*NumYears) + ") records.";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    m = 0;
    for (int i=0; i<NumYears; ++i) {
        Sigma.push_back(std::stod(dataMap["Value"][m++]));
    }
    for (int i=0; i<NumYears; ++i) {
        Zeta.push_back(std::stod(dataMap["Value"][m++]));
    }

    return true;
}


bool
nmfSimulation_Tab2::getFleetCatchTotals(
        const std::string &Species,
        const int &NumYears,
        const int &NumAges,
        const std::string &TableName,
        boost::numeric::ublas::matrix<double> &TableData)
{
    int m;
    int NumRecords;
    int NumFleets;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    std::string units;
    double sf = 1.0;

    // Find number of fleets for the species
    fields = {"NumFleets"};
    queryStr  = "SELECT COUNT(DISTINCT(Fleet)) AS NumFleets FROM " + TableName;
    queryStr += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr += " AND SpeName = '" + Species + "'";
    dataMap   = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumFleets = std::stoi(dataMap["NumFleets"][0]);

    // Sum up yearly totals per fleet per age
    fields     = {"SystemName","SpeName","Fleet","Year","Age","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Fleet,Year,Age,Value,Units FROM " + TableName;
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " AND SpeName = '" + Species + "'";
    queryStr  += " ORDER BY Fleet,Year,Age";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();

    if (NumFleets == 0) {
        msg = "ByYear: No fleets found in table: " + TableName;
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    if (NumRecords == 0) {
        msg = "ByYear: No records found in table: " + TableName;
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    if (NumRecords != NumFleets*NumYears*NumAges) {
        msg  = "ByYear: Incorrect number of records found in table: " + TableName + "\n";
        msg += "Found " + std::to_string(NumRecords) + " records.\n";
        msg += "Expecting NumFleets*NumYears*NumAges: " + std::to_string(NumFleets*NumYears*NumAges);
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    nmfUtils::initialize(TableData,NumYears,NumAges);

    // Get scalefactor from units.  Put values into units of fish.
    units = dataMap["Units"][0];
    if (units == "000 Fish")
        sf = 0.001;
    else if (units == "000 000 Fish")
        sf = 0.000001;

    m = 0;
    for (int Fleet = 0; Fleet < NumFleets; ++Fleet) {
        for (int Year = 0; Year < NumYears; ++Year) {
            for (int Age = 0; Age < NumAges; ++Age) {
                TableData(Year,Age) += sf*std::stod(dataMap["Value"][m++]);
            }
        }
    }

    return true;
}


bool
nmfSimulation_Tab2::getPredatorPreyData(
        std::string TableName,
        boost::numeric::ublas::matrix<double>& TableData)
{
    int m;
    int NumRecords;
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    fields     = {"SystemName","PredatorName","PreyName","Value"};
    queryStr   = "SELECT SystemName,PredatorName,PreyName,Value FROM " + TableName;
    queryStr  += " WHERE SystemName = '" + m_ProjectSettingsConfig + "'";
    queryStr  += " ORDER BY PredatorName,PreyName";
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["PredatorName"].size();
    if (NumRecords == 0) {
        msg = "getPredatorPreyData: No records found in table: " + TableName;
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    NumSpecies = std::sqrt(NumRecords);
    nmfUtils::initialize(TableData,NumSpecies,NumSpecies);

    m = 0;
    while (m < NumRecords)
    {
        for (int row = 0; row < NumSpecies; ++row)
        {
            for (int col = 0; col < NumSpecies; ++col) {
                TableData(row,col) = std::stod(dataMap["Value"][m++]);
            }
        }
    }

    return true;
}


/*
 *
 * Eq'n (4) of Curtis' Dissertation:
 * w   = weight by age
 * i,j = prey,predator species
 * a,b = prey,predator age groups
 * η   = preferred weight ratio
 * σ^2 = preferred weight variance
 * g(i,a,j,b,t) = exp[-1/(2*σ(i,j)^2)(ln(w(j,b,t)/w(i,a,t))-η(i,j)^2))]
 *
 */
void
nmfSimulation_Tab2::calculateSizePreference(
        const std::vector<std::string>& allSpecies,
        std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
        const boost::numeric::ublas::matrix<double>& preferredRatioEta,
        const boost::numeric::ublas::matrix<double>& preferredLTRatio,
        const boost::numeric::ublas::matrix<double>& preferredGTRatio,
        std::map<std::string, boost::numeric::ublas::matrix<double> >& sizePreferenceG)
{
    int NumYears;
    int NumAges;
    int predNum = 0;
    int preyNum = 0;
    double weightFactor;
    double variance;
    double weightRatio;
    double eta;
    boost::numeric::ublas::matrix<double> tempSizePreferencefG;

    for (std::string predSpecies : allSpecies) {
        preyNum = 0;
        for (std::string preySpecies : allSpecies) {
            NumYears = weightMap[preySpecies].size1();
            NumAges  = weightMap[preySpecies].size2();
            nmfUtils::initialize(tempSizePreferencefG,NumYears,NumAges);
            for (int year = 0; year < NumYears; ++year) {
                for (int age = 0; age < NumAges; ++age) {
                    weightRatio  = weightMap[predSpecies](year,age) /
                                   weightMap[preySpecies](year,age);
                    eta = preferredRatioEta(predNum,preyNum);
                    weightFactor = (std::log(weightRatio) - eta);
                    variance = (weightRatio < eta) ? preferredLTRatio(predNum,preyNum) :
                                                     preferredGTRatio(predNum,preyNum);
                    tempSizePreferencefG(year,age) = std::exp((-1.0/(2.0*variance*variance)) *
                                                     weightFactor*weightFactor);
                }
            }
            sizePreferenceG[predSpecies+","+preySpecies] = tempSizePreferencefG;
            ++preyNum;
        }
        ++predNum;
    }
}

/*
 *
 * Eq'n (5) of Curtis' Dissertation:
 *
 * ρ = general vulnerability
 * g = size preference
 *
 * ν(i,a,j,b,t) = ρ(i,j)⋅g(i,a,j,b,t)
 *
 */
void
nmfSimulation_Tab2::calculateSuitability(
   const std::vector<std::string>& allSpecies,
   std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
   const boost::numeric::ublas::matrix<double>& vulnerabilityRho,
   std::map<std::string, boost::numeric::ublas::matrix<double> >& sizePreferenceG,
   std::map<std::string, boost::numeric::ublas::matrix<double> >& suitability)
{
    int NumYears;
    int NumAges;
    int predNum = 0;
    int preyNum = 0;
    std::string index;
    boost::numeric::ublas::matrix<double> tempSuitability;

    for (std::string predSpecies : allSpecies) {
        preyNum = 0;
        for (std::string preySpecies : allSpecies) {
            index = predSpecies+","+preySpecies;
            NumYears = weightMap[preySpecies].size1();
            NumAges  = weightMap[preySpecies].size2();
            nmfUtils::initialize(tempSuitability,NumYears,NumAges);
            for (int year = 0; year < NumYears; ++year) {
                for (int age = 0; age < NumAges; ++age) {
                    tempSuitability(year,age) = vulnerabilityRho(preyNum,predNum) *
                                                sizePreferenceG[index](year,age);
                }
            }
            suitability[index] = tempSuitability;
            ++preyNum;
        }
        ++predNum;
    }
}


void
nmfSimulation_Tab2::calculateScaledSuitability(
    const std::vector<std::string>& allSpecies,
    std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
    std::map<std::string, boost::numeric::ublas::matrix<double> >& vulnerabilityNu,
    std::map<std::string, boost::numeric::ublas::matrix<double> >& scaledSuitabilityNuTilde)
{
    int NumYears;
    int NumAges;
    int predNum = 0;
    int preyNum = 0;
    double nuSum = 0;


    double nuOther = 0; // RSK not sure what this is.
std::cout << "\nError: Using an average nu value for nuOther. Fix this! ***\n" << std::endl;
    // Hack just to find a reasonable nu other...DELETE THIS EVENTUALLY
    double aveVulnerabilityNu = 0;
    NumYears = weightMap[allSpecies[0]].size1();
    NumAges  = weightMap[allSpecies[0]].size2();
    for (int year = 0; year < NumYears; ++year) {
        for (int age = 0; age < NumAges; ++age) {
            aveVulnerabilityNu += vulnerabilityNu[allSpecies[0]+","+allSpecies[1]](year,age);
        }
    }
    aveVulnerabilityNu /= (NumYears+NumAges);
    nuOther = aveVulnerabilityNu;



    std::string index;
    boost::numeric::ublas::matrix<double> tempScaledSuitability;

    for (std::string predSpecies : allSpecies) {
        preyNum = 0;
        for (std::string preySpecies : allSpecies) {
            index = predSpecies+","+preySpecies;
            NumYears = weightMap[preySpecies].size1();
            NumAges  = weightMap[preySpecies].size2();
            nmfUtils::initialize(tempScaledSuitability,NumYears,NumAges);
            for (int year = 0; year < NumYears; ++year) {
                for (int age = 0; age < NumAges; ++age) {
                    for (std::string preySpecies2 : allSpecies) {
                        for (int age2 = 0; age2 < NumAges; ++age2) {
                            nuSum += vulnerabilityNu[predSpecies+","+preySpecies2](year,age2);
                        }
                    }
                    tempScaledSuitability(year,age) = vulnerabilityNu[index](year,age)/(nuSum + nuOther);
                }
            }
            scaledSuitabilityNuTilde[index] = tempScaledSuitability;
            ++preyNum;
        }
        ++predNum;
    }
}

void
nmfSimulation_Tab2::calculateTotalMortality(
        const std::vector<std::string>& allSpecies,
        std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
        const boost::numeric::ublas::matrix<double>& preferredRatioEta,
        const boost::numeric::ublas::matrix<double>& preferredLTRatio,
        const boost::numeric::ublas::matrix<double>& preferredGTRatio,
        const boost::numeric::ublas::matrix<double>& vulnerabilityRho,
        boost::numeric::ublas::matrix<double>& naturalMortalityM1,
        boost::numeric::ublas::matrix<double>& fishingMortalityF,
        boost::numeric::ublas::matrix<double>& totalMortalityZ)

{
    std::map<std::string, boost::numeric::ublas::matrix<double> > sizePreferenceG;
    std::map<std::string, boost::numeric::ublas::matrix<double> > vulnerabilityNu;
    std::map<std::string, boost::numeric::ublas::matrix<double> > scaledSuitabilityNuTilde;

    // From Kiersten Curti's Dissertation, pp 11+
    /*
     * Z = Total Mortality
     * F = Fishing Mortality
     * M = Total Natural Mortality
     * M1 = Residual Natural Mortality
     * M2 = Predation Mortality
     *
     * Z = F + M
     * M = M1 + M2
     *
     */

    // Calculate the size preference, g.
    calculateSizePreference(allSpecies,weightMap,preferredRatioEta,
                            preferredLTRatio,preferredGTRatio,
                            sizePreferenceG);

    // Next calculate the suitability
    calculateSuitability(allSpecies,weightMap,vulnerabilityRho,
                         sizePreferenceG,
                         vulnerabilityNu);

    // Next calculate the scaled suitability
    calculateScaledSuitability(allSpecies,weightMap,vulnerabilityNu,
                               scaledSuitabilityNuTilde);

    // Next calculate prey biomass available to predators
//    calculateAvailablePreyBiomass(scaledSuitabilityNuTilde,
//                                  preyBiomass);



}

void
nmfSimulation_Tab2::setUseUserNuOther(bool useNuOther)
{
    m_useUserSuppliedNuOther = useNuOther;
}

void
nmfSimulation_Tab2::setUserNuOther(double nuOther)
{
    m_userSuppliedNuOther = nuOther;
}

void
nmfSimulation_Tab2::callback_RunPB()
{
    int MinAge,MaxAge;
    int FirstYear,LastYear;
    float MinLength,MaxLength;
    int NumLengthBins;
    std::string Species = getSpecies().toStdString();
    std::map<std::string,boost::numeric::ublas::matrix<double> > Abundance;
    std::map<std::string,std::vector<double> > Recruitment;
    std::map<std::string,std::vector<double> > SpawningStockBiomass;
    std::map<std::string,boost::numeric::ublas::matrix<double> > NaturalMortality;
    std::map<std::string,boost::numeric::ublas::matrix<double> > FishingMortality;
    std::map<std::string,boost::numeric::ublas::matrix<double> > PredationMortality;
    QString RecruitmentType = getRecruitmentType();
    std::string label = (RecruitmentType == "Ricker") ?
                "Abundance (Ricker):" :
                "Abundance (Beverton-Holt):";
    NuOther nuOther;

    emit LoadUserSuppliedNuOther();

    nuOther.useUserNuOther = m_useUserSuppliedNuOther;
    nuOther.nuOther        = m_userSuppliedNuOther;
//std::cout << "nu other data: " << m_useUserSuppliedNuOther
//          << "," << m_userSuppliedNuOther << std::endl;


    // Calculate abundance data
    nmfAbundance abundance(m_databasePtr,m_logger,
                           Abundance,Recruitment,SpawningStockBiomass,
                           NaturalMortality,FishingMortality,PredationMortality);
    abundance.getData(nuOther,RecruitmentType,Abundance,Recruitment,
                      SpawningStockBiomass,PredationMortality);

    emit UpdateSuitabilityNuOther(abundance.getNuOther());

    // Write data to text edit box
    m_databasePtr->getSpeciesData(m_logger,Species,
                                  MinAge,MaxAge,FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    updateSummaryWindow(FirstYear,LastYear,MinAge,MaxAge,label,
                        Abundance[Species],Recruitment[Species],
                        SpawningStockBiomass[Species]);
    m_Abundance            = Abundance[Species];
    m_Recruitment          = Recruitment[Species];
    m_SpawningStockBiomass = SpawningStockBiomass[Species];
    m_NaturalMortality     = NaturalMortality[Species];
    m_FishingMortality     = FishingMortality[Species];
    m_PredationMortality   = PredationMortality[Species];

    // Send signal to update output chart
    emit UpdateOutputCharts("Simulation");
}
/*
void
nmfSimulation_Tab2::callback_RunPB_OLD()
{
    int MinAge,MaxAge;
    int FirstYear,LastYear;
    float MinLength,MaxLength;
    int NumLengthBins;
    int NumYears,NumAges;
    std::string label;
    QString RecruitmentType = getRecruitmentType();
    std::map<std::string,double> alpha;
    std::map<std::string,double> beta;
    std::map<std::string,double> gamma;
    std::vector<double> sigma;
    std::vector<double> zeta;
    std::vector<std::string> allSpecies;
    boost::numeric::ublas::matrix<double> weight;
    boost::numeric::ublas::matrix<double> maturity;
    boost::numeric::ublas::matrix<double> initialAbundance;
    boost::numeric::ublas::matrix<double> fisheryCatch;
    boost::numeric::ublas::matrix<double> preferredRatio;
    boost::numeric::ublas::matrix<double> preferredGTRatio;
    boost::numeric::ublas::matrix<double> preferredLTRatio;
    boost::numeric::ublas::matrix<double> vulnerabilityRho;
    std::string Species = getSpecies().toStdString();
    std::string abundanceDriver;

    m_Abundance.clear();
    m_Biomass.clear();
    m_NaturalMortality.clear();
    m_FishingMortality.clear();
    m_PredationMortality.clear();
    m_TotalMortality.clear();
    m_Recruitment.clear();
    m_SpawningStockBiomass.clear();

    nmfMSCAAUtils::getSpeciesData(m_databasePtr,m_logger,Species,
                                   MinAge,MaxAge,FirstYear,LastYear,
                                   MinLength,MaxLength,NumLengthBins);
    NumYears = LastYear - FirstYear + 1;
    NumAges  = MaxAge - MinAge + 1;
    nmfUtils::initialize(m_Abundance,NumYears,NumAges);
    nmfUtils::initialize(m_Biomass,NumYears,NumAges);
    m_FirstYear = FirstYear;
    m_MinAge    = MinAge;

    // Get all the species names
    nmfMSCAAUtils::getAllSpecies(m_databasePtr,m_logger,allSpecies);

    std::map<std::string, boost::numeric::ublas::matrix<double> > WeightMap;

    // Get Abundance Driver string
    abundanceDriver = getAbundanceDriver();
    if (abundanceDriver.empty()) {
        m_logger->logMsg(nmfConstants::Warning,"No Abundance Driver data found.");
        return;
    }

    // Get database data
    for (std::string species : allSpecies) {
        if (! nmfMSCAAUtils::getDatabaseData(m_databasePtr,m_logger,m_ProjectSettingsConfig,Species,NumYears,NumAges,"Weight", weight)) {
            m_logger->logMsg(nmfConstants::Warning,"No Weight Year-Age data found.");
            return;
        }
        WeightMap[species] = weight;
    }
    if (! nmfMSCAAUtils::getDatabaseData(m_databasePtr,m_logger,m_ProjectSettingsConfig,Species,NumYears,NumAges,"Maturity", maturity)) {
        m_logger->logMsg(nmfConstants::Warning,"No Maturity Year-Age data found.");
        return;
    }
    if (! nmfMSCAAUtils::getDatabaseData(m_databasePtr,m_logger,m_ProjectSettingsConfig,Species,1,NumAges,"InitialAbundance", initialAbundance)) {
        m_logger->logMsg(nmfConstants::Warning,"No InitialAbundance Year-Age data found.");
        return;
    }
    if (! getSpeciesParameters("SimulationParametersSpecies",alpha,beta,gamma)) {
        m_logger->logMsg(nmfConstants::Warning,"No alpha,beta,gamma parameter data found.");
        return;
    }

    if (! getYearlyParameters(Species,NumYears,"SimulationParametersYearly",sigma,zeta)) {
        m_logger->logMsg(nmfConstants::Warning,"No sigma,zeta parameter data found.");
        return;
    }

    if (! nmfMSCAAUtils::getMortalityData(m_databasePtr,m_logger,m_ProjectSettingsConfig,Species,NumYears,MinAge,MaxAge,"MortalityNatural",m_NaturalMortality)) {
        m_logger->logMsg(nmfConstants::Warning,"No Natural Mortality Year-Age data found.");
        return;
    }

    if (! nmfMSCAAUtils::getMortalityData(m_databasePtr,m_logger,m_ProjectSettingsConfig,Species,NumYears,MinAge,MaxAge,"MortalityFishing",m_FishingMortality)) {
        m_logger->logMsg(nmfConstants::Warning,"No Fishing Mortality Year-Age data found.");
        return;
    }

    if (! getPredatorPreyData("PredatorPreyPreferredRatio",preferredRatio)) {
        m_logger->logMsg(nmfConstants::Warning,"No Predator Prey Preferred Ratio data found.");
        return;
    }

    if (! getPredatorPreyData("PredatorPreyVarianceLTRatio",preferredLTRatio)) {
        m_logger->logMsg(nmfConstants::Warning,"No Predator Prey Variance < η data found.");
        return;
    }

    if (! getPredatorPreyData("PredatorPreyVarianceGTRatio",preferredGTRatio)) {
        m_logger->logMsg(nmfConstants::Warning,"No Predator Prey Variance > η data found.");
        return;
    }

    if (! getPredatorPreyData("PredatorPreyVulnerability",vulnerabilityRho)) {
        m_logger->logMsg(nmfConstants::Warning,"No Predator Prey Vulnerability found.");
        return;
    }

    if (! getFleetCatchTotals(Species,NumYears,NumAges,"CatchFishery",fisheryCatch)) {
        m_logger->logMsg(nmfConstants::Warning,"No Catch Year-Age data found.");
        return;
    }

//    calculateTotalMortality(allSpecies, WeightMap,preferredRatio, preferredLTRatio,
//                            preferredGTRatio,vulnerabilityRho,
//                            m_NaturalMortality,m_FishingMortality,m_TotalMortality);

    // Per p. 733 of 2013 Butterworth,Rademeyer paper on Statistical Catch At Age
    if (RecruitmentType == "Ricker") {
        nmfRicker Ricker(Species,MinAge,MaxAge,FirstYear,LastYear,
                         alpha,beta,gamma,zeta,sigma,initialAbundance,abundanceDriver,
                         weight,maturity,m_NaturalMortality,m_FishingMortality,
                         fisheryCatch);
        Ricker.getAbundance(m_Abundance,m_Recruitment,m_SpawningStockBiomass);
        label = "Abundance (Ricker):";
    } else if (RecruitmentType == "Beverton-Holt") {
        nmfBevertonHolt BevertonHolt(Species,MinAge,MaxAge,FirstYear,LastYear,
                         alpha,beta,gamma,zeta,sigma,initialAbundance,abundanceDriver,
                         weight,maturity,m_NaturalMortality,m_FishingMortality,
                         fisheryCatch);
        BevertonHolt.getAbundance(m_Abundance,m_Recruitment,m_SpawningStockBiomass);
        label = "Abundance (Beverton-Holt):";
    }

//    updateSummaryWindow(FirstYear,LastYear,MinAge,MaxAge,
//                        label,m_Abundance); //,m_Recruitment,m_SpawningStockBiomass);

    emit UpdateSimulationOutputWindow();
}
*/
void
nmfSimulation_Tab2::showChartMortalityVsTime(
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
//  std::string LineStyle = "SolidLine";
    bool xAxisIsInteger = true;
    QStringList RowLabelsForBars;
    QStringList HoverLabels = {};
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

    // Find the selected Ages

    if (ClearChart) {
        ChartWidget->removeAllSeries();
    }
    lineChart.populateChart(ChartWidget,
                            ChartType,
                            LineStyle,
                            nmfConstantsMSCAA::ShowFirstPoint,
                            nmfConstants::DontShowLegend,
                            StartYear,
                            xAxisIsInteger,
                            YMinSliderVal,
                            nmfConstantsMSCAA::DontLeaveGapsWhereNegative,
                            Data,
                            RowLabelsForBars,
                            ColumnLabelsForLegend,
                            HoverLabels,
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
nmfSimulation_Tab2::showChartAbundanceVsTime(
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
    QStringList HoverLabels = {};
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
                            nmfConstants::DontShowLegend,
                            StartYear,
                            xAxisIsInteger,
                            YMinSliderVal,
                            nmfConstantsMSCAA::DontLeaveGapsWhereNegative,
                            Data,
                            RowLabelsForBars,
                            ColumnLabelsForLegend,
                            HoverLabels,
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
nmfSimulation_Tab2::showChartSSBvsTime(QChart* ChartWidget,
                                       const std::string& Species,
                                       const boost::numeric::ublas::matrix<double>& Data,
                                       const QString& Scale)
{
    nmfChartLine lineChart;
    std::string ChartType = "FreeLine";
    std::string LineStyle = "SolidLine";
    int StartYear=0;
    bool xAxisIsInteger = true;
    QStringList RowLabelsForBars;
    QStringList HoverLabels = {};
    QStringList ColumnLabelsForLegend = {"one","two"};
    std::string MainTitle = "Spawning Stock Biomass vs Time: "+Species;
    std::string XLabel    = "Time (Years)";
    std::string YLabel    = "Spawning Stock Biomass";
    std::vector<bool> GridLines = {true,true};
    int Theme = 0;
    double YMinSliderVal = 0;
    QColor dashedLineColor = Qt::black;
    QList<QColor> LineColors = {Qt::blue};
    std::string ScaleStr = (Scale == "Default") ? "" : Scale.toStdString()+" ";
    YLabel += " (" + ScaleStr + "Metric Tons)";

    ChartWidget->removeAllSeries();
    lineChart.populateChart(ChartWidget,
                            ChartType,
                            LineStyle,
                            nmfConstantsMSCAA::ShowFirstPoint,
                            nmfConstants::DontShowLegend,
                            StartYear,
                            xAxisIsInteger,
                            YMinSliderVal,
                            nmfConstantsMSCAA::DontLeaveGapsWhereNegative,
                            Data,
                            RowLabelsForBars,
                            ColumnLabelsForLegend,
                            HoverLabels,
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
nmfSimulation_Tab2::showChartRecruitmentVsSSB(QChart* ChartWidget,
                                              const std::string& Species,
                                              const boost::numeric::ublas::matrix<double>&  Data,
                                              const QString& Scale)
{
    nmfChartLine lineChart;
    std::string ChartType = "FreeLine";
    std::string LineStyle = "SolidLine";
    int StartYear=0;
    bool xAxisIsInteger = false;
    QStringList RowLabelsForBars;
    QStringList HoverLabels = {};
    QStringList ColumnLabelsForLegend = {"one","two"};
    std::string MainTitle = "Recruitment vs Spawning Stock Biomass: "+Species;
    std::string XLabel    = "Spawning Stock Biomass (Metric Tons)";
    std::string ScaleStr  = (Scale == "Default") ? "" : Scale.toStdString()+" ";
    std::string YLabel    = "Recruitment (" + ScaleStr + "Fish)";
    std::vector<bool> GridLines = {true,true};
    int Theme = 0;
    double YMinSliderVal = 0;
    QColor dashedLineColor = Qt::black;
    QList<QColor> LineColors = {Qt::blue};

    ChartWidget->removeAllSeries();
    lineChart.populateChart(ChartWidget,
                            ChartType,
                            LineStyle,
                            nmfConstantsMSCAA::ShowFirstPoint,
                            nmfConstants::DontShowLegend,
                            StartYear,
                            xAxisIsInteger,
                            YMinSliderVal,
                            nmfConstantsMSCAA::DontLeaveGapsWhereNegative,
                            Data,
                            RowLabelsForBars,
                            ColumnLabelsForLegend,
                            HoverLabels,
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
nmfSimulation_Tab2::showChartAbundance3d(const bool     showLogData,
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
    int NumRows = m_Abundance.size1();
    int NumCols = m_Abundance.size2();
    QString xLabelFormat = "Age %d";
    QString zLabelFormat = "%d";

    nmfUtils::initialize(AbundanceScaled,NumRows,NumCols);
    nmfUtils::initialize(RowValues,      NumRows,NumCols);
    nmfUtils::initialize(ColumnValues,   NumRows,NumCols);
    for (int i = 0; i < NumRows; ++i) {
        for (int j = 0; j < NumCols; ++j) {
            RowValues(i,j)    = FirstYear + i;
            ColumnValues(i,j) = MinAge + j;
            if (showLogData) {
                AbundanceScaled(i,j) = sf * std::log(m_Abundance(i,j));
            } else {
                AbundanceScaled(i,j) = sf * m_Abundance(i,j);
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


bool
nmfSimulation_Tab2::clearBeforeNewSummary()
{
    return Simulation_Tab2_ClearCB->isChecked();
}

void
nmfSimulation_Tab2::updateSummaryWindow(const int  &FirstYear,
                                        const int  &LastYear,
                                        const int  &MinAge,
                                        const int  &MaxAge,
                                        const std::string &msg,
                                        const boost::numeric::ublas::matrix<double> &Abundance,
                                        const std::vector<double> &Recruitment,
                                        const std::vector<double> &SpawningBiomass)
{
    int NumYears = Abundance.size1();
    int NumAges  = Abundance.size2();
    QString text = QString::fromStdString(msg) + "\n\n          ";
    QString value;
    QString currentText;

    text += "  ";
    for (int age = MinAge; age <= MaxAge; ++age) {
        text += QString("%1 %2        ").arg("Arg").arg(age,2,'d',0,'0');
        //text += value.sprintf("Age %2d        ",age);
    }
    text += "\n";

    for (int year = 0; year < NumYears; ++year) {
        text += QString::number(year+FirstYear) + " ";
        for (int age = 0; age < NumAges; ++age) {
            text += QString("%1").arg(Abundance(year,age),14,'f',2,'0');
            //text += value.sprintf("%14.2f",Abundance(year,age));
        }
        text += "\n";
    }

    text += QString("%1").arg("\n        Recruitment   SpawningBiomass\n\n");
    //text += value.sprintf("\n        Recruitment   SpawningBiomass\n\n");
    text += QString("%1 %2 %3").arg(FirstYear,4,'d',0,'0').arg("       -").arg("             -");
    //text += value.sprintf("%4d %8s %14s\n",FirstYear,"-","-");
    for (unsigned long i = 0; i < Recruitment.size(); ++i) {
        text += QString("%1 %2 %3").arg(FirstYear+i+1,4,'d',0,'0').arg(Recruitment[i],14,'f',2).arg(SpawningBiomass[i],14,'f',2);
//        text += value.sprintf("%4d %14.2f %14.2f\n",
//                              FirstYear+i+1,
//                              Recruitment[i],
//                              SpawningBiomass[i]);
    }

    text += "\n";

    if (clearBeforeNewSummary()) {
        Simulation_Tab2_SummaryTE->setText(text);
    } else {
        currentText = Simulation_Tab2_SummaryTE->toPlainText();
        Simulation_Tab2_SummaryTE->setText(currentText+"\n"+text);
    }
}

void
nmfSimulation_Tab2::ReadSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    delete settings;
}

void
nmfSimulation_Tab2::clearWidgets()
{
    nmfUtilsQt::clearTableView({Simulation_Tab2_InitialAbundanceTV,
                                Simulation_Tab2_RecruitmentParametersTV});
}


bool
nmfSimulation_Tab2::loadWidgets()
{
std::cout << "nmfSimulation_Tab2::loadWidgets()" << std::endl;
    std::string units = "";

    ReadSettings();

    clearWidgets();

    nmfMSCAAUtils::loadParameterTable(m_databasePtr,m_logger,
                                      m_ProjectSettingsConfig,
                                      Simulation_Tab2_RecruitmentParametersTV,
                                      "SimulationParametersSpecies",
                                      getSpecies(),
                                      getRecruitmentType());

    nmfMSCAAUtils::loadTable(m_databasePtr,m_logger,
                             m_ProjectSettingsConfig,
                             Simulation_Tab2_InitialAbundanceTV,
                             "InitialAbundance",
                             getSpecies(),"0",units,
                             nmfConstants::dontIncludeTotalColumn,
                             nmfConstants::FirstYearOnly);

    return true;

}



