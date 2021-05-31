#include "nmfADMB.h"
#include <stdio.h>

#ifdef _WIN32
   #include <windows.h>
   #include <tchar.h>
#endif

nmfADMB::nmfADMB(nmfDatabase*   databasePtr,
                 nmfLogger*     logger,
                 std::string&   projectDir,
                 std::string&   projectName,
                 std::string&   projectSettingsConfig,
                 QTextEdit*     outputTE,
                 const QString& type,
                 bool           clearSummary)
{
    m_database              = databasePtr;
    m_logger                = logger;
    m_projectDir            = projectDir;
    m_projectName           = projectName;
    m_projectSettingsConfig = projectSettingsConfig;
    m_speciesData.MinAge    = -1;
    m_outputTE              = outputTE;
    m_type                  = type;
    m_trophic               = 0;
    m_debug                 = 0;
    m_origPinFile.clear();
    m_origTplFile.clear();

    if (clearSummary) {
        clearSummaryTextBox();
        QString msg = "========== Optimization Software: ADMB ==========<br>";
        appendSummaryTextBox(msg);
    }
    setFilenames();
}

QString
nmfADMB::getADMBDir()
{
    std::string msg;

    // Does Project dir exists? If not, make it.
    QString ADMBDir = QDir(QString::fromStdString(m_projectDir)).filePath(QString::fromStdString(m_projectName));
    ADMBDir = QDir(ADMBDir).filePath("ADMB");
    ADMBDir = QDir(ADMBDir).filePath(m_type);
    if (! QDir(ADMBDir).exists()) {
        if (! QDir().mkpath(ADMBDir)) {
            msg = "Couldn't create file: " + ADMBDir.toStdString();
            m_logger->logMsg(nmfConstants::Error,msg);
        }
    }

    return ADMBDir;
}

void
nmfADMB::setFilenames()
{
    QString ADMBDir = getADMBDir();

    QString prefix = "MSCAA";
    m_dataFile        = QDir(ADMBDir).filePath(prefix+".dat");
    m_parameterFile   = QDir(ADMBDir).filePath(prefix+".par");
    m_pinFile         = QDir(ADMBDir).filePath(prefix+".pin");
    m_tplFile         = QDir(ADMBDir).filePath(prefix+".tpl");
    m_reportFile      = QDir(ADMBDir).filePath(prefix+".rep");
    m_buildOutput     = QDir(ADMBDir).filePath(prefix+"_Build.out");
    m_runOutput       = QDir(ADMBDir).filePath(prefix+"_Run.out");
}

void
nmfADMB::clearSummaryTextBox()
{
    m_outputTE->clear();
}

void
nmfADMB::appendSummaryTextBox(QString& msg)
{
    m_outputTE->append(msg);
    m_outputTE->update();
}

void
nmfADMB::setTrophic(const int& trophic)
{
    QString msg = "Trophic: " + QString::number(trophic) + "<br>";
    m_trophic = trophic;
    appendSummaryTextBox(msg);
}

void
nmfADMB::setPinFile(std::string pinFile)
{
    m_origPinFile = pinFile;
}

void
nmfADMB::setTplFile(std::string tplFile)
{
    m_origTplFile = tplFile;
}

void
nmfADMB::setDebug(const int& debug)
{
    QString msg = "Debug Level: " + QString::number(debug) + "<br>";
    m_debug = debug;
    appendSummaryTextBox(msg);
}

bool
nmfADMB::createInputFiles()
{

    // 1. Write .dat file
    if (! writeADMBDataFile(m_dataFile,m_speciesData)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::createInputFiles: Problem with writeADMBDataFile for dataFile: "+m_dataFile.toStdString());
        return false;
    }

    // 2. Write .par file
    if (! writeADMBParameterFile(m_parameterFile)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::createInputFiles: Problem with writeADMBParameterFile");
        return false;
    }

    // 3. Write .pin file
    if (! writeADMBPinFile(m_pinFile)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::createInputFiles: Problem with writeADMBPinFile");
        return false;
    }

    // 4. Write .tpl file
    if (! writeADMBTPLFile(m_tplFile)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::createInputFiles: Problem with writeADMBTPLFile");
        return false;
    }

    // 5. Copy any .cxx files necessary, needed for proper building
    if (! writeADMBCxxFiles(m_tplFile)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::createInputFiles: Problem with writeADMBCxxFile");
        return false;
    }

    return true;
}

bool
nmfADMB::build()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfADMB::build()");

    // Build ADMB
    if (! buildADMB(m_tplFile,m_buildOutput)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::buildExecutable: ADMB build did not complete ");
        return false;
    }
    return true;
}

bool
nmfADMB::run()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfADMB::run()");

    // Run ADMB
    if (! runADMB(m_tplFile,m_runOutput)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::run: Problem with runADMB");
        return false;
    }
    return true;
}


bool
nmfADMB::getSurveyWeights(
        const int& NumSpecies,
        int&       NumSurveys,
        QString&   TSwtValues,
        QString&   SPwtValues)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    NumSurveys = 0;

    fields     = {"Surveys"};
    queryStr   = "SELECT COUNT(DISTINCT(Survey)) as Surveys FROM SurveyWeights";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["Surveys"].size();
    if (NumRecords > 0) {
        NumSurveys = std::stoi(dataMap["Surveys"][0]);
    } else {
        msg = "nmfADMB::getSurveyWeights: No Surveys found.";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    fields     = {"SystemName","SpeName","Survey","TSwtValue","SPwtValue"};
    queryStr   = "SELECT SystemName,SpeName,Survey,TSwtValue,SPwtValue FROM SurveyWeights";
    queryStr  += " ORDER BY Survey,SpeName";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords != NumSpecies*NumSurveys) {
        msg  = "nmfADMB::getSurveyWeights: Incorrect number of records found in SurveyWeights. ";
        msg += "Expecting " + std::to_string(NumSpecies*NumSurveys) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int survey = 0; survey < NumSurveys; ++survey) {
        for (int species = 0; species < NumSpecies; ++species) {
            TSwtValues += "  " + QString::fromStdString(dataMap["TSwtValue"][m]);
            SPwtValues += "  " + QString::fromStdString(dataMap["SPwtValue"][m++]);
        }
    }

    return true;
}

bool
nmfADMB::getSurveyMonths(
        const int& NumSpecies,
        const int& NumSurveys,
        QString&   FICMonthValues)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    fields     = {"SystemName","SpeName","Survey","Value"};
    queryStr   = "SELECT SystemName,SpeName,Survey,Value FROM SurveyMonth";
    queryStr  += " ORDER BY SpeName";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords != NumSpecies*NumSurveys) {
        msg  = "nmfADMB::getSurveyMonths: Incorrect number of records found in SurveyMonths. ";
        msg += "Expecting " + std::to_string(NumSpecies*NumSurveys) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int species = 0; species < NumSpecies; ++species) {
        for (int survey = 0; survey < NumSurveys; ++survey) {
            FICMonthValues += "  " + QString::fromStdString(dataMap["Value"][m++]);
        }
        FICMonthValues += "\n";
    }

    return true;
}

void
nmfADMB::clear(StructSpeciesData& SpeciesData)
{
    SpeciesData.MinAge.clear();
    SpeciesData.MaxAge.clear();
    SpeciesData.FirstAgePrtRcrt.clear();
    SpeciesData.AgeFullRcrt.clear();
    SpeciesData.AgeFullRcrtSurvey.clear();
    SpeciesData.SurveySel.clear();
    SpeciesData.FirstYear.clear();
    SpeciesData.LastYear.clear();
    SpeciesData.NumSurveys.clear();
    SpeciesData.NumFleets.clear();
    SpeciesData.Nseg.clear();
    SpeciesData.aAge1ph.clear();
    SpeciesData.aFtph.clear();
    SpeciesData.dAge1ph.clear();
    SpeciesData.dFtph.clear();
    SpeciesData.ficph.clear();
    SpeciesData.fishph.clear();
    SpeciesData.Yr1ph.clear();
    SpeciesData.Rhoph.clear();
    SpeciesData.TCwt.clear();
    SpeciesData.CPwt.clear();
    SpeciesData.Bwt.clear();
    SpeciesData.Ywt.clear();
    SpeciesData.Rwt.clear();
    SpeciesData.FHwt.clear();
    SpeciesData.Bthres.clear();
    SpeciesData.Rthres.clear();
    SpeciesData.MinLength.clear();
    SpeciesData.MaxLength.clear();
    SpeciesData.NumLengthBins.clear();
    SpeciesData.CatchAtAge.clear();
}

bool
nmfADMB::getSpeciesData(
        const int& NumSpecies,
        StructSpeciesData& SpeciesData)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    fields     = {"SpeName","MinAge","MaxAge","FirstAgePrtRcrt","AgeFullRcrt","AgeFullRcrtSurvey",
                  "SurveySel","FirstYear","LastYear","NumSurveys","NumFleets","Nseg","aAge1ph",
                  "aFtph","dAge1ph","dFtph","ficph","fishph","Yr1ph","Rhoph","TCwt","CPwt","Bwt",
                  "Ywt","Rwt","FHwt","Bthres","Rthres","MinLength","MaxLength","NumLengthBins","CatchAtAge"};
    queryStr   = "SELECT SpeName,MinAge,MaxAge,FirstAgePrtRcrt,AgeFullRcrt,AgeFullRcrtSurvey,";
    queryStr  += "SurveySel,FirstYear,LastYear,NumSurveys,NumFleets,Nseg,aAge1ph,aFtph,dAge1ph,";
    queryStr  += "dFtph,ficph,fishph,Yr1ph,Rhoph,TCwt,CPwt,Bwt,Ywt,Rwt,FHwt,Bthres,Rthres,";
    queryStr  += "MinLength,MaxLength,NumLengthBins,CatchAtAge FROM Species ORDER BY SpeName";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SpeName"].size();
    if (NumRecords != NumSpecies) {
        msg  = "nmfADMB::getSpeciesData: Incorrect number of records found in getSpeciesData. ";
        msg += "Expecting " + std::to_string(NumSpecies) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    clear(SpeciesData);
    for (int species = 0; species < NumSpecies; ++species) {
        SpeciesData.MinAge            += "  " + QString::fromStdString(dataMap["MinAge"][m]);
        SpeciesData.MaxAge            += "  " + QString::fromStdString(dataMap["MaxAge"][m]);
        SpeciesData.FirstAgePrtRcrt   += "  " + QString::fromStdString(dataMap["FirstAgePrtRcrt"][m]);
        SpeciesData.AgeFullRcrt       += "  " + QString::fromStdString(dataMap["AgeFullRcrt"][m]);
        SpeciesData.AgeFullRcrtSurvey += "  " + QString::fromStdString(dataMap["AgeFullRcrtSurvey"][m]);
        SpeciesData.SurveySel         += "  " + QString::fromStdString(dataMap["SurveySel"][m]);
        SpeciesData.FirstYear         += "  " + QString::fromStdString(dataMap["FirstYear"][m]);
        SpeciesData.LastYear          += "  " + QString::fromStdString(dataMap["LastYear"][m]);
        SpeciesData.NumSurveys        += "  " + QString::fromStdString(dataMap["NumSurveys"][m]);
        SpeciesData.NumFleets         += "  " + QString::fromStdString(dataMap["NumFleets"][m]);
        SpeciesData.Nseg              += "  " + QString::fromStdString(dataMap["Nseg"][m]);
        SpeciesData.aAge1ph           += "  " + QString::fromStdString(dataMap["aAge1ph"][m]);
        SpeciesData.aFtph             += "  " + QString::fromStdString(dataMap["aFtph"][m]);
        SpeciesData.dAge1ph           += "  " + QString::fromStdString(dataMap["dAge1ph"][m]);
        SpeciesData.dFtph             += "  " + QString::fromStdString(dataMap["dFtph"][m]);
        SpeciesData.ficph             += "  " + QString::fromStdString(dataMap["ficph"][m]);
        SpeciesData.fishph            += "  " + QString::fromStdString(dataMap["fishph"][m]);
        SpeciesData.Yr1ph             += "  " + QString::fromStdString(dataMap["Yr1ph"][m]);
        SpeciesData.Rhoph             += "  " + QString::fromStdString(dataMap["Rhoph"][m]);
        SpeciesData.TCwt              += "  " + QString::fromStdString(dataMap["TCwt"][m]);
        SpeciesData.CPwt              += "  " + QString::fromStdString(dataMap["CPwt"][m]);
        SpeciesData.Bwt               += "  " + QString::fromStdString(dataMap["Bwt"][m]);
        SpeciesData.Ywt               += "  " + QString::fromStdString(dataMap["Ywt"][m]);
        SpeciesData.Rwt               += "  " + QString::fromStdString(dataMap["Rwt"][m]);
        SpeciesData.FHwt              += "  " + QString::fromStdString(dataMap["FHwt"][m]);
        SpeciesData.Bthres            += "  " + QString::fromStdString(dataMap["Bthres"][m]);
        SpeciesData.Rthres            += "  " + QString::fromStdString(dataMap["Rthres"][m]);
        SpeciesData.MinLength         += "  " + QString::fromStdString(dataMap["MinLength"][m]);
        SpeciesData.MaxLength         += "  " + QString::fromStdString(dataMap["MaxLength"][m]);
        SpeciesData.NumLengthBins     += "  " + QString::fromStdString(dataMap["NumLengthBins"][m]);
        SpeciesData.CatchAtAge        += "  " + QString::fromStdString(dataMap["CatchAtAge"][m]);

        ++m;
    }

    // Find numbers per species
    QStringList NumFleets = SpeciesData.NumFleets.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    SpeciesData.Fleets.clear();
    for (QString fleet : NumFleets) {
        SpeciesData.Fleets.push_back(fleet.toInt());
    }
    QStringList NumSurveys = SpeciesData.NumSurveys.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    SpeciesData.Surveys.clear();
    for (QString survey : NumSurveys) {
        SpeciesData.Surveys.push_back(survey.toInt());
    }

    QStringList FirstYears = SpeciesData.FirstYear.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    QStringList LastYears  = SpeciesData.LastYear.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    SpeciesData.Years.clear();
    for (int i = 0; i < FirstYears.size(); ++i) {
        SpeciesData.Years.push_back(LastYears[i].toInt()-FirstYears[i].toInt()+1);
    }

    QStringList MinAge = SpeciesData.MinAge.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    QStringList MaxAge = SpeciesData.MaxAge.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    SpeciesData.Ages.clear();
    for (int i = 0; i < MinAge.size(); ++i) {
        SpeciesData.Ages.push_back(MaxAge[i].toInt()-MinAge[i].toInt()+1);
    }

    return true;
}

bool
nmfADMB::getSystemData(
        StructSystemData& SystemData)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    fields     = {"SystemName","TotalBiomass","FH_FirstYear","FH_LastYear","NumSpInter","Owt","LogNorm","MultiResid"};
    queryStr   =  "SELECT SystemName,TotalBiomass,FH_FirstYear,FH_LastYear,NumSpInter,Owt,LogNorm,MultiResid FROM `System`";
    queryStr  +=  " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords == 0) {
        msg  = "nmfADMB::getSystemData: No records found in getSystemData for SystemName = '";
        msg += m_projectSettingsConfig + "'";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    QStringList parts = QString::fromStdString(dataMap["TotalBiomass"][0]).trimmed().split("_");

    double totalBiomass = parts[0].toDouble();
    QString units = parts[1];
    totalBiomass = (units == "Metric Tons") ? totalBiomass : totalBiomass/1000.0;

    SystemData.TotalBiomass = "  " + QString::number(totalBiomass);
    SystemData.FH_FirstYear = "  " + QString::fromStdString(dataMap["FH_FirstYear"][0]);
    SystemData.FH_LastYear  = "  " + QString::fromStdString(dataMap["FH_LastYear"][0]);
    SystemData.NumSpInter   = "  " + QString::fromStdString(dataMap["NumSpInter"][0]);
    SystemData.Owt          = "  " + QString::fromStdString(dataMap["Owt"][0]);
    SystemData.LogNorm      = "  " + QString::fromStdString(dataMap["LogNorm"][0]);
    SystemData.MultiResid   = "  " + QString::fromStdString(dataMap["MultiResid"][0]);

    return true;
}

bool
nmfADMB::getPredationMortalityEstimates(
        const StructSpeciesData& SpeciesData,
        QString& M2Estimates)
{
    int totNumAges = 0;

    QStringList minAgeList = SpeciesData.MinAge.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    QStringList maxAgeList = SpeciesData.MaxAge.split(QRegExp("\\s+"),Qt::SkipEmptyParts); // split and ignore "space" characters
    for (int i = 0; i < minAgeList.size(); ++i) {
        totNumAges += maxAgeList[i].toInt() - minAgeList[i].toInt() + 1;
    }

    m_logger->logMsg(nmfConstants::Warning,"Warning: Hard-coding M2 estimates to all 0's. Revisit this logic!");
    M2Estimates = "  0";
    M2Estimates = M2Estimates.repeated(totNumAges); // Hard-code to 0's for now. RSK re-visit this later

    return true;
}

bool
nmfADMB::getPreferredData(
        const int& NumSpecies,
        QString TableName,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;

    fields     = {"SystemName","PredatorName","PreyName","Value"};
    queryStr   = "SELECT SystemName,PredatorName,PreyName,Value FROM " + TableName.toStdString();
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != NumSpecies*NumSpecies) {
        msg  = "nmfADMB::getPreferredData: Incorrect number of records found in: " + TableName.toStdString() + ". ";
        msg += "Expecting " + std::to_string(NumSpecies*NumSpecies) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int i = 0; i < NumSpecies; ++i) {
        for (int j = 0; j < NumSpecies; ++j) {
            Data += "  " + QString::fromStdString(dataMap["Value"][m++]);
        }
        Data += "\n";
    }

    return true;
}

bool
nmfADMB::getTotalCatch(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    double sf = 1.0;
    QString units;
    int TotNumRecordsCalculated = 0;

    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Fleets[i]*SpeciesData.Years[i];
    }

    fields     = {"SystemName","SpeName","Fleet","Year","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Fleet,Year,Value,Units FROM CatchFisheryTotal";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) {
        msg  = "nmfADMB::getTotalCatch: Incorrect number of records found in: CatchFisheryTotal. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    units = QString::fromStdString(dataMap["Units"][0]);
    if (units == "000 Metric Tons")
        sf = 1000;
    else if (units == "000 000 Metric Tons")
        sf = 1000000;

    for (int i = 0; i < NumSpecies; ++i) {
        for (int j = 0; j < SpeciesData.Fleets[i]; ++j) {
            for (int k = 0; k < SpeciesData.Years[i]; ++k) {
                Data += "  " + QString::number(std::stod(dataMap["Value"][m++])*sf);
            }
            Data += "\n";
        }
    }

    return true;
}

bool
nmfADMB::getWeightAtAge(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    double sf = 1.0;
    QString units;
    int TotNumRecordsCalculated = 0;

    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Ages[i]*SpeciesData.Years[i];
    }

    fields     = {"SystemName","SpeName","Year","Age","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Year,Age,Value,Units FROM Weight";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) {
        msg  = "nmfADMB::getWeightAtAge: Incorrect number of records found in: Weight. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    units = QString::fromStdString(dataMap["Units"][0]);
    if (units == "Kilograms")
        sf = 1000.0;

    for (int i = 0; i < NumSpecies; ++i) {
        Data += "# Weight At Age for Species: " + QString::number(i+1) + " (grams)\n";
        for (int j = 0; j < SpeciesData.Years[i]; ++j) {
            for (int k = 0; k < SpeciesData.Ages[i]; ++k) {
                Data += "  " + QString::number(std::stod(dataMap["Value"][m++])*sf);
            }
            Data += "\n";
        }
        Data += "\n";
    }

    return true;
}


bool
nmfADMB::getCatch(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    double sf = 1.0;
    QString units;
    int TotNumRecordsCalculated = 0;

    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Ages[i]*SpeciesData.Years[i];
    }

    fields     = {"SystemName","SpeName","Fleet","Year","Age","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Fleet,Year,Age,Value,Units FROM CatchFishery";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) {
        msg  = "nmfADMB::getCatch: Incorrect number of records found in: CatchFishery. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    units = QString::fromStdString(dataMap["Units"][0]);
    if (units == "000 Fish")
        sf = 0.001;
    else if (units == "000 000 Fish")
        sf = 0.000001;

    for (int i = 0; i < NumSpecies; ++i) {
        Data += "# Catch for Species: " + QString::number(i+1) + " (# of fish)\n";
        for (int j = 0; j < SpeciesData.Years[i]; ++j) {
            for (int k = 0; k < SpeciesData.Ages[i]; ++k) {
                Data += "  " + QString::number(std::stod(dataMap["Value"][m++])*sf);
            }
            Data += "\n";
        }
    }

    return true;
}

bool
nmfADMB::getTotalSurvey(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    int TotNumRecordsCalculated = 0;

    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Surveys[i]*SpeciesData.Years[i];
    }

    fields     = {"SystemName","SpeName","Survey","Year","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Survey,Year,Value,Units FROM CatchSurveyTotal";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) {
        msg  = "nmfADMB::getTotalSurvey: Incorrect number of records found in: CatchSurveyTotal. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int i = 0; i < NumSpecies; ++i) {
        Data += "# Total FIC for Species: " + QString::number(i+1) + "\n";
        for (int j = 0; j < SpeciesData.Surveys[i]; ++j) {
//            Data += "# Total FIC for Species: " + QString::number(i+1) +
//                    ", Survey: " + QString::number(j+1) + "\n";
            for (int k = 0; k < SpeciesData.Years[i]; ++k) {
                Data += "  " + QString::number(std::stod(dataMap["Value"][m++]));
            }
            Data += "\n";
        }
    }

    return true;
}


bool
nmfADMB::getNaturalMortality(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        int& NumSegments,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    int TotNumRecordsCalculated = 0;
    int NumNonAgeColNameItems = 2;

    // Find number of segments
    fields      = {"Segs"};
    queryStr    = "SELECT COUNT(DISTINCT(Segment)) AS Segs FROM MortalityNatural WHERE ColName = 'First Year'";
    dataMap     = m_database->nmfQueryDatabase(queryStr, fields);
    NumSegments = std::stoi(dataMap["Segs"][0]);

    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Ages[i]; // 27
    }
    // Account for the First Year and Last Year ColName items
    TotNumRecordsCalculated = NumSegments*TotNumRecordsCalculated +
                              NumNonAgeColNameItems*NumSegments*NumSpecies;

    fields     = {"SystemName","SpeName","ColName","Value"};
    queryStr   = "SELECT SystemName,SpeName,ColName,Value FROM MortalityNatural";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) { // -2*NumSpecies because FirstYear and LastYear are also in the ColName field
        msg  = "nmfADMB::getNaturalMortality: Incorrect number of records found in: MortalityNatural. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int i = 0; i < NumSpecies; ++i) {
        Data += "# M1 for Species: " + QString::number(i+1) + "\n";
        for (int n = 0; n < NumSegments; ++n) {
            for (int k = 0; k < SpeciesData.Ages[i]+NumNonAgeColNameItems; ++k) {
                if ((dataMap["ColName"][m] != "First Year") &&
                    (dataMap["ColName"][m] != "Last Year"))
                {
                    Data += "  " + QString::number(std::stod(dataMap["Value"][m]));
                }
                ++m;
            }
            Data += "\n";
        }
        Data += "\n";
    }

    return true;
}

bool
nmfADMB::getConsumptionBiomass(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    int TotNumRecordsCalculated = 0;

    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Years[i]*SpeciesData.Ages[i];
    }

    fields     = {"SystemName","SpeName","Year","Age","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Year,Age,Value,Units FROM Consumption";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) {
        msg  = "nmfADMB::getConsumptionBiomass: Incorrect number of records found in: Consumption. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int i = 0; i < NumSpecies; ++i) {
        Data += "# Consumption:Biomass Ratio for Species: " + QString::number(i+1) + "\n";
        for (int j = 0; j < SpeciesData.Years[i]; ++j) {
            for (int k = 0; k < SpeciesData.Ages[i]; ++k) {
                Data += "  " + QString::number(std::stod(dataMap["Value"][m++]));
            }
            Data += "\n";
        }
        Data += "\n";
    }

    return true;
}


bool
nmfADMB::getSurvey(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    int TotNumRecordsCalculated = 0;

    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Surveys[i] *
                                   SpeciesData.Years[i]   *
                                   SpeciesData.Ages[i];
    }

    fields     = {"SystemName","SpeName","Survey","Year","Age","Value","Units"};
    queryStr   = "SELECT SystemName,SpeName,Survey,Year,Age,Value,Units FROM CatchSurvey";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) {
        msg  = "nmfADMB::getConsumptionBiomass: Incorrect number of records found in: CatchSurvey. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int i = 0; i < NumSpecies; ++i) {
        for (int s = 0; s < SpeciesData.Surveys[i]; ++s) {
            Data += "# FIC for Species: " + QString::number(i+1) +
                    ", Survey: " + QString::number(s+1) + "\n";
            for (int j = 0; j < SpeciesData.Years[i]; ++j) {
                for (int k = 0; k < SpeciesData.Ages[i]; ++k) {
                    Data += "  " + QString::number(std::stod(dataMap["Value"][m++]));
                }
                Data += "\n";
            }
            Data += "\n";
        }
        Data += "\n";
    }

    return true;
}


bool
nmfADMB::getFoodHabit(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        int& BinSize,
        QString& Data)
{
    int m = 0;
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    int TotNumRecordsCalculated = 0;
    int NumBins;

    // Find number of bins (assume every species has same bin size)
    fields     = {"SystemName","PredatorName","ColName","Value"};
    queryStr   = "SELECT SystemName,PredatorName,ColName,Value FROM Diet";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords > 2) {
        int FirstYear = std::stoi(dataMap["Value"][0]); // First Year
        int LastYear  = std::stoi(dataMap["Value"][1]); // Last Year
        BinSize = LastYear - FirstYear + 1;
    } else {
        return false;
    }

    // Find NumBins and calculate how many records "should" be found
    fields   = {"Bins"};
    queryStr = "SELECT COUNT(DISTINCT(Bin)) AS Bins from Diet where ColName = 'First Year'";
    dataMap  = m_database->nmfQueryDatabase(queryStr, fields);
    NumBins  = std::stoi(dataMap["Bins"][0]);
    for (int i=0; i<NumSpecies; ++i) {
        TotNumRecordsCalculated += SpeciesData.Ages[i]*NumBins*(2+NumSpecies+1); // 2 = First and Last Year, 1 = Other Food
    }

    fields     = {"SystemName","PredatorName","ColName","Value"};
    queryStr   = "SELECT SystemName,PredatorName,ColName,Value FROM Diet";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) { // -2*NumSpecies because FirstYear and LastYear are also in the ColName field
        msg  = "nmfADMB::getFoodHabit: Incorrect number of records found in: Diet. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int i = 0; i < NumSpecies; ++i) {
        Data += "# Food Habit for Species: " + QString::number(i+1) + "\n";
        for (int j = 0; j < SpeciesData.Ages[i]; ++j) {
            for (int n = 0; n < NumBins; ++n) {
                for (int k = 0; k < 2+NumSpecies+1; ++k) {
                    if ((dataMap["ColName"][m] != "First Year") &&
                        (dataMap["ColName"][m] != "Last Year"))
                    {
                        Data += "  " + QString::number(std::stod(dataMap["Value"][m]));
                    }
                    ++m;
                }
                Data += "\n";
            }
        }
        Data += "\n";
    }

    return true;
}

bool
nmfADMB::getSurveyAges(
        const int& NumSpecies,
        const StructSpeciesData& SpeciesData,
        QString& FICfage,
        QString& FIClage)
{
    FICfage.clear();
    FIClage.clear();
    m_logger->logMsg(nmfConstants::Warning,"Warning: Assuming FIC ages are same as all other age ranges (defined in Species tab).");

    for (int i = 0; i < NumSpecies; ++i) {
        for (int j = 0; j < SpeciesData.Surveys[i]; ++j) {
            FICfage += "  " + SpeciesData.MinAge.split(QRegExp("\\s+"),Qt::SkipEmptyParts)[i];
            FIClage += "  " + SpeciesData.MaxAge.split(QRegExp("\\s+"),Qt::SkipEmptyParts)[i];
        }
        FICfage += "\n";
        FIClage += "\n";
    }

    return true;
}

bool
nmfADMB::getNaturalMortalityFirstYearPerSegment(
        const int& NumSegments,
        const StructSpeciesData& SpeciesData,
        QString& Data)
{
   int NumYears = SpeciesData.Years[0]; // Ex. 35 (assume num years is same for all species)
   int inc      = NumYears/NumSegments; // Ex. 17.5 => 17
   Data = "  1";
   for (int i = 0; i < NumSegments-1; ++i) {
       Data += "  " + QString::number((i+1)*inc);
   }

   return true;
}


bool
nmfADMB::getInteraction(
        const int& NumSpecies,
        QString& PredValues,
        QString& PreyValues)
{
    int NumRecords;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;
    std::string msg;
    int TotNumRecordsCalculated = NumSpecies;

    fields     = {"SystemName","PredValue","PreyValue"};
    queryStr   = "SELECT SystemName,PredValue,PreyValue FROM PredatorPreyInteractionsVec";
    queryStr  += " WHERE SystemName = '" + m_projectSettingsConfig + "'";
    dataMap    = m_database->nmfQueryDatabase(queryStr, fields);
    NumRecords = dataMap["SystemName"].size();
    if (NumRecords != TotNumRecordsCalculated) {
        msg  = "nmfADMB::getInteraction: Incorrect number of records found in: PredatorPreyInteractionsVec. ";
        msg += "Expecting " + std::to_string(TotNumRecordsCalculated) + " but found " + std::to_string(NumRecords) + ".";
        m_logger->logMsg(nmfConstants::Error,msg);
        return false;
    }

    for (int i = 0; i < NumRecords; ++i) {
        PredValues += "  " + QString::number(std::stod(dataMap["PredValue"][i]));
        PreyValues += "  " + QString::number(std::stod(dataMap["PreyValue"][i]));
    }

    return true;
}

bool
nmfADMB::getSurveyFirstYearPerSegment(
        const int& NumSpecies,
        const int& NumSegments,
        const StructSpeciesData& SpeciesData,
        QString& FICyrValues)
{
    int NumYears = SpeciesData.Years[0]; // Ex. 35 (assume num years is same for all species)
    int inc      = NumYears/NumSegments; // Ex. 17.5 => 17
    QStringList Nsegs = SpeciesData.Nseg.split(QRegExp("\\s+"),Qt::SkipEmptyParts);

    // Beginning year of FIC segments for species where nseg > 1; if nseg == 1, FICyr = 0
    for (int seg = 0; seg < Nsegs.size(); ++seg) {
        if (seg <= 1) {
            FICyrValues += "  0";
        } else {
            FICyrValues += "  " + QString::number((Nsegs[seg].toInt()-1)*inc);
        }
    }
    FICyrValues += "\n";

    return true;
}

bool
nmfADMB::writeADMBDataFile(const QString& dataFile,
                                 StructSpeciesData& SpeciesData)
{
    bool retv = false;
    int NumSpecies;
    int NumSurveys;
    int BinSize=0;
    int NumSegments=0;
    int Trophic;
    QFile file(dataFile);
    QString msg;
    QString TSwtValues;
    QString SPwtValues;
    QString FICMonthValues;
    QString M2Estimates;
    QString PreferredWeightsEta;
    QString PreferredWeightsSigmaLT;
    QString PreferredWeightsSigmaGT;
    QString TotCValues;
    QString WeightAtAgeValues;
    QString CatchValues;
    QString TotFICValues;
    QString NaturalMortalityValues;
    QString CBRatioValues;
    QString FICValues;
    QString FHValues;
    QString FICfageValues;
    QString FIClageValues;
    QString M1yrValues;
    QString PredValues;
    QString PreyValues;
    QString FICyrValues;
    StructSystemData SystemData;
    std::vector<std::string> SpeciesList;

    m_database->getAllSpecies(m_logger, SpeciesList);
    NumSpecies = SpeciesList.size();

    if (! getSurveyWeights(NumSpecies,NumSurveys,TSwtValues,SPwtValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getSurveyWeights");
        return false;
    }

    if (! getSurveyMonths(NumSpecies,NumSurveys,FICMonthValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getSurveyMonths");
        return false;
    }

    if (! getSpeciesData(NumSpecies,SpeciesData)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getSpeciesData");
        return false;
    }

    if (! getSystemData(SystemData)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getSystemData");
        return false;
    }
    if (! getPredationMortalityEstimates(SpeciesData,M2Estimates)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getPredationMortalityEstimates");
        return false;
    }

    // Ignore predator prey data if running single species
    if (1) { // (m_trophic == 1) {
        if (! getPreferredData(NumSpecies,"PredatorPreyPreferredRatio",PreferredWeightsEta)) {
            m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getPreferredData");
            return false;
        }
        if (! getPreferredData(NumSpecies,"PredatorPreyVarianceGTRatio",PreferredWeightsSigmaLT)) {
            m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getPreferredData");
            return false;
        }
        if (! getPreferredData(NumSpecies,"PredatorPreyVarianceLTRatio",PreferredWeightsSigmaGT)) {
            m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getPreferredData");
            return false;
        }
    } else {
        m_logger->logMsg(nmfConstants::Warning,"nmfADMB::writeADMBDataFile: Running single species. Ignoring multi-species preferred predator-prey data");
    }

    if (! getTotalCatch(NumSpecies,SpeciesData,TotCValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getTotalCatch");
        return false;
    }
    if (! getWeightAtAge(NumSpecies,SpeciesData,WeightAtAgeValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getWeightAtAge");
        return false;
    }
    if (! getCatch(NumSpecies,SpeciesData,CatchValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getCatch");
        return false;
    }
    if (! getTotalSurvey(NumSpecies,SpeciesData,TotFICValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error 12");
        return false;
    }
    if (! getNaturalMortality(NumSpecies,SpeciesData,NumSegments,NaturalMortalityValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getNaturalMortality");
        return false;
    }

    // Ignore Consumption:Biomass ratio if running single species
    if (1) { // (m_trophic == 1) {
        if (! getConsumptionBiomass(NumSpecies,SpeciesData,CBRatioValues)) {
            m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getConsumptionBiomass");
            return false;
        }
    } else {
        m_logger->logMsg(nmfConstants::Warning,"nmfADMB::writeADMBDataFile: Running single species. Ignoring multi-species consumption:biomass ratio data");
    }

    if (! getSurvey(NumSpecies,SpeciesData,FICValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getSurvey");
        return false;
    }

    // Ignore Food Habit data if running single species
    if (1) { // (m_trophic == 1) {
        if (! getFoodHabit(NumSpecies,SpeciesData,BinSize,FHValues)) {
            m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getFoodHabit");
            return false;
        }
    } else {
        m_logger->logMsg(nmfConstants::Warning,"nmfADMB::writeADMBDataFile: Running single species. Ignoring multi-species food habit data");
    }

    if (! getSurveyAges(NumSpecies,SpeciesData,FICfageValues,FIClageValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getSurveyAges");
        return false;
    }
    if (! getNaturalMortalityFirstYearPerSegment(NumSegments,SpeciesData,M1yrValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getNaturalMortalityFirstYearPerSegment");
        return false;
    }

    // Ignore Predator-Prey Interaction data if running single species
    if (1) { // (m_trophic == 1) {
        if (! getInteraction(NumSpecies,PredValues,PreyValues)) {
            m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getInteraction");
            return false;
        }
    } else {
        m_logger->logMsg(nmfConstants::Warning,"nmfADMB::writeADMBDataFile: Running single species. Ignoring predator-prey interaction data");
    }

    if (! getSurveyFirstYearPerSegment(NumSpecies,NumSegments,SpeciesData,FICyrValues)) {
        m_logger->logMsg(nmfConstants::Error,"nmfADMB::writeADMBDataFile: Error getFICBeginYears");
        return false;
    }

    // Override system data if user has set m_Trophic by kicking off a Single Species run
    Trophic = (m_trophic == 0) ? m_trophic : (SystemData.NumSpInter.toInt() > 0);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);

        QDateTime now  = QDateTime::currentDateTime();
        QString   time = now.toString("yyyy-MM-dd hh:mm:ss");

        fileStream << "# Data file produced by NOAA's National Marine Fisheries Service MSCAA Tool: " << time << "\n";
        fileStream << "# Modify parameterization to allow for multiple phase declarations for each parameter\n\n";
        fileStream << "# Debug Level\n";
        fileStream << "#  " << m_debug << "\n\n"; // RSK - this doesn't work when uncommented
        fileStream << "# Simulation\n";
        fileStream << "  1\n\n";
        fileStream << "# Trophic\n";
        fileStream << "  " << Trophic << "\n\n";
        fileStream << "# nsp\n";
        fileStream << "  " << NumSpecies << "\n\n";
        fileStream << "# nFIC\n";
        fileStream << "  " << NumSurveys << "\n\n";
        fileStream << "# o (Tiny number for calculation of lognormal distributions\n";
        fileStream << SystemData.LogNorm << "\n\n";
        fileStream << "# p (Tiny number for calculation of multinomial residuals)\n";
        fileStream << SystemData.MultiResid << "\n\n";
        fileStream << "# Nint  (Number of Species interactions)\n";
        fileStream << SystemData.NumSpInter << "\n\n";
        fileStream << "# Binsize  (This is Food Habit (FH) bin size)\n";
        fileStream << "  " << BinSize << "\n\n";
        fileStream << "# EcoB\n";
        fileStream << SystemData.TotalBiomass << "\n\n";
        fileStream << "# Owt (Weight for the Other Food penalty term)\n";
        fileStream << SystemData.Owt << "\n\n";
        fileStream << "# Fyr\n";
        fileStream << SpeciesData.FirstYear << "\n\n";
        fileStream << "# Lyr\n";
        fileStream << SpeciesData.LastYear << "\n\n";
        fileStream << "# FHfyr\n";
        fileStream << SystemData.FH_FirstYear << "\n\n";
        fileStream << "# FHlyr\n";
        fileStream << SystemData.FH_LastYear << "\n\n";
        fileStream << "# Nage\n";
        fileStream << SpeciesData.MaxAge << "\n\n";
        fileStream << "# Mnseg\n";
        fileStream << "  " << NumSegments << "\n\n";
        fileStream << "# Nseg (Number of segments for the FIC data; for each segment, a separate q is estimated\n";
        fileStream << SpeciesData.Nseg << "\n\n";
        fileStream << "# agePR\n";
        fileStream << SpeciesData.FirstAgePrtRcrt << "\n\n";
        fileStream << "# ageFR\n";
        fileStream << SpeciesData.AgeFullRcrt << "\n\n";
        fileStream << "# ficFR\n";
        fileStream << SpeciesData.AgeFullRcrtSurvey << "\n\n";
        fileStream << "# FICs_lage\n";
        fileStream << SpeciesData.SurveySel << "\n\n";
        fileStream << "# aAge1ph (aAge1 estimation phase)\n";
        fileStream << SpeciesData.aAge1ph << "\n\n";
        fileStream << "# aFtph (aFt estimation phase)\n";
        fileStream << SpeciesData.aFtph << "\n\n";
        fileStream << "# dAge1ph (dAge1 estimation phase)\n";
        fileStream << SpeciesData.dAge1ph << "\n\n";
        fileStream << "# dFtph (dFt estimation phase)\n";
        fileStream << SpeciesData.dFtph << "\n\n";
        fileStream << "# ficph (Survey selectivity, FICsel, estimation phase)\n";
        fileStream << SpeciesData.ficph << "\n\n";
        fileStream << "# fishph (Fishery selectivity, agesel, estimation phase)\n";
        fileStream << SpeciesData.fishph << "\n\n";
        fileStream << "# Yr1ph (Yr1 estimation phase)\n";
        fileStream << SpeciesData.Yr1ph << "\n\n";
        fileStream << "# Pred (interactions)\n";
        fileStream << PredValues << "\n\n";
        fileStream << "# Prey (interactions)\n";
        fileStream << PreyValues << "\n\n";
        fileStream << "# Rhoph (Phase in which each rho is estimated)\n";
        fileStream << SpeciesData.Rhoph << "\n\n";
        fileStream << "# M1yr (The first year in each M1 segment, including the 1st segment)\n";
        fileStream << M1yrValues << "\n\n";
        fileStream << "# TCwt (Total annual commercial catch in weight)\n";
        fileStream << SpeciesData.TCwt << "\n\n";
        fileStream << "# CPwt (Commercial catch proportions at age)\n";
        fileStream << SpeciesData.CPwt << "\n\n";
        fileStream << "# Bwt (Weight for Biomass Penalty Term, Bpen)\n";
        fileStream << SpeciesData.Bwt << "\n\n";
        fileStream << "# Ywt (Weight for Yr1 Penalty Term, Ypen))\n";
        fileStream << SpeciesData.Ywt << "\n\n";
        fileStream << "# Rwt (Weight for Recruitment Penalty Term, Rpen)\n";
        fileStream << SpeciesData.Rwt << "\n\n";
        fileStream << "# FHwt (Food habits proportions by weight)\n";
        fileStream << SpeciesData.FHwt << "\n\n";
        fileStream << "# Bthres (Biomass threshold used in the penalty function to avoid B == 0, which would cause M2 calc to crash)\n";
        fileStream << SpeciesData.Bthres << "\n\n";
        fileStream << "# Rthres (Threshold for the coefficient of variation of recruitment)\n";
        fileStream << SpeciesData.Rthres << "\n\n";
        fileStream << "# TSwt\n";
        fileStream << TSwtValues << "\n\n";
        fileStream << "# SPwt\n";
        fileStream << SPwtValues << "\n\n";
        fileStream << "# iM2 (M2 estimates (from previous model run) to use in non-FH years)\n";
        fileStream << M2Estimates << "\n\n";
        fileStream << "# Eta\n";
        fileStream << PreferredWeightsEta;
        fileStream << "# Sigma1\n";
        fileStream << PreferredWeightsSigmaLT;
        fileStream << "# Sigma2\n";
        fileStream << PreferredWeightsSigmaGT;
        fileStream << "# fic_fage\n";
        fileStream << FICfageValues;
        fileStream << "# fic_lage\n";
        fileStream << FIClageValues;
        fileStream << "# FICmonth\n";
        fileStream << FICMonthValues << "\n";
        fileStream << "# FICyr (Beginning year of FIC segments for species where nseg > 1; if nseg == 1, FICyr = 0)\n";
        fileStream << FICyrValues << "\n";
        fileStream << "# TotC.tmt (Total Fishery Catch in metric tons)\n";
        fileStream << TotCValues << "\n";
        fileStream << WeightAtAgeValues << "\n";
        fileStream << CatchValues << "\n";
        fileStream << TotFICValues << "\n";
        fileStream << NaturalMortalityValues;
        fileStream << CBRatioValues;
        fileStream << FICValues;
        fileStream << FHValues;
        fileStream << "# Eof\n";
        fileStream << "  " << 54321 << "\n\n";
        file.close();

        msg = "<strong>Wrote data file: </strong>" + dataFile;
        appendSummaryTextBox(msg);
        retv = true;
    }

    return retv;
}

bool
nmfADMB::writeADMBParameterFile(const QString& parameterFile)
{
    bool retv = false;
    QFile file(parameterFile);
    QString msg;

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);

        fileStream << "# Number of parameters = 251 Objective function value = 27801.7435990657  Maximum gradient component = 0.000746478605425415\n";
        fileStream << "# iRho[1]:\n";
        fileStream << "2.99573200000\n";
        fileStream << "# iRho[2]:\n";
        fileStream << "2.30258500001\n";
        fileStream << "# iRho[3]:\n";
        fileStream << "2.99573200001\n";
        fileStream << "# aAge1[1]:\n";
        fileStream << "4.37839158846\n";
        fileStream << "# aAge1[2]:\n";
        fileStream << "4.85707852034\n";
        fileStream << "# aAge1[3]:\n";
        fileStream << "5.10321639498\n";
        fileStream << "# aFt[1]:\n";
        fileStream << "-1.51879298686\n";
        fileStream << "# aFt[2]:\n";
        fileStream << "-1.40750973787\n";
        fileStream << "# aFt[3]:\n";
        fileStream << "-1.74486717993\n";
        fileStream << "# idAge1[1]:\n";
        fileStream << " 0.141525502485 0.112661402496 0.00140198850679 -0.135239797450 -0.239796097446 -0.264565497473 -0.274148097432 -0.299476197426 -0.346241997383 -0.404590597340 -0.183795097411 -0.00743357144604 0.0872547025291 0.128014002513 0.141278402520 0.141174902545 0.124454202534 0.0913486225562 0.0470174425778 0.0540125225775 0.0672368625718 0.0349463725779 -0.000259736211646 -0.0301089174040 0.0122358025813 0.0498803325655 0.0800641225570 0.102627502564 0.136369602548 0.142781902535 0.135145402521 0.124628302522 0.116576302517 0.113019402517\n";
        fileStream << "# idAge1[2]:\n";
        fileStream << " -0.0554865024007 -0.281295602481 -0.515213302561 -0.587403102606 -0.534935402619 -0.402081802606 -0.259510102563 -0.0532658324884 0.0492545275593 0.0769443175687 0.0935991775608 0.0563325675391 0.00835312052275 0.0283451275319 0.0493509875343 0.0781077975448 0.141739397573 0.235732397596 0.255153997618 0.189270897606 0.113839497580 0.135098297611 0.160356297600 0.198526297637 0.153102197661 0.120092197641 0.0881007076313 0.0808707176295 0.0441167576141 0.0414179576084 0.0498882676002 0.0679509275886 0.0833255675820 0.0903216475839\n";
        fileStream << "# idAge1[3]:\n";
        fileStream << " 0.592296069201 0.856427370523 0.939030770925 0.763949570017 0.340289667892 -0.230195834693 -1.01634003783 -1.35270903895 -1.25811103865 -0.865324137239 -0.397536335409 0.0200292663956 0.314623767739 0.445123068369 0.389269368120 0.154147867004 -0.188758634541 -0.497841435807 -0.639324336366 -0.579644636124 -0.381716635329 -0.136900934283 0.281789967664 0.538722968902 0.638343769400 0.515301168793 0.196858667264 -0.0854487340387 -0.261472334813 -0.180883934467 0.0795026066879 0.238474567422 0.375840168077 0.392187368153\n";
        fileStream << "# idFt[1]:\n";
        fileStream << " 0.825645788695 0.825645788710 0.825645788707 0.825645788710 0.825645788713 0.825645788718 0.825645788725 0.825645788736 0.825645788747 -0.783792011361 -0.783792011356 -0.783792011354 -0.783792011352 -0.783792011350 -0.783792011350 -0.783792011349 -0.783792011350 -0.783792011351 -0.0906449116070 -0.0906449116055 -0.783792011351 -0.783792011352 -0.783792011352 -0.0906449116137 -0.0906449116133 -0.0906449116134 -0.0906449116138 0.314820188426 0.314820188426 0.314820188425 0.314820188425 0.314820188428 0.314820188431 0.314820188435 0.314820188438\n";
        fileStream << "# idFt[2]:\n";
        fileStream << " -0.895075402849 -0.895075402859 -0.895075402822 -0.895075402790 -0.895075402787 -0.895075402785 -0.201928302851 -0.201928302855 -0.201928302860 0.203536897106 0.203536897104 0.203536897104 0.491218897152 0.491218897151 0.491218897150 0.714362497165 0.714362497165 0.714362497169 0.203536897106 0.203536897108 0.714362497169 0.714362497166 0.714362497169 0.203536897108 0.203536897113 0.203536897112 0.203536897100 -0.201928302871 -0.201928302871 -0.201928302872 -0.201928302872 -0.201928302871 -0.201928302871 -0.201928302870 -0.201928302869\n";
        fileStream << "# idFt[3]:\n";
        fileStream << " 1.05172004604 1.05172004606 1.05172004610 1.05172004614 0.828576746899 -0.557717552420 -0.557717552411 -0.557717552401 -0.557717552395 -0.557717552393 0.135429648166 0.135429648170 0.135429648168 0.135429648169 -0.557717552379 -0.557717552379 -0.557717552379 -0.557717552379 -0.557717552379\n";
        fileStream << "# iFICsel[1]:\n";
        fileStream << " 0.250000000000 0.350000000000 0.550000000000 0.750000000000\n";
        fileStream << "# iFICsel[2]:\n";
        fileStream << " 0.250000000000 0.450000000000 0.650000000000 0.850000000000\n";
        fileStream << "# iFICsel[3]:\n";
        fileStream << " 0.330000000000 0.750000000000 0.870000000000 0.930000000000\n";
        fileStream << "# iagesel[1]:\n";
        fileStream << " 0.00703358700005 0.161108900001 0.838891000000\n";
        fileStream << "# iagesel[2]:\n";
        fileStream << " 0.000335350100036 0.0179862100001 0.500000000000 0.982013800000\n";
        fileStream << "# iagesel[3]:\n";
        fileStream << " 0.000123394600033 0.00247262300004 0.0474258700001 0.500000000000 0.952574100000\n";
        fileStream << "# iYr1[1]:\n";
        fileStream << " 4.31748800000 4.02535200000 3.71357200000 3.40119700000 3.13549400000 2.83321300000 2.48490700000 2.19722500000 1.94591000000 1.60943800000\n";
        fileStream << "# iYr1[2]:\n";
        fileStream << " 5.10594500000 4.80402100000 4.51086000000 4.20469300000 3.91202300000 3.61091800000\n";
        fileStream << "# iYr1[3]:\n";
        fileStream << " 5.29831700000 4.99721200000 4.70048000000 4.39444900000 4.09434500000 3.80666200000 3.49650800000 3.17805400000 2.89037200000 2.56494900000 2.30258500000\n";

        fileStream << "\n";
        file.close();

        msg = "<strong>Wrote parameter file: </strong>" + parameterFile;
        appendSummaryTextBox(msg);
        retv = true;
    }

    return retv;
}

bool
nmfADMB::writeADMBPinFile(const QString& pinFile)
{
    if (! m_origPinFile.empty()) {
        nmfUtils::copyFile(m_origPinFile,pinFile.toStdString());
        QString msg = "<strong>Copied pin file: </strong>" + pinFile;
        appendSummaryTextBox(msg);
    } else {
        return false;
    }
    return true;
}

bool
nmfADMB::writeADMBTPLFile(const QString& tplFile)
{
    if (! m_origTplFile.empty()) {
        nmfUtils::copyFile(m_origTplFile,tplFile.toStdString());
        QString msg = "<strong>Copied tpl file: </strong>" + tplFile;
        appendSummaryTextBox(msg);
    } else {
        return false;
    }
    return true;
}

bool
nmfADMB::writeADMBCxxFiles(const QString& tplFile)
{
    QStringList filesToCopy;
    QString msg;

    if (! m_origTplFile.empty()) {
        QFileInfo target(QString::fromStdString(m_origTplFile));
        QFileInfo dest(tplFile);

        // Get .cxx files
        nmfUtils::getFilesWithExt(target.absolutePath().toStdString(),
                                  ".cxx",filesToCopy);

        std::string destDir = dest.absolutePath().toStdString();
        std::string destFile;
        for (QString filename : filesToCopy) {
            QFileInfo file(filename);
            destFile = destDir + "/" + file.fileName().toStdString();
            nmfUtils::copyFile(filename.toStdString(),destFile);
        }

        msg = "<strong>Copying .cxx files to: </strong>" + QString::fromStdString(destDir);
        appendSummaryTextBox(msg);
    } else {
        return false;
    }
    return true;
}

QString
nmfADMB::parseReportFile(std::map<std::string,boost::numeric::ublas::matrix<double> >& Abundance)
{
    QString errorMsg = "";
    StructSpeciesData SpeciesData;

    if (m_speciesData.MinAge == -1) {
        errorMsg = "nmfADMB::parseReportFile: Species Data (MinAge) not properly defined";
        return errorMsg;
    }
    std::vector<std::string> SpeciesList;
    m_database->getAllSpecies(m_logger, SpeciesList);
    int NumSpecies = SpeciesList.size();

    if (! getSpeciesData(NumSpecies,SpeciesData)) {
        errorMsg = "nmfADMB::parseReportFile: Problem reading Species data";
        m_logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return errorMsg;
    }

    // Read .rep file
    if (! readADMBReportFile(m_reportFile,SpeciesData,Abundance)) {
        errorMsg = "nmfADMB::parseReportFile: Problem with readADMBReportFile";
        m_logger->logMsg(nmfConstants::Error,errorMsg.toStdString());
        return errorMsg;
    }

    return errorMsg;
}

void
nmfADMB::execCmd(std::string path, std::string cmd)
{

#ifdef __linux__

    // Find the full path location of the admb command and use that in the system call.
    // If not, when running from a file browser GUI app on linux (i.e. Nemo), the admb
    // executable may not be found.
    std::string result="";
    FILE* file = popen("which admb", "r");
    char buffer[100];
    fscanf(file, "%100s", buffer);
    pclose(file);
    result = buffer;
    result = QString::fromStdString(result).trimmed().toStdString();
//std::cout << "result: >" << result << "< " << std::endl;
//std::cout << "cmd: >" << cmd << "< " << std::endl;
    if (QString::fromStdString(cmd).contains("admb ") && (!result.empty())) {
        // Replace "admb" with the full path of the admb executable
        QString cmdStr = QString::fromStdString(cmd);
        QString subStr("admb ");
        QString newStr = QString::fromStdString(result) + " ";
        cmd = cmdStr.replace(subStr,newStr).toStdString();
    }
    m_logger->logMsg(nmfConstants::Normal,"cmd: "+cmd);    
//  popen(cmd.c_str(),"r");
    int retv = std::system(cmd.c_str());
//  int retv = QProcess::execute("admb",argList);
    m_logger->logMsg(nmfConstants::Normal, "cmd completed: retv = "+std::to_string(retv));
//  m_logger->logMsg(nmfConstants::Normal, "cmd completed");

#elif _WIN32

    /*
    STARTUPINFO startInfo;
    PROCESS_INFORMATION procInfo;

    ZeroMemory(&startInfo,sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);
    ZeroMemory(&procInfo,sizeof(procInfo));

    TCHAR* target = new TCHAR[cmd.size()+1];
    target[cmd.size()] = 0;
    std::copy(cmd.begin(),cmd.end(),target);

    if (! CreateProcess(
                nullptr,
                target,
                nullptr,
                nullptr,
                false,
                0,
                nullptr,
                nullptr,
                &startInfo,
                &procInfo))
    {
        std::string msg = "Error on CreateProcess for: " + cmd;
        m_logger->logMsg(nmfConstants::Error,msg);
    }
    WaitForSingleObject(procInfo.hProcess,INFINITE);

    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);
*/

    // Run multiple dos commands
    cmd = "pushd " + path + " & " + cmd + " & popd";
    QString cmdStr = QString::fromStdString(cmd);
    cmd = cmdStr.replace("/","\\\\").toStdString();
    m_logger->logMsg(nmfConstants::Normal, "Run cmd: "+cmd);
    m_logger->logMsg(nmfConstants::Normal, "Run Output");

    FILE *file;
    char line[100];
    std::string msg;
//  cmd += " 2> nul";
    file = _popen(cmd.c_str(),"rt");
    while (fgets(line,100,file)) {
        msg = " | " + std::string(line);
        m_logger->logMsg(nmfConstants::Normal,
                         QString::fromStdString(msg).trimmed().toStdString());
    }

#endif
}


bool
nmfADMB::buildADMB(const QString& tplFile,
                   const QString& buildOutputFile)
{
    m_logger->logMsg(nmfConstants::Normal,"nmfADMB::buildADMB");

    std::string cmd;
    QString msg;
    QElapsedTimer admbTimer;

    // Start timer to time ADMB Build
    admbTimer.start();

    // Change directory to the tpl directory.  Necessary for admb to run properly on Windows.
    QFileInfo target(tplFile);
    QFileInfo dest(buildOutputFile);
    QString targetPath = target.absolutePath();
    QString targetName = target.fileName();
    QString destPath   = dest.absolutePath();
    QString destName   = dest.fileName();

    if (targetPath != destPath) {
        m_logger->logMsg(nmfConstants::Error,"Error: tpl dir not the same as build output dir");
        return false;
    }

    // 1. Build .cpp file
    QString fullTargetName = QDir(targetPath).filePath(targetName);
    QString fullDestName   = QDir(targetPath).filePath(destName);
    cmd = "admb \"" + fullTargetName.toStdString() + "\" > \"" + fullDestName.toStdString() + "\"";
    execCmd(targetPath.toStdString(),cmd);
    msg = "<strong><br>Build command:</strong><br><br>" + QString::fromStdString(cmd);
    appendSummaryTextBox(msg);

    // 2. Check that .cpp file was built correctly
    QString text;
    QFile file(buildOutputFile);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream in(&file);
    text = in.readAll().trimmed().replace("\n","<br>");
    file.close();
    msg = "<strong><br>Build output:</strong><br><br>" + text + "<br>";
    appendSummaryTextBox(msg);
    if (! text.contains("Successfully built")) {
        m_logger->logMsg(nmfConstants::Error,"Error building admb executable.");
        return false;
    }

    // Print Build time
    msg = getTimerString(admbTimer,"Build time:");
    appendSummaryTextBox(msg);

    return true;
}


bool
nmfADMB::runADMB(const QString& tplFile,
                 const QString& runOutputFile)
{
    std::string cmd;
    QFileInfo fileInfo(tplFile);
    QString fileBase = fileInfo.baseName();
    QString filePath = fileInfo.absolutePath();
    QString msg;
    QElapsedTimer admbTimer;

    // Start timer to time ADMB Run
    admbTimer.start();

    QString fullRunOutputFile = "\"" + QDir(filePath).filePath(runOutputFile) + "\"";
    cmd = "\"" + QDir(filePath).filePath(fileBase).toStdString() + "\"";
    if (m_debug != 0) {
        cmd += "  > " + fullRunOutputFile.toStdString() +
               " 2> " + fullRunOutputFile.toStdString(); // This last part redirects stderr as well
    }

    execCmd(filePath.toStdString(),cmd);
    msg = "<strong>Run command:</strong><br><br>" + QString::fromStdString(cmd);
    appendSummaryTextBox(msg);

    // Read output file into string and print
    std::string output;
    std::string line;
    std::ifstream fptr(runOutputFile.toStdString());
    if (fptr) {
        while (std::getline(fptr,line)) {
            output += line+"<br>";
        }
        fptr.close();
    }
    msg = "<strong><br>Debug output:</strong><br><br>" + QString::fromStdString(output);
    appendSummaryTextBox(msg);

    // Print Run time
    msg = getTimerString(admbTimer,"Run time:");
    appendSummaryTextBox(msg);

    return true;
}


QString
nmfADMB::getTimerString(QElapsedTimer& admbTimer,
                        const QString& header)
{
    double timerSec  = admbTimer.elapsed()/1000.0;
    double timerMin  = timerSec/60.0;
    double timerHour = timerMin/60.0;
    QString msg = "<br><strong>" + header + "</strong><br><br>";

    if (timerHour >= 1.0) {
        msg += QString::number(timerHour,'f',1) + " hour(s)<br>";
    } else if (timerMin >= 1.0) {
        msg += QString::number(timerMin,'f',1)  + " minute(s)<br>";
    } else {
        msg += QString::number(timerSec,'f',1)  + " second(s)<br>";
    }

    return msg;
}

bool
nmfADMB::readADMBReportFile(
        const QString& reportFile,
        const StructSpeciesData& speciesData,
        std::map<std::string,boost::numeric::ublas::matrix<double> >& Abundance)
{
    bool retv = false;
    QFile file(reportFile);
    QString line;
    QStringList slist;
    boost::numeric::ublas::matrix<double> matrix;
    int NumSpecies; // = speciesData.Years.size();
    int year=0;
    int speciesCount = -1;
    int NumAges  = -1;
    int NumYears = -1;
    std::vector<std::string> SpeciesVec;

    m_database->getAllSpecies(m_logger, SpeciesVec);
    NumSpecies = SpeciesVec.size();

    // initialize m_Abundance
    for (int i = 0; i < NumSpecies; ++i) {
        nmfUtils::initialize(matrix,speciesData.Years[i],speciesData.Ages[i]);
        Abundance[SpeciesVec[i]] = matrix;
    }

    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
          return retv;
    QTextStream in(&file);
    bool readingAbundance = false;
    std::string msg;
    while (in.readLineInto(&line)) {
        if (line.contains("N.")) {
            readingAbundance = true;
            ++speciesCount;
            year = 0;
            NumYears = speciesData.Years[speciesCount];
            NumAges  = speciesData.Ages[speciesCount];
            continue;
        }
        if (readingAbundance && (year < NumYears)) {
            slist = line.trimmed().split(" ");
            if (slist.size() != NumAges) {
                msg = "nmfMSCAA_Tab5::readADMBReportFile: Expecting " + std::to_string(NumAges) +
                      " age groups, found: " + std::to_string(slist.size());
                m_logger->logMsg(nmfConstants::Error,msg);
                return false;
            }
            for (int age = 0; age < NumAges; ++age) {
                Abundance[SpeciesVec[speciesCount]](year,age) = slist[age].toDouble();
            }
            ++year;
            retv = true;
        }
    }

    std::cout << "Read Summary:" << std::endl;
    std::cout << "  Read " << Abundance.size() << " abundance matrices" << std::endl;

    for (auto const& item : Abundance) {
        std::cout << item.first << ": "
                  << "  Size: " << item.second.size1()
                  << " x " << item.second.size2() << std::endl;

    }

    return retv;
}
