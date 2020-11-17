/**
 * @file nmfSSCAATab01.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 1st tabbed SSCAA pane
 *
 * This file contains the GUI definition for the 1st tabbed SSCAA Tabbed window.
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
 */

#ifndef NMFSSCAATAB1_H
#define NMFSSCAATAB1_H

#include <string>

#include <QComboBox>
#include <QHeaderView>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

#include <nmfDatabase.h>
#include <nmfLogger.h>

/**
 * @brief The class containing the interactions for the 1st SSCAA tabbed pane: Fishery Catch-At-Age Data
 */
class nmfSSCAA_Tab1: public QObject
{
    Q_OBJECT

private:
    nmfDatabase*   m_databasePtr;
    nmfLogger*     m_logger;
    std::string    m_projectSettingsConfig;
    std::string    m_projectDir;
    QString        m_previousAbundanceUnits;
    QString        m_previousWeightUnits;

//  nmfTableView*  SSCAA_Tab1_CatchTV;
//  nmfTableView2* SSCAA_Tab1_TotalTV;
    QTableView*    SSCAA_Tab1_CatchAtAgeTV;
    QTableView*    SSCAA_Tab1_CatchAtLengthTV;
    QTableView*    SSCAA_Tab1_TotalTV;
    QTabWidget*    SSCAA_Tabs;
    QWidget*       SSCAA_Tab1_Widget;
    QComboBox*     SSCAA_Tab1_AbundanceUnitsCMB;
    QComboBox*     SSCAA_Tab1_WeightUnitsCMB;
    QPushButton*   SSCAA_Tab1_NextPB;
    QLabel*        SSCAA_Tab1_SpeciesCatchLBL;
    QLabel*        SSCAA_Tab1_SpeciesTotalLBL;
    QPushButton*   SSCAA_Tab1_LoadPB;
    QPushButton*   SSCAA_Tab1_SavePB;
    QVBoxLayout*   SSCAA_Tab1_AbundanceLT;
    QVBoxLayout*   SSCAA_Tab1_WeightLT;
    QComboBox*     SSCAA_Tab1_FleetCMB;
    QLabel*        SSCAA_Tab1_AbundanceTitleLBL;

    void clearWidgets();
    std::string getAbundanceUnits();
    std::string getFleet();
    std::string getSpecies();
    std::string getWeightUnits();
    bool isCatchAtAge(QString species);
    void readSettings();
    void setAbundanceUnits(std::string units);
    void setWeightUnits(std::string units);

public:
    /**
     * @brief class constructor for nmfSSCAA_Tab1
     * @param tab : parent tab widget into which to place this widget as a child tabbed pane
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param projectDir : the name of the project directory
     */
    nmfSSCAA_Tab1(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSSCAA_Tab1();

    /**
     * @brief Populates the widgets in this class with initial data
     */
    bool loadWidgets();
    /**
     * @brief Gets the Catch at Age table
     * @return Returns the QTableView containing the Catch at Age data
     */
    QTableView* getTableCatchAtAge();
    /**
     * @brief Gets the Catch at Length table
     * @return Returns the QTableView containing the Catch at Length data
     */
    QTableView* getTableCatchAtLength();
    /**
     * @brief Gets the Total Weight table
     * @return Returns the QTableView containing the Total Weight data
     */
    QTableView* getTableTotalWeight();
    /**
     * @brief Reloads the widgets after the species has been changed
     * @param species : name of current species
     */
    void setSpecies(QString species);

public Q_SLOTS:
    /**
     * @brief Callback invoked when user changes abundance units combobox
     * @param newUnits : QString value of current abundance units combobox item
     */
    void callback_AbundanceUnitsChanged(QString newUnits);
    /**
     * @brief Callback invoked when the user changes the fleet combobox
     * @param fleet : current fleet set in combobox
     */
    void callback_FleetCMB(QString fleet);
    /**
     * @brief Called when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Called when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Called when the user clicks the Save button
     */
    void callback_SavePB();
    /**
     * @brief Callback invoked when user moves the middle scroll bar
     * @param value : int value of scroll bar position
     */
    void callback_Slider1Moved(int value);
    /**
     * @brief Callback invoked when user moves the right scroll bar
     * @param value : int value of scroll bar position
     */
    void callback_Slider2Moved(int value);
    /**
     * @brief Callback invoked when user changes weight units combobox
     * @param newUnits : QString value of current weight units combobox item
     */
    void callback_WeightUnitsCMB(QString newUnits);
};

#endif // NMFSSCAATAB1_H
