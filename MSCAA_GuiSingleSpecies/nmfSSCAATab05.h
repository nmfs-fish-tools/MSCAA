#ifndef NMFSSCAATAB5_H
#define NMFSSCAATAB5_H

//#include "nmfMSCAATableIO.h"

#include <QCheckBox>

class nmfSSCAA_Tab5: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;
    int           m_currentSegmentIndex;

    QTableView*   SSCAA_Tab5_NaturalMortalityTV;
    QTableView*   SSCAA_Tab5_FishingMortalityTV;
//  nmfTableView* SSCAA_Tab5_NaturalMortalityTV;
    QTabWidget*   SSCAA_Tabs;
    QWidget*      SSCAA_Tab5_Widget;
    QPushButton*  SSCAA_Tab5_PrevPB;
    QPushButton*  SSCAA_Tab5_NextPB;
    QPushButton*  SSCAA_Tab5_LoadPB;
    QPushButton*  SSCAA_Tab5_SavePB;
    QVBoxLayout*  SSCAA_Tab5_TVLayoutLT;
    QVBoxLayout*  SSCAA_Tab5_TVLayoutLT2;
    QLabel*       SSCAA_Tab5_SpeciesLBL;
    QComboBox*    SSCAA_Tab5_NumSegCMB;
    QCheckBox*    SSCAA_Tab5_AutoFillSegCB;
    QCheckBox*    SSCAA_Tab5_YearlyFCB;

public:
    nmfSSCAA_Tab5(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSSCAA_Tab5();

    void clearWidgets();
    bool loadWidgets();
    std::string getSpecies();
    void speciesChanged(QString species);
    int getNumSegments();
    int loadNumSegments(QString species);
    bool isAutoFillChecked();
    QTableView* getNaturalMortalityTable();
    QTableView* getFishingMortalityTable();
    void setNumSegments(int numSegments);
    void ReadSettings();

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_PrevPB();
    void callback_NextPB();
    void callback_NumSegCMB(int value);
    void callback_AutoFillSegPB(bool isChecked);
    void callback_YearlyFPB(bool isChecked);

};

#endif
