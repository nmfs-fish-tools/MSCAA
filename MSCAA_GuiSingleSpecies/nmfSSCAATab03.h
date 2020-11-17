#ifndef NMFSSCAATAB3_H
#define NMFSSCAATAB3_H

/**
 * @brief The class containing the interactions for the 3rd SSCAA tabbed pane: Weight-At-Age Data
 */
class nmfSSCAA_Tab3: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    QString       m_originalUnits;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;

    QTableView*   SSCAA_Tab3_WeightTV;
//  nmfTableView* SSCAA_Tab3_WeightTV;
    QTabWidget*   SSCAA_Tabs;
    QWidget*      SSCAA_Tab3_Widget;
    QPushButton*  SSCAA_Tab3_PrevPB;
    QPushButton*  SSCAA_Tab3_NextPB;
    QGroupBox*    SSCAA_Tab3_WeightGB;
    QPushButton*  SSCAA_Tab3_LoadPB;
    QPushButton*  SSCAA_Tab3_SavePB;
    QComboBox*    SSCAA_Tab3_UnitsCMB;
    QVBoxLayout*  SSCAA_Tab3_TVLayoutLT;
    QLabel*       SSCAA_Tab3_SpeciesLBL;

public:
    nmfSSCAA_Tab3(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSSCAA_Tab3();

    bool loadWidgets();
    void clearWidgets();
    std::string getUnits();
    std::string getSpecies();
    void speciesChanged(QString species);
    void setUnits(std::string units);
    QTableView* getTable();
    void ReadSettings();

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_NextPB();
    void callback_UnitsChanged(QString newUnits);

};

#endif
