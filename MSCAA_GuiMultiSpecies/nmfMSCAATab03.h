/**
 *
 * @file nmfMSCAATab03.h
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
 * for the 3rd tabbed MSCAA pane.
 */
#ifndef NMFMSCAATAB3_H
#define NMFMSCAATAB3_H

#include <QComboBox>

#include "nmfMSCAATableIO.h"

/**
 * @brief The class containing the interactions for the 3rd MSCAA tabbed pane
 */
class nmfMSCAA_Tab3: public QObject
{
    Q_OBJECT

    std::string   m_ProjectSettingsConfig;
    std::string   m_ProjectDir;
    nmfLogger*    m_logger;
    nmfDatabase*  m_databasePtr;
    int           m_currentSegmentIndex;

//  nmfTableView* MSCAA_Tab3_DietTV;
    QTableView*   MSCAA_Tab3_DietTV;
    QTabWidget*   MSCAA_Tabs;
    QWidget*      MSCAA_Tab3_Widget;
    QPushButton*  MSCAA_Tab3_NextPB;
    QPushButton*  MSCAA_Tab3_PrevPB;
    QPushButton*  MSCAA_Tab3_LoadPB;
    QPushButton*  MSCAA_Tab3_SavePB;
    QComboBox*    MSCAA_Tab3_PredatorSpeciesCMB;
    QComboBox*    MSCAA_Tab3_PredatorAgeCMB;
    QComboBox*    MSCAA_Tab3_UnitsCMB;
    QVBoxLayout*  MSCAA_Tab3_TVLayoutLT;
    QLabel*       MSCAA_Tab3_SpeciesLBL;
    QComboBox*    MSCAA_Tab3_NumSegCMB;
    QCheckBox*    MSCAA_Tab3_AutoFillSegCB;
    QComboBox*    MSCAA_Tab3_BinTypeCMB;

    void clearWidgets();
    QString getBinType();
    int getNumBins();
    int getNumSpecies();
    QString getPrMSCAA_Tab3_NextPBedatorAge();
    QString getPredatorSpecies();
    QString getPredatorAge();
    std::string getUnits();
    bool isAutoFillChecked();
    int loadNumBins(QString species);
    void setNumBins(int numBins);
    void readSettings();
    void setUnits(std::string units);

public:
    /**
     * @brief Constructor for the nmfMSCAA_Tab03 class
     * @param tab : the parent tab widget
     * @param logger : pointer to logger class
     * @param databasePtr : pointer to database class
     * @param projectDir : string representing the project directory
     */
    nmfMSCAA_Tab3(QTabWidget*  tab,
                  nmfLogger*   logger,
                  nmfDatabase* databasePtr,
                  std::string& projectDir);
    virtual ~nmfMSCAA_Tab3();

    /**
     * @brief Get the Diet data table
     * @return Returns the Diet data QTableView
     */
    QTableView* getTableDiet();
    /**
     * @brief Populates the widgets in this class with initial data
     * @return true if load was successful, else false
     */
    bool loadWidgets();

public Q_SLOTS:
    /**
     * @brief Callback invoked with user toggles the First/Last Year Segment checkbox
     * @param Returns boolean signifying whether or not the checkbox was checked
     */
    void callback_AutoFillSegCB(bool isAutoFillChecked);
    /**
     * @brief Callback invoked when user selects the bin type
     * @param Returns QString of bin type selected
     */
    void callback_BinTypeCMB(QString binPref);
    /**
     * @brief Callback invoked when user presses the Load button
     */
    void callback_LoadPB();
    /**
     * @brief Callback invoked when user presses the Next Page button
     */
    void callback_NextPB();
    /**
     * @brief Callback invoked when user selects item from Number of Years Per Bin combobox
     * @param Returns the number of years per bin selected
     */
    void callback_NumSegCMB(int value);
    /**
     * @brief Callback invoked when user presses the Previous Page button
     */
    void callback_PrevPB();
    /**
     * @brief Callback invoked when user selects item from the Predator Species combobox
     * @param Returns QString of predator species selected
     */
    void callback_PredatorSpeciesCMB(QString name);
    /**
     * @brief Callback invoked when user selects item from the Predator Age combobox
     * @param Returns QString of predator age selected
     */
    void callback_PredatorAgeCMB(QString age);
    /**
     * @brief Callback invoked when user presses the Save button
     */
    void callback_SavePB();
};

#endif
