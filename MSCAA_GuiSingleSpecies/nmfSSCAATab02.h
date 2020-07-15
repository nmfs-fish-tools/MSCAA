/**
 * @file nmfSSCAATab02.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 2nd tabbed SSCAA pane
 *
 * This file contains the GUI definition for the 2nd tabbed SSCAA Tabbed window.
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
#ifndef NMFSSCAATAB2_H
#define NMFSSCAATAB2_H

#include <string>
#include <QComboBox>

/**
 * @brief The class containing the interactions for the 2nd SSCAA tabbed pane
 */
class nmfSSCAA_Tab2: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    QString       m_originalUnits;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;

    QTableView*   SSCAA_Tab2_CatchTV;
    QTableView*   SSCAA_Tab2_TotalTV;
//  nmfTableView* SSCAA_Tab2_CatchTV;
    QTabWidget*   SSCAA_Tabs;
    QWidget*      SSCAA_Tab2_Widget;
    QPushButton*  SSCAA_Tab2_PrevPB;
    QPushButton*  SSCAA_Tab2_NextPB;
    QPushButton*  SSCAA_Tab2_LoadPB;
    QPushButton*  SSCAA_Tab2_SavePB;
    QComboBox*    SSCAA_Tab2_UnitsCMB;
    QComboBox*    SSCAA_Tab2_TotalUnitsCMB;
    QLabel*       SSCAA_Tab2_SpeciesLBL;
    QVBoxLayout*  SSCAA_Tab2_TVLT;
    QVBoxLayout*  SSCAA_Tab2_TotalTVLT;
    QComboBox*    SSCAA_Tab2_SurveyNumCMB;

    void clearWidgets();
    std::string getUnits();
    std::string getTotalUnits();
    std::string getSpecies();
    int getSurveyNum();
    void loadNumSurveys(QString species);
    void readSettings();
    void setUnits(std::string units);

public:
    /**
     * @brief class constructor for nmfSSCAA_Tab1
     * @param tab : parent tab widget into which to place this widget as a child tabbed pane
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param projectDir : the name of the project directory
     */
    nmfSSCAA_Tab2(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfSSCAA_Tab2();
    /**
     * @brief Populates the widgets in this class with initial data
     */
    bool loadWidgets();
    /**
     * @brief Reloads the widgets after the species has been changed
     * @param species : name of current species
     */
    void setSpecies(QString species);
    /**
     * @brief Gets the Survey Catch at Age table
     * @return Returns the QTableView containing the Survey Catch at Age data
     */
    QTableView* getTableSurveyCatchAtAge();
    /**
     * @brief Gets the Survey Total table
     * @return Returns the QTableView containing the Survey Total data
     */
    QTableView* getTableSurveyTotal();

public Q_SLOTS:
    /**
     * @brief Called when the user clicks the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Called when the user clicks the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Called when the user clicks the Previous Page button
     */
    void callback_PrevPB();
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
     * @brief Callback invoked when user changes the survey number combobox
     * @param survey : the current survey number
     */
    void callback_SurveyNumCMB(QString survey);
    /**
     * @brief Callback invoked when user Survey Total units combobox
     * @param newUnits : QString value of current Survey Total units combobox item
     */
    void callback_TotalUnitsCMB(QString newUnits);
    /**
     * @brief Callback invoked when user changes Catch at Age units combobox
     * @param newUnits : QString value of current Survey Catch at Age units combobox item
     */
    void callback_UnitsChangedCMB(QString newUnits);

};

#endif
