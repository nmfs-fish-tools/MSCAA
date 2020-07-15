/**
 *
 * @file nmfMSCAATab04.h
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
 * for the 4th tabbed MSCAA pane.
 */

#ifndef NMFMSCAATAB4_H
#define NMFMSCAATAB4_H

#include "nmfMSCAATableIO.h"

#include <QComboBox>

/**
 * @brief The class containing the interactions for the 4th MSCAA tabbed pane
 */
class nmfMSCAA_Tab4: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;

//  nmfTableView* MSCAA_Tab4_ConsumptionTV;
    QTableView*   MSCAA_Tab4_ConsumptionTV;
    QTabWidget*   MSCAA_Tabs;
    QWidget*      MSCAA_Tab4_Widget;
    QPushButton*  MSCAA_Tab4_PrevPB;
    QPushButton*  MSCAA_Tab4_NextPB;
    QPushButton*  MSCAA_Tab4_LoadPB;
    QPushButton*  MSCAA_Tab4_SavePB;
    QComboBox*    MSCAA_Tab4_UnitsCMB;
    QVBoxLayout*  MSCAA_Tab4_TVLayoutLT;
    QLabel*       MSCAA_Tab4_SpeciesLBL;

    void clearWidgets();
    std::string getUnits();
    std::string getSpecies();
    void readSettings();
    void setUnits(std::string units);

public:
    /**
     * @brief Constructor for the nmfMSCAA_Tab04 class
     * @param tab : the parent tab widget
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param projectDir : string representing the project directory
     */
    nmfMSCAA_Tab4(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfMSCAA_Tab4();

    /**
     * @brief Called when the user changes the species and the Consumption::Biomass table must be updated
     * @param new species whose data should be displayed
     */
    void changeSpecies(QString species);
    /**
     * @brief Populates the widgets in this class with initial data
     * @return true if load was successful, else false
     */
    bool loadWidgets();
    /**
     * @brief Get the Consumption::Biomass ratio table
     * @return Returns the Consumption::Biomass ratio QTableView
     */
    QTableView* getTableConsumptionBiomass();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user presses the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user presses the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when user presses the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user presses the Save button
     */
    void callback_SavePB();

};

#endif
