/**
 *
 * @file nmfMSCAATab02.h
 * @date Nov 21, 2016
 * @author Ronald Klasky
 *
 * @brief This file contains the class definition for the GUI elements and functionality
 * for the 2nd tabbed MSCAA pane.
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

#ifndef NMFMSCAATAB2_H
#define NMFMSCAATAB2_H

#include <QComboBox>

#include "nmfMSCAATableIO.h"

/**
 * @brief The class containing the interactions for the 2nd MSCAA tabbed pane
 */
class nmfMSCAA_Tab2: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;
    int           m_currentSegmentIndex;

    QTabWidget*   MSCAA_Tabs;
    QWidget*      MSCAA_Tab2_Widget;
    QTabWidget*   MSCAA_Tab2_RatioTBW;
    QPushButton*  MSCAA_Tab2_PrevPB;
    QPushButton*  MSCAA_Tab2_NextPB;
    QPushButton*  MSCAA_Tab2_LoadPB;
    QPushButton*  MSCAA_Tab2_SavePB;
    QTableView*   MSCAA_Tab2_PreferredTV;
    QTableView*   MSCAA_Tab2_VarianceLessThanTV;
    QTableView*   MSCAA_Tab2_VarianceGreaterThanTV;
    QVBoxLayout*  MSCAA_Tab2_PreferredLT;
    QVBoxLayout*  MSCAA_Tab2_VarianceLessThanLT;
    QVBoxLayout*  MSCAA_Tab2_VarianceGreaterThanLT;

    void clearWidgets();
    int  getNumSpecies();
    void getSpecies(std::vector<std::string>& species);
    void loadTable(QTableView* tableView,
                   std::string tableName);
    void readSettings();
    void saveTable(QTableView* tableView,
                   std::string tableName);

public:
    /**
     * @brief Constructor for the nmfMSCAA_Tab02 class
     * @param tab : the parent tab widget
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param projectDir : string representing the project directory
     */
    nmfMSCAA_Tab2(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfMSCAA_Tab2();

    /**
     * @brief Get the Preferred Weight Ratio table
     * @return Returns the Preferred Weight Ratio QTableView
     */
    QTableView* getTablePreferred();
    /**
     * @brief Get the Variance Weight Ratio Less than Nu table
     * @return Returns the Variance Weight Ratio Less than Nu QTableView
     */
    QTableView* getTableVarianceLessThan();
    /**
     * @brief Get the Variance Weight Ratio Greater than Nu table
     * @return Returns the Variance Weight Ratio Greater than Nu QTableView
     */
    QTableView* getTableVarianceGreaterThan();
    /**
     * @brief Populates the widgets in this class with initial data
     * @return true
     */
    bool loadWidgets();

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
     * @brief Callback invoked when user presses the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user presses the Next Page button
     */
    void callback_NextPB();

};

#endif
