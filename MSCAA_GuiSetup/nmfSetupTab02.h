/**
 * @file nmfSetupTab02.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 2nd tabbed Setup pane
 *
 * This file contains the GUI definition for the 2nd Setup Tabbed window.
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

#ifndef NMFSETUPTAB2_H
#define NMFSETUPTAB2_H

#include <QComboBox>
#include <QProgressDialog>

#include <nmfConstantsMSCAA.h>

/**
 * @brief The class containing the interactions for the 2nd Setup tabbed pane
 */
class nmfSetup_Tab2: public QObject
{
    Q_OBJECT

    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;

    QTabWidget*  Setup_Tabs;
    QWidget*     Setup_Tab2_Widget;
    QGroupBox*   Setup_Tab2_ProjectDataGB;
    QComboBox*   Setup_Tab2_ProjectDatabaseCMB;
    QPushButton* Setup_Tab2_ProjectDirBrowsePB;
    QPushButton* Setup_Tab2_DelDatabasePB;
    QPushButton* Setup_Tab2_AddDatabasePB;
    QPushButton* Setup_Tab2_RefreshDatabasePB;
    QPushButton* Setup_Tab2_SaveProjectPB;
    QPushButton* Setup_Tab2_NewProjectPB;
    QPushButton* Setup_Tab2_DelProjectPB;
    QPushButton* Setup_Tab2_BrowseProjectPB;
    QPushButton* Setup_Tab2_SetProjectPB;
    QPushButton* Setup_Tab2_ReloadProjectPB;
    QLineEdit*   Setup_Tab2_ProjectNameLE;
    QLineEdit*   Setup_Tab2_ProjectAuthorLE;
    QLineEdit*   Setup_Tab2_ProjectDescLE;
    QLineEdit*   Setup_Tab2_ProjectDirLE;
    QTextEdit*   SetupOutputTE;
    QProgressDialog* progressDlg;

    QString m_ProjectDir;
    QString m_ProjectDatabase;
    QString m_ProjectName;
    QString m_ProjectAuthor;
    QString m_ProjectDescription;
    std::set<std::string> m_ValidDatabases;

    bool authenticateDatabase(const std::string& databaseName);
    QString getProjectDatabase();
    QString getProjectName();
    void initDatabase(QString database);
    bool isProjectDataValid();
    bool isValidProjectName(QString projectName);
    void loadDatabaseNames(QString NameToSelect);
    void loadProject(nmfLogger *m_logger, QString fileName);
    void readSettings();
    void saveSettings();
    void setProjectAuthor(QString author);
    void setProjectDescription(QString description);
    void setProjectDirectory(QString directory);
    void setProjectName(QString name);

signals:
    /**
     * @brief Signal emitted signifying user wishes to load a new database
     * @param QString of database nameto load
     */
    void LoadDatabase(QString database);
    /**
     * @brief Signal emitted signifying user wishes to load a project
     */
    void LoadProject();
    /**
     * @brief Signal emitted signifying that all the applications widgets should be reloaded
     */
    void ReloadWidgets();
    /**
     * @brief Signal emitted signifying a project has just been saved
     */
    void SavedProject();

public:
    /**
     * @brief class constructor for nmfSetup_Tab2
     * @param tab : parent tab widget into which to place this widget as a child tabbed pane
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param setupOutputTE : unused; kept in for possible future usage
     */
    nmfSetup_Tab2(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  QTextEdit*   setupOutputTE);
    virtual ~nmfSetup_Tab2();

    /**
     * @brief Clears the line edit widgets in this tabbed pane
     */
    void clearProject();
    /**
     * @brief Creates all of the necessary database tables for the passed database
     * name. If this is called and any tables are already created, those tables
     * won't be affected.
     * @param database : QString name of database whose tables to create
     */
    void createTables(QString database);
    /**
     * @brief Enables all of the project data widget from the project data group box being checked
     */
    void enableProjectData();
    /**
     * @brief Set the enable state of the subsequent tabs in this tabbed pane group
     * @param bool enable state (true for enabled, false for disabled)
     */
    void enableSetupTabs(bool enable);
    /**
     * @brief Loads the current class widgets with initial data
     */
    void loadWidgets();
    /**
     * @brief Sets the current item in the pulldown to the passed database name
     * @param dbName : name of database to set the pulldown to
     */
    void setProjectDatabase(QString dbName);

public Q_SLOTS:
    /**
     * @brief Callback invoked when user clicks the add database button
     */
    void callback_Setup_Tab2_AddDatabase();
    /**
     * @brief Callback invoked when user clicks the browse projects button
     */
    void callback_Setup_Tab2_BrowseProject();
    /**
     * @brief Callback invoked when user changes the current database
     * @param dbName : QString name of new database
     */
    void callback_Setup_Tab2_DatabaseChanged(QString dbName);
    /**
     * @brief Callback invoked when user clicks the delete database button
     */
    void callback_Setup_Tab2_DelDatabase();
    /**
     * @brief Callback invoked when user clicks the delete project button
     */
    void callback_Setup_Tab2_DelProject();
    /**
     * @brief Callback invoked when user clicks the new project button
     */
    void callback_Setup_Tab2_NewProject();
    /**
     * @brief Callback invoked after user enters the author's name
     */
    void callback_Setup_Tab2_ProjectAuthorAdd();
    /**
     * @brief Callback invoked after user enters the project description
     */
    void callback_Setup_Tab2_ProjectDescAdd();
    /**
     * @brief Callback invoked after user enters a new project directory
     */
    void callback_Setup_Tab2_ProjectDirAdd();
    /**
     * @brief Callback invoked when user clicks the browse projects button
     */
    void callback_Setup_Tab2_ProjectDirBrowse();
    /**
     * @brief Callback invoked after user enters the project name
     */
    void callback_Setup_Tab2_ProjectNameAdd();
    /**
     * @brief Callback invoked when the user wants to refresh the database pulldown list
     */
    void callback_Setup_Tab2_RefreshDatabase();
    /**
     * @brief Callback invoked when user clicks the reload project button
     */
    void callback_Setup_Tab2_ReloadProject();
    /**
     * @brief Callback invoked when user clicks the save project button
     */
    void callback_Setup_Tab2_SaveProject();
    /**
     * @brief Callback invoked when user wants to cancel the
     * progress dialog after all tables have been created
     */
    void callback_progressDlgCancel();

};

#endif // NMFSETUPTAB2_H
