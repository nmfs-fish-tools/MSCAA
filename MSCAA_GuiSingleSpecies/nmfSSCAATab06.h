#ifndef NMFSSCAATAB6_H
#define NMFSSCAATAB6_H

#include <Q3DSurface>
#include <QTableView>

#include "nmfChartSurface.h"
#include "nmfChartLine.h"
#include "VerboseDebugLevelDialog.h"

/**
 * @brief The class containing the interactions for the 6th SSCAA tabbed pane: Run
 */
class nmfSSCAA_Tab6: public QObject
{
    Q_OBJECT

    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;
    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    std::string   m_ProjectName;
    boost::numeric::ublas::matrix<double> m_Abundance;
    std::map<std::string,boost::numeric::ublas::matrix<double> > m_AbundanceAll; // one matrix or every species

    QTableView*   SSCAA_Tab6_EstimationsTV;
    QTableView*   SSCAA_Tab6_WeightingsTV;
    QTabWidget*   SSCAA_Tabs;
    QWidget*      SSCAA_Tab6_Widget;
    QPushButton*  SSCAA_Tab6_PrevPB;
    QPushButton*  SSCAA_Tab6_LoadPB;
    QPushButton*  SSCAA_Tab6_SavePB;
    QPushButton*  SSCAA_Tab6_RunPB;
    QVBoxLayout*  SSCAA_Tab6_EstimationLT;
    QVBoxLayout*  SSCAA_Tab6_WeightingsLT;
    QLabel*       SSCAA_Tab6_SpeciesLBL;
    QComboBox*    SSCAA_Tab6_FleetsCMB;
    QTextEdit*    SSCAA_Tab6_SummaryTE;
    QPushButton*  SSCAA_Tab6_OptFilesPB;
    QLineEdit*    SSCAA_Tab6_DebugLE;
    QPushButton*  SSCAA_Tab6_DebugPB;

    void writeToFileStream(QTextStream& fileStream,
          boost::numeric::ublas::matrix<double>& matrix);

public:
    nmfSSCAA_Tab6(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir,
                  std::string& projectName);
    virtual ~nmfSSCAA_Tab6();

    void clearWidgets();
    bool loadWidgets();
    QTableView* getTable1();
    QTableView* getTable2();
    bool saveToSpeciesTable(const std::vector<std::string>& ColLabels,
                            QTableView* tableView);
    bool loadFromSpeciesTable(const std::vector<std::string>& ColLabels,
                              QTableView* tableView);
    bool writeADMBDataFile(const QString& datFile,
                           const QString& Species,
                           const QString& Fleet,
                           const int& FirstYear,
                           const int& LastYear,
                           const int& MinAge,
                           const int& MaxAge);
    bool writeADMBParameterFile(const QString& filename,
                                const int& NumYears,
                                const int& NumAges);
    bool writeADMBTPLFile(const QString& filename);
//    bool runADMB(const QString& tplFile,
//                 const QString& buildOutputFile,
//                 const QString& runOutputFile);
//    bool readADMBReportFile(const QString& reportFile);
    void speciesChanged(QString species);
    QString getSpecies();
    QString getFleet();
    boost::numeric::ublas::matrix<double>* getAbundance();
    void showChartAbundance3d(const bool     showLogData,
                              const int&     MinAge,
                              const int&     FirstYear,
                              Q3DSurface*    Graph3D,
                              const QString& XTitle,
                              const QString& YTitle,
                              const QString& ZTitle,
                              const QString& Scale,
                              const double&  sf);
    void showChartAbundanceVsTime(
            QChart* ChartWidget,
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
    void clearSummaryTE();
    void appendSummaryTE(QString& msg);
    QString getADMBDir();
    void readLastReportFile(const QString& type);
    int getVerboseDebugLevel();
    void ReadSettings();

signals:
    void UpdateOutputCharts(QString mode);
    void RunADMB(int trophic);

public Q_SLOTS:
    void callback_LoadPB();
    bool callback_SavePB();
    void callback_PrevPB();
    void callback_NextPB();
    void callback_RunPB();
    void callback_OptFilesPB();
    void callback_DebugPB();

};

#endif
