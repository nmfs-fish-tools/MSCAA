/**
 * @file nmfOutputControls.h
 * @brief This file contains the GUI definition for the MSCAA Output window's controls
 * @date June 1, 2019
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

#ifndef MSCAA_GUIOUTPUTCONTROLS_H
#define MSCAA_GUIOUTPUTCONTROLS_H

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QListView>
#include <QMessageBox>
#include <QObject>
#include <QStringListModel>

#include <nmfLogger.h>
#include <nmfDatabase.h>

/**
 * @brief The class containing the controls for the Output dock widget
 */
class MSCAA_GuiOutputControls: public QObject
{

    Q_OBJECT

    QGroupBox*   m_ControlsGroupBox;
    nmfLogger*   m_logger;
    nmfDatabase* m_databasePtr;
    std::string  m_ProjectDir;
    std::string  m_ProjectSettingsConfig;
    QStringListModel*             m_SpeciesOrGuildModel;
    std::map<QString,QStringList> m_SortedForecastLabelsMap;

    QLabel*      OutputTypeLBL;
    QLabel*      OutputSpeciesLBL;
    QLabel*      OutputAgeListLBL;
    QLabel*      OutputScaleLBL;
    QLabel*      OutputMortalityTypeLBL;
    QComboBox*   OutputSpeciesCMB;
    QComboBox*   OutputChartTypeCMB;
    QComboBox*   OutputScaleCMB;
    QCheckBox*   OutputLogCB; // Take natural log of data
    QCheckBox*   OutputNaturalMortalityCB;
    QCheckBox*   OutputFishingMortalityCB;
    QCheckBox*   OutputPredationMortalityCB;
    QListView*   OutputAgeListLV;
    QListView*   OutputMortalityListLV;

    QHash<QString,int> SpeciesHash;

    bool getSpecies(int& NumSpecies, QStringList& SpeciesList);
    void initConnections();
    void initWidgets();
    bool isAggProd();
    void readSettings();

public:
    /**
     * @brief Class constructor
     * @param controlsGroupBox : group box containing all of the output controls
     * @param logger : logger pointer with which to send messages to the logger
     * @param databasePtr : database pointer with which to send message to the database
     * @param projectDir : project directory
     */
    MSCAA_GuiOutputControls(QGroupBox*   controlsGroupBox,
                            nmfLogger*   logger,
                            nmfDatabase* databasePtr,
                            std::string& projectDir);
    virtual ~MSCAA_GuiOutputControls();

    /**
     * @brief Gets the selected age items
     * @return The list of selected age item indexes
     */
    QModelIndexList getAgeListSelectedIndexes();
    /**
     * @brief Gets the viewport associated with the age listview widget
     * @return The widget corresponding the the age listview
     */
    QWidget*        getAgeListViewport();
    /**
     * @brief Gets the current scale setting for the Output chart
     * @return QString representing the current output chart's scale value
     */
    QString         getOutputScale();
    /**
     * @brief Gets the current species name for the Output chart
     * @return  QString representing the current output chart's Species name
     */
    QString         getOutputSpecies();
    /**
     * @brief Gets the current species index for the Output chart
     * @return  integer representing the current output chart's Species index
     */
    int             getOutputSpeciesIndex();
    /**
     * @brief Gets the current chart type for the Output chart
     * @return  QString representing the current output chart's type
     */
    QString         getOutputChartType();
    /**
     * @brief Gets a list of all of the selected age values from the age listview
     * @return list of the selected ages
     */
    QStringList     getSelectedAges();
    /**
     * @brief Converts a species name to its index in the species combo box
     * @param species : name of species whose index is to be found
     * @return index of passed species name
     */
    int             getSpeciesNumFromName(QString species);
    /**
     * @brief Loads the age listview with the passed age list
     * @param ageList : list of ages to load the age listview with
     */
    void            loadAgeLV(QStringList& ageList);
    /**
     * @brief Selects the first item in the age listview
     */
    void            selectFirstAgeGroup();
    /**
     * @brief Selects the first item in the mortality listview
     * @param isMortality : unused (only implemented for "true")
     */
    void            selectFirstMortalityGroup(bool isMortality);
    /**
     * @brief Sets the chart type combo box to the passed value
     * @param type : name of chart type to set the chart type combo box at
     */
    void            setOutputChartType(QString type);
    /**
     * @brief Sets the species combobox value to the passed index
     * @param index : index of value of species to be set as current
     */
    void            setOutputSpeciesIndex(int index);
    /**
     * @brief Convenience function to tell if the user has checked the natural log box
     * @return true if the box has bee checked, else false
     */
    bool            showNaturalLogOfData();
    /**
     * @brief Convenience function to quickly tell if there are any selections in the age listview widget
     * @return true if something has been selected, else false
     */
    bool            thereAreSelections();
    /**
     * @brief Toggles the scale widget enabled-ness
     * @param enable : true or false for enabled or not enabled
     */
    void            toggleScaleWidgets(bool enable);

signals:
    /**
     * @brief Signal emitted when the user selects from the age listview
     * @param mode : unused (for future versions)
     * @param indexList : list of age indexes selected
     */
    void AbundanceAgeGroupsSelected(QString mode,
                                    QModelIndexList indexList);
    /**
     * @brief Signal emitted when the user selects from the mortality agelist
     * @param ageIndexList :  list of age indexes selected
     * @param mortalityTypeIndexList : list of mortality types
     */
    void MortalityAgeGroupsSelected(QModelIndexList ageIndexList,
                                    QModelIndexList mortalityTypeIndexList);
    /**
     * @brief Signal emitted when the user wants to show the chart
     * @param OutputType : the type of chart to show
     * @param OutputScale : the scale of the chart to be shown
     */
    void ShowChart(QString OutputType, QString OutputScale);
    /**
     * @brief Signal emitted when the user wants to update the age listview widget
     */
    void UpdateAgeList();

public slots:
    /**
     * @brief Callback invoked when the user makes a selection in the age listview
     * @param sel : what was just selected
     * @param desel : what was just deselected
     */
    void callback_AgeGroupSelectionChanged(QItemSelection sel, QItemSelection desel);
    /**
     * @brief Callback invoked when the user makes a selection in the mortality type listview
     * @param sel : what was just selected
     * @param desel : what was just deselected
     */
    void callback_MortalityTypeSelectionChanged(QItemSelection sel, QItemSelection desel);
    /**
     * @brief Callback invoked when the user changes the chart type
     * @param outputType : type of chart to be displayed
     */
    void callback_OutputChartTypeCMB(QString outputType);
    /**
     * @brief Callback invoked when the user checks the natural log checkbox
     * @param isChecked : describes if the box is checked or unchecked (true or false)
     */
    void callback_OutputLogCB(bool isChecked);
    /**
     * @brief Callback invoked when the user changes the output scale
     * @param scale : QString representing desired scale for the current chart
     */
    void callback_OutputScaleCMB(QString scale);
};




#endif
