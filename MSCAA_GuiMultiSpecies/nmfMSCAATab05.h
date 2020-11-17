/**
 *
 * @file nmfMSCAATab05.h
 * @date Nov 21, 2016
 * @author Ronald Klasky
 *
 * @copyright
 * Public Domain Notice\n
 *
 * National Oceanic And Atmospheric Administration\n\n
 *
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee/contractor and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic
 * And Atmospheric Administration and the U.S. Government have not placed any
 * restriction on its use or reproduction.  Although all reasonable efforts have
 * been taken to ensure the accuracy and reliability of the software and data,
 * the National Oceanic And Atmospheric Administration and the U.S. Government
 * do not and cannot warrant the performance or results that may be obtained
 * by using this software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express
 * or implied, including warranties of performance, merchantability or fitness
 * for any particular purpose.\n\n
 *
 * Please cite the author(s) in any work or product based on this material.
 *
 * @brief This file contains the class definition for the GUI elements and functionality
 * for the 5th tabbed MSCAA pane.
 */
#pragma once

#include <QProgressDialog>
#include <QComboBox>
#include <QChart>
#include <Q3DSurface>

#include "nmfChartSurface.h"
#include "nmfChartLine.h"
#include "VerboseDebugLevelDialog.h"

/**
 * @brief The class containing the interactions for the 5th MSCAA tabbed pane
 */
class nmfMSCAA_Tab5: public QObject
{
    Q_OBJECT

    int          m_FirstYear;
    int          m_MinAge;
    int          m_Trophic;
    std::string  m_ProjectSettingsConfig;
    std::string  m_ProjectDir;
    std::string  m_ProjectName;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    std::map<std::string,boost::numeric::ublas::matrix<double> > m_Abundance; // one matrix or every species

    QTabWidget*  MSCAA_Tabs;
    QWidget*     MSCAA_Tab5_Widget;
    QVBoxLayout* MSCAA_Tab5_ProportionsLT;
    QVBoxLayout* MSCAA_Tab5_PhaseLT;
    QTableView*  MSCAA_Tab5_ProportionsTV;
    QTableView*  MSCAA_Tab5_PhaseTV;
    QTextEdit*   MSCAA_Tab5_SummaryTE;
    QPushButton* MSCAA_Tab5_PrevPB;
    QPushButton* MSCAA_Tab5_LoadPB;
    QPushButton* MSCAA_Tab5_SavePB;
    QPushButton* MSCAA_Tab5_RunPB;
    QPushButton* MSCAA_Tab5_OptFilesPB;
    QPushButton* MSCAA_Tab5_DebugPB;
    QLineEdit*   MSCAA_Tab5_LogNormLE;
    QLineEdit*   MSCAA_Tab5_MultiResidLE;
    QLineEdit*   MSCAA_Tab5_DebugLE;

    void clearWidgets();
    int  getVerboseDebugLevel();
    bool loadPhases();
    bool loadProportions();
    bool loadSystemData();
    void readSettings();
    bool saveSpeciesData();
    bool saveSystemData();

signals:
    /**
     * @brief Clears the output chart and data areas
     */
    void ClearOutput();
    /**
     * @brief Signal signifying that the first species should be selected
     */
    void SelectDefaultSpecies();
    /**
     * @brief Signal signifying that the output charts should be updated
     * @param mode : species Single Species or Multi Species
     */
    void UpdateOutputCharts(QString mode);

public:
    nmfMSCAA_Tab5(QTabWidget*  tabs,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir,
                  std::string& projectName);
    virtual ~nmfMSCAA_Tab5();

    /**
     * @brief Gets the Abundance matrix data
     * @param Species : Species for which the abundance data will be returned
     * @return Matrix of abundance data
     */
    boost::numeric::ublas::matrix<double>* getAbundance(std::string Species);
    /**
     * @brief Get the Food Habits Proportions by Weight table
     * @return Returns the Food Habits Proportions QTableView
     */
    QTableView* getTableFoodHabitsProportions();
    /**
     * @brief Get the Phase table
     * @return Returns the Phase QTableView
     */
    QTableView* getTablePhase();
    /**
     * @brief Populates the widgets in this class with initial data
     * @return true if load was successful, else false
     */
    bool loadWidgets();
    /**
     * @brief Draws a 3d surface of the abundance data for the passed species
     * @param species : species whose data to show
     * @param showLogData : show data on a natural log scale
     * @param minAge : the minimum age group
     * @param firstYear : the first year of the abundance time-series
     * @param graph3D : qt surface on which to load the points to be displayed
     * @param xTitle : title to display for the x-axis
     * @param yTitle : title to display for the y-axis
     * @param zTitle : title to display for the z-axis
     * @param scale : QString representing the scale encoding
     * @param sf : The numeric scale factor (as a double)
     */
    void showChartAbundance3d(
            const std::string& species,
            const bool         showLogData,
            const int&         minAge,
            const int&         firstYear,
            Q3DSurface*        graph3D,
            const QString&     xTitle,
            const QString&     yTitle,
            const QString&     zTitle,
            const QString&     scale,
            const double&      sf);

    /**
     * @brief showChartAbundanceVsTime
     * @param chartWidget : the 2d chart on which to draw the passed data
     * @param species : the particular species whose data to draw
     * @param startYear : the first year of the abundance time-series
     * @param ageGroupNames : the data column labels to be used in the legend
     * @param data : the matrix of data to draw on the 2d chart
     * @param scale : QString representing the scale encoding
     */
    void showChartAbundanceVsTime(
            QChart* chartWidget,
            const std::string& species,
            const int& startYear,
            const QStringList& ageGroupNames,
            const boost::numeric::ublas::matrix<double>& data,
            const QString& scale);
    /**
     * @brief Read and load the last ADMB run's abundance data
     * @param The type of the last run (SingleSpecies or MultiSpecies)
     */
    void readLastReportFile(const QString& type);

public Q_SLOTS:
    /**
     * @brief Callback invoked when user presses the Debug button
     */
    void callback_DebugPB();
    /**
     * @brief Callback invoked when user presses the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user presses the Opt Files... button
     */
    void callback_OptFilesPB();
    /**
     * @brief Callback invoked when user presses the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user presses the Run button
     */
    void callback_RunPB();
    /**
     * @brief Callback invoked when user presses the Run button from the Single Species group
     */
    void callback_RunPB(int Trophic);
    /**
     * @brief Callback invoked when user presses the Save button
     */
    void callback_SavePB();

};

