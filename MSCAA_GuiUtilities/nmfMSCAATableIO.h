/**
 * @file nmfMSCAATableIO.h
 * @brief Contains the MSCAA namespace utility functions
 *
 */

#pragma once

#include <QComboBox>
#include <QTableView>
#include <QTextEdit>
#include <QStandardItemModel>

#include "nmfDatabase.h"
#include "nmfLogger.h"
#include "nmfADMB.h"

/**
 * @brief These are the MSCAA utility namespace functions
 */
namespace nmfMSCAAUtils {

/**
 * @brief Assigns the first year into the first segment and the last year into the last segment
 * @param firstYear : the first year of the data
 * @param lastYear : the last year of the data
 * @param numSegments : total number of segments
 * @param firstYears : vector of segment first years
 * @param lastYears : vector of segment last years
 */
void calculateFirstLastSegments(int firstYear,
                                int lastYear,
                                int numSegments,
                                std::vector<QString>& firstYears,
                                std::vector<QString>& lastYears);

/**
 * @brief Calculates the year range for each segment
 * @param firstYear : the first year of the data
 * @param lastYear : the last year of the data
 * @param numSegments : total number of segments
 * @param firstYears : vector of segment first years
 * @param lastYears : vector of segment last years
 */
void calculateSegmentYears(int firstYear,
                           int lastYear,
                           int numSegments,
                           std::vector<QString>& firstYears,
                           std::vector<QString>& lastYears);

/**
 * @brief Checks every first and last year in each segment that they're in the correct range
 * @param logger : pointer to logger api class
 * @param firstYear : the first year of the data
 * @param lastYear : the last year of the data
 * @param smodel : data model for all year ranges
 * @return Returns True if no error, else False
 */
bool checkYearValues(
        nmfLogger* logger,
        int firstYear,
        int lastYear,
        QStandardItemModel* smodel);

/**
 * @brief Creates bins table based upon the passed total number of bins requested
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget
 * @param table : name of database table
 * @param numBins : total number of bins
 * @param predatorName : predator name
 * @param predatorAge : predator age
 * @param isAutoFillChecked : boolean signifying if the auto fill checkbox is checked
 */
void createNewNumberOfBinsTable(nmfDatabase* databasePtr,
                                nmfLogger*   logger,
                                std::string  projectSettingsConfig,
                                QTableView*  tableView,
                                std::string  table,
                                int          numBins,
                                QString      predatorName,
                                QString      predatorAge,
                                bool         isAutoFillChecked);

/**
 * @brief Creates bins table based upon passed number of years per bin
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget
 * @param table : name of database table
 * @param numYearsPerBin : number of years per bin
 * @param predatorName : predator name
 * @param predatorAge : predator age
 * @param isAutoFillChecked : boolean signifying if the auto fill checkbox is checked
 */
void createNewYearsPerBinTable(nmfDatabase* databasePtr,
                               nmfLogger*   logger,
                               std::string  projectSettingsConfig,
                               QTableView*  tableView,
                               std::string  table,
                               int          numYearsPerBin,
                               QString      predatorName,
                               QString      predatorAge,
                               bool         isAutoFillChecked);

/**
 * @brief Gets the directory in which the ADMB files are placed
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectDir : the directory in which the project exists
 * @param projectName : the name of the project
 * @param projectSettingsConfig : name of the project settings config file
 * @param type : model type ("SingleSpecies" or "MultiSpecies")
 * @param summaryTextBox
 * @return
 */
QString getADMBDir(
        nmfDatabase*   databasePtr,
        nmfLogger*     logger,
        std::string&   projectDir,
        std::string&   projectName,
        std::string&   projectSettingsConfig,
        const QString& type,
        QTextEdit*     summaryTextBox);

/**
 * @brief Gets vector of strings describing a species age groups from min age to max age
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param species : the species whose age groups are to be returned
 * @param ages : vector of age group string values (i.e., "Age 1", "Age 2", ...)
 */
void getAllAges(nmfDatabase* databasePtr,
                nmfLogger*   logger,
                QString      species,
                std::vector<std::string>& ages);

/**
 * @brief Gets vector of strings describing a species year range starting with 1
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param species : the species whose age groups are to be returned
 * @param firstYear : the first year of a species' data range
 * @param years : vector of years (starting with "1") of a species time period range
 */
void getAllYears(nmfDatabase* databasePtr,
                 nmfLogger* logger,
                 QString species,
                 int& firstYear,
                 std::vector<std::string>& years);

/**
 * @brief Gets the database data corresponding to the passed table name
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param species : the species whose data to query from database
 * @param fleet : fleet to specify in database query if table is equal to "CatchFishery"
 * @param numYears : number of years of data
 * @param numAges : number of age groups of data
 * @param tableName : name of database table
 * @param tableData : matrix of data to return
 * @return True if data query was successful, else False
 */
bool getDatabaseData(
        nmfDatabase*       databasePtr,
        nmfLogger*         logger,
        const std::string& projectSettingsConfig,
        const std::string& species,
        const std::string& fleet,
        const int&         numYears,
        const int&         numAges,
        const std::string& tableName,
        boost::numeric::ublas::matrix<double> &tableData);

/**
 * @brief Gets the food habits first year and last year values
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param systemName : name of model system
 * @param firstYear : first year of data
 * @param lastYear : last year of data
 * @return True if data query was successful, else False
 */
bool getFHYearRange(nmfDatabase* databasePtr,
                    nmfLogger*   logger,
                    std::string  systemName,
                    int&         firstYear,
                    int&         lastYear);

/**
 * @brief Gets the mortality data corresponding to the passed parameters
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param species : the species whose data to query from database
 * @param numYears : number of years of data
 * @param minAge : minimum age of species
 * @param maxAge : maximum age of species
 * @param tableName : name of database table
 * @param tableData : matrix containing the returned mortality data
 * @return True if data query was successful, else False
 */
bool getMortalityData(
        nmfDatabase* databasePtr,
        nmfLogger*   logger,
        const std::string&  projectSettingsConfig,
        const std::string &species,
        const int &numYears,
        const int &minAge,
        const int &maxAge,
        const std::string &tableName,
        boost::numeric::ublas::matrix<double> &tableData);

/**
 * @brief Gets number of bins from passed table
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableName : name of database table
 * @param binType : type of bin (i.e., "Years per Bin" or "Number of Bins")
 * @param predatorName : predator name
 * @param predatorAge : predator age
 * @return Returns number of bins
 */
int getNumBins(nmfDatabase* databasePtr,
               nmfLogger*   logger,
               std::string  projectSettingsConfig,
               std::string  table,
               QString&     binType,
               QString      predatorName,
               QString      predatorAge);

/**
 * @brief Load the specified binned table into the passed tableView widget
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget into which to load the table data
 * @param table : name of database table
 * @param numBins : number of bins
 * @param binType : type of bin (i.e., "Years per Bin" or "Number of Bins")
 * @param predatorName : predator name
 * @param predatorAge : predator age
 */
void loadBinnedTable(nmfDatabase* databasePtr,
                     nmfLogger*  logger,
                     std::string projectSettingsConfig,
                     QTableView* tableView,
                     std::string table,
                     int         numBins,
                     QString     binType,
                     QString     predatorName,
                     QString     predatorAge);

/**
 * @brief Loads the Catch at Length data into the passed tableView widget
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget into which to load the table data
 * @param table : name of database table
 * @param species : species whose data to load
 * @param fleet : fleet whose data to load
 * @param units : units of data
 */
void loadCatchAtLengthTable(nmfDatabase* databasePtr,
                            nmfLogger*   logger,
                            std::string  projectSettingsConfig,
                            QTableView*  tableView,
                            std::string  table,
                            QString      species,
                            std::string  fleet);

/**
 * @brief Load the passed combobox with the type of data specified
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param cbox : the combobox to load
 * @param type : type of data to load (i.e., "Name","Age","Year")
 * @param species : the current species' data to get
 */
void loadComboBox(nmfDatabase* databasePtr,
                  nmfLogger*   logger,
                  QComboBox*   cbox,
                  QString      type,
                  QString      species);

/**
 * @brief Load fleets combo box for the given species
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param species : the current species' data to get
 * @param cmBox : the combo box widget with which to load the specified species' fleet data
 */
void loadFleetComboBox(nmfDatabase*       databasePtr,
                       nmfLogger*         logger,
                       const std::string& projectSettingsConfig,
                       const QString&     species,
                       QComboBox*         cmBox);
/**
 * @brief loadLastADMB
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectDir : project directory
 * @param projectName : project name
 * @param projectSettingsConfig : name of the project settings config file
 * @param type : type of model to run (i.e., "SingleSpecies" or "MultiSpecies")
 * @param tabWidget : tab widget on which to display any popup error messages
 * @param summaryTextBox : textEdit widget to write output messages from ADMB
 * @param abundanceData : the abundance data parsed from the last ADMB report file
 * @return Returns the QString containing the report file text or an error message
 */
QString loadLastADMB(nmfDatabase*   databasePtr,
                     nmfLogger*     logger,
                     std::string&   projectDir,
                     std::string&   projectName,
                     std::string&   projectSettingsConfig,
                     const QString& type,
                     QTabWidget*    tabWidget,
                     QTextEdit*     summaryTextBox,
                     std::map<std::string,boost::numeric::ublas::matrix<double> >& abundanceData);

/**
 * @brief Loads mortality tableView with mortality data from database
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget into which to load the table data
 * @param table : name of database table
 * @param numSegments : number of segments
 * @param species : species whose data should be loaded
 * @param includeTotalColumn : boolean to include an additional column representing totals
 * @param isAutoFillChecked : if True the segment years will be automatically calculated, else only the first and last segments will be displayed
 */
void loadMortalityTable(nmfDatabase* databasePtr,
                        nmfLogger*   logger,
                        std::string  projectSettingsConfig,
                        QTableView*  tableView,
                        std::string  table,
                        int          numSegments,
                        QString      species,
                        bool         includeTotalColumn,
                        bool         isAutoFillChecked);

/**
 * @brief Loads the alpha, beta, and gamma parameters into their tableView matrix
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget into which to load the table data
 * @param table : name of database table
 * @param species : species whose data should be loaded
 * @param algorithm : recruitment type (i.e., "Ricker" or "Beverton-Holt")
 */
void loadParameterTable(nmfDatabase* databasePtr,
                        nmfLogger*   logger,
                        std::string  projectSettingsConfig,
                        QTableView*  tableView,
                        std::string  table,
                        QString      species,
                        QString      algorithm);

/**
 * @brief Loads the specified table into the passed tableView widget
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget into which to load the table data
 * @param table : name of database table
 * @param species : species whose data should be loaded
 * @param surveyOrFleet : survey or fleet name
 * @param units : units of data stored to be passed back to calling routine
 * @param includeTotalColumn : boolean signifying if there should be a totals column displayed
 * @param displayFirstYearOnly : boolean signifying if only the first year's data should be displayed
 */
void loadTable(nmfDatabase* databasePtr,
               nmfLogger*   logger,
               std::string  projectSettingsConfig,
               QTableView*  tableView,
               std::string  table,
               QString      species,
               std::string  surveyOrFleet,
               std::string& units,
               bool         includeTotalColumn,
               bool         displayFirstYearOnly);

/**
 * @brief Loads the specified totals table into the passed tableView widget
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @param tableView : tableView widget into which to load the table data
 * @param table : name of database table
 * @param species : species whose data should be loaded
 * @param surveyOrFleet : survey or fleet name
 * @param units : units of data stored to be passed back to calling routine
 */
void loadTheTotalTable(nmfDatabase* databasePtr,
                       nmfLogger*   logger,
                       std::string  projectSettingsConfig,
                       QTableView*  tableView,
                       std::string  table,
                       QString      species,
                       std::string  surveyOrFleet,
                       std::string& units);

/**
 * @brief Rescales the data present in the passed tableView widget
 * @param tableView : tableView that contains the data to be rescaled
 * @param unitsSF : scale factor with which to divide the tableView data by
 */
void rescaleModel(QTableView* tableView,
                  double      unitsSF);

/**
 * @brief Runs the ADMB optimizer software
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectDir : the directory in which the project exists
 * @param projectName : the name of the project
 * @param projectSettingsConfig : name of the project settings config file
 * @param trophic : if 0, a Single Species model is run; else if 1, a Multi Species model is run
 * @param verboseDebugLevel : set the ADMB debug level
 * @param type : type of model to run (i.e., "SingleSpecies" or "MultiSpecies")
 * @param tabWidget : tabWidget over which to display any popup messages
 * @param summaryTextBox : textEdit widget to write output messages from ADMB
 * @param abundanceData : the abundance data parsed from the ADMB report file
 * @return Returns True if no errors, else False
 */
bool runOptimizerADMB(
        nmfDatabase*     databasePtr,
        nmfLogger*       logger,
        std::string&     projectDir,
        std::string&     projectName,
        std::string&     projectSettingsConfig,
        int&             trophic,
        int&             verboseDebugLevel,
        const QString&   type,
        QTabWidget*      tabWidget,
        QTextEdit*       summaryTextBox,
        std::map<std::string,boost::numeric::ublas::matrix<double> >& abundanceData);
/**
 * @brief Saves a binned table
 * @param tabWidget : tabWidget over which to display any popup messages
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param tableView : tableView widget into which to load the table data
 * @param projectSettingsConfig : name of the project settings config file
 * @param table : name of database table
 * @param numBins : number of bins
 * @param binType : type of bin (i.e., "Years per Bin" or "Number of Bins")
 * @param predatorSpecies : name of predator species
 * @param predatorAge : predator age
 * @return Returns True if no errors, else False
 */
bool saveBinnedTable(QTabWidget*   tabWidget,
                     nmfDatabase*  databasePtr,
                     nmfLogger*    logger,
                     QTableView*   tableView,
                     std::string   projectSettingsConfig,
                     std::string   table,
                     int           numBins,
                     QString       binType,
                     QString       predatorSpecies,
                     QString       predatorAge);
/**
 * @brief Saves a Catch at Length table
 * @param tabWidget : tabWidget over which to display any popup messages
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param tableView : tableView widget into which to load the table data
 * @param projectSettingsConfig : name of the project settings config file
 * @param table : name of database table
 * @param species : name of species whose Catch data to save
 * @param fleet : name of fleet associated with species
 * @param units : units of Catch data
 * @return Returns True if no errors, else False
 */
bool saveCatchAtLengthTable(QTabWidget*  tabWidget,
                            nmfDatabase* databasePtr,
                            nmfLogger*   logger,
                            QTableView*  tableView,
                            std::string  projectSettingsConfig,
                            std::string  table,
                            std::string  species,
                            std::string  fleet,
                            std::string  units);
/**
 * @brief Saves a parameter table
 * @param tabWidget : tabWidget over which to display any popup messages
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param tableView : tableView widget into which to load the table data
 * @param projectSettingsConfig : name of the project settings config file
 * @param table : name of database table
 * @param species : species whose data should be loaded
 * @param algorithm : recruitment type (i.e., "Ricker" or "Beverton-Holt")
 * @return Returns True if no errors, else False
 */
bool saveParameterTable(QTabWidget*  tabWidget,
                        nmfDatabase* databasePtr,
                        nmfLogger*   logger,
                        QTableView*  tableView,
                        std::string  projectSettingsConfig,
                        std::string  table,
                        std::string  species,
                        std::string  algorithm);

/**
 * @brief Saves a Proportion table (i.e., "CatchSurveyProportion")
 * @param tabWidget : tabWidget over which to display any popup messages
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param tableView : tableView widget into which to load the table data
 * @param projectSettingsConfig : name of the project settings config file
 * @param table : name of database table
 * @param species : species whose data should be loaded
 * @param surveyOrFleet : name of survey of fleet associated with this species
 * @return Returns True if no errors, else False
 */
bool saveProportionTable(QTabWidget*  tabWidget,
                         nmfDatabase* databasePtr,
                         nmfLogger*   logger,
                         QTableView*  tableView,
                         std::string  projectSettingsConfig,
                         std::string  table,
                         std::string  species,
                         std::string  surveyOrFleet);
/**
 * @brief Saves a tableView's data to a database table
 * @param tabWidget : tabWidget over which to display any popup messages
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param tableView : tableView widget into which to load the table data
 * @param projectSettingsConfig : name of the project settings config file
 * @param table : name of database table
 * @param species : species whose data should be loaded
 * @param surveyOrFleet : name of survey of fleet associated with this species
 * @param units : units associated with the data to save
 * @param isProportion : signifies that the data are proportions and will print an error if totals are > 1
 * @param includeTotalColumn : signifies that the last column is a totals column
 * @param displayFirstYearOnly : displays the first year only after the save completes when the tableView reloads
 * @return Returns True if no errors, else False
 */
bool saveTable(QTabWidget*  tabWidget,
               nmfDatabase* databasePtr,
               nmfLogger*   logger,
               QTableView*  tableView,
               std::string  projectSettingsConfig,
               std::string  table,
               std::string  species,
               std::string  surveyOrFleet,
               std::string  units,
               bool         isProportion,
               bool         includeTotalColumn,
               bool         displayFirstYearOnly);

/**
 * @brief Saves a totals table
 * @param tabWidget : tabWidget over which to display any popup messages
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param tableView : tableView widget into which to load the table data
 * @param projectSettingsConfig : name of the project settings config file
 * @param table : name of database table
 * @param species : species whose data should be loaded
 * @param surveyOrFleet : name of survey of fleet associated with this species
 * @param units : units associated with the data to save
 * @return Returns True if no errors, else False
 */
bool saveTheTotalTable(QTabWidget*  tabWidget,
                       nmfDatabase* databasePtr,
                       nmfLogger*   logger,
                       QTableView*  tableView,
                       std::string  projectSettingsConfig,
                       std::string  table,
                       std::string  species,
                       std::string  surveyOrFleet,
                       std::string  units);

/**
 * @brief Used to set all age group values to the frist age group's values
 * @param tableView : tableView widget containing the data to set
 */
void setColumnsToFirst(QTableView* tableView);

/**
 * @brief Checks to see if the passed System name config exists in the System database table
 * @param databasePtr : pointer to the database api class
 * @param logger : pointer to logger api class
 * @param projectSettingsConfig : name of the project settings config file
 * @return Returns True if the named System exists in the System table
 */
bool systemTableExists(nmfDatabase* databasePtr,
                       nmfLogger*   logger,
                       std::string& projectSettingsConfig);

}

