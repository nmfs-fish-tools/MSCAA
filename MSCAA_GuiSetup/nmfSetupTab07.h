/**
 * @file nmfSetupTab07.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 7th tabbed Setup pane
 *
 * This file contains the GUI definition for the 7th Setup Tabbed window.
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

#ifndef NMFSETUPTAB7_H
#define NMFSETUPTAB7_H

#include <QCheckBox>
#include <QInputDialog>
#include <QSpinBox>

#include "nmfConstantsMSCAA.h"

class QDialogButtonBox;
class QDialog;
class QComboBox;

/**
 * @brief The class containing the interactions for the 7th Setup tabbed pane
 */
class nmfSetup_Tab7: public QObject
{
    Q_OBJECT

    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;

    QTabWidget*  Setup_Tabs;
    QWidget*     Setup_Tab7_Widget;
    QVBoxLayout* Setup_Tab7_AveLengthLT;
    QTableView*  Setup_Tab7_AgeLengthTV;
    QPushButton* Setup_Tab7_MinMaxPB;
    QPushButton* Setup_Tab7_LoadPB;
    QPushButton* Setup_Tab7_SavePB;
    QPushButton* Setup_Tab7_PrevPB;
    QLabel*      Setup_Tab7_SpeciesLBL;

    void clearWidgets();
    QString getSpecies();
    void getSpeciesData(QString& Species,
                        int& MinAge,
                        int& MaxAge,
                        int& FirstYear,
                        int& LastYear,
                        int& MinLength,
                        int& MaxLength);
    void readSettings();
    void saveSettings();

public:
    /**
     * @brief class constructor for nmfSetup_Tab7
     * @param tab : parent tab widget into which to place this widget as a child tabbed pane
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param setupOutputTE : unused; kept in for possible future usage
     * @param projectDir : the name of the project directory
     */
    nmfSetup_Tab7(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  QTextEdit*   setupOutputTE,
                  std::string& projectDir);
    virtual ~nmfSetup_Tab7();

    /**
     * @brief Populates the widgets in this class with initial data
     */
    void loadWidgets();
    /**
     * @brief Reloads the widgets after the species has been changed
     * @param species : name of current species
     */
    void setSpecies(QString species);
    /**
     * @brief Gets the Age Length Key table
     * @return Returns the QTableView containing the Age Length Key data
     */
    QTableView* getTableAgeLengthKey();

public Q_SLOTS:
    /**
     * @brief Callback invoked when user presses the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user presses the Set Min/Max button.
     * This button sets the average lengths for the first and last ages
     * using length data specified in the Species table
     */
    void callback_MinMaxPB();
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
