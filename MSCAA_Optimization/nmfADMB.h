/** @file nmfADMB.h
 * @brief Function prototypes for the ADMB module.
 *
 * This contains the prototypes for the ADMB functionality.  ADMB
 * is a 3rd party API for parameter estimation.
 *
 */

#pragma once

#include "nmfDatabase.h"
#include "nmfConstants.h"
#include "nmfConstantsMSCAA.h"
#include "nmfUtils.h"
#include "nmfUtilsQt.h"
#include <unistd.h>

#include <boost/filesystem.hpp>

#include <QObject>
#include <QTextEdit>


/**
 * @brief ADMB file names
 *
 * This struct contains the file names that ADMB requires for input and output.
 *
 */
struct StructADMBFilenames {
    QString dataFile;      /**< Input data file containing initial values (.dat) */
    QString tplFile;       /**< Input file containing ADMB template code from which ADMB creates C++ and executable code (.tpl) */
    QString pinFile;       /**< Input file containing initial values for initial model parameters (.pin)*/
    QString buildOutput;   /**< Output file containing the terminal output from the ADMB Build (.out) */
    QString parameterFile; /**< Output file containing parameter estimates in ASCII format (.par) */
    QString reportFile;    /**< Output file containing user-defined estimated parameters and derived data in user-friendly format (.rep) */
    QString runOutput;     /**< Output file containing the terminal output from the ADMB Run (.out) */
};

/**
 * @brief Species data
 *
 * This struct contains Species data concatenated into strings. Each element in this
 * struct is a particular Species parameter that's been combined into a single string
 * for all of the Species.
 *
 */
struct StructSpeciesData {
    QString MinAge;            /**< Minimum Age Group */
    QString MaxAge;            /**< Maximum Age Group */
    QString FirstAgePrtRcrt;   /**< First age when partially recruited to the fishery; used to avoid hitting parameter bounds */
    QString AgeFullRcrt;       /**< Age of full recruitment to the fishery */
    QString AgeFullRcrtSurvey; /**< Age of full recruitment to the fishery-independent survey, FIC */
    QString SurveySel;         /**< Survey selectivity coefficient for the last age class; used to anchor the curve */
    QString FirstYear;         /**< First Year of Data */
    QString LastYear;          /**< Last Year of Data */
    QString NumSurveys;        /**< Number of Surveys */
    QString NumFleets;         /**< Number of Fishing Fleets */
    QString Nseg;              /**< Number of segments for the FIC data; for each segment, a separate q is estimated */
    QString aAge1ph;           /**< aAge1 estimation phase */
    QString aFtph;             /**< aFt estimation phase */
    QString dAge1ph;           /**< dAge1 estimation phase */
    QString dFtph;             /**< dFt estimation phase */
    QString ficph;             /**< Survey selectivity, FICsel, estimation phase */
    QString fishph;            /**< Fishery selectivity, agesel, estimation phase */
    QString Yr1ph;             /**< Yr1 estimation phase */
    QString Rhoph;             /**< Phase in which each rho is estimated */
    QString TCwt;              /**< Total annual commercial catch in weight */
    QString CPwt;              /**< Commercial catch proportions at age */
    QString Bwt;               /**< Weight for Biomass Penalty Term, Bpen */
    QString Ywt;               /**< Weight for Yr1 Penalty Term, Ypen) */
    QString Rwt;               /**< Weight for Recruitment Penalty Term, Rpen */
    QString FHwt;              /**< Food habits proportions by weight */
    QString Bthres;            /**< Biomass threshold used in the penalty function to avoid B == 0, which would cause M2 calc to crash */
    QString Rthres;            /**< Threshold for the coefficient of variation of recruitment */
    QString MinLength;         /**< Minimum Species length */
    QString MaxLength;         /**< Maximum Species length */
    QString NumLengthBins;     /**< Number of Length Bins */
    QString CatchAtAge;        /**< Catch-at-Age flag (0 = Use Length-At-Age data, 1 = Use Catch-At-Age data) */
    std::vector<int> Fleets;   /**< Vector of Number of Fleets for each Species */
    std::vector<int> Surveys;  /**< Vector of Number of Surveys for each Species */
    std::vector<int> Ages;     /**< Vector of Age Group range for each Species */
    std::vector<int> Years;    /**< Vector of Year range for each Species */
};

/**
 * @brief System data
 *
 * This struct contains the System-specific data, i.e., data applying to all Species
 */
struct StructSystemData {
    QString TotalBiomass; /**< Total System Biomass formatted with units ex. 20000_Metric Tons */
    QString FH_FirstYear; /**< First Year of Food Habit data */
    QString FH_LastYear;  /**< Last Year of Food Habit data */
    QString NumSpInter;   /**< Number of Species Interactions */
    QString Owt;          /**< Other Food Weight */
    QString LogNorm;      /**< Tiny number for calculation of lognormal distributions */
    QString MultiResid;   /**< Tiny number for calculation of multinomial residuals */
};



/**
 * @brief The nmfADMB interface class
 */
class nmfADMB
{
private:
    QString           m_buildOutput;
    nmfDatabase*      m_database;
    QString           m_dataFile;
    int               m_debug;
    nmfLogger*        m_logger;
    std::string       m_origPinFile;
    std::string       m_origTplFile;
    QTextEdit*        m_outputTE;
    QString           m_parameterFile;
    QString           m_pinFile;
    std::string       m_projectDir;
    std::string       m_projectName;
    std::string       m_projectSettingsConfig;
    QString           m_reportFile;
    QString           m_runOutput;
    StructSpeciesData m_speciesData;
    QString           m_tplFile;
    int               m_trophic;
    QString           m_type;

    void clear(StructSpeciesData& SpeciesData);

    bool getSurveyWeights(
            const int& NumSpecies,
            int&       NumSurveys,
            QString&   TSwtValues,
            QString&   SPwtValues);
    bool getSurveyMonths(
            const int& NumSpecies,
            const int& NumSurveys,
            QString&   FICMonthValues);
    bool getSpeciesData(
            const int& NumSpecies,
            StructSpeciesData& SpeciesData);
    bool getSystemData(
            StructSystemData& SystemData);
    bool getPredationMortalityEstimates(const StructSpeciesData& SpeciesData,
                        QString& M2Estimates);
    bool getPreferredData(
            const int& NumSpecies,
            QString TableName,
            QString& PreferredWeightsEta);
    bool getTotalCatch(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& Data);
    bool getWeightAtAge(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& Data);
    bool getCatch(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& Data);
    bool getTotalSurvey(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& Data);
    bool getNaturalMortality(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            int& NumSegments,
            QString& Data);
    bool getConsumptionBiomass(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& Data);
    bool getSurvey(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& Data);
    bool getFoodHabit(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            int& BinSize,
            QString& Data);
    bool getSurveyAges(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& FICfage,
            QString& FIClage);
    bool getNaturalMortalityFirstYearPerSegment(
            const int& NumSegments,
            const StructSpeciesData& SpeciesData,
            QString& Data);
    bool getInteraction(
            const int& NumSpecies,
            const StructSpeciesData& SpeciesData,
            QString& PredValues,
            QString& PreyValues);
    bool getSurveyFirstYearPerSegment(
            const int& NumSpecies,
            const int& NumSegments,
            const StructSpeciesData& SpeciesData,
            QString& FICyrValues);
    bool writeADMBDataFile(const QString& dataFile,
                           StructSpeciesData& SpeciesData);
    bool writeADMBParameterFile(const QString& parameterFile);
    bool writeADMBPinFile(const QString& pinFile);
    bool writeADMBTPLFile(const QString& tplFile);
    bool writeADMBCxxFiles(const QString& tplFile);

    /** @brief Creates the C++ code that will eventually be
     *         built and run for the parameter estimation.
     *
     * @param tplFile The name of the tpl file that ADMB converts to C++.
     * @return boolean signifying if the method completed without error.
     */
    bool buildADMB(const QString& tplFile,
                   const QString& buildOutputFile);


    QString getTimerString(QTime& ADMBTimer,
                           const QString& header);
    /**
     * @brief Runs the ADMB executable. Creates multiple output files.
     *
     * One of the files produced is a .rep report file. This file
     * is an ASCII listing of the estimated parameters and values
     * produced with those estimated parameters as defined by the
     * user in the tpl file.
     *
     * @param tplFile - tpl data file with ADMB converts to C++
     * @param runOutputFile - output log file from the ADMB run
     * @return True/False signifying success or failure
     */
    bool runADMB(const QString& tplFile,
                 const QString& runOutputFile);

    /**
     * @brief readADMBReportFile
     * @param reportFile - Name of the report file that ADMB produces
     * @param speciesData - Structure holding Species-specific data
     * @param Abundance - Matrix structure holding Abundance data for each Species
     * @return True/False on a Success or Failure report file read
     */
    bool readADMBReportFile(
            const QString& reportFile,
            const StructSpeciesData& speciesData,
            std::map<std::string,boost::numeric::ublas::matrix<double> >& Abundance);

    /**
     * @brief Construct the file names ADMB needs for input and output
     */
    void setFilenames();

    /**
     * @brief Clears the messages from the summary text box
     */
    void clearSummaryTextBox();

    /**
     * @brief Appends a message to the summary text box
     * @param The message to write to the summary text box
     */
    void appendSummaryTextBox(QString& msg);

public:
    /**
     * @brief nmfADMB constructor
     * @param database : database pointer
     * @param logger : logger pointer
     * @param projectDir : project directory
     * @param projectName : project name
     * @param projectSettingsConfig : project systems config file name
     * @param outputTE : output text window
     * @param type : model type ("SingleSpecies" or "MultiSpecies")
     * @param clear : clears the summary text window prior to running
     */
    nmfADMB(nmfDatabase*   database,
            nmfLogger*     logger,
            std::string&   projectDir,
            std::string&   projectName,
            std::string&   projectSettingsConfig,
            QTextEdit*     outputTE,
            const QString& type,
            bool           clear);
   ~nmfADMB() {}

    /**
     * @brief Calls the private ADMB build command
     * @return Boolean signifying success or failure
     */
    bool build();

    /**
     * @brief Creates/copies the necessary ADMB input files (.dat, .par, .pin, .tpl, and *.cxx)
     * @return True if files created successfully, else False
     */
    bool createInputFiles();

    /**
     * @brief Gets the directory in which ADMB stores all of its files (input, output, and executable)
     * @return The directory in which the ADMB files are located
     */
    QString getADMBDir();

    /**
     * @brief Reads the .rep report file and extracts the Abundance data.
     * @param The Abundance data structure passed by reference
     * @return The Abundance data structure populated with data from the .rep report file
     */
    QString parseReportFile(std::map<std::string,boost::numeric::ublas::matrix<double> >& Abundance);

    /**
     * @brief Calls the private ADMB run command
     * @return True/False signifying success or failure
     */
    bool run();

    /**
     * @brief Sets ADMB to run a Single Species or Multi Species model
     * @param trophic : If 0, a Single Species model is run. If 1, a Multi Species model is run.
     */
    void setTrophic(const int& trophic);

    /**
     * @brief Sets ADMB to the given Verbose Debug level.
     * @param debug : If 0, no debug statements are printed. If other than 0, then the appropriate statement are printed for the particular code.
     */
    void setDebug(const int& debug);

    /**
     * @brief Sets the name of the initial parameter file needed by ADMB. This is a user supplied file.
     * @param pinFile : name of the pin file
     */
    void setPinFile(std::string pinFile);

    /**
     * @brief Sets the name of the template file needed by ADMB. This is a user supplied file.
     * @param tplFile : name of the tpl file
     */
    void setTplFile(std::string tplFile);

};

