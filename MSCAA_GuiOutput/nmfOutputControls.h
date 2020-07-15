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
#include <QStringListModel>

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
    QComboBox*   OutputTypeCMB;
    QComboBox*   OutputScaleCMB;
    QCheckBox*   OutputLogCB; // Take natural log of data
    QCheckBox*   OutputNaturalMortalityCB;
    QCheckBox*   OutputFishingMortalityCB;
    QCheckBox*   OutputPredationMortalityCB;
    QListView*   OutputAgeListLV;
    QListView*   OutputMortalityListLV;

    QHash<QString,int> SpeciesHash;

    void initWidgets();
    void initConnections();
    void readSettings();
    bool getSpecies(int& NumSpecies, QStringList& SpeciesList);
    bool isAggProd();
    void AlgorithmIdentifiers(std::string& Algorithm,
                              std::string& Minimizer,
                              std::string& ObjectiveCriterion,
                              std::string& Scaling,
                              std::string& CompetitionForm,
                              bool         showMsg);

    void saveSettings();

public:
    MSCAA_GuiOutputControls(QGroupBox*   ControlsGroupBox,
                            nmfLogger*   Logger,
                            nmfDatabase* DatabasePtr,
                            std::string& ProjectDir);
    virtual ~MSCAA_GuiOutputControls();

    int  getSpeciesNumFromName(QString SpeciesName);
    void loadSpeciesControlWidget();
    void loadWidgets();
    void loadAgeLV(QStringList& ageList);
    QStringList getSelectedAges();

    void refresh();
    QString         getOutputScale();
    QString         getOutputType();
    QString         getOutputSpecies();
    int             getOutputSpeciesIndex();
    QWidget*        getListViewViewport();
    QModelIndexList getListViewSelectedIndexes();
    void            setOutputType(QString type);
    void            setOutputSpeciesIndex(int index);
    void            selectFirstAgeGroup();
    void            selectFirstMortalityGroup(bool isMortality);
    bool            thereAreSelections();
    bool            showNaturalLogOfData();
    void            enableScaleWidgets(bool enable);

signals:
    void SetChartView2d(bool isVisible);
    void ShowChart(QString OutputType, QString OutputScale);
    void SelectCenterSurfacePoint();
    void UpdateSummaryStatistics();
    void UpdateAgeList();
    void AbundanceAgeGroupsSelected(QString mode,
                                    QModelIndexList indexList);
    void MortalityAgeGroupsSelected(QModelIndexList ageIndexList,
                                    QModelIndexList mortalityTypeIndexList);

public slots:
    void callback_OutputTypeCMB(QString outputType);
    void callback_OutputSpeciesCMB(QString outputSpecies);
    void callback_OutputScaleCMB(QString scale);
    void callback_AgeGroupSelectionChanged(QItemSelection sel, QItemSelection desel);
    void callback_MortalityTypeSelectionChanged(QItemSelection sel, QItemSelection desel);
    void callback_OutputLogCB(bool isChecked);
};




#endif
