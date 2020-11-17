/**
 * @file nmfSetupTab03.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 3rd tabbed Setup pane
 *
 * This file contains the GUI definition for the 3rd Setup Tabbed window.
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

#ifndef NMFSETUPTAB3_H
#define NMFSETUPTAB3_H

#include "nmfUtilsQt.h"
#include "nmfConstantsMSCAA.h"

/**
 * @brief The class containing the interactions for the 3rd Setup tabbed pane
 */
class nmfSetup_Tab3: public QObject
{
    Q_OBJECT

    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    QStandardItemModel* m_smodelSpecies;
    QStandardItemModel* m_smodelOtherPredSpecies;
    std::vector<std::string> m_modelPresetNames;
    std::map<std::string,std::vector<std::string> > m_modelPresets;
    QStringList m_colLabelsSpecies;

    QTabWidget*   Setup_Tabs;
    QComboBox*    Setup_Tab3_ModelTypeCMB;
    QComboBox*    Setup_Tab3_RandomnessCMB;
    QComboBox*    Setup_Tab3_ModelNameCMB;
    QComboBox*    Setup_Tab3_GrowthFormCMB;
    QComboBox*    Setup_Tab3_PredationFormCMB;
    QComboBox*    Setup_Tab3_HarvestFormCMB;
    QComboBox*    Setup_Tab3_CompetitionFormCMB;
    QWidget*      Setup_Tab3_Widget;
    QPushButton*  Setup_Tab3_CreateTemplatesPB;
    QSpinBox*     Setup_Tab3_NumSpeciesSB;
    QTableWidget* Setup_Tab3_SpeciesTW;
    QLineEdit*    Setup_Tab2_ProjectDirLE;
    QLineEdit*    Setup_Tab2_FirstYearLE;
    QLineEdit*    Setup_Tab2_LastYearLE;
    QLineEdit*    Setup_Tab2_NumSeasonsLE;
    QCheckBox*    Setup_Tab3_SpeciesCB;
    QLabel*       Setup_Tab3_NumSpeciesLBL;
    QPushButton*  Setup_Tab3_AddSpeciesPB;
    QPushButton*  Setup_Tab3_DelSpeciesPB;
    QPushButton*  Setup_Tab3_SavePB;
    QPushButton*  Setup_Tab3_LoadPB;
    QPushButton*  Setup_Tab3_PrevPB;
    QPushButton*  Setup_Tab3_NextPB;
    QPushButton*  Setup_Tab3_ReloadSpeciesPB;

    void clearWidgets();
    int  getNumColumnsSpecies();
    void loadSpecies();
    void populateARowSpecies(int row, int ncols);
    void readSettings();
    void removeFromTable(std::string field,
                         QTableWidgetItem* itemToRemove,
                         QList<QString>& TablesToDeleteFrom);
    void saveFleetData();
    void saveSettings();

public:
    /**
     * @brief class constructor for nmfSetup_Tab3
     * @param tab : parent tab widget into which to place this widget as a child tabbed pane
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param setupOutputTE : unused; kept in for possible future usage
     * @param projectDir : the name of the project directory
     */
    nmfSetup_Tab3(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  QTextEdit*   setupOutputTE,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab3();

    /**
     * @brief Get the table containing the Species data
     * @return returns the Species qtable widget
     */
    QTableWidget* getTableSpecies();
    /**
     * @brief Load all of the widgets for this tabbed pane
     */
    void loadWidgets();

signals:
    void ReloadWidgets();
    /**
     * @brief Signal emitted signifying that new species have been loaded
     */
    void LoadedSpecies();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user presses the Add Species button
     */
    void callback_Setup_Tab3_AddSpeciesPB();
    /**
     * @brief Callback invoked when user presses the Del Species button
     */
    void callback_Setup_Tab3_DelSpeciesPB();
    /**
     * @brief Callback invoked when user presses the Load button
     */
    void callback_Setup_Tab3_LoadPB();
    /**
     * @brief Callback invoked when user presses the Next Page button
     */
    void callback_Setup_Tab3_NextPB();
    /**
     * @brief Callback invoked when user changes the number of species combobox
     * @param value : the number of species to be defined and used
     */
    void callback_Setup_Tab3_NumSpeciesCMB(int value);
    /**
     * @brief Callback invoked when user presses the Previous Page button
     */
    void callback_Setup_Tab3_PrevPB();
    /**
     * @brief Callback invoked when user presses the Save button
     */
    void callback_Setup_Tab3_SavePB();
    /**
     * @brief Callback invoked when user checks the Species widgets on/off
     * @param state : checked state of Species group box
     */
    void callback_Setup_Tab3_SpeciesCB(bool state);
    /**
     * @brief Callback invoked when user enters a value in a cell in the Species table
     */
    void callback_Setup_Tab3_SpeciesTableChanged(int,int);

};

#endif // NMFSETUPTAB4_H
