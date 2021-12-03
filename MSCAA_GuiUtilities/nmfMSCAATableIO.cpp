
#include "nmfMSCAATableIO.h"
//#include "nmfConstants.h"

#include <QComboBox>

namespace nmfMSCAAUtils {

//void
//defineADMBFiles(QString& ADMBDir,
//                ADMBFilesStruct& ADMBFiles)
//{
//    // Load ADMBFiles struct with necessary file paths
//    QString prefix = "MSCAA";
//    ADMBFiles.dataFile        = QDir(ADMBDir).filePath(prefix+".dat");
//    ADMBFiles.parameterFile   = QDir(ADMBDir).filePath(prefix+".par");
//    ADMBFiles.pinFile         = QDir(ADMBDir).filePath(prefix+".pin");
//    ADMBFiles.tplFile         = QDir(ADMBDir).filePath(prefix+".tpl");
//    ADMBFiles.reportFile      = QDir(ADMBDir).filePath(prefix+".rep");
//    ADMBFiles.buildOutput     = QDir(ADMBDir).filePath(prefix+"_Build.out");
//    ADMBFiles.runOutput       = QDir(ADMBDir).filePath(prefix+"_Run.out");
//}


void calculateFirstLastSegments(int FirstYear,
                                int LastYear,
                                int numSegments,
                                std::vector<QString>& FirstYears,
                                std::vector<QString>& LastYears)
{
    for (int seg = 0; seg < numSegments; ++seg) {
        FirstYears.push_back("");
        LastYears.push_back("");
    }
    FirstYears[0]            = QString::number(FirstYear);
    LastYears[numSegments-1] = QString::number(LastYear);
}


void calculateSegmentYears(int FirstYear,
                           int LastYear,
                           int numSegments,
                           std::vector<QString>& FirstYears,
                           std::vector<QString>& LastYears)
{
    // Create Years lists using number of segments
    int numYears            = LastYear - FirstYear + 1;
    int yearsPerSegment     = numYears / numSegments;
    int yearsPerLastSegment = numYears - numSegments*yearsPerSegment;
    int theStartYear;

    FirstYears.clear();
    LastYears.clear();

    if (numSegments == 1) {
        FirstYears.push_back(QString::number(FirstYear));
        LastYears.push_back(QString::number(LastYear));
    } else {
        if (yearsPerLastSegment == 0) {
            for (int seg = 0; seg < numSegments; ++seg) {
                theStartYear = FirstYear+seg*yearsPerSegment;
                FirstYears.push_back(QString::number(theStartYear));
                LastYears.push_back(QString::number(theStartYear+yearsPerSegment-1));
            }
        } else {
            for (int seg = 0; seg < numSegments-1; ++seg) {
                theStartYear = FirstYear+seg*yearsPerSegment;
                FirstYears.push_back(QString::number(theStartYear));
                LastYears.push_back(QString::number(theStartYear+yearsPerSegment-1));
            }
            theStartYear = LastYears[numSegments-2].toInt()+1;
            FirstYears.push_back(QString::number(theStartYear));
            LastYears.push_back(QString::number(LastYear));
        }
    }
}


bool checkYearValues(nmfLogger* logger,
                     int FirstYear,
                     int LastYear,
                     QStandardItemModel* smodel)
{
    int currFirstYear,currLastYear;
//  int prevFirstYear=0;
    int prevLastYear=0;
    int numRows = smodel->rowCount();
    std::string msg;

    for (int row = 0; row < numRows; ++row) {
        currFirstYear = smodel->index(row,0).data().toInt();
        currLastYear  = smodel->index(row,1).data().toInt();
        if (currFirstYear > currLastYear) {
            msg = "currFirstYear > currLastYear (";
            msg += std::to_string(currFirstYear) + " > ";
            msg += std::to_string(currLastYear) + ")";
            logger->logMsg(nmfConstants::Error,msg);
            return false;
        }
        if (row > 0) {
            if (currFirstYear-prevLastYear != 1) { // Means there's a gap or overlap.
                msg = "currFirstYear-prevLastYear != 1 (";
                msg += std::to_string(currFirstYear) + " - ";
                msg += std::to_string(prevLastYear) + ")";
                logger->logMsg(nmfConstants::Error,msg);
                return false;
            }
            if (row == numRows-1) {
                if (currLastYear != LastYear) {
                    msg = "currFirstYear != LastYear (";
                    msg += std::to_string(currFirstYear) + " != ";
                    msg += std::to_string(LastYear) + ")";
                    logger->logMsg(nmfConstants::Error,msg);
                    return false;
                }
            }
        } else {
            if (currFirstYear != FirstYear) {
                msg = "currFirstYear != FirstYear (";
                msg += std::to_string(currFirstYear) + " != ";
                msg += std::to_string(FirstYear) + ")";
                logger->logMsg(nmfConstants::Error,msg);
                return false;
            }
        }
        //prevFirstYear = currFirstYear;
        prevLastYear  = currLastYear;
    }

    return true;
}


void createNewNumberOfBinsTable(nmfDatabase* databasePtr,
                                nmfLogger*   logger,
                                std::string  projectSettingsConfig,
                                QTableView*  tableView,
                                std::string  table,
                                int          numBins,
                                QString      predatorName,
                                QString      predatorAge,
                                bool         isAutoFillChecked)
{
    bool foundSpecies;
    int NumSpecies;
    int NumCols;
    int UpperLimit;
    int NumRows = numBins;
    int minAge,maxAge,FirstYear,LastYear;
    float MinLength,MaxLength;
    int NumLengthBins;
    std::vector<QString> FirstYears;
    std::vector<QString> LastYears;
    QStandardItem *item;
    QStandardItemModel* smodel;
    std::vector<std::string> allSpecies;
    QStringList HorizontalList;
    bool isDietTable = (table == nmfConstantsMSCAA::TableDiet);

    foundSpecies = databasePtr->getSpecies(logger,allSpecies);
    if (! foundSpecies)
        return;

    databasePtr->getSpeciesData(logger,predatorName.toStdString(),
                   minAge,maxAge,FirstYear,LastYear,
                   MinLength,MaxLength,NumLengthBins);
    if (isDietTable) {
        // Don't use FirstYear and Last Year from above
        // Need to get the FirstYear and LastYear from the System table as they've been
        // saved their specifically for Food Habit (i.e., Diet) calculations.  This year
        // range is independent of Species and so is saved in the System table;
        getFHYearRange(databasePtr,logger,projectSettingsConfig,FirstYear,LastYear);
    }

    NumSpecies = allSpecies.size();
    HorizontalList << "First Year" << "Last Year";

    if (isDietTable) {
        for (std::string species : allSpecies) {
            HorizontalList << QString::fromStdString(species);
        }
        HorizontalList << "Other Food";
        NumCols = 2+ NumSpecies +1; //2+ for FirstYear and LastYear; +1 for the Other Food column
    } else {
        for (int age = minAge; age <= maxAge; ++age) {
            HorizontalList << "Age " + QString::number(age);
        }
        NumCols = 2+ (maxAge-minAge+1); //2+ for FirstYear and LastYear
    }

    smodel = new QStandardItemModel(NumRows,NumCols);

    if (isAutoFillChecked) {
        calculateSegmentYears(FirstYear,LastYear,numBins,
                              FirstYears,LastYears);
    } else {
        calculateFirstLastSegments(FirstYear,LastYear,numBins,
                                   FirstYears,LastYears);
    }
    UpperLimit = (isDietTable) ? NumCols-1 : NumCols;
    for (int i=0; i<NumRows; ++i) {
        for (int j = 0; j < UpperLimit; ++j) {
            if (j == 0) {
                item = new QStandardItem(FirstYears[i]);
            } else if (j == 1) {
                item = new QStandardItem(LastYears[i]);
            } else {
                item  = new QStandardItem("");
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, j, item);
        }
        if (isDietTable) {
            item  = new QStandardItem("0.0");
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, NumCols-1, item);
        }
    }
    smodel->setHorizontalHeaderLabels(HorizontalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}


void createNewYearsPerBinTable(nmfDatabase* databasePtr,
                               nmfLogger*   logger,
                               std::string  projectSettingsConfig,
                               QTableView*  tableView,
                               std::string  table,
                               int          NumYearsPerBin,
                               QString      predatorName,
                               QString      predatorAge,
                               bool         isAutoFillChecked)
{
    bool foundSpecies;
    int NumSpecies;
    int NumCols;
    int UpperLimit;
    int NumRows;
    int NumBins;
    int minAge,maxAge,FirstYear,LastYear;
    float MinLength,MaxLength;
    int NumLengthBins;
    std::vector<QString> FirstYears;
    std::vector<QString> LastYears;
    QStandardItem *item;
    QStandardItemModel* smodel;
    std::vector<std::string> allSpecies;
    QStringList HorizontalList;
    bool isDietTable = (table == nmfConstantsMSCAA::TableDiet);
    int NumYears;

    foundSpecies = databasePtr->getSpecies(logger,allSpecies);
    if (! foundSpecies)
        return;

    databasePtr->getSpeciesData(logger,predatorName.toStdString(),
                   minAge,maxAge,FirstYear,LastYear,
                   MinLength,MaxLength,NumLengthBins);
    if (isDietTable) {
        // Don't use FirstYear and Last Year from above
        // Need to get the FirstYear and LastYear from the System table as they've been
        // saved their specifically for Food Habit (i.e., Diet) calculations.  This year
        // range is independent of Species and so is saved in the System table;
        getFHYearRange(databasePtr,logger,projectSettingsConfig,FirstYear,LastYear);
    }

    NumYears = (LastYear-FirstYear+1);
    NumBins  = NumYears/NumYearsPerBin;
    NumBins  = (NumBins == int(float(NumYears)/float(NumYearsPerBin))) ? NumBins : NumBins+1;
    NumRows  = NumBins;

    NumSpecies = allSpecies.size();
    HorizontalList << "First Year" << "Last Year";

    if (isDietTable) {
        for (std::string species : allSpecies) {
            HorizontalList << QString::fromStdString(species);
        }
        HorizontalList << "Other Food";
        NumCols = 2+ NumSpecies +1; //2+ for FirstYear and LastYear; +1 for the Other Food column
    } else {
        for (int age = minAge; age <= maxAge; ++age) {
            HorizontalList << "Age " + QString::number(age);
        }
        NumCols = 2+ (maxAge-minAge+1); //2+ for FirstYear and LastYear
    }

    smodel = new QStandardItemModel(NumRows,NumCols);

    if (isAutoFillChecked) {

        int fYear = FirstYear;
        int lYear = FirstYear+NumYearsPerBin-1;
        for (int i=0; i<NumBins; ++i) {
            FirstYears.push_back(QString::number(fYear));
            lYear = (lYear > LastYear) ? LastYear : lYear;
            LastYears.push_back(QString::number(lYear));
            fYear += NumYearsPerBin;
            lYear += NumYearsPerBin;
        }
        //        calculateSegmentYears(FirstYear,LastYear,numBins,
        //                              FirstYears,LastYears);
    } else {
        calculateFirstLastSegments(FirstYear,LastYear,NumBins,
                                   FirstYears,LastYears);
    }
    UpperLimit = (isDietTable) ? NumCols-1 : NumCols;
    for (int i=0; i<NumRows; ++i) {
        for (int j = 0; j < UpperLimit; ++j) {
            if (j == 0) {
                item = new QStandardItem(FirstYears[i]);
            } else if (j == 1) {
                item = new QStandardItem(LastYears[i]);
            } else {
                item  = new QStandardItem("");
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, j, item);
        }
        if (isDietTable) {
            item  = new QStandardItem("0.0");
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, NumCols-1, item);
        }
    }
    smodel->setHorizontalHeaderLabels(HorizontalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}

QString getADMBDir(
        nmfDatabase*   databasePtr,
        nmfLogger*     logger,
        std::string&   projectDir,
        std::string&   projectName,
        std::string&   projectSettingsConfig,
        const QString& type,
        QTextEdit*     summaryTextBox)
{
    nmfADMB admb(databasePtr,logger,projectDir,
                 projectName,projectSettingsConfig,
                 summaryTextBox,type,
                 nmfConstantsMSCAA::DontClearSummary);
    return admb.getADMBDir();
}


void getAllAges(nmfDatabase* databasePtr,
                nmfLogger* logger,
                QString species,
                std::vector<std::string>& ages)
{
    bool  foundSpecies;
    int   minAge=0;
    int   maxAge=0;
    int   FirstYear=0;
    int   LastYear=0;
    float MinLength;
    float MaxLength;
    int   NumLengthBins;

    ages.clear();

    foundSpecies = databasePtr->getSpeciesData(logger,species.toStdString(),
                                  minAge,maxAge,FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies)
        return;

    for (int age=minAge; age<=maxAge; ++age) {
        ages.push_back("Age "+std::to_string(age));
    }
}

void getAllYears(nmfDatabase* databasePtr,
                 nmfLogger* logger,
                 QString species,
                 int& firstYear,
                 std::vector<std::string>& years)
{
    bool foundSpecies;
    int minAge=0;
    int maxAge=0;
    int lastYear=0;
    float minLength;
    float maxLength;
    int numLengthBins;

    foundSpecies = databasePtr->getSpeciesData(logger,species.toStdString(),
                                  minAge,maxAge,firstYear,lastYear,
                                  minLength,maxLength,numLengthBins);
    if (foundSpecies) {
        years.clear();
        for (int year=1; year <= (lastYear-firstYear+1); ++year) {
            years.push_back(std::to_string(year));
        }
    }

}


bool getDatabaseData(
        nmfDatabase* databasePtr,
        nmfLogger*   logger,
        const std::string&  projectSettingsConfig,
        const std::string &species,
        const std::string &fleet,
        const int &numYears,
        const int &numAges,
        const std::string &tableName,
        boost::numeric::ublas::matrix<double> &tableData)
{
    int m;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    double sf = 1.0;

    fields     = {"SystemName","SpeName","Year","Age","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Year,Age,Value,Units FROM " + tableName;
    queryStr  += " WHERE SpeName = '"  + species + "'";
    if (tableName == nmfConstantsMSCAA::TableCatchFishery) {
        queryStr += " AND Fleet = '" + fleet + "'";
    }
    queryStr  += " AND SystemName = '" + projectSettingsConfig + "'";
    queryStr  += " ORDER BY Year,Age";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        msg = "nmfMSCAAUtils::getDatabaseData: No records found in table: " + tableName;
        logger->logMsg(nmfConstants::Error,msg);
        msg = "query: " + queryStr;
        logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    if (NumRecords != numYears*numAges) {
        msg = "nmfMSCAAUtils::getDatabaseData: Incorrect number of records found in table: " + tableName + "\n";
        msg += "Found " + std::to_string(NumRecords) + " records.\n";
        msg += "Calculated NumYears*NumAges (" + std::to_string(numYears*numAges) + ") records.";
        logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    // Scale to kilograms if Weight table
    if (tableName == nmfConstantsMSCAA::TableWeight) {
        if (dataMap["Units"][0] == "Grams") {
            sf = 1000.0;
        }
    }

    m = 0;
    nmfUtils::initialize(tableData,numYears,numAges);
    for (int year = 0; year < numYears; ++year) {
        for (int age = 0; age < numAges; ++age) {
            tableData(year,age) = sf*std::stod(dataMap["Value"][m++]);
        }
    }
    return true;
}

bool getFHYearRange(nmfDatabase* databasePtr,
                    nmfLogger*   logger,
                    std::string  systemName,
                    int&         firstYear,
                    int&         lastYear)
{
    std::string msg;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // Get species data
    fields     = {"SystemName","FH_FirstYear","FH_LastYear"};
    queryStr   = "SELECT SystemName,FH_FirstYear,FH_LastYear FROM " + nmfConstantsMSCAA::TableModels;
    queryStr  += "WHERE SystemName = '" + systemName + "'";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    if (dataMap["SystemName"].size() == 0) {
        msg = "getFHYearRange: No records found in System";
        logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    firstYear = std::stoi(dataMap["FH_FirstYear"][0]);
    lastYear  = std::stoi(dataMap["FH_LastYear"][0]);

    return true;
}


bool getMortalityData(
        nmfDatabase* databasePtr,
        nmfLogger*   logger,
        const std::string&  projectSettingsConfig,
        const std::string &species,
        const int &numYears,
        const int &minAge,
        const int &maxAge,
        const std::string &tableName,
        boost::numeric::ublas::matrix<double> &tableData)
{
    int m;
    int NumRecords;
    int NumAges;
    int FirstYear,LastYear;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    fields     = {"SystemName","SpeName","Segment","ColName","Value"};
    queryStr   = "SELECT SystemName,SpeName,Segment,ColName,Value FROM " + tableName;
    queryStr  += " WHERE SystemName = '" + projectSettingsConfig + "'";
    queryStr  += " AND SpeName = '" + species + "'";
    queryStr  += " ORDER BY Segment";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        msg = "nmfMSCAAUtils::getMortalityData: No records found in table: " + tableName;
        logger->logMsg(nmfConstants::Error,msg);
        return false;
    }
    NumAges = maxAge - minAge + 1;
    nmfUtils::initialize(tableData,numYears,NumAges);

    m = 0;
    int VeryFirstYear = std::stoi(dataMap["Value"][0]);
    while (m < NumRecords) {
        FirstYear = std::stoi(dataMap["Value"][m++]);
        LastYear  = std::stoi(dataMap["Value"][m++]);
        for (int year=FirstYear; year<=LastYear; ++year) {
            if (year == FirstYear) {
                for (int age=0; age<NumAges; ++age) {
                    tableData(year-VeryFirstYear,age) =
                            std::stod(dataMap["Value"][m++]);
                }
            } else {
                for (int age=0; age<NumAges; ++age) {
                    tableData(year-VeryFirstYear,age) =
                            tableData(year-VeryFirstYear-1,age);
                }
            }
        }
    }

    return true;
}


int getNumBins(nmfDatabase* databasePtr,
               nmfLogger*   logger,
               std::string  projectSettingsConfig,
               std::string  table,
               QString&     binType,
               QString      predatorName,
               QString      predatorAge)
{
    int numBins=0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    bool isDietTable = (table == nmfConstantsMSCAA::TableDiet);

    if (isDietTable) {
        fields     = {"SystemName","PredatorName","PredatorAge","Bin","BinType","ColName","Value"};
        queryStr   = "SELECT SystemName,PredatorName,PredatorAge,Bin,BinType,ColName,Value FROM " + table;
        queryStr  += " WHERE PredatorName = '" + predatorName.toStdString() + "'";
        queryStr  += " AND PredatorAge = '" + predatorAge.toStdString() + "'";
        queryStr  += " AND SystemName = '" + projectSettingsConfig + "'";
        queryStr  += " ORDER BY Bin";
        dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["PredatorName"].size();
        if (NumRecords == 0) {
            // logger->logMsg(nmfConstants::Error,"Found 0 records in Diet");
            return 0;
        }
        numBins = std::stoi(dataMap["Bin"][NumRecords-1]);
        binType = QString::fromStdString(dataMap["BinType"][0]);
        if (binType == "Years per Bin") {
            numBins = std::stoi(dataMap["Value"][1]) -
                      std::stoi(dataMap["Value"][0]);
        }
    } else {
        fields     = {"SystemName","SpeName","Segment","ColName","Value"};
        queryStr   = "SELECT SystemName,SpeName,Segment,ColName,Value FROM " + table;
        queryStr  += " WHERE SystemName = '" + projectSettingsConfig + "'";
        queryStr  += " AND SpeName = '" + predatorName.toStdString() + "'";
        queryStr  += " ORDER BY Segment";
        dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
        if (NumRecords == 0) {
            // logger->logMsg(nmfConstants::Error,"Found 0 records in Mortality");
            return 0;
        }
        numBins = std::stoi(dataMap["Segment"][NumRecords-1]);
    }
    return numBins+1;
}


void loadBinnedTable(nmfDatabase* databasePtr,
                     nmfLogger*   logger,
                     std::string  projectSettingsConfig,
                     QTableView*  tableView,
                     std::string  table,
                     int          numBins,
                     QString      binType,
                     QString      predatorName,
                     QString      predatorAge)
{
    bool foundSpecies;
    int m;
    int NumCols;
    int NumRows;
    int FirstYear;
    int LastYear;
    int NumRecords;
    int minAge;
    int maxAge;
    int NumSpecies;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::string value;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel;
    QStringList HorizontalList;
    QStringList VerticalList;
    QStandardItem *item;
    std::vector<std::string> allSpecies;
    bool isDietTable = (table == nmfConstantsMSCAA::TableDiet);

    if (isDietTable && (predatorName.isEmpty() || predatorAge.isEmpty())) {
        return;
    }

    // Get species information
    foundSpecies = databasePtr->getSpecies(logger,allSpecies);
    if (! foundSpecies)
        return;

    databasePtr->getSpeciesData(logger,predatorName.toStdString(),
                   minAge,maxAge,FirstYear,LastYear,
                   MinLength,MaxLength,NumLengthBins);

    NumSpecies = allSpecies.size();
    HorizontalList << "First Year" << "Last Year";
    if (isDietTable) {
        for (std::string species : allSpecies) {
            HorizontalList << QString::fromStdString(species);
        }
        HorizontalList << "Other Food";
        NumCols = 2+ NumSpecies +1; //2+ for FirstYear and LastYear; +1 for the Other Food column
    } else {
        for (int age = minAge; age <= maxAge; ++age) {
            HorizontalList << "Age " + QString::number(age);
        }
        NumCols = 2+ (maxAge-minAge)+1; //2+ for FirstYear and LastYear
    }

    NumRows = numBins;

    // Get table data
    if (isDietTable) {
        fields     = {"SystemName","PredatorName","PredatorAge","Bin","BinType","ColName","Value"};
        queryStr   = "SELECT SystemName,PredatorName,PredatorAge,Bin,BinType,ColName,Value FROM " + table;
        queryStr  += " WHERE SystemName = '" + projectSettingsConfig + "'";
        queryStr  += " AND PredatorName = '" + predatorName.toStdString() + "'";
        queryStr  += " AND PredatorAge = '" + predatorAge.toStdString() + "'";
        queryStr  += " AND BinType = '" + binType.toStdString() + "'";
        queryStr  += " ORDER BY Bin";
        dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["PredatorName"].size();
        if (binType == "Years per Bin") {
            NumRows = std::stoi(dataMap["Bin"][NumRecords-1]) + 1;
        }
    } else {
        fields     = {"SystemName","SpeName","Segment","ColName","Value"};
        queryStr   = "SELECT SystemName,SpeName,Segment,ColName,Value FROM " + table;
        queryStr  += " WHERE SystemName = '" + projectSettingsConfig + "'";
        queryStr  += " AND SpeName = '" + predatorName.toStdString() + "'";
        queryStr  += " ORDER BY Segment";
        dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
        NumRecords = dataMap["SpeName"].size();
    }

    smodel = new QStandardItemModel(NumRows,NumCols);

    m = 0;
    for (int i=0; i<NumRows; ++i) {
        for (int j = 0; j < NumCols; ++j) {
            if (m < NumRecords) {
                value = (isDietTable) ? dataMap["Value"][m] : dataMap["Value"][m];
                item  = new QStandardItem(QString::fromStdString(value));
                item->setTextAlignment(Qt::AlignCenter);
                smodel->setItem(i, j, item);
                ++m;
            }
        }
    }
    smodel->setHorizontalHeaderLabels(HorizontalList);
    smodel->setVerticalHeaderLabels(VerticalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}


void loadCatchAtLengthTable(nmfDatabase* databasePtr,
                            nmfLogger*   logger,
                            std::string  projectSettingsConfig,
                            QTableView*  tableView,
                            std::string  table,
                            QString      species,
                            std::string  fleet)
{
    bool foundSpecies;
    int m=0;
    int NumRows;
    int FirstYear;
    int LastYear;
    int NumRecords;
    int minAge;
    int maxAge;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel;
    QStringList HorizontalList;
    QStringList VerticalList;
    QStandardItem *item;

    if (species.isEmpty()) {
        logger->logMsg(nmfConstants::Error,"loadCatchAtLengthTable: Received no Species");
        return;
    }

    foundSpecies = databasePtr->getSpeciesData(logger,species.toStdString(),
                                  minAge,maxAge,FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies) {
        logger->logMsg(nmfConstants::Error,"loadCatchAtLengthTable: Couldn't find any Species");
        return;
    }
    NumRows = LastYear-FirstYear+1;
    smodel = new QStandardItemModel( NumRows, NumLengthBins );

    // Read data from table: nmfConstantsMSCAA::TableCatchAtLengthFishery
    fields     = {"SystemName","SpeName","Fleet","Year","BinNumber","BinName","Value"};
    queryStr   = "SELECT SystemName,SpeName,Fleet,Year,BinNumber,BinName,Value FROM " + table;
    queryStr  += " WHERE SpeName = '"  + species.toStdString() + "'";
    queryStr  += " AND SystemName = '" + projectSettingsConfig + "'";
    queryStr  += " AND Fleet = '"      + fleet + "'";
    queryStr  += " ORDER BY Year,BinNumber";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        logger->logMsg(nmfConstants::Error,"loadCatchAtLengthTable: Found 0 records in table: "+table);
        return;
    }

    // Get min and max lengths and num bins
    float binInc = (MaxLength - MinLength)/NumLengthBins;
    float min=MinLength,max;
    std::string binName;
    for (int row = 0; row < NumRows; ++row) {
        VerticalList << QString::number(FirstYear+row);
        for (int col = 0; col < NumLengthBins; ++col) {
            if (row == 0) {
                max = (col == NumLengthBins-1) ? min+binInc : min+binInc-1;
                binName = //"Bin " + std::to_string(binNum+1)+"\n"+
                           std::to_string(int(min))+"-"+std::to_string(int(max));
                HorizontalList << QString::fromStdString(binName);
                min = max + 1;
             }
            if (m < NumRecords) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m]));
            } else {
                item = new QStandardItem("test");
            }
            ++m;
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
        }
    }
    smodel->setHorizontalHeaderLabels(HorizontalList);
    smodel->setVerticalHeaderLabels(VerticalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}

void loadComboBox(nmfDatabase* databasePtr,
                  nmfLogger* logger,
                  QComboBox* cbox,
                  QString type,
                  QString species)
{
    int firstYear=0;
    std::vector<std::string> allItems;

    allItems.clear();

    // Get appropriate items to load into combobox
    if (type == "Name") {
        databasePtr->getSpecies(logger,allItems);
    } else if (type == "Age") {
        getAllAges(databasePtr,logger,species,allItems);
    } else if (type == "Year") {
        getAllYears(databasePtr,logger,species,firstYear,allItems);
    }

    // Load items into a cleared combobox
    cbox->clear();
    for (std::string item : allItems) {
        cbox->addItem(QString::fromStdString(item));
    }
}

void loadFleetComboBox(nmfDatabase* databasePtr,
                       nmfLogger*   logger,
                       const std::string& projectSettingsConfig,
                       const QString& species,
                       QComboBox* cmBox)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QString fleetName;

    fields     = {"SystemName","SpeName","FleetNumber","FleetName"};
    queryStr   = "SELECT SystemName,SpeName,FleetNumber,FleetName FROM " + nmfConstantsMSCAA::TableFleets;
    queryStr  += " WHERE SpeName = '"  + species.toStdString() + "'";
    queryStr  += " AND SystemName = '" + projectSettingsConfig + "'";
    queryStr  += " ORDER BY SpeName,FleetNumber";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        logger->logMsg(nmfConstants::Error,"loadFleetComboBox: No records found in table Fleets");
        return;
    }
    cmBox->blockSignals(true);
    cmBox->clear();
    for (int i = 0; i < NumRecords; ++i) {
        fleetName = QString::fromStdString(dataMap["FleetName"][i]);
        if (fleetName != "X") {
            cmBox->addItem(fleetName);
        }
    }
    cmBox->blockSignals(false);
}

QString loadLastADMB(
        nmfDatabase*   databasePtr,
        nmfLogger*     logger,
        std::string&   projectDir,
        std::string&   projectName,
        std::string&   projectSettingsConfig,
        const QString& type,
        QTabWidget*    tabWidget,
        QTextEdit*     summaryTextBox,
        std::map<std::string,boost::numeric::ublas::matrix<double> >& abundanceData)
{
    nmfADMB admb(databasePtr,logger,projectDir,
                 projectName,projectSettingsConfig,
                 summaryTextBox,type,
                 nmfConstantsMSCAA::ClearSummary);
    QString parseStr = admb.parseReportFile(abundanceData);
    if (! parseStr.isEmpty()) {
        parseStr  = "\n" + parseStr;
        parseStr += "\n\nPlease re-run " + type + " model.";
        QMessageBox::warning(tabWidget, "Error",parseStr,QMessageBox::Ok);
    }
    return parseStr;
}

void loadMortalityTable(nmfDatabase* databasePtr,
                        nmfLogger* logger,
                        std::string projectSettingsConfig,
                        QTableView* tableView,
                        std::string table,
                        int numSegments,
                        QString species,
                        bool includeTotalColumn,
                        bool isAutoFillChecked)
{
    bool foundSpecies;
    int m;
    int NumCols;
    int NumRows;
    int FirstYear;
    int LastYear;
    int NumRecords;
    int minAge;
    int maxAge;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel;
    QStringList HorizontalList;
    QStringList VerticalList;
    QStandardItem *item;
    std::vector<QString> FirstYears;
    std::vector<QString> LastYears;

    if (species.isEmpty()) {
        return;
    }

    foundSpecies = databasePtr->getSpeciesData(logger,species.toStdString(),
                                  minAge,maxAge,FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies)
        return;

    NumRows = numSegments;
    NumCols = 2 + (maxAge-minAge+1); // FirstYear, LastYear, and all the Age groups
    if (includeTotalColumn) {
        ++NumCols;
    }

    // Get table data
    fields     = {"SystemName","SpeName","Age","Value"};
    queryStr   = "SELECT SystemName,SpeName,Age,Value FROM " + table;
    queryStr  += " WHERE SpeName = '" + species.toStdString() + "'";
    queryStr  += " AND SystemName = '" + projectSettingsConfig + "'";
    queryStr  += " ORDER BY Age";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();

    smodel = new QStandardItemModel( NumRows, NumCols );

    if (isAutoFillChecked) {
        calculateSegmentYears(FirstYear,LastYear,numSegments,
                              FirstYears,LastYears);
    } else {
        calculateFirstLastSegments(FirstYear,LastYear,numSegments,
                                   FirstYears,LastYears);
    }

    m = 0;
    item = nullptr;
    for (int i=0; i<NumRows; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (j == 0) {
                item = new QStandardItem(FirstYears[i]);
            } else if (j == 1) {
                item = new QStandardItem(LastYears[i]);
            }
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, j, item);
        }
        for (int j=2; j<NumCols-1; ++j)
        {
            if (m < NumRecords) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m]));
            } else {
                item = new QStandardItem("");
            }
            ++m;
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, j, item);
        }
    }
    HorizontalList << "First Year" << "Last Year";
    for (int age = minAge; age <= maxAge; ++age) {
        HorizontalList << "Age " + QString::number(age);
    }

    smodel->setHorizontalHeaderLabels(HorizontalList);
    smodel->setVerticalHeaderLabels(VerticalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}

void loadParameterTable(nmfDatabase* databasePtr,
                        nmfLogger*   logger,
                        std::string  projectSettingsConfig,
                        QTableView*  tableView,
                        std::string  table,
                        QString      species,
                        QString      algorithm)
{
    int m=0;
    int FirstYear,LastYear,minAge,maxAge;
    int NumLengthBins;
    int NumRecords;
    float MinLength,MaxLength;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel;
    QStringList HorizontalList;
    QStringList VerticalList;
    QStandardItem *item;
    int NumColumns = 3;

    if (species.isEmpty()) {
        return;
    }

    if (! databasePtr->getSpeciesData(logger,species.toStdString(),
                                  minAge,maxAge,FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins))
        return;

    smodel = new QStandardItemModel( 1, NumColumns );

    // Read data from table: nmfConstantsMSCAA::TableSimulationParametersSpecies
    fields     = {"SystemName","Algorithm","SpeName","ParameterName","Value"};
    queryStr   = "SELECT SystemName,Algorithm,SpeName,ParameterName,Value FROM " + table;
    queryStr  += " WHERE SpeName = '"  + species.toStdString() + "'";
    queryStr  += " AND SystemName = '" + projectSettingsConfig + "'";
    queryStr  += " AND Algorithm = '"      + algorithm.toStdString() + "'";
    queryStr  += " ORDER BY ParameterName";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();

    // Load model
    m = 0;
    VerticalList << QString::number(FirstYear);
    HorizontalList << "alpha" << "beta" << "gamma";
    for (int col = 0; col < NumColumns; ++col) {
        if (m < NumRecords) {
            item = new QStandardItem(QString::fromStdString(dataMap["Value"][m++]));
        } else {
            item = new QStandardItem(QString::fromStdString(""));
        }
        item->setTextAlignment(Qt::AlignCenter);
        smodel->setItem(0, col, item);
    }

    smodel->setHorizontalHeaderLabels(HorizontalList);
    smodel->setVerticalHeaderLabels(VerticalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}

void loadTable(nmfDatabase* databasePtr,
               nmfLogger*   logger,
               std::string  projectSettingsConfig,
               QTableView*  tableView,
               std::string  table,
               QString      species,
               std::string  surveyOrFleet,
               std::string& units,
               bool         includeTotalColumn,
               bool         displayFirstYearOnly)
{
    bool foundSpecies;
    int m;
    int NumCols;
    int NumRows;
    int FirstYear;
    int LastYear;
    int NumRecords;
    int minAge;
    int maxAge;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel;
    QStringList HorizontalList;
    QStringList VerticalList;
    QStandardItem *item;
    bool hasSurvey = (table == nmfConstantsMSCAA::TableCatchSurvey);

    if (species.isEmpty()) {
        return;
    }

    foundSpecies = databasePtr->getSpeciesData(logger,species.toStdString(),
                                  minAge,maxAge,FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies) {
        logger->logMsg(nmfConstants::Error,"loadTable: Couldn't find any Species");
        return;
    }

    NumRows = (displayFirstYearOnly) ? 1 : LastYear - FirstYear + 1;
    NumCols = maxAge - minAge;
    if (includeTotalColumn) {
        ++NumCols;
    }

    // Get table data
    if ((table == nmfConstantsMSCAA::TableWeight)      || (table == nmfConstantsMSCAA::TableMaturity) ||
        (table == nmfConstantsMSCAA::TableConsumption) || (table == nmfConstantsMSCAA::TableInitialAbundance)) {
        fields    = {"SystemName","SpeName","Year","Age","Value","Units"};
        queryStr  = "SELECT SystemName,SpeName,Year,Age,Value,Units FROM " + table;
        queryStr += " WHERE SpeName = '"  + species.toStdString() + "'";
        queryStr += " AND SystemName = '" + projectSettingsConfig + "'";
        queryStr += " ORDER BY Year,Age";
    } else if (hasSurvey) {
        fields    = {"SystemName","SpeName","Survey","Year","Age","Value","Units"};
        queryStr  = "SELECT SystemName,SpeName,Survey,Year,Age,Value,Units FROM " + table;
        queryStr += " WHERE SpeName = '"  + species.toStdString() + "'";
        queryStr += " AND SystemName = '" + projectSettingsConfig + "'";
        queryStr += " AND Survey = "      + surveyOrFleet;
        queryStr += " ORDER BY Year,Age";
    } else {
        fields    = {"SystemName","SpeName","Fleet","Year","Age","Value","Units"};
        queryStr  = "SELECT SystemName,SpeName,Fleet,Year,Age,Value,Units FROM " + table;
        queryStr += " WHERE SpeName = '"  + species.toStdString() + "'";
        queryStr += " AND SystemName = '" + projectSettingsConfig + "'";
        queryStr += " AND Fleet = '" + surveyOrFleet + "'";
        queryStr += " ORDER BY Year,Age";
    }
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords == 0) {
        logger->logMsg(nmfConstants::Error,"loadTable: Found 0 records in table: "+table);
        logger->logMsg(nmfConstants::Error,queryStr);
//      return;
    }

    smodel = new QStandardItemModel( NumRows, NumCols+1 );
    m = 0;
    for (int row=0; row<NumRows; ++row) {
        VerticalList << QString::number(FirstYear+row);
        for (int col=0; col<NumCols+1; ++col)
        {
            if (row == 0) {
                if (col < NumCols) {
                    HorizontalList << QString::fromStdString("Age ") +
                                      QString::number(minAge+col);
                } else {
                    if (includeTotalColumn) {
                        HorizontalList << "Total";
                    } else {
                        HorizontalList << QString::fromStdString("Age ") +
                                          QString::number(minAge+col);
                    }
                }
            }
            if (m < NumRecords) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m]));
            } else {
                item = new QStandardItem("");
            }
            ++m;
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
        }
    }
    if (NumRecords > 0) {
        units = dataMap["Units"][0];
    }

    smodel->setHorizontalHeaderLabels(HorizontalList);
    smodel->setVerticalHeaderLabels(VerticalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}

void loadTheTotalTable(nmfDatabase* databasePtr,
                       nmfLogger* logger,
                       std::string projectSettingsConfig,
                       QTableView* tableView,
                       std::string table,
                       QString species,
                       std::string surveyOrFleet,
                       std::string& units)
{
    bool foundSpecies;
    int m;
    int NumCols;
    int NumRows;
    int FirstYear;
    int LastYear;
    int NumRecords;
    int minAge;
    int maxAge;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    QStandardItemModel* smodel;
    QStringList HorizontalList;
    QStringList VerticalList;
    QStandardItem *item;
    bool hasSurvey = (table == nmfConstantsMSCAA::TableCatchSurveyTotal);

    if (species.isEmpty()) {
        return;
    }
    foundSpecies = databasePtr->getSpeciesData(logger,species.toStdString(),
                                  minAge,maxAge,FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies) {
        logger->logMsg(nmfConstants::Error,"loadTheTotalTable: No Species found");
        return;
    }

    NumRows = LastYear - FirstYear;
    NumCols = 1;

    // Get table data
    if (hasSurvey) {
        fields     = {"SystemName","SpeName","Survey","Year","Value","Units"};
        queryStr   = "SELECT SystemName,SpeName,Survey,Year,Value,Units FROM " + table;
        queryStr  += " WHERE SpeName = '" + species.toStdString() + "'";
        queryStr  += " AND SystemName = '" + projectSettingsConfig + "'";
        queryStr  += " AND Survey = " + surveyOrFleet;
        queryStr  += " ORDER BY Year";
    } else {
        fields    = {"SystemName","SpeName","Fleet","Year","Value","Units"};
        queryStr  = "SELECT SystemName,SpeName,Fleet,Year,Value,Units FROM " + table;
        queryStr += " WHERE SpeName = '" + species.toStdString() + "'";
        queryStr += " AND SystemName = '" + projectSettingsConfig + "'";
        queryStr += " AND Fleet = '" + surveyOrFleet + "'";
        queryStr += " ORDER BY Year";
    }
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();

    smodel = new QStandardItemModel(NumRows,NumCols);
    m = 0;
    HorizontalList << "Total";
    for (int i=0; i<=NumRows; ++i) {
        VerticalList << QString::number(FirstYear+i);
        for (int j=0; j<NumCols; ++j)
        {
            if (m < NumRecords) {
                item = new QStandardItem(QString::fromStdString(dataMap["Value"][m]));
            } else {
                item = new QStandardItem("");
            }
            ++m;
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(i, j, item);
        }
    }
    if (NumRecords > 0) {
        units = dataMap["Units"][0];
    }

    smodel->setHorizontalHeaderLabels(HorizontalList);
    smodel->setVerticalHeaderLabels(VerticalList);
    tableView->setModel(smodel);
    tableView->resizeColumnsToContents();
}

void rescaleModel(QTableView* tableView,
                  double      unitsSF)
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    double value;
    QModelIndex index;

    if (int(unitsSF) == 0) {
        unitsSF = 1.0;
        std::cout << "Error: rescaleModel: Found 0 value for unitsSF" << std::endl;
    }

    for (int row=0; row<smodel->rowCount(); ++row) {
      for (int col=0; col<smodel->columnCount(); ++col) {
          index = smodel->index(row,col);
          value = index.data().toDouble()/unitsSF;
          smodel->setData(index,value);
      }
    }
    tableView->resizeColumnsToContents();
}

bool runOptimizerADMB(
        nmfDatabase*   databasePtr,
        nmfLogger*     logger,
        std::string&   projectDir,
        std::string&   projectName,
        std::string&   projectSettingsConfig,
        int&           trophic,
        int&           verboseDebugLevel,
        const QString& type,
        QTabWidget*    tabWidget,
        QTextEdit*     summaryTextBox,
        std::map<std::string,boost::numeric::ublas::matrix<double> >& abundanceData)
{
    bool buildOK;
    bool runOK;
    QString msg;
    QMessageBox::StandardButton reply;

    nmfADMB admb(databasePtr,logger,projectDir,
                 projectName,projectSettingsConfig,
                 summaryTextBox,type,
                 nmfConstantsMSCAA::ClearSummary);
//  admb.setData(SpeciesData);
    admb.setDebug(verboseDebugLevel);
    admb.setTrophic(trophic);

    // Load tpl file
    // "/home/rklasky/admb/examples/kiersten_3Species/3Species.tpl";
    std::string origTplFile = QFileDialog::getOpenFileName(tabWidget,
         QObject::tr("Please load ADMB Tpl File"), "", QObject::tr("Tpl Files (*.tpl)")).toStdString();
    admb.setTplFile(origTplFile);
    if (origTplFile.empty()) {
        logger->logMsg(nmfConstants::Error,"No valid tpl file entered.");
        return false;
    }

    // Load pin file
    // "/home/rklasky/admb/examples/kiersten_3Species/3Species.pin";
    std::string origPinFile = QFileDialog::getOpenFileName(tabWidget,
         QObject::tr("Please load ADMB Pin File"), "", QObject::tr("Pin Files (*.pin)")).toStdString();
    admb.setPinFile(origPinFile);
    if (origPinFile.empty()) {
        logger->logMsg(nmfConstants::Error,"No valid pin file entered.");
        return false;
    }

    if (! admb.createInputFiles()) {
        QMessageBox::warning(tabWidget, "Error", "\nError on ADMB input file creation. Check Log.", QMessageBox::Ok);
        return false;
    }

    // Build ADMB Executable
    reply = QMessageBox::question(tabWidget, QT_TR_NOOP("Build Info"),
                                  QT_TR_NOOP("\nReady to build ADMB C++ file. This may take a few moments.\n\nContinue?"),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return false;
    }


    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // -----------------------
       buildOK = admb.build();
    // -----------------------
    QApplication::restoreOverrideCursor();

    //
    // Run ADMB
    //
    if (buildOK) {
        msg  = "\nADMB build complete. Run Model?\n\n";
        msg += "Approx. run times (Multi-Species Mode):\n";
        msg += "  3 Species ~ 2 minutes on Linux Workstation\n";
        msg += "  3 Species ~ 3 minutes on Windows Laptop\n";
        msg += "  9 Species ~ 2 hours on Linux Workstation\n";
        reply = QMessageBox::question(tabWidget, QT_TR_NOOP("Build Complete"),
                                      QT_TR_NOOP(msg.toLatin1()),
                                      QMessageBox::No|QMessageBox::Yes,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::Yes) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            // -----------------------
               runOK = admb.run();
            // -----------------------
            QApplication::restoreOverrideCursor();
            if (runOK) {
                admb.parseReportFile(abundanceData);
            } else {
                msg  = "Error: Couldn't run MSCAA's ADMB executable.\n\n";
                msg += "Check Run Summary window for more information.";
                logger->logMsg(nmfConstants::Error,msg.toStdString());
                msg = "\n"+msg+"\n";
                QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
            }

            return true;
        }
    } else {
//        msg = "Error: Couldn't run admb. Please check that the executable can be found in the path environment variable.";
        msg  = "Error: Couldn't build MSCAA's ADMB executable.\n\n";
        msg += "Check Run Summary window for more information.";
        logger->logMsg(nmfConstants::Error,msg.toStdString());
        msg = "\n"+msg+"\n";
        QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
    }

    return false;
}

bool saveBinnedTable(QTabWidget*   tabWidget,
                     nmfDatabase*  databasePtr,
                     nmfLogger*    logger,
                     QTableView*   tableView,
                     std::string   projectSettingsConfig,
                     std::string   table,
                     int           numBins,
                     QString       binType,
                     QString       predatorSpecies,
                     QString       predatorAge)
{
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    bool foundSpecies;
    bool foundInvalidYearlySum;
    bool yearValuesOK;
    int minAge;
    int maxAge;
    int FirstYear;
    int LastYear;
    int UpperLimit;
    int NumColumns = smodel->columnCount();
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::string deleteCmd="";
    std::string saveCmd="";
    std::string errorMsg;
    std::string value;
    std::string ColName;
    std::string saveStr;
    std::string binTypeStr;
    std::string deleteStr;
    double YearlySum;
    QModelIndex index;
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    std::vector<std::string> allSpecies;
    bool isDietTable = (table == nmfConstantsMSCAA::TableDiet);

    foundSpecies = databasePtr->getSpecies(logger,allSpecies);
    if (!foundSpecies)
        return false;

    // Get range data for ages and years
    databasePtr->getSpeciesData(logger,predatorSpecies.toStdString(),
                   minAge,maxAge,FirstYear,LastYear,
                   MinLength,MaxLength,NumLengthBins);
    if (isDietTable) {
        getFHYearRange(databasePtr,logger,projectSettingsConfig,
                       FirstYear, LastYear);
    }

    yearValuesOK = checkYearValues(logger,FirstYear,LastYear,smodel);
    if (! yearValuesOK) {
        msg = "\nError Found in First and Last Year values.\n\n";
        msg += "Re-check: \n";
        msg += "[1] First Year and Last Year values are correct.\n";
        msg += "[2] There are no Bins that overlap.\n";
        msg += "[3] There are no gaps in Bins.\n";
        msg += "[4] That the First Year and Last Year values haven't changed from their default values.\n";
        QMessageBox::critical(tabWidget, QObject::tr("Bin Error"),
                             msg,QMessageBox::Ok);
        return false;
    }

    // Build insert command from the model data
    saveCmd = "INSERT INTO " + table;
    if (isDietTable) {
        saveCmd += " (MohnsRhoLabel,SystemName,PredatorName,PredatorAge,Bin,BinType,ColName,Value) VALUES ";
    } else {
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Segment,ColName,Value) VALUES ";
    }

    UpperLimit = (isDietTable) ? NumColumns-1 : NumColumns;
    for (int bin=0; bin<smodel->rowCount(); ++bin) {
        foundInvalidYearlySum = false;
        YearlySum = 0;
        for (int col=0; col<NumColumns; ++col) { // 1st two cols are years, then age groups, last col is Other Food
            index = smodel->index(bin,col);
            value = index.data().toString().toStdString();
            if (col == 0)
                ColName = "First Year";
            else if (col == 1)
                ColName = "Last Year";
            else if ((col >=2) && (col < UpperLimit)) {
                ColName = (isDietTable) ? allSpecies[col-2] : "Age " + std::to_string(minAge+col-2);
                YearlySum += std::stod(value);
            } else if (col == NumColumns-1)
                ColName = "Other Food";
            if (isDietTable && (col == NumColumns-1)) {
                value = std::to_string(1.0-YearlySum);
            }
            saveStr    = "','" + predatorSpecies.toStdString() + "'";
            binTypeStr = "";
            if (isDietTable) {
                saveStr   += ", " + predatorAge.toStdString();
                binTypeStr = " ,'" + binType.toStdString() + "'";
            }
            saveCmd += "('" + MohnsRhoLabel +
                    "','" + projectSettingsConfig +
                    saveStr +
                    " , " + std::to_string(bin+col/100.0) +
                    binTypeStr +
                    ",'" + ColName +
                    "', " + value + "),";
            if (isDietTable && (YearlySum > 1.0)) {
                foundInvalidYearlySum = true;
            }
        }
        if (foundInvalidYearlySum) {
            msg = "\nOther Food values must be <= 1. These are auto calculated as 1 - sum(Species Values).\n";
            QMessageBox::critical(tabWidget, QObject::tr("Other Food Error"),
                                  msg,QMessageBox::Ok);
            return false;
        }
    }

    // Delete the current Species entry here
    if (isDietTable) {
        deleteStr = "' AND PredatorName = '" + predatorSpecies.toStdString() +
                    "' AND PredatorAge  = "  + std::to_string(predatorAge.toInt());
    } else {
         deleteStr = "' AND SpeName = '" + predatorSpecies.toStdString() + "'";
    }
    deleteCmd = "DELETE FROM " + table + " WHERE SystemName = '" +
                projectSettingsConfig + deleteStr;

    errorMsg = databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from " + QString::fromStdString(table) + " table";
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveBinnedTable: DELETE error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveBinnedTable: Write table error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(tabWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Reload it so that the totals get recalculated
    loadBinnedTable(databasePtr,logger,
                    projectSettingsConfig,
                    tableView,table,
                    numBins,binType,
                    predatorSpecies,
                    predatorAge);

    return true;
}

bool saveCatchAtLengthTable(QTabWidget*  tabWidget,
               nmfDatabase* databasePtr,
               nmfLogger*   logger,
               QTableView*  tableView,
               std::string  projectSettingsConfig,
               std::string  table,
               std::string  species,
               std::string  fleet,
               std::string  units)
{
    bool foundSpecies;
    int minAge;
    int maxAge;
    int FirstYear;
    int LastYear;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::string deleteCmd="";
    std::string saveCmd="";
    std::string errorMsg;
    std::string value;
    std::string binName;
    std::string surveyOrFleetStr;
    QModelIndex index;
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());

    foundSpecies = databasePtr->getSpeciesData(logger,species,minAge,maxAge,
                                  FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies)
        return false;

    // Build insert command from the model data
    saveCmd  = "INSERT INTO " + table;
    saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Fleet,Year,BinNumber,BinName,Value,Units) VALUES ";
    for (int year=0; year<smodel->rowCount(); ++year) {
        for (int binNum=0; binNum<smodel->columnCount(); ++binNum) {
            binName = smodel->horizontalHeaderItem(binNum)->text().toStdString();
            index = smodel->index(year,binNum);
            value = index.data().toString().toStdString();
            saveCmd += "('"  + MohnsRhoLabel +
                       "','" + projectSettingsConfig +
                       "','" + species +
                       "','" + fleet +
                       "', " + std::to_string(FirstYear+year) +
                       " , " + std::to_string(binNum) +
                       " ,'" + binName +
                       "', " + value +
                       " ,'" + units + "'),";
        }
    }

    // Delete the current Species entry here
    surveyOrFleetStr = " AND Fleet = '" + fleet + "'";
    deleteCmd = "DELETE FROM " + table +
            " WHERE SystemName = '" + projectSettingsConfig +
            "' AND SpeName = '" + species + "' " + surveyOrFleetStr;
    errorMsg = databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from " + QString::fromStdString(table) + " table";
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveCatchAtLengthTable: DELETE error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveCatchAtLengthTable: Write table error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(tabWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Reload it so that the totals get recalculated
    loadCatchAtLengthTable(databasePtr,logger,
              projectSettingsConfig,
              tableView,table,
              QString::fromStdString(species),
              fleet);

    return true;
}

bool saveParameterTable(QTabWidget*  tabWidget,
                        nmfDatabase* databasePtr,
                        nmfLogger*   logger,
                        QTableView*  tableView,
                        std::string  projectSettingsConfig,
                        std::string  table,
                        std::string  species,
                        std::string  algorithm)
{
    int minAge;
    int maxAge;
    int FirstYear;
    int LastYear;
    int NumLengthBins;
    float MinLength;
    float MaxLength;
    std::string value;
    std::string deleteCmd="";
    std::string saveCmd="";
    std::string errorMsg;
    QModelIndex index;
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    std::vector<std::string> parameterName = {"alpha","beta","gamma"};

    if (! databasePtr->getSpeciesData(logger,species,minAge,maxAge,
                                  FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins))
        return false;

    // Build insert command from the model data
    saveCmd  = "INSERT INTO " + table;
    saveCmd += " (MohnsRhoLabel,SystemName,Algorithm,SpeName,ParameterName,Value) VALUES ";

    for (int col=0; col<smodel->columnCount(); ++col) {
        index = smodel->index(0,col);
        value = index.data().toString().toStdString();
        saveCmd += "('"  + MohnsRhoLabel +
                "','" + projectSettingsConfig +
                "','" + algorithm +
                "','" + species +
                "','" + parameterName[col] +
                "', " + value + " ),";
    }

    // Delete the current Species entry here
    deleteCmd = "DELETE FROM " + table +
            " WHERE SystemName = '" + projectSettingsConfig +
            "' AND SpeName = '" + species + " "
            "' AND Algorithm = '" + algorithm + "'";
    errorMsg = databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from " + QString::fromStdString(table) + " table";
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveParameterTable: DELETE error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveParameterTable: Write table error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(tabWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Reload it so that the totals get recalculated
    loadParameterTable(databasePtr,logger,
              projectSettingsConfig,
              tableView,table,
              QString::fromStdString(species),
              QString::fromStdString(algorithm));

    return true;
}

bool saveProportionTable(QTabWidget*  tabWidget,
                         nmfDatabase* databasePtr,
                         nmfLogger*   logger,
                         QTableView*  tableView,
                         std::string  projectSettingsConfig,
                         std::string  table,
                         std::string  species,
                         std::string  surveyOrFleet)
{
    int NumRows;
    int NumCols;
    double value;
    double rowTotal;
    std::string saveCmd;
    std::string errorMsg;
    std::string deleteCmd;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    std::string units = "";
    std::string MohnsRhoLabel = "";
    std::string surveyStr;
    std::string surveyDeleteStr;
    QStandardItem* rowHeaderItem;
    QStandardItem* colHeaderItem;
    QString msg;
    bool isSurvey = (table == nmfConstantsMSCAA::TableCatchSurveyProportion);

    NumRows = smodel->rowCount();
    NumCols = smodel->columnCount();

    saveCmd = "INSERT INTO " + table;
    if (isSurvey) {
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Survey,Year,Age,Value,Units) VALUES ";
        surveyStr = surveyOrFleet + ",";
    } else {
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Fleet,Year,Age,Value,Units) VALUES ";
        surveyStr = "'" + surveyOrFleet + "',";
    }
    for (int row = 0; row < NumRows; ++row) {
        rowHeaderItem = smodel->verticalHeaderItem(row);
        // Find row total to make the proportion
        rowTotal = 0;
        for (int col = 0; col < NumCols; ++col) {
            rowTotal += smodel->index(row,col).data().toDouble();
        }
        if (int(rowTotal) == 0) {
            logger->logMsg(nmfConstants::Warning,"Warning: Found row total of 0 in table: "+table);
            return false;
        } else {
            for (int col = 0; col < NumCols; ++col) {
                colHeaderItem = smodel->horizontalHeaderItem(col);
                value = smodel->index(row,col).data().toDouble() / rowTotal;
                saveCmd += "('" + MohnsRhoLabel +
                        "','" + projectSettingsConfig +
                        "','" + species +
                        "',"  + surveyStr +
                        rowHeaderItem->text().toStdString() +
                        ", "  + colHeaderItem->text().split(" ")[1].toStdString() +
                        ", "  + std::to_string(value) +
                        ",'"  + units + "'),";
            }
        }
    }

    // Delete the current Species entry here
    surveyDeleteStr = (isSurvey) ? " AND Survey = " + surveyOrFleet : "";
    deleteCmd = "DELETE FROM " + table +
                " WHERE SystemName = '" + projectSettingsConfig +
                "' AND SpeName = '" + species + "' " + surveyDeleteStr;
    errorMsg = databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from " + QString::fromStdString(table) + " table";
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveProportionTable: DELETE error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveProportionTable: Write table error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(tabWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    return true;
}

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
               bool         displayFirstYearOnly)
{
    bool foundSpecies;
    int foundSumGreaterThanOne = 0;
    int minAge;
    int maxAge;
    int FirstYear;
    int LastYear;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    double sum;
    std::string deleteCmd="";
    std::string saveCmd="";
    std::string errorMsg;
    std::string value;
    std::string surveyOrFleetStr;
    QModelIndex index;
    std::string MohnsRhoLabel = ""; // placeholder
    QString msg;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    bool hasSurvey = (table == nmfConstantsMSCAA::TableCatchSurvey);
    bool noSurveyOrFleet = (table == nmfConstantsMSCAA::TableWeight)      ||
                           (table == nmfConstantsMSCAA::TableMaturity)    ||
                           (table == nmfConstantsMSCAA::TableConsumption) ||
                           (table == nmfConstantsMSCAA::TableInitialAbundance);

    foundSpecies = databasePtr->getSpeciesData(logger,species,minAge,maxAge,
                                  FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies)
        return false;

    // Build insert command from the model data
    saveCmd = "INSERT INTO " + table;
    if (noSurveyOrFleet) {
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Year,Age,Value,Units) VALUES ";
    } else if (hasSurvey)
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Survey,Year,Age,Value,Units) VALUES ";
    else
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Fleet,Year,Age,Value,Units) VALUES ";

    for (int year=0; year<smodel->rowCount(); ++year) {
        sum = 0;
        for (int age=0; age<smodel->columnCount(); ++age) {
            index = smodel->index(year,age);
            value = index.data().toString().toStdString();
            if (includeTotalColumn) {
                if (age == smodel->columnCount()-1) {
                    value = std::to_string(sum);
                } else {
                    sum += index.data().toDouble();
                }
                if (sum > 1.0) {
                    ++foundSumGreaterThanOne;
                }
            }
            //surveyStr = (hasSurvey) ? "', " + surveyOrFleet : "'";
            if (noSurveyOrFleet) {
                surveyOrFleetStr = ",";
            } else {
                surveyOrFleetStr = (hasSurvey) ?
                                    ", " + surveyOrFleet + ", " :
                                    ",'" + surveyOrFleet + "',";
            }
            saveCmd += "('"  + MohnsRhoLabel +
                       "','" + projectSettingsConfig +
                       "','" + species + "'" + surveyOrFleetStr +
                               std::to_string(FirstYear+year) +
                       ", "  + std::to_string(minAge+age) +
                       ", "  + value + ",'" + units + "'),";
        }
    }
    if ((foundSumGreaterThanOne > 0) && isProportion) {
        QMessageBox::StandardButton reply = QMessageBox::question(tabWidget, QObject::tr("Check Totals"),
                                                                  QObject::tr("\nFound Total(s) > 1.0. Continue with Save?"),
                                                                  QMessageBox::No|QMessageBox::Yes,
                                                                  QMessageBox::Yes);
        if (reply == QMessageBox::No) {
            return false;
        }
    }

    // Delete the current Species entry here
    if (noSurveyOrFleet) {
        surveyOrFleetStr = "";
    } else {
        surveyOrFleetStr = (hasSurvey) ? " AND Survey = "  + surveyOrFleet :
                                         " AND Fleet  = '" + surveyOrFleet + "'";
    }
    deleteCmd = "DELETE FROM " + table +
            " WHERE SystemName = '" + projectSettingsConfig +
            "' AND SpeName = '" + species + "' " + surveyOrFleetStr;
    errorMsg = databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from " + QString::fromStdString(table) + " table";
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveTable: DELETE error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveTable: Write table error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(tabWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Reload it so that the totals get recalculated
    loadTable(databasePtr,logger,
              projectSettingsConfig,
              tableView,table,
              QString::fromStdString(species),
              surveyOrFleet,units,
              includeTotalColumn,
              displayFirstYearOnly);

    return true;
}

bool saveTheTotalTable(QTabWidget* tabWidget,
                       nmfDatabase* databasePtr,
                       nmfLogger* logger,
                       QTableView* tableView,
                       std::string projectSettingsConfig,
                       std::string table,
                       std::string species,
                       std::string surveyOrFleet,
                       std::string units)
{
    bool foundSpecies;
    int minAge;
    int maxAge;
    int FirstYear;
    int LastYear;
    float MinLength;
    float MaxLength;
    int NumLengthBins;
    std::string deleteCmd="";
    std::string saveCmd="";
    std::string errorMsg;
    std::string value;
    QModelIndex index;
    std::string MohnsRhoLabel = ""; // placeholder
    std::string surveyOrFleetStr;
    QString msg;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    bool hasSurvey = (table == nmfConstantsMSCAA::TableCatchSurveyTotal);

    foundSpecies = databasePtr->getSpeciesData(logger,species,minAge,maxAge,
                                  FirstYear,LastYear,
                                  MinLength,MaxLength,NumLengthBins);
    if (! foundSpecies)
        return false;

    // Build insert command from the model data
    saveCmd = "INSERT INTO " + table;
    if (hasSurvey)
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Survey,Year,Value,Units) VALUES ";
    else
        saveCmd += " (MohnsRhoLabel,SystemName,SpeName,Fleet,Year,Value,Units) VALUES ";
    for (int year=0; year<smodel->rowCount(); ++year) {
        for (int total=0; total<smodel->columnCount(); ++total) {
            index = smodel->index(year,total);
            value = index.data().toString().toStdString();
            surveyOrFleetStr = (hasSurvey) ? "', " + surveyOrFleet :
                                      "','" + surveyOrFleet + "'";
            saveCmd += "('" + MohnsRhoLabel +
                    "','" + projectSettingsConfig +
                    "','" + species + surveyOrFleetStr +
                    "," + std::to_string(FirstYear+year) +
                    ", " + value + ",'" + units + "'),";
        }
    }

    // Delete the current Species entry here
    surveyOrFleetStr = (hasSurvey) ? " AND Survey = " + surveyOrFleet :
                                     " AND Fleet = '" + surveyOrFleet + "'";
    deleteCmd = "DELETE FROM " + table +
            " WHERE SystemName = '" + projectSettingsConfig +
            "' AND SpeName = '" + species + "' " + surveyOrFleetStr;
    errorMsg = databasePtr->nmfUpdateDatabase(deleteCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        msg = "\nError in Save command. Couldn't delete all records from " + QString::fromStdString(table) + " table";
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveTable: DELETE error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + deleteCmd);
        QMessageBox::warning(tabWidget, "Error", msg, QMessageBox::Ok);
        return false;
    }

    // Save the new data
    saveCmd = saveCmd.substr(0,saveCmd.size()-1);
    errorMsg = databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        logger->logMsg(nmfConstants::Error,"nmfMSCAAUtils::saveTheTotalTable: Write table error: " + errorMsg);
        logger->logMsg(nmfConstants::Error,"cmd: " + saveCmd);
        QMessageBox::warning(tabWidget, "Error",
                             "\nError in Save command.  Check that all cells are populated.\n",
                             QMessageBox::Ok);
        return false;
    }

    // Reload it so that the totals get recalculated
    loadTheTotalTable(databasePtr,logger,
                      projectSettingsConfig,
                      tableView,table,
                      QString::fromStdString(species),
                      surveyOrFleet,units);

    return true;
}

void setColumnsToFirst(QTableView* tableView)
{
    int NumRows;
    int NumCols;
    std::string firstColValue;
    QModelIndex firstColIndex;
    QStandardItemModel* smodel = qobject_cast<QStandardItemModel*>(tableView->model());
    QStandardItem *item;

    NumRows = smodel->rowCount();
    NumCols = smodel->columnCount();

    for (int row = 0; row < NumRows; ++row) {
        firstColIndex = smodel->index(row,2);
        firstColValue = firstColIndex.data().toString().toStdString();
        for (int col = 3; col < NumCols; ++col) {
            item = new QStandardItem(QString::fromStdString(firstColValue));
            item->setTextAlignment(Qt::AlignCenter);
            smodel->setItem(row, col, item);
        }
    }
    tableView->resizeColumnsToContents();
}

bool systemTableExists(nmfDatabase*  databasePtr,
                       nmfLogger*    logger,
                       std::string&  projectSettingsConfig)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    // See if System data already exists
    fields     = {"SystemName"};
    queryStr   = "SELECT SystemName FROM " + nmfConstantsMSCAA::TableModels;
    queryStr  += "WHERE SystemName = '" + projectSettingsConfig + "'";
    dataMap    = databasePtr->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();

    return (NumRecords == 1);
}

bool removePreviousRunsData(
        QWidget*       parent,
        nmfDatabase*   databasePtr,
        nmfLogger*     logger,
        std::string&   projectDir,
        std::string&   projectName,
        std::string&   projectSettingsConfig,
        const QString& type,
        QTextEdit*     summaryTextBox)
{
    QStringList filters;
    QString msg;
    QMessageBox::StandardButton reply;

    // Remove previous run's data files
    QString admbDir = getADMBDir(
       databasePtr,logger,projectDir,projectName,
       projectSettingsConfig,type,summaryTextBox);

    msg = "\nOK to remove previous run's output data files? ";
    msg += "This will remove all files in directory: \n\n";
    msg += admbDir + "\n";
    reply = QMessageBox::question(parent, "Remove Previous Run's Files",
                                  msg,
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::Yes);
    if (reply == QMessageBox::No) {
        return false;
    }

    QDir dir(admbDir);
    filters << "MSCAA*" << "*.cxx" << "admodel.*" << "fmin.log";
    dir.setNameFilters(filters);
    for(const QString & filename: dir.entryList()){
        dir.remove(filename);
        msg = "Removing: " + filename;
        logger->logMsg(nmfConstants::Normal,msg.toStdString());
    }
    return true;
}


}
