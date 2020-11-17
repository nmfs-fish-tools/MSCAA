/**
 *
 * @file nmfMSCAATab01.h
 * @date Nov 21, 2016
 * @author Ronald Klasky
 *
 * @brief This file contains the class definition for the GUI elements and functionality
 * for the 1st tabbed MSCAA pane.
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
 */

#ifndef NMFMSCAATAB1_H
#define NMFMSCAATAB1_H

#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>

#include "nmfMSCAATableIO.h"

/**
 * @brief The class containing the interactions for the 1st MSCAA tabbed pane
 */
class nmfMSCAA_Tab1: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    QString       m_OriginalUnits;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;
    int           m_currentSegmentIndex;

    QTabWidget*   MSCAA_Tabs;
    QWidget*      MSCAA_Tab1_Widget;
    QVBoxLayout*  MSCAA_Tab1_LayoutLT;
    QVBoxLayout*  MSCAA_Tab1_LayoutLT2;
    QLineEdit*    MSCAA_Tab1_TotalBiomassLE;
    QComboBox*    MSCAA_Tab1_UnitsCMB;
    QLineEdit*    MSCAA_Tab1_FirstYearLE;
    QLineEdit*    MSCAA_Tab1_LastYearLE;
    QTableView*   MSCAA_Tab1_InteractionsTV;
    QTableView*   MSCAA_Tab1_VulnerabilityTV;
    QPushButton*  MSCAA_Tab1_NextPB;
    QPushButton*  MSCAA_Tab1_LoadPB;
    QPushButton*  MSCAA_Tab1_SavePB;
    QTableView*   MSCAA_Tab1_PredPreyInteractionTV;
    QLineEdit*    MSCAA_Tab1_OtherFoodWtLE;

    void clearWidgets();
    int  getNumInteractions();
    int  getNumSpecies();
    std::string getUnits();
    bool loadInteractionsData();
    bool loadInteractionsVecData();
    bool loadSystemData();
    bool loadVulnerabilityData();
    void readSettings();
    bool saveInteractionsData();
    bool saveSystemData();
    bool saveVulnerabilityData();
    bool saveInteractionsVecData();
    void setUnits(std::string units);
    void updatePredPreyInteractionTable();

public:
    /**
     * @brief Constructor for the nmfMSCAA_Tab01 class
     * @param tab : the parent tab widget
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param projectDir : string representing the project directory
     */
    nmfMSCAA_Tab1(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfMSCAA_Tab1();

    /**
     * @brief Get the Interactions table
     * @return Returns the Interactions QTableView
     */
    QTableView* getTableInteractions();
    /**
     * @brief Get the Vulnerability table
     * @return Returns the Vulnerability QTableView
     */
    QTableView* getTableVulnerability();
    /**
     * @brief Populates the widgets in this class with initial data
     * @return true
     */
    bool loadWidgets();

signals:
    /**
     * @brief Signal signifying that the system data for this GUI has been successfully loaded
     */
    void LoadedSystemData();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user presses the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user presses the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when user presses the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when user changes the Total Ecosystem Biomass units
     * @param QString representing the units
     */
    void callback_UnitsCMB(QString units);
};

#endif
