
#include "nmfSetupTab03.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"
#include "nmfConstants.h"

nmfSetup_Tab3::nmfSetup_Tab3(QTabWidget  *tabs,
                             nmfLogger   *theLogger,
                             nmfDatabase *theDatabasePtr,
                             QTextEdit   *theSetupOutputTE,
                             std::string &theProjectDir)
{
    QUiLoader loader;

    Setup_Tabs    = tabs;
    m_databasePtr = theDatabasePtr;
    m_logger      = theLogger;
    m_ProjectDir    = theProjectDir;
    m_smodelSpecies = NULL;
    m_smodelOtherPredSpecies = NULL;
    m_colLabelsSpecies.clear();

    readSettings();

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab03.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab3_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab3_Widget->setObjectName("Setup_Tab3_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab3_Widget, tr("3. Species Setup"));

    Setup_Tab3_SpeciesTW       = Setup_Tabs->findChild<QTableWidget *>("Setup_Tab3_SpeciesTW");
    Setup_Tab3_NumSpeciesSB    = Setup_Tabs->findChild<QSpinBox     *>("Setup_Tab3_NumSpeciesSB");
    Setup_Tab3_AddSpeciesPB    = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_AddSpeciesPB");
    Setup_Tab3_DelSpeciesPB    = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_DelSpeciesPB");
    Setup_Tab3_LoadPB          = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_LoadPB");
    Setup_Tab3_SavePB          = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_SavePB");
    Setup_Tab3_PrevPB          = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_PrevPB");
    Setup_Tab3_NextPB          = Setup_Tabs->findChild<QPushButton  *>("Setup_Tab3_NextPB");

    connect(Setup_Tab3_NumSpeciesSB,    SIGNAL(valueChanged(int)),
            this,                       SLOT(callback_Setup_Tab3_NumSpeciesCMB(int)));
    connect(Setup_Tab3_AddSpeciesPB,    SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_AddSpeciesPB()));
    connect(Setup_Tab3_DelSpeciesPB,    SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_DelSpeciesPB()));
    connect(Setup_Tab3_LoadPB,          SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_LoadPB()));
    connect(Setup_Tab3_SavePB,          SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_SavePB()));
    connect(Setup_Tab3_PrevPB,          SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_PrevPB()));
    connect(Setup_Tab3_NextPB,          SIGNAL(clicked()),
            this,                       SLOT(callback_Setup_Tab3_NextPB()));
    connect(Setup_Tab3_SpeciesTW,       SIGNAL(cellClicked(int,int)),
            this,                       SLOT(callback_Setup_Tab3_SpeciesTableChanged(int,int)));

    Setup_Tab3_PrevPB->setText("\u25C1--");
    Setup_Tab3_SavePB->setEnabled(true);
    Setup_Tab3_SpeciesTW->setSelectionMode(QAbstractItemView::ContiguousSelection);

    m_colLabelsSpecies << "Name" << "Min Age" << "Max Age" <<
                          "First Age Partial Rec" << "Age Full Rec" <<
                          "Age Full Rec Survey" << "Survey Sel Last Age" <<
                          "First Year" << "Last Year" << "Num Surveys" <<
                          "Num Fleets" << "Num FIC Segs" << "Min Length" << "Max Length" <<
                          "Num Length Bins" << "Catch-At-Age";

}


nmfSetup_Tab3::~nmfSetup_Tab3()
{
}

QTableWidget*
nmfSetup_Tab3::getTableSpecies()
{
    return Setup_Tab3_SpeciesTW;
}

void
nmfSetup_Tab3::callback_Setup_Tab3_LoadPB()
{
    loadSpecies();

    QMessageBox::information(Setup_Tabs,"Reload",
                             "\nSpecies reloaded.\n",
                             QMessageBox::Ok);
}


void
nmfSetup_Tab3::callback_Setup_Tab3_DelSpeciesPB()
{
    std::string cmd;
    std::string errorMsg;
    QString SpeciesName;
    QString msg;
    QMessageBox::StandardButton reply;
    QList<std::string> TablesToDeleteFrom1 = { // Delete with SpeName field
                                               nmfConstantsMSCAA::TableAgeLengthKey,
                                               nmfConstantsMSCAA::TableCatchAtLengthFishery,
                                               nmfConstantsMSCAA::TableCatchFishery,
                                               nmfConstantsMSCAA::TableCatchFisheryProportion,
                                               nmfConstantsMSCAA::TableCatchFisheryTotal,
                                               nmfConstantsMSCAA::TableCatchSurvey,
                                               nmfConstantsMSCAA::TableCatchSurveyProportion,
                                               nmfConstantsMSCAA::TableCatchSurveyTotal,
                                               nmfConstantsMSCAA::TableConsumption,
                                               nmfConstantsMSCAA::TableCovariates,
                                               nmfConstantsMSCAA::TableFleets,
                                               nmfConstantsMSCAA::TableInitialAbundance,
                                               nmfConstantsMSCAA::TableMaturity,
                                               nmfConstantsMSCAA::TableMortalityFishing,
                                               nmfConstantsMSCAA::TableMortalityNatural,
                                               nmfConstantsMSCAA::TableSimulationParametersSpecies,
                                               nmfConstantsMSCAA::TableSimulationParametersYearly,
                                               nmfConstantsMSCAA::TableSpecies,
                                               nmfConstantsMSCAA::TableSurveyMonth,
                                               nmfConstantsMSCAA::TableWeight};
    QList<std::string> TablesToDeleteFrom2 = { // Delete with PredatorName,ColName fields
                                               nmfConstantsMSCAA::TableDiet};
    QList<std::string> TablesToDeleteFrom3 = { // Delete with PredatorName,PreyName fields
                                               nmfConstantsMSCAA::TablePredatorPreyInteractions,
                                               nmfConstantsMSCAA::TablePredatorPreyPreferredRatio,
                                               nmfConstantsMSCAA::TablePredatorPreyVarianceGTRatio,
                                               nmfConstantsMSCAA::TablePredatorPreyVarianceLTRatio,
                                               nmfConstantsMSCAA::TablePredatorPreyVulnerability};

    // Check to make sure user really wants to delete the selected Species and associated data.
    msg = "\nDeleting Species will cause all data associated with the selected";
    msg += "\nSpecies to be deleted. This cannot be undone.\n\nOK to proceed?";
    reply = QMessageBox::question(Setup_Tabs, tr("Deleting Species"), tr(msg.toLatin1()),
                                  QMessageBox::No|QMessageBox::Yes,
                                  QMessageBox::No);
    if (reply == QMessageBox::No) {
       return;
    }

    // Delete row(s) from table.  If the Species has been saved, this should happen automatically when
    // the Species are reloaded.  However, if the user is just entering the data and hasn't yet
    // saved and wants to delete a row, this statement is necessary.
    QList<QTableWidgetItem*> selItems = Setup_Tab3_SpeciesTW->selectedItems();
    int numSelItems = selItems.size();
    if (numSelItems == 0) {
        return;
    }
    int firstSelRow = selItems[0]->row();
    int lastSelRow  = selItems[numSelItems-1]->row();
    if (firstSelRow > lastSelRow) {
        std::swap(firstSelRow,lastSelRow);
    }
    int firstRow = firstSelRow;
    int numRowsToDelete = lastSelRow - firstSelRow + 1;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int i=0; i<numRowsToDelete; ++i) {
        removeFromTable("SpeName",      Setup_Tab3_SpeciesTW->item(firstRow,0),TablesToDeleteFrom1); // Remove from database table
        removeFromTable("PredatorName", Setup_Tab3_SpeciesTW->item(firstRow,0),TablesToDeleteFrom2);
        removeFromTable("ColName",      Setup_Tab3_SpeciesTW->item(firstRow,0),TablesToDeleteFrom2);
        removeFromTable("PredatorName", Setup_Tab3_SpeciesTW->item(firstRow,0),TablesToDeleteFrom3);
        removeFromTable("PreyName",     Setup_Tab3_SpeciesTW->item(firstRow,0),TablesToDeleteFrom3);
        Setup_Tab3_SpeciesTW->removeRow(firstRow);// RSK - Remove from widget (Refactor this with a model!!)
    }
    QApplication::restoreOverrideCursor();

    emit LoadedSpecies();

    // Enable spin box if there are 0 rows left
    Setup_Tab3_NumSpeciesSB->setEnabled(Setup_Tab3_SpeciesTW->rowCount() == 0);

} // end callback_Setup_Tab3_DelSpeciesPB

void
nmfSetup_Tab3::removeFromTable(
        std::string field,
        QTableWidgetItem *itemToRemove,
        QList<std::string>& TablesToDeleteFrom)
{
    std::string cmd;
    std::string errorMsg;
    QString SpeciesName;

    if (itemToRemove != NULL) {
        SpeciesName = itemToRemove->text();
        if (! SpeciesName.isEmpty()) {
            for (std::string tableName : TablesToDeleteFrom)
            {
                cmd = "DELETE FROM " + tableName +" WHERE " + field +
                      " = '" + SpeciesName.toStdString() + "'";
                errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
                if (nmfUtilsQt::isAnError(errorMsg)) {
                    m_logger->logMsg(nmfConstants::Error,"nmfSetupTab4 removeFromTable: Delete table error: " + errorMsg);
                    m_logger->logMsg(nmfConstants::Error,"cmd: " + cmd);
                    return;
                }
            }
        }
    }
}

void
nmfSetup_Tab3::callback_Setup_Tab3_AddSpeciesPB()
{
    int numRows = Setup_Tab3_SpeciesTW->rowCount();
    int numCols = getNumColumnsSpecies();

    if (numRows == 0) {
        Setup_Tab3_SpeciesTW->setColumnCount(numCols);
        Setup_Tab3_SpeciesTW->setHorizontalHeaderLabels(m_colLabelsSpecies);
        Setup_Tab3_SpeciesTW->resizeColumnsToContents();
    }

    Setup_Tab3_SpeciesTW->insertRow(numRows);
    populateARowSpecies(numRows,getNumColumnsSpecies());

} // end callback_Setup_Tab3_AddSpeciesPB


void
nmfSetup_Tab3::callback_Setup_Tab3_NextPB()
{
    int nextPage = Setup_Tabs->currentIndex()+1;
    Setup_Tabs->setCurrentIndex(nextPage);
}

void
nmfSetup_Tab3::callback_Setup_Tab3_PrevPB()
{
    int prevPage = Setup_Tabs->currentIndex()-1;
    Setup_Tabs->setCurrentIndex(prevPage);
}

void
nmfSetup_Tab3::saveFleetData()
{

}

void
nmfSetup_Tab3::callback_Setup_Tab3_SavePB()
{
    int NumSpecies = Setup_Tab3_SpeciesTW->rowCount();
    std::string msg;
    std::string errorMsg;
    std::string saveCmd;
    QStringList ColLabels = {"SpeName","MinAge","MaxAge","FirstAgePrtRcrt",
                             "AgeFullRcrt","AgeFullRcrtSurvey","SurveySel",
                             "FirstYear","LastYear","NumSurveys","NumFleets","Nseg",
                             "MinLength","MaxLength","NumLengthBins","CatchAtAge"};
    std::string quote = "";

    // Check each cell for missing fields
    int numCols;
    std::vector<std::string> Tables = {nmfConstantsMSCAA::TableSpecies};
    std::vector<int> NumSpeciesOrGuilds= {NumSpecies};
    int tableNum = -1;
    for (QTableWidget *tw : {Setup_Tab3_SpeciesTW}) {
        ++tableNum;
        numCols = tw->columnCount();
        for (int i=0; i<NumSpeciesOrGuilds[tableNum]; ++i) {
            for (int j=0; j<numCols; ++j) {
//              if ((Tables[tableNum] == "Guilds") || (Tables[tableNum] == nmfConstantsMSCAA::TableSpecies) )
                if (Tables[tableNum] == nmfConstantsMSCAA::TableSpecies)
                {
                    msg.clear();
                    if (tw->item(i,j)->text().isEmpty()) {
                        msg = "Missing field(s) in " + Tables[tableNum] + " table";
                    } else if ((tw->horizontalHeaderItem(j)->text() == "Num Surveys") &&
                               (tw->item(i,j)->text().toInt() < 1)) {
                        msg = "Illegal value in Num Surveys column. Values there must be at least 1.";
                    }
                    if (! msg.empty()) {
                        m_logger->logMsg(nmfConstants::Error,msg);
                        QMessageBox::warning(Setup_Tabs,"Error",
                                             QString::fromStdString("\n"+msg+"\n"),
                                             QMessageBox::Ok);
                        return;
                    }
                }
            }
        }
    }

    // RSK check and make sure year ranges are the same for all species
    int Col_FirstYear = 7;
    int Col_LastYear  = 8;
    int FirstYear = Setup_Tab3_SpeciesTW->item(0,Col_FirstYear)->text().toInt();
    int LastYear  = Setup_Tab3_SpeciesTW->item(0,Col_LastYear)->text().toInt();
    for (int i=1; i<NumSpecies; ++i) {
        if ((FirstYear != Setup_Tab3_SpeciesTW->item(i,Col_FirstYear)->text().toInt()) ||
            (LastYear  != Setup_Tab3_SpeciesTW->item(i,Col_LastYear)->text().toInt()))
        {
            msg = "Please assure that Species have same First and Last Year values.";
            m_logger->logMsg(nmfConstants::Error,msg);
            QMessageBox::warning(Setup_Tabs,"Error",
                                 QString::fromStdString("\n"+msg+"\n"),
                                 QMessageBox::Ok);
            return;
        }
    }

    for (int i=0; i<NumSpeciesOrGuilds[tableNum]; ++i) {
        saveCmd += "REPLACE INTO " + nmfConstantsMSCAA::TableSpecies +
                   " SET SpeName ='" +
                   Setup_Tab3_SpeciesTW->item(i,0)->text().toStdString() + "', ";
        for (int col = 1; col < ColLabels.size(); ++col) {
            saveCmd += ColLabels[col].toStdString() + " = " + quote +
                       Setup_Tab3_SpeciesTW->item(i, col)->text().toStdString() +
                       quote + ",";
        }
        saveCmd = saveCmd.substr(0,saveCmd.size()-1) + "; ";
    }
    errorMsg = m_databasePtr->nmfUpdateDatabase(saveCmd);
    if (nmfUtilsQt::isAnError(errorMsg)) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetupTab4 callback_Setup_Tab3_SavePB (Species): Write table error: " + errorMsg);
        m_logger->logMsg(nmfConstants::Error,"saveCmd: " + saveCmd);
        QMessageBox::warning(Setup_Tabs,"Warning",
                             "\nCouldn't UPDATE Species table.\n",
                             QMessageBox::Ok);
        return;
    }

    loadSpecies();

    emit LoadedSpecies();

    saveFleetData();

    QMessageBox::information(Setup_Tabs, "Save",
                             tr("\nSpecies saved.\n"));

} // end callback_Setup_Tab3_SavePB


void
nmfSetup_Tab3::populateARowSpecies(int row, int ncols)
{
    QTableWidgetItem *item;

    for (int j=0;j<ncols; ++j) {
        switch (j) {
            default:
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                Setup_Tab3_SpeciesTW->setItem(row,j,item);
                break;
            }
    }
    Setup_Tab3_SpeciesTW->resizeColumnsToContents();

} // end PopulateARowSpecies


void
nmfSetup_Tab3::callback_Setup_Tab3_NumSpeciesCMB(int numSpecies)
{
    int ncols;
    int nrows;

    Setup_Tab3_SpeciesTW->clear();

    ncols = m_colLabelsSpecies.size();
    nrows = numSpecies;

    Setup_Tab3_SpeciesTW->setRowCount(nrows);
    Setup_Tab3_SpeciesTW->setColumnCount(ncols);

    // Put widgets items in each cell
    for (int i=0; i<nrows; ++i)  {
        populateARowSpecies(i,ncols);
    }
    Setup_Tab3_SpeciesTW->setHorizontalHeaderLabels(m_colLabelsSpecies);
    Setup_Tab3_SpeciesTW->resizeColumnsToContents();

} // end callback_Setup_Tab3_NumSpecies


void
nmfSetup_Tab3::readSettings() {
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");

    settings->beginGroup("Settings");
    m_ProjectSettingsConfig = settings->value("Name","").toString().toStdString();
    settings->endGroup();
    settings->beginGroup("SetupTab");
    m_ProjectDir = settings->value("ProjectDir","").toString().toStdString();
    settings->endGroup();
    delete settings;

} // end readSettings

void
nmfSetup_Tab3::saveSettings()
{

} // end saveSettings


/*
 * Load up all of the Guilds and Species widgets with data from the database.
 */
void
nmfSetup_Tab3::
loadSpecies()
{
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    clearWidgets();

    // Read database and load Species
    fields = {"SpeName","MinAge","MaxAge","FirstAgePrtRcrt",
              "AgeFullRcrt","AgeFullRcrtSurvey","SurveySel",
              "FirstYear","LastYear","NumSurveys","NumFleets","Nseg",
              "MinLength","MaxLength","NumLengthBins","CatchAtAge"};
    queryStr   = "SELECT SpeName,MinAge,MaxAge,FirstAgePrtRcrt,";
    queryStr  += "AgeFullRcrt,AgeFullRcrtSurvey,SurveySel,";
    queryStr  += "FirstYear,LastYear,NumSurveys,NumFleets,Nseg,";
    queryStr  += "MinLength,MaxLength,NumLengthBins,CatchAtAge FROM " + nmfConstantsMSCAA::TableSpecies;
    dataMap    = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();
    if (NumSpecies == 0) {
        m_logger->logMsg(nmfConstants::Error,"nmfSetup_Tab3::LoadSpecies: No Species Found");
        return;
    } else {
        m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab3::LoadSpecies: Num Species found: "+std::to_string(NumSpecies));
    }

    callback_Setup_Tab3_NumSpeciesCMB(NumSpecies);

    // Load up all of the Species widgets with data from the database.
    if (NumSpecies > 0) {

        Setup_Tab3_NumSpeciesSB->setValue(NumSpecies);
        Setup_Tab3_NumSpeciesSB->setEnabled(false);

        for (int i=0; i<NumSpecies; ++i) {

            // Populate text fields
            Setup_Tab3_SpeciesTW->item(i, 0)->setText(QString::fromStdString(dataMap["SpeName"][i]));
            Setup_Tab3_SpeciesTW->item(i, 1)->setText(QString::fromStdString(dataMap["MinAge"][i]));
            Setup_Tab3_SpeciesTW->item(i, 2)->setText(QString::fromStdString(dataMap["MaxAge"][i]));
            Setup_Tab3_SpeciesTW->item(i, 3)->setText(QString::fromStdString(dataMap["FirstAgePrtRcrt"][i]));
            Setup_Tab3_SpeciesTW->item(i, 4)->setText(QString::fromStdString(dataMap["AgeFullRcrt"][i]));
            Setup_Tab3_SpeciesTW->item(i, 5)->setText(QString::fromStdString(dataMap["AgeFullRcrtSurvey"][i]));
            Setup_Tab3_SpeciesTW->item(i, 6)->setText(QString::fromStdString(dataMap["SurveySel"][i]));
            Setup_Tab3_SpeciesTW->item(i, 7)->setText(QString::fromStdString(dataMap["FirstYear"][i]));
            Setup_Tab3_SpeciesTW->item(i, 8)->setText(QString::fromStdString(dataMap["LastYear"][i]));
            Setup_Tab3_SpeciesTW->item(i, 9)->setText(QString::fromStdString(dataMap["NumSurveys"][i]));
            Setup_Tab3_SpeciesTW->item(i,10)->setText(QString::fromStdString(dataMap["NumFleets"][i]));
            Setup_Tab3_SpeciesTW->item(i,11)->setText(QString::fromStdString(dataMap["Nseg"][i]));
            Setup_Tab3_SpeciesTW->item(i,12)->setText(QString::fromStdString(dataMap["MinLength"][i]));
            Setup_Tab3_SpeciesTW->item(i,13)->setText(QString::fromStdString(dataMap["MaxLength"][i]));
            Setup_Tab3_SpeciesTW->item(i,14)->setText(QString::fromStdString(dataMap["NumLengthBins"][i]));
            Setup_Tab3_SpeciesTW->item(i,15)->setText(QString::fromStdString(dataMap["CatchAtAge"][i]));

        } // end for i
    } // end if

    Setup_Tab3_SpeciesTW->resizeColumnsToContents();
    Setup_Tab3_NumSpeciesSB->setEnabled(NumSpecies == 0);
    Setup_Tab3_NumSpeciesSB->setValue(NumSpecies);

    Setup_Tab3_SpeciesTW->update();
    Setup_Tab3_SpeciesTW->repaint();

    emit LoadedSpecies();

} // end LoadSpecies


void
nmfSetup_Tab3::loadWidgets()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab3::loadWidgets()");

    readSettings();

    loadSpecies();

} // end loadWidgets


void
nmfSetup_Tab3::clearWidgets()
{
    // Clear current Species first in case the database has no Species yet
    // (need to clear any existing data in the GUI)
    Setup_Tab3_SpeciesTW->setRowCount(0);
    Setup_Tab3_SpeciesTW->setColumnCount(0);
}

void
nmfSetup_Tab3::callback_Setup_Tab3_SpeciesCB(bool state)
{
    int NumSpecies;
    std::vector<std::string> fields;
    std::map<std::string, std::vector<std::string> > dataMap;
    std::string queryStr;

    fields = {"SpeName"};
    queryStr = "SELECT SpeName FROM " + nmfConstantsMSCAA::TableSpecies;
    dataMap  = m_databasePtr->nmfQueryDatabase(queryStr, fields);
    NumSpecies = dataMap["SpeName"].size();

    Setup_Tab3_NumSpeciesLBL->setEnabled(state);

    if (state) {
        if (NumSpecies > 0)
            Setup_Tab3_NumSpeciesSB->setEnabled(false);
        else
            Setup_Tab3_NumSpeciesSB->setEnabled(true);
    } else {
        Setup_Tab3_NumSpeciesSB->setEnabled(false);
    }
    Setup_Tab3_AddSpeciesPB->setEnabled(state);
    Setup_Tab3_DelSpeciesPB->setEnabled(state);
    Setup_Tab3_LoadPB->setEnabled(state);
    Setup_Tab3_SpeciesTW->setEnabled(state);

} // end callback_Setup_Tab3_SpeciesCB


int
nmfSetup_Tab3::getNumColumnsSpecies()
{
    return m_colLabelsSpecies.size();
}


void
nmfSetup_Tab3::callback_Setup_Tab3_SpeciesTableChanged(int row, int col)
{
    Setup_Tab3_NumSpeciesSB->setEnabled(false);
}


