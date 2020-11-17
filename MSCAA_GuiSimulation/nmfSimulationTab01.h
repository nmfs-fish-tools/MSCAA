#ifndef NMFSIMULATIONTAB1_H
#define NMFSIMULATIONTAB1_H

#include "nmfMSCAATableIO.h"

#include <string>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHeaderView>

class nmfSimulation_Tab1: public QObject
{
    Q_OBJECT

    std::string     m_ProjectSettingsConfig;
    std::string     m_ProjectDir;
    nmfLogger*      m_logger;
    nmfDatabase*    m_databasePtr;

//  nmfTableView*   SSCAA_Tab1_CatchTV;
//  nmfTableView2*  SSCAA_Tab1_TotalTV;
    QTableView*     Simulation_Tab1_YearlyParametersTV;
    QTabWidget*     Simulation_Tabs;
    QWidget*        Simulation_Tab1_Widget;
    QPushButton*    Simulation_Tab1_NextPB;
    QPushButton*    Simulation_Tab1_LoadPB;
    QPushButton*    Simulation_Tab1_SavePB;
    QVBoxLayout*    Simulation_Tab1_YearlyParametersLT;
    QLabel*         Simulation_Tab1_SpeciesLBL;
    QDoubleSpinBox* Simulation_Tab1_NuOtherDSB;
    QCheckBox*      Simulation_Tab1_NuOtherCB;

    bool loadYearlyParameters();
    bool saveYearlyParameters();

public:
    nmfSimulation_Tab1(QTabWidget*  tabs,
                       nmfLogger*   logger,
                       nmfDatabase* databasePtr,
                       std::string& projectDir);
    virtual ~nmfSimulation_Tab1();

    void clearWidgets();
    bool loadWidgets();
    void ReadSettings();
    void speciesChanged(QString species);
    QTableView* getYearlyParametersTable();
    QString getSpecies();
    void setSpecies(QString species);
    bool useNuOther();
    double getNuOther();

signals:
    void RunSimulation();

public Q_SLOTS:
    void callback_LoadPB();
    void callback_SavePB();
    void callback_NextPB();
    void callback_UpdateSuitabilityNuOther(double nuOther);
    void callback_NuOtherCB(int state);
    void callback_RunSimulation(double value);

};

#endif
