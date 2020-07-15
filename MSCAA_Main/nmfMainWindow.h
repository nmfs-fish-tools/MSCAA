#ifndef NMFMAINWINDOW_H
#define NMFMAINWINDOW_H

#include <QListView>
#include <QtTest/QtTest>
#include <QWhatsThis>
#include <QChartView>

#include "nmfDatabase.h"
#include "nmfDatabaseConnectDialog.h"
#include "nmfUtilsStatistics.h"
#include "nmfStructsQt.h"
#include "nmfUtilsQt.h"
#include "nmfEntityModel.h"
#include "nmfLogWidget.h"

#include "nmfSimulationTab01.h"
#include "nmfSimulationTab02.h"
#include "nmfSetupTab01.h"
#include "nmfSetupTab02.h"
#include "nmfSetupTab03.h"
#include "nmfSetupTab04.h"
#include "nmfSetupTab05.h"
#include "nmfSetupTab06.h"
#include "nmfSetupTab07.h"
#include "nmfSSCAATab01.h"
#include "nmfSSCAATab02.h"
#include "nmfSSCAATab03.h"
#include "nmfSSCAATab04.h"
#include "nmfSSCAATab05.h"
#include "nmfSSCAATab06.h"
#include "nmfMSCAATab01.h"
#include "nmfMSCAATab02.h"
#include "nmfMSCAATab03.h"
#include "nmfMSCAATab04.h"
#include "nmfMSCAATab05.h"
#include "nmfMSCAATableIO.h"
#include "nmfOutputControls.h"

#include <boost/version.hpp>
#include <boost/filesystem.hpp>
//#include <nlopt.hpp>

namespace Ui {
class nmfMainWindow;
}

class nmfMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit nmfMainWindow(QWidget *parent = nullptr);
    ~nmfMainWindow();

    /**
     * @brief Notifies main routine if MainWindow has been started correctly with MySQL active
     * @return true/false signifying the state of the application start
     */
    bool isStartUpOK();

private:
    Ui::nmfMainWindow *m_UI;

    std::string    m_ProjectDir;
    std::string    m_ProjectDatabase;
    std::string    m_ProjectName;
    std::string    m_ProjectSettingsConfig;
    int            m_SetupFontSize;
    QChart*        m_ChartWidget;
    QChartView*    m_ChartView2d;
    QWidget*       m_ChartView3d;
    Q3DSurface*    m_Graph3D;
    nmfLogger*     m_logger;
    nmfEntityModel m_entityModel;
    nmfDatabase*   m_databasePtr;
    std::string    m_Username;
    std::string    m_Password;
    QDialog*       m_PreferencesDlg;
    QWidget*       m_PreferencesWidget;
    bool           m_isStartUpOK;

    QWidget*       EntityListWidget;
    QListView*     EntityListLV;
    QLabel*        EntityListLBL;
    QWidget*       NavigatorTreeWidget;
    QTreeWidget*   NavigatorTree;
//    nmfSimulation_Tab1* Simulation_Tab1_ptr;
//    nmfSimulation_Tab2* Simulation_Tab2_ptr;
    nmfSetup_Tab1* Setup_Tab1_ptr;
    nmfSetup_Tab2* Setup_Tab2_ptr;
    nmfSetup_Tab3* Setup_Tab3_ptr;
    nmfSetup_Tab4* Setup_Tab4_ptr;
    nmfSetup_Tab5* Setup_Tab5_ptr;
    nmfSetup_Tab6* Setup_Tab6_ptr;
    nmfSetup_Tab7* Setup_Tab7_ptr;
    nmfSSCAA_Tab1* SSCAA_Tab1_ptr;
    nmfSSCAA_Tab2* SSCAA_Tab2_ptr;
    nmfSSCAA_Tab3* SSCAA_Tab3_ptr;
    nmfSSCAA_Tab4* SSCAA_Tab4_ptr;
    nmfSSCAA_Tab5* SSCAA_Tab5_ptr;
    nmfSSCAA_Tab6* SSCAA_Tab6_ptr;
    nmfMSCAA_Tab1* MSCAA_Tab1_ptr;
    nmfMSCAA_Tab2* MSCAA_Tab2_ptr;
    nmfMSCAA_Tab3* MSCAA_Tab3_ptr;
    nmfMSCAA_Tab4* MSCAA_Tab4_ptr;
    nmfMSCAA_Tab5* MSCAA_Tab5_ptr;
    MSCAA_GuiOutputControls* Output_Controls_ptr;


    void closeEvent(QCloseEvent *event);
    /**
     * @brief Forces user to input and save project data.  Until they do so, application
     * functionality is disabled (i.e., grayed out).
     */
    void enableApplicationFeatures(bool enable);
    QTableView* findTableInFocus();
    QString getADMBVersion();
    QString getNavigationGroup();
    QString getSpecies();
    void initConnections();
    void initGUIs();
    void initializeNavigatorTree();
    void initInputPages();
    void loadDatabase();
    void loadEntityList();
    void loadGUIs();
    bool queryUserPreviousDatabase();
    void readProjectSettings();
    void readSettings();
    void saveSettings();
    void setChartView(QString type);
    void setDefaultDockWidgetsVisibility();
    void setup2dChart();
    void setup3dChart();
    void setupLogWidget();
    void showDockWidgets(bool show);
    void updateWindowTitle();
    void initializePreferencesDlg();
    bool saveScreenshot(QString &outputFile, QPixmap &pm);
    void populateOutputDataTable(
            int& MinAge,
            int& FirstYear,
            boost::numeric::ublas::matrix<double> *Abundance);
signals:
    void EditTable(QString action);
    void SpeciesSelected(QString name);

public slots:
    void menu_about();
    void menu_clear();
    void menu_copy();
    void menu_createTables();
    void menu_deselectAll();
    void menu_exportDatabase();
    void menu_importDatabase();
    void menu_layoutDefault();
    void menu_layoutOutput();
    void menu_paste();
    void menu_pasteAll();
    void menu_preferences();
    void menu_quit();
    void menu_screenShot();
    void menu_selectAll();
    void menu_showLastADMBRun();
    void menu_showTableNames();
    void menu_whatsThis();
    void callback_AbundanceAgeGroupsSelected(
            QString mode,
            QModelIndexList indexList);
    void callback_EntityList(QModelIndex index);
    void callback_LoadUserSuppliedNuOther();
    void callback_MortalityAgeGroupsSelected(QModelIndexList ageIndexList,
                                             QModelIndexList mortalityTypendexList);
    void callback_MSCAATabChanged(int tab);
    void callback_NavigatorSelectionChanged();
    void callback_ProjectSaved();
    void callback_RefreshEntityList();
    void callback_ReloadWidgets();
    void callback_SelectDefaultSpecies();
    void callback_SetupTabChanged(int tab);
    void callback_ShowChart(const QString& mode);
    void callback_ShowChart(const QString& type,
                            const QString& scale);
    void callback_ShowChart(const QString& mode,
                            const QString& type,
                            const QString& scale);
    void callback_SimulationTabChanged(int tab);
    void callback_SSCAATabChanged(int tab);
    void callback_UpdateAgeList();
    void callback_PreferencesSetStyleSheet(QString style);

};

#endif // NMFMAINWINDOW_H
