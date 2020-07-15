/**
 * @file nmfSetupTab06.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 6th tabbed Setup pane
 *
 * This file contains the GUI definition for the 6th Setup Tabbed window.
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

#ifndef NMFSETUPTAB6_H
#define NMFSETUPTAB6_H

#include <nmfMSCAATableIO.h>

#include <QCheckBox>
#include <QInputDialog>
#include <QSpinBox>

#include "nmfConstantsMSCAA.h"

class QComboBox;
class QDialog;
class QDialogButtonBox;
class QRadioButton;

/**
 * @brief The class containing the interactions for the 6th Setup tabbed pane
 */
class nmfSetup_Tab6: public QObject
{
    Q_OBJECT

    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;

    QTabWidget*  Setup_Tabs;
    QWidget*     Setup_Tab6_Widget;
    QVBoxLayout* Setup_Tab6_FleetNameLT;
    QTableView*  Setup_Tab6_FleetNameTV;
    QPushButton* Setup_Tab6_LoadPB;
    QPushButton* Setup_Tab6_SavePB;
    QPushButton* Setup_Tab6_PrevPB;
    QPushButton* Setup_Tab6_NextPB;
    QRadioButton* Setup_Tab6_AbundanceDriverFRB;
    QRadioButton* Setup_Tab6_AbundanceDriverCRB;

    void clearWidgets();
    void getSpecies(std::vector<std::string>& SpeciesVec,
                    std::map<std::string,int>& SpeciesFleetsMap,
                    int& MaxNumFleets);
    void loadFleetData();
    void loadSystemData(bool emitSignal);
    void readSettings();
    bool saveFleetData();
    void saveSettings();
    bool saveSystemData();

public:
    /**
     * @brief class constructor for nmfSetup_Tab6
     * @param tab : parent tab widget into which to place this widget as a child tabbed pane
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param setupOutputTE : unused; kept in for possible future usage
     * @param projectDir : the name of the project directory
     */
    nmfSetup_Tab6(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  QTextEdit*   setupOutputTE,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab6();

    /**
     * @brief Populates the widgets in this class with initial data
     */
    void loadWidgets();
    /**
     * @brief Reloads the widgets after the species has been changed
     * @param species : unused
     */
    void setSpecies(QString species);
    /**
     * @brief Gets the Fleet table
     * @return Returns the QTableView containing the Fleet data
     */
    QTableView* getTableFleetData();

signals:
    /**
     * @brief Signal signifying that system data was loaded
     */
    void LoadedSystemData();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user presses the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user wants to reload system data without emitting a signal
     */
    void callback_LoadSystemDataNoEmit();
    /**
     * @brief Callback invoked when user presses the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when user presses the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Signal emitted signifying a project has just been saved
     */
    void callback_SavePB();
};

#endif
