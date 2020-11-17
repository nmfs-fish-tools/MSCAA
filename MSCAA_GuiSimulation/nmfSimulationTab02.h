#ifndef NMFSIMULATIONTAB2_H
#define NMFSIMULATIONTAB2_H

#include <Q3DSurface>

#include <math.h>

#include "nmfMSCAATableIO.h"
#include "nmfChartLine.h"
#include "nmfConstantsMSCAA.h"
#include "nmfAbundance.h"

#include <QTableView>
#include <QComboBox>

using namespace QtDataVisualization;

class nmfSimulation_Tab2: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;
    bool          m_useUserSuppliedNuOther;
    double        m_userSuppliedNuOther;
    boost::numeric::ublas::matrix<double> m_Abundance;
    boost::numeric::ublas::matrix<double> m_Biomass;
    boost::numeric::ublas::matrix<double> m_NaturalMortality;
    boost::numeric::ublas::matrix<double> m_FishingMortality;
    boost::numeric::ublas::matrix<double> m_PredationMortality;
    boost::numeric::ublas::matrix<double> m_TotalMortality;
    std::vector<double> m_Recruitment;
    std::vector<double> m_SpawningStockBiomass;

    QTabWidget*   Simulation_Tabs;
    QWidget*      Simulation_Tab2_Widget;
    QPushButton*  Simulation_Tab2_PrevPB;
    QPushButton*  Simulation_Tab2_LoadPB;
    QPushButton*  Simulation_Tab2_SavePB;
    QPushButton*  Simulation_Tab2_RunPB;
    QComboBox*    Simulation_Tab2_RecruitmentCMB;
    QCheckBox*    Simulation_Tab2_AutoRunCB;
    QLabel*       Simulation_Tab2_SpeciesLBL;
    QTableView*   Simulation_Tab2_InitialAbundanceTV;
    QTableView*   Simulation_Tab2_RecruitmentParametersTV;
    QTextEdit*    Simulation_Tab2_SummaryTE;
    QComboBox*    Simulation_Tab2_FontSizeCMB;
    QCheckBox*    Simulation_Tab2_ClearCB;

    bool clearBeforeNewSummary();
    void setParametersTooltips();

public:
    nmfSimulation_Tab2(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfSimulation_Tab2();

    void clearWidgets();
    bool loadWidgets();
    QString getRecruitmentType();
    QString getSpecies();
    QTableView* getAbundanceTable();
    QTableView* getParameterTable();
    void setSpecies(QString species);
    void speciesChanged(QString species);
    boost::numeric::ublas::matrix<double>* getAbundance();
    boost::numeric::ublas::matrix<double>* getNaturalMortality();
    boost::numeric::ublas::matrix<double>* getFishingMortality();
    boost::numeric::ublas::matrix<double>* getPredationMortality();
    std::vector<double>* getRecruitment();
    std::vector<double>* getSpawningStockBiomass();
    bool getSpeciesParameters(
            const std::string &TableName,
            std::map<std::string,double> &Alpha,
            std::map<std::string,double> &Beta,
            std::map<std::string,double> &Gamma);
    bool getYearlyParameters(
            const std::string &Species,
            const int &NumYears,
            const std::string &TableName,
            std::vector<double> &Sigma,
            std::vector<double> &Zeta);
    bool getFleetCatchTotals(
            const std::string &Species,
            const int &NumYears,
            const int &NumAges,
            const std::string &TableName,
            boost::numeric::ublas::matrix<double> &TableData);
    bool getPredatorPreyData(
            std::string tableName,
            boost::numeric::ublas::matrix<double>& data);
    void setUseUserNuOther(bool useNuOther);
    void setUserNuOther(double nuOther);
    std::string getAbundanceDriver();

    void updateSummaryWindow(const int  &FirstYear,
                             const int  &LastYear,
                             const int  &MinAge,
                             const int  &MaxAge,
                             const std::string &msg,
                             const boost::numeric::ublas::matrix<double> &Abundance,
                             const std::vector<double> &Recruitment,
                             const std::vector<double> &SpawningBiomass);
    bool autoRun();
//    void updateOutputWindow(const int &FirstYear,
//                            const int &LastYear,
//                            const int &MinAge,
//                            const int &MaxAge,
//                            const boost::numeric::ublas::matrix<double> &m_Abundance);
    void showChartAbundance3d(const bool     showLogData,
                              const int&     MinAge,
                              const int&     FirstYear,
                              Q3DSurface*    Graph3D,
                              const QString& XTitle,
                              const QString& YTitle,
                              const QString& ZTitle,
                              const QString& Scale,
                              const double&  sf);
    void showChartAbundanceVsTime(QChart* ChartWidget,
                                  const std::string& Species,
                                  const int& StartYear,
                                  const QStringList& AgeGroupNames,
                                  const boost::numeric::ublas::matrix<double>& Data,
                                  const QString& Scale);
    void showChartMortalityVsTime(
            bool ClearChart,
            std::string LineStyle,
            QChart* ChartWidget,
            const std::string& Species,
            const int& StartYear,
            const QStringList& AgeGroupNames,
            const boost::numeric::ublas::matrix<double>& Data,
            const QString& Scale);
    void showChartSSBvsTime(
            QChart* ChartWidget,
            const std::string& Species,
            const boost::numeric::ublas::matrix<double>& Data,
            const QString& Scale);
    void showChartRecruitmentVsSSB(
            QChart* ChartWidget,
            const std::string& Species,
            const boost::numeric::ublas::matrix<double>& Data,
            const QString& Scale);
    void calculateTotalMortality(
            const std::vector<std::string>&              allSpecies,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
            const boost::numeric::ublas::matrix<double>& preferredRatioEta,
            const boost::numeric::ublas::matrix<double>& preferredLTRatio,
            const boost::numeric::ublas::matrix<double>& preferredGTRatio,
            const boost::numeric::ublas::matrix<double>& vulnerabilityRho,
            boost::numeric::ublas::matrix<double>& naturalMortalityM1,
            boost::numeric::ublas::matrix<double>& fishingMortalityF,
            boost::numeric::ublas::matrix<double>& totalMortalityZ);
    void calculateSizePreference(
            const std::vector<std::string>& allSpecies,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
            const boost::numeric::ublas::matrix<double>& preferredRatioEta,
            const boost::numeric::ublas::matrix<double>& preferredLTRatio,
            const boost::numeric::ublas::matrix<double>& preferredGTRatio,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& sizePreferenceG);
    void calculateSuitability(
            const std::vector<std::string>& allSpecies,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
            const boost::numeric::ublas::matrix<double>& vulnerability,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& sizePreferenceG,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& generalVulnerabilityRho);
    void calculateScaledSuitability(
            const std::vector<std::string>& allSpecies,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& weightMap,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& vulnerabilityNu,
            std::map<std::string, boost::numeric::ublas::matrix<double> >& scaledSuitabilityNuTilde);
    void ReadSettings();

signals:
    void UpdateOutputCharts(QString mode);
    void UpdateSuitabilityNuOther(double nuOther);
    void LoadUserSuppliedNuOther();

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_RunPB();
//    void callback_RunPB_OLD();
    void callback_FontSizeCMB(QString fontSize);
    void callback_RecruitmentCMB(QString algorithm);

};

#endif
