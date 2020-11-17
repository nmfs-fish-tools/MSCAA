/**
 * @file nmfSetupTab04.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 4th tabbed Setup pane
 *
 * This file contains the GUI definition for the 4th Setup Tabbed window.
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

#ifndef NMFSETUPTAB4_H
#define NMFSETUPTAB4_H

#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>

#include "nmfMSCAATableIO.h"
#include "nmfConstantsMSCAA.h"

/**
 * @brief The class containing the interactions for the 4th Setup tabbed pane
 */
class nmfSetup_Tab4: public QObject
{
    Q_OBJECT

    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;

    QTabWidget*  Setup_Tabs;
    QWidget*     Setup_Tab4_Widget;
    QPushButton* Setup_Tab4_LoadPB;
    QPushButton* Setup_Tab4_SavePB;
    QPushButton* Setup_Tab4_PrevPB;
    QPushButton* Setup_Tab4_NextPB;
    QTableView*  Setup_Tab4_CovariatesTV;
    QVBoxLayout* Setup_Tab4_CovariatesLT;
    QSpinBox*    Setup_Tab4_CovariatesSB;
    QLabel*      Setup_Tab4_SpeciesLBL;

    void clearWidgets();
    QString getSpecies();
    void readSettings();
    void refreshTable();
    void saveSettings();

public:
    /**
     * @brief class constructor for nmfSetup_Tab4
     * @param tab : parent tab widget into which to place this widget as a child tabbed pane
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param setupOutputTE : unused; kept in for possible future usage
     * @param projectDir : the name of the project directory
     */
    nmfSetup_Tab4(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  QTextEdit*   setupOutputTE,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab4();

    /**
     * @brief Gets the environmental covariates table
     * @return Returns the Environmental Covariates QTableView widget
     */
    QTableView* getTableEnvCovariates();
    /**
     * @brief Populates the widgets in this class with initial data
     * @return true if load was successful, else false
     */
    bool loadWidgets();
    /**
     * @brief Sets this GUI's widget data to reflect the passed species
     * @param species : species to set and whose data to load
     */
    void setSpecies(QString species);

public Q_SLOTS:
    /**
     * @brief Callback invoked when user changes number of covariates widget
     * @param value : number of covariates set by the user
     */
    void callback_CovariatesSB(int value);
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
     * @brief Signal emitted signifying a project has just been saved
     */
    void callback_SavePB();

};

#endif // NMFSETUPTAB4_H
