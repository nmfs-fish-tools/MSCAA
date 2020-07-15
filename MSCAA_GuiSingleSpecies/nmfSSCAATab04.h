#ifndef NMFSSCAATAB4_H
#define NMFSSCAATAB4_H

//#include "nmfMSCAATableIO.h"

class nmfSSCAA_Tab4: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;

    QTableView*   SSCAA_Tab4_MaturityTV;
//  nmfTableView* SSCAA_Tab4_MaturityTV;
    QTabWidget*   SSCAA_Tabs;
    QWidget*      SSCAA_Tab4_Widget;
    QPushButton*  SSCAA_Tab4_PrevPB;
    QPushButton*  SSCAA_Tab4_NextPB;
    QPushButton*  SSCAA_Tab4_LoadPB;
    QPushButton*  SSCAA_Tab4_SavePB;
    QVBoxLayout*  SSCAA_Tab4_MaturityLT;
    QLabel*       SSCAA_Tab4_SpeciesLBL;

public:
    nmfSSCAA_Tab4(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSSCAA_Tab4();

    void clearWidgets();
    bool loadWidgets();
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

};

#endif
