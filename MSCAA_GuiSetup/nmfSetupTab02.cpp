
#include "nmfSetupTab02.h"
#include "nmfConstants.h"
#include "nmfUtilsQt.h"
#include "nmfUtils.h"


nmfSetup_Tab2::nmfSetup_Tab2(QTabWidget  *tabs,
                             nmfLogger   *theLogger,
                             nmfDatabase *theDatabasePtr,
                             QTextEdit   *theSetupOutputTE)
{
    QUiLoader loader;

    Setup_Tabs    = tabs;
    m_logger      = theLogger;
    m_databasePtr = theDatabasePtr;

    // Load ui as a widget from disk
    QFile file(":/forms/Setup/Setup_Tab02.ui");
    file.open(QFile::ReadOnly);
    Setup_Tab2_Widget = loader.load(&file,Setup_Tabs);
    Setup_Tab2_Widget->setObjectName("Setup_Tab2_Widget");
    file.close();

    // Add the loaded widget as the new tabbed page
    Setup_Tabs->addTab(Setup_Tab2_Widget, tr("2. Project Setup"));

    //SetupOutputTE = theSetupOutputTE;

//  LastProjectDatabase.clear();
    m_ProjectDir.clear();
    m_ProjectDatabase.clear();
    m_ProjectName.clear();
    m_ProjectAuthor.clear();
    m_ProjectDescription.clear();
    m_ValidDatabases  = {};
//  NewProject = false;

    Setup_Tab2_ProjectNameLE      = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectNameLE");
    Setup_Tab2_ProjectDirBrowsePB = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_BrowseProjectDirPB");
    Setup_Tab2_ProjectDirLE       = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectDirLE");
    Setup_Tab2_ProjectDatabaseCMB = Setup_Tabs->findChild<QComboBox   *>("Setup_Tab2_ProjectDatabaseCMB");
    Setup_Tab2_ProjectAuthorLE    = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectAuthorLE");
    Setup_Tab2_ProjectDescLE      = Setup_Tabs->findChild<QLineEdit   *>("Setup_Tab2_ProjectDescriptionLE");
    Setup_Tab2_AddDatabasePB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_AddDatabasePB");
    Setup_Tab2_DelDatabasePB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_DelDatabasePB");
    Setup_Tab2_RefreshDatabasePB  = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_RefreshDatabasePB");
    Setup_Tab2_SaveProjectPB      = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_SaveProjectPB");
    Setup_Tab2_NewProjectPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_NewProjectPB");
    Setup_Tab2_BrowseProjectPB    = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_BrowseProjectPB");
    Setup_Tab2_DelProjectPB       = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_DelProjectPB");
    Setup_Tab2_ReloadProjectPB    = Setup_Tabs->findChild<QPushButton *>("Setup_Tab2_ReloadProjectPB");
    Setup_Tab2_ProjectDataGB      = Setup_Tabs->findChild<QGroupBox   *>("Setup_Tab2_ProjectDataGB");

    Setup_Tab2_ProjectNameLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectDirLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectAuthorLE->setClearButtonEnabled(true);
    Setup_Tab2_ProjectDescLE->setClearButtonEnabled(true);

    connect(Setup_Tab2_ProjectDirBrowsePB, SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_ProjectDirBrowse()));
    connect(Setup_Tab2_DelDatabasePB,      SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_DelDatabase()));
    connect(Setup_Tab2_SaveProjectPB,      SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_SaveProject()));
    connect(Setup_Tab2_RefreshDatabasePB,  SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_RefreshDatabase()));
    connect(Setup_Tab2_NewProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_NewProject()));
    connect(Setup_Tab2_DelProjectPB,       SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_DelProject()));
    connect(Setup_Tab2_BrowseProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_BrowseProject()));
    connect(Setup_Tab2_ReloadProjectPB,    SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_ReloadProject()));

    connect(Setup_Tab2_ProjectNameLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_Setup_Tab2_ProjectNameAdd()));
    connect(Setup_Tab2_ProjectAuthorLE,    SIGNAL(editingFinished()),
            this,                          SLOT(callback_Setup_Tab2_ProjectAuthorAdd()));
    connect(Setup_Tab2_ProjectDescLE,      SIGNAL(editingFinished()),
            this,                          SLOT(callback_Setup_Tab2_ProjectDescAdd()));

    connect(Setup_Tab2_ProjectDirLE,       SIGNAL(returnPressed()),
            this,                          SLOT(callback_Setup_Tab2_ProjectDirAdd()));
    connect(Setup_Tab2_AddDatabasePB,      SIGNAL(clicked()),
            this,                          SLOT(callback_Setup_Tab2_AddDatabase()));
    connect(Setup_Tab2_ProjectDatabaseCMB, SIGNAL(currentTextChanged(QString)),
            this,                          SLOT(callback_Setup_Tab2_DatabaseChanged(QString)));

    readSettings();

    loadDatabaseNames("");

}

nmfSetup_Tab2::~nmfSetup_Tab2()
{
    saveSettings();
}

void
nmfSetup_Tab2::enableSetupTabs(bool enable)
{
    for (int i=2; i<Setup_Tabs->count(); ++i) {
        Setup_Tabs->setTabEnabled(i,enable);
    }
}

bool
nmfSetup_Tab2::isProjectDataValid()
{
    return (! (getProjectName().isEmpty()                             ||
               Setup_Tab2_ProjectDirLE->text().isEmpty()              ||
               Setup_Tab2_ProjectDatabaseCMB->currentText().isEmpty() ||
               Setup_Tab2_ProjectAuthorLE->text().isEmpty()           ||
               Setup_Tab2_ProjectDescLE->text().isEmpty()));

} // end validProjectData

void
nmfSetup_Tab2::clearProject()
{
    setProjectAuthor("");
    setProjectDescription("");
    setProjectDirectory("");
    setProjectName("");
}

void
nmfSetup_Tab2::enableProjectData()
{
    Setup_Tab2_ProjectDataGB->setChecked(true);
}

void
nmfSetup_Tab2::setProjectAuthor(QString author)
{
    Setup_Tab2_ProjectAuthorLE->setText(author);
}

void
nmfSetup_Tab2::setProjectDescription(QString description)
{
    Setup_Tab2_ProjectDescLE->setText(description);
}

void
nmfSetup_Tab2::setProjectDirectory(QString directory)
{
    Setup_Tab2_ProjectDirLE->setText(directory);

}

void
nmfSetup_Tab2::setProjectName(QString name)
{
    Setup_Tab2_ProjectNameLE->setText(name);

}

void
nmfSetup_Tab2::callback_Setup_Tab2_SaveProject()
{
    QString fullFilename;
    std::string msg;
    QMessageBox::StandardButton reply;
    QString fileSuffix;
    std::ofstream outFile;

    if (! isProjectDataValid()) {
        QMessageBox::warning(Setup_Tabs,
                             tr("Missing Data"),
                             tr("\nPlease enter missing data above."),
                             QMessageBox::Ok);
        return;
    }

    m_ProjectName     = getProjectName();
    m_ProjectDatabase = getProjectDatabase();

    if (! isValidProjectName(m_ProjectName)) {
        return;
    }

    Setup_Tabs->setCursor(Qt::WaitCursor);

    // Build the filename, add the .prj prefix if one isn't already there and
    // check to see if the file already exists.
    fullFilename = QDir(m_ProjectDir).filePath(m_ProjectName);
    fileSuffix =  QFileInfo(fullFilename).suffix();
    if (fileSuffix != "prj")
        fullFilename += ".prj";
    if (QFileInfo(fullFilename).exists()) {
        msg  = "\nThe project file already exists:\n\n" + fullFilename.toStdString() + "\n\n";
        msg += "OK to overwrite it?\n";
        reply = QMessageBox::question(Setup_Tabs,
                                      tr("File Found"),
                                      tr(msg.c_str()),
                                      QMessageBox::Yes|QMessageBox::No,
                                      QMessageBox::Yes);
        if (reply == QMessageBox::No) {
            Setup_Tabs->setCursor(Qt::ArrowCursor);
            return;
        }
    }

    // Write out the project file
    outFile = std::ofstream(fullFilename.toLatin1());
    outFile << "#\n";
    outFile << "# This is an MSCAA Project file.\n";
    outFile << "#\n";
    outFile << "# It defines the Project database as well as other\n";
    outFile << "# information for the Project. The following data are:\n";
    outFile << "# Project directory, Project database, Author, and Description.\n";
    outFile << "#\n";
    outFile << m_ProjectDir.toStdString() << "\n";
    outFile << m_ProjectDatabase.toStdString() << "\n";
    outFile << m_ProjectAuthor.toStdString() << "\n";
    outFile << m_ProjectDescription.toStdString() << "\n";
    outFile.close();

//    LastProjectDatabase = ProjectDatabase;

    saveSettings();

    emit SavedProject();

    // Load the project
    QString fileName = QDir(m_ProjectDir).filePath(m_ProjectName+".prj");
    loadProject(m_logger,fileName);

    readSettings();

    emit ReloadWidgets();

    Setup_Tabs->setCursor(Qt::ArrowCursor);

} // end callback_Setup_Tab2_SaveProject

void
nmfSetup_Tab2::callback_progressDlgCancel()
{
    disconnect(progressDlg, SIGNAL(canceled()), this, SLOT(callback_progressDlgCancel()));
}


bool
nmfSetup_Tab2::isValidProjectName(QString projectName)
{
    std::string msg;
    bool isValid = false;
    bool isAtLeastOneChar = (! projectName.isEmpty());

    // Use regex to validate name
    QRegularExpression re("^[a-zA-Z0-9_]*$");
    QRegularExpressionMatch match = re.match(m_ProjectName);
    isValid = match.hasMatch();

    if ((! isAtLeastOneChar) || (! isValid)) {
        msg = "Invalid project name: " + projectName.toStdString();
        m_logger->logMsg(nmfConstants::Error,msg);
        msg += "\n\nA project name must contain at least one character and consist";
        msg += "\nonly of letters, numbers, and underscore characters.";
        QMessageBox::warning(Setup_Tabs,"Invalid Project Name",
                             QString::fromStdString("\n"+msg+"\n"),
                             QMessageBox::Ok);
    }

    return (isAtLeastOneChar && isValid);
}



void
nmfSetup_Tab2::callback_Setup_Tab2_AddDatabase()
{
    bool ok;
    std::string msg;
    std::string cmd;
    std::string errorMsg="";

    QString enteredName = QInputDialog::getText(Setup_Tabs,
                                         tr("Add Database"),
                                         tr("Enter new database name:"),
                                         QLineEdit::Normal,
                                         "", &ok);
    if (! ok || enteredName.isEmpty())
        return;

    if (m_ValidDatabases.find(enteredName.toStdString()) != m_ValidDatabases.end()) // Means it found it
    {
        msg = "\nDatabase already exists. Please enter another database name.";
        QMessageBox::critical(Setup_Tabs,tr("Invalid Database Name"),tr(msg.c_str()),QMessageBox::Ok);
        return;
    }

    Setup_Tabs->setCursor(Qt::WaitCursor);

    // OK to now add the database and create the necessary table definitions
    cmd = "CREATE database " + enteredName.toStdString();
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("Function: callback_Setup_Tab2_AddDatabase ",errorMsg);
    } else {
        createTables(enteredName);
        loadDatabaseNames(enteredName);
        Setup_Tab2_ProjectDatabaseCMB->setCurrentText(enteredName);
        std::string msg = "\nDatabase created: " + enteredName.toStdString() + "\n";
        QMessageBox::information(Setup_Tabs, "Set up Project",
                                 tr(msg.c_str()));
    }

    saveSettings();

    Setup_Tabs->setCursor(Qt::ArrowCursor);

} // end callback_Setup_Tab2_AddDatabase


void
nmfSetup_Tab2::callback_Setup_Tab2_DelDatabase()
{
    bool deleteOK=true;
    std::string cmd;
    std::string msg;
    std::string errorMsg="";
    QString databaseToDelete = Setup_Tab2_ProjectDatabaseCMB->currentText();
    int databaseIndexToDelete = Setup_Tab2_ProjectDatabaseCMB->currentIndex();

    //std::cout << "Database delete not fully implemented." << std::endl;
    QMessageBox::StandardButton reply;

    msg = "\nOK to permanently delete the database: " +
            databaseToDelete.toStdString() + " ?\n";
    reply = QMessageBox::question(Setup_Tabs, tr("Delete Database"),
         tr(msg.c_str()), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {

        QApplication::setOverrideCursor(Qt::WaitCursor);

        // Remove database from mysql and reload widget
        cmd = "DROP database " + databaseToDelete.toStdString();
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            Setup_Tabs->setCursor(Qt::ArrowCursor);
            nmfUtils::printError("Function: callback_Setup_Tab2_DelDatabase ",errorMsg);
            deleteOK = false;
        }
        loadDatabaseNames("");
        Setup_Tab2_ProjectDatabaseCMB->setCurrentIndex(databaseIndexToDelete-1);

        QApplication::restoreOverrideCursor();

        if (deleteOK) {
            msg = "\nDatabase deleted: " + databaseToDelete.toStdString() + "\n";
            QMessageBox::information(Setup_Tabs, "Delete",
                                     tr(msg.c_str()));
        }

    }

} // end callback_Setup_Tab2_DelDatabase


void
nmfSetup_Tab2::callback_Setup_Tab2_RefreshDatabase()
{
    loadDatabaseNames(Setup_Tab2_ProjectDatabaseCMB->currentText());
    QMessageBox::information(Setup_Tabs, "Refresh", "\nDatabase list refreshed.\n", QMessageBox::Ok);
}

void
nmfSetup_Tab2::callback_Setup_Tab2_DatabaseChanged(QString db)
{
//    LastProjectDatabase = ProjectDatabase;
    m_ProjectDatabase = db; //Setup_Tab2_ProjectDatabaseCMB->currentText();
} // end callback_Setup_Tab2_DatabaseChanged



void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectDirBrowse()
{
    // Launch file dialog
    QString dir = QFileDialog::getExistingDirectory(
                Setup_Tabs,
                tr("Open Project Directory"),
                m_ProjectDir+"/..",
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (! dir.isEmpty()) {
        Setup_Tab2_ProjectDirLE->setText(dir);
        m_ProjectDir = dir;
    }
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectDirAdd()
{
    m_ProjectDir = Setup_Tab2_ProjectDirLE->text();
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectNameAdd()
{
    m_ProjectName = getProjectName();
//    NewProject = true;
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectAuthorAdd()
{
    m_ProjectAuthor = Setup_Tab2_ProjectAuthorLE->text();
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ProjectDescAdd()
{
    m_ProjectDescription = Setup_Tab2_ProjectDescLE->text();
}

void
nmfSetup_Tab2::loadProject(nmfLogger *logger, QString fileName)
{    
    std::string msg;

    if (fileName.isEmpty() || (fileName == ".")) {
        QMessageBox::critical(Setup_Tabs,tr("Project Error"),
                              tr("\nPlease create a Project.\n"),
                              QMessageBox::Ok);
        return;
    }

    // If the file doesn't have an extension, add .prj
    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName += ".prj";
    }

    // Read file and load data......
    QFile file(fileName);
    if (! file.open(QIODevice::ReadOnly)) {
        msg = "Error nmfSetupTab2::loadProject: Cannot open file for reading: "+fileName.toStdString();
        logger->logMsg(nmfConstants::Normal,msg);
        msg = "\nError nmfSetupTab2::loadProject: Cannot open file for reading:\n\n"+fileName.toStdString();
        QMessageBox::critical(Setup_Tabs,tr("Project Error"),
                              tr(msg.c_str()),
                              QMessageBox::Ok);
        return;
    }
    QTextStream in(&file);
    logger->logMsg(nmfConstants::Normal,"Reading Project File: " + fileName.toStdString());

    // Skip initial comments
    QString line = in.readLine();
    while (line.trimmed()[0] == "#") {
        line = in.readLine();
    }

    m_ProjectDir         = line;
    m_ProjectDatabase    = in.readLine();
    m_ProjectAuthor      = in.readLine();
    m_ProjectDescription = in.readLine();

    Setup_Tab2_ProjectNameLE->clear();
    Setup_Tab2_ProjectDirLE->clear();
    //Setup_Tab2_ProjectDatabaseCMB->clear();
    Setup_Tab2_ProjectAuthorLE->clear();
    Setup_Tab2_ProjectDescLE->clear();

    QFileInfo filenameNoPath(fileName);
    Setup_Tab2_ProjectNameLE->setText(filenameNoPath.baseName());
    Setup_Tab2_ProjectDirLE->setText(m_ProjectDir);
    Setup_Tab2_ProjectDatabaseCMB->setCurrentText(m_ProjectDatabase);
    Setup_Tab2_ProjectAuthorLE->setText(m_ProjectAuthor);
    Setup_Tab2_ProjectDescLE->setText(m_ProjectDescription);
    m_ProjectName = filenameNoPath.baseName();

    file.close();

    saveSettings();
    emit LoadProject();
    readSettings();

    initDatabase(m_ProjectDatabase);

    logger->logMsg(nmfConstants::Normal,"loadProject end");

} // end loadProject



void
nmfSetup_Tab2::initDatabase(QString database)
{
    std::string cmd;
    std::string msg;
    std::string errorMsg;

    cmd  = "USE " + database.toStdString();
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    m_logger->logMsg(nmfConstants::Normal,cmd);
    if (errorMsg != " ") {
        msg = "[Error 1] nmfSetup_Tab2::initDatabase: " + errorMsg;
        m_logger->logMsg(nmfConstants::Error,msg);
    }
}

void
nmfSetup_Tab2::callback_Setup_Tab2_BrowseProject()
{
    QString fileName = QFileDialog::getOpenFileName(Setup_Tabs,
        tr("Load a Project"), m_ProjectDir, tr("Project Files (*.prj)"));

    if (! fileName.isEmpty()) {
        loadProject(m_logger,fileName);
        //updateOutputWidget();
//        NewProject = true;
    }

   Setup_Tab2_BrowseProjectPB->clearFocus();

} // end callback_Setup_Tab2_BrowseProject


void
nmfSetup_Tab2::callback_Setup_Tab2_NewProject()
{
    Setup_Tab2_ProjectNameLE->clear();
    Setup_Tab2_ProjectAuthorLE->clear();
    Setup_Tab2_ProjectDescLE->clear();
    Setup_Tab2_ProjectDatabaseCMB->clearEditText();
}


void
nmfSetup_Tab2::callback_Setup_Tab2_DelProject()
{
    std::string msg;
    QString projectPrefix = getProjectName();
    std::string ProjectToDelete = projectPrefix.toStdString()+".prj";
    QString fileToDelete = QDir(m_ProjectDir).filePath(QString::fromStdString(ProjectToDelete));

    if (! isValidProjectName(projectPrefix)) {
        return;
    }
    if (! QFileInfo(fileToDelete).exists())
    {
        msg = "Project file doesn't exist:\n\n" + fileToDelete.toStdString();
        QMessageBox::warning(Setup_Tabs,"Delete Error",
                             QString::fromStdString("\n"+msg+"\n"),
                             QMessageBox::Ok);
        return;
    }

    msg = "\nOK to delete the Project file: " + ProjectToDelete + " ?\n";
    QMessageBox::StandardButton reply = QMessageBox::question(Setup_Tabs, tr("Delete Project"),
         tr(msg.c_str()), QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        fileToDelete = QDir(m_ProjectDir).filePath(QString::fromStdString(ProjectToDelete));
        QFile file(fileToDelete);
        if (file.remove()) {
            QMessageBox::information(Setup_Tabs,
                                     tr("Remove Project"),
                                     tr("\nProject file removed successfully."),
                                     QMessageBox::Ok);
            callback_Setup_Tab2_NewProject(); // Clear Project fields
        } else {
            msg = "\nCouldn't remove Project file: " + ProjectToDelete;
            QMessageBox::critical(Setup_Tabs,
                                  tr("Remove Project"),
                                  tr(msg.c_str()),
                                  QMessageBox::Ok);
        }
    }
}

void
nmfSetup_Tab2::callback_Setup_Tab2_ReloadProject()
{
    readSettings();
}

QString
nmfSetup_Tab2::getProjectName()
{
    return Setup_Tab2_ProjectNameLE->text();
}

QString
nmfSetup_Tab2::getProjectDatabase()
{
    return Setup_Tab2_ProjectDatabaseCMB->currentText();
}

void
nmfSetup_Tab2::setProjectDatabase(QString dbName)
{
    if (nmfUtils::isOSWindows()) {
        Setup_Tab2_ProjectDatabaseCMB->setCurrentText(dbName.toLower());
    } else {
        Setup_Tab2_ProjectDatabaseCMB->setCurrentText(dbName);
    }
}

void
nmfSetup_Tab2::readSettings()
{
    // Read the settings and load into class variables.
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");
    settings->beginGroup("SetupTab");
    m_ProjectName        = settings->value("ProjectName","").toString();
    m_ProjectDir         = settings->value("ProjectDir","").toString();
    m_ProjectDatabase    = settings->value("ProjectDatabase","").toString();
    m_ProjectAuthor      = settings->value("ProjectAuthor","").toString();
    m_ProjectDescription = settings->value("ProjectDescription","").toString();
    settings->endGroup();
    delete settings;

    // Load class variables into appropriate widgets.
    Setup_Tab2_ProjectNameLE->setText(m_ProjectName);
    Setup_Tab2_ProjectDirLE->setText(m_ProjectDir);
    Setup_Tab2_ProjectAuthorLE->setText(m_ProjectAuthor);
    Setup_Tab2_ProjectDescLE->setText(m_ProjectDescription);
    Setup_Tab2_ProjectDatabaseCMB->setCurrentText(m_ProjectDatabase);

} // end readSettings


void
nmfSetup_Tab2::saveSettings()
{
    QSettings* settings = nmfUtilsQt::createSettings(nmfConstantsMSCAA::SettingsDirWindows,"MSCAA");
    settings->beginGroup("SetupTab");
    settings->setValue("ProjectName",        m_ProjectName);
    settings->setValue("ProjectDir",         m_ProjectDir);
    settings->setValue("ProjectDatabase",    m_ProjectDatabase);
    settings->setValue("ProjectAuthor",      m_ProjectAuthor);
    settings->setValue("ProjectDescription", m_ProjectDescription);
    settings->endGroup();
    delete settings;
std::cout << "Saving db name: " << m_ProjectDatabase.toStdString() << std::endl;
//    emit SaveMainSettings();

} // end saveSettings


void
nmfSetup_Tab2::loadDatabaseNames(QString NameToSelect)
{
    QList<QString> authenticatedDatabases = {};
    m_ValidDatabases.clear();
    Setup_Tab2_ProjectDatabaseCMB->clear();

    // Get the list of authenticated databases and add them to the appropriate widget
    if (m_databasePtr->getListOfAuthenticatedDatabaseNames(authenticatedDatabases)) {
        foreach (QString item, authenticatedDatabases) {
            m_ValidDatabases.insert(item.toStdString());
            Setup_Tab2_ProjectDatabaseCMB->addItem(item);
        }
    }

    if (! NameToSelect.isEmpty()) {
        if (nmfUtils::isOSWindows()) {
            Setup_Tab2_ProjectDatabaseCMB->setCurrentText(NameToSelect.toLower());
        } else {
            Setup_Tab2_ProjectDatabaseCMB->setCurrentText(NameToSelect);
        }
    } else {
        Setup_Tab2_ProjectDatabaseCMB->setCurrentIndex(0);
    }
}


void
nmfSetup_Tab2::loadWidgets()
{
    m_logger->logMsg(nmfConstants::Normal,"nmfSetup_Tab2::loadWidgets()");

    readSettings();
    loadDatabaseNames(m_ProjectDatabase);

} // end loadWidgets

void
nmfSetup_Tab2::createTables(QString databaseName)
{
    int pInc = 0;
    std::string fullTableName;
    std::string cmd;
    std::string errorMsg;
    std::string db = databaseName.toStdString();
    bool okToCreateMoreTables = true;

    progressDlg = new QProgressDialog("\nCreating Tables...\n",
                                      "Cancel", 0, 35, Setup_Tabs);
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setValue(pInc);
    progressDlg->setRange(0,29);
    progressDlg->show();
    connect(progressDlg, SIGNAL(canceled()), this, SLOT(callback_progressDlgCancel()));

    // 1 of 29: Species
    fullTableName = db + ".Species";
    cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
    cmd += "(SpeName           varchar(100) NOT NULL,";
    cmd += " MinAge            int(11) NULL,";
    cmd += " MaxAge            int(11) NULL,";
    cmd += " FirstAgePrtRcrt   int(11) NULL,";
    cmd += " AgeFullRcrt       int(11) NULL,";
    cmd += " AgeFullRcrtSurvey int(11) NULL,";
    cmd += " SurveySel         float   NULL,";
    cmd += " FirstYear         int(11) NULL,";
    cmd += " LastYear          int(11) NULL,";
    cmd += " NumSurveys        int(11) NULL,";
    cmd += " NumFleets         int(11) NULL,";
    cmd += " Nseg              int(11) NULL,";
    cmd += " aAge1ph           int(11) NULL,";
    cmd += " aFtph             int(11) NULL,";
    cmd += " dAge1ph           int(11) NULL,";
    cmd += " dFtph             int(11) NULL,";
    cmd += " ficph             int(11) NULL,";
    cmd += " fishph            int(11) NULL,";
    cmd += " Yr1ph             int(11) NULL,";
    cmd += " Rhoph             int(11) NULL,";
    cmd += " TCwt              float   NULL,";
    cmd += " CPwt              float   NULL,";
    cmd += " Bwt               float   NULL,";
    cmd += " Ywt               float   NULL,";
    cmd += " Rwt               float   NULL,";
    cmd += " FHwt              float   NULL,";
    cmd += " Bthres            float   NULL,";
    cmd += " Rthres            float   NULL,";
    cmd += " MinLength         float   NULL,";
    cmd += " MaxLength         float   NULL,";
    cmd += " NumLengthBins     int(11) NULL,";
    cmd += " CatchAtAge        int(11) NULL,";
    cmd += " PRIMARY KEY (SpeName))";
    errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
    if (errorMsg != " ") {
        nmfUtils::printError("[Error 1] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
        okToCreateMoreTables = false;
    } else {
        m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
        progressDlg->setValue(++pInc);
        progressDlg->update();
    }
    if (! okToCreateMoreTables)
        return;

    // 2 of 29: CatchFishery
    // 3 of 29: CatchFisheryProportion
    for (std::string tableName : {"CatchFishery",
                                  "CatchFisheryProportion"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Fleet         varchar(50) NOT NULL,";
        cmd += " Year          int(11)     NOT NULL,";
        cmd += " Age           int(11)     NOT NULL,";
        cmd += " Value         float       NOT NULL,";
        cmd += " Units         varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Fleet,Year,Age,Units))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 2] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 4 of 29: Weight
    // 5 of 29: Maturity
    // 6 of 29: Consumption
    // 7 of 29: InitialAbundance
    for (std::string tableName : {"Weight",
                                  "Maturity",
                                  "Consumption",
                                  "InitialAbundance"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Year          int(11) NOT NULL,";
        cmd += " Age           int(11) NOT NULL,";
        cmd += " Value         float   NOT NULL,";
        cmd += " Units         varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Year,Age,Units))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 2] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 8 of 29: CatchSurvey
    // 9 of 29: CatchSurveyProportion
    for (std::string tableName : {"CatchSurvey",
                                  "CatchSurveyProportion"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Survey        int(11) NOT NULL,";
        cmd += " Year          int(11) NOT NULL,";
        cmd += " Age           int(11) NOT NULL,";
        cmd += " Value         float   NOT NULL,";
        cmd += " Units         varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Survey,Year,Age,Units))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 2] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }


    // 10 of 29: CatchFisheryTotal
    for (std::string tableName : {"CatchFisheryTotal"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Fleet         varchar(50) NOT NULL,";
        cmd += " Year          int(11) NOT NULL,";
        cmd += " Value         float   NOT NULL,";
        cmd += " Units         varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Fleet,Year,Units))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 3] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 11 of 29: CatchSurveyTotal
    for (std::string tableName : {"CatchSurveyTotal"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Survey        int(11)     NOT NULL,";
        cmd += " Year          int(11)     NOT NULL,";
        cmd += " Value         float       NOT NULL,";
        cmd += " Units         varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Survey,Year,Units))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 4] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 12 of 29: MortalityNatural
    // 13 of 29: MortalityFishing
    for (std::string tableName : {"MortalityNatural",
                                  "MortalityFishing"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Segment       float       NOT NULL,";
        cmd += " ColName       varchar(50) NOT NULL,";
        cmd += " Value         float       NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Segment,ColName))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 5] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 14 of 29: Diet
    for (std::string tableName : {"Diet"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " PredatorName  varchar(50) NOT NULL,";
        cmd += " PredatorAge   int(11)     NOT NULL,";
        cmd += " Bin           float       NOT NULL,"; // float so the columns can be sorted
        cmd += " BinType       varchar(50) NOT NULL,";
        cmd += " ColName       varchar(50) NOT NULL,";
        cmd += " Value         float   NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,PredatorName,PredatorAge,Bin,BinType,ColName))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 6] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 15 of 29: SurveyMonth
    for (std::string tableName : {"SurveyMonth"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Survey        int(11)     NOT NULL,";
        cmd += " Value         int(11)     NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Survey))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 7] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 16 of 29: SurveyWeights
    for (std::string tableName : {"SurveyWeights"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Survey        int(11)     NOT NULL,";
        cmd += " TSwtValue     float       NOT NULL,";
        cmd += " SPwtValue     float       NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Survey))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 8] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 17 of 29: System
    for (std::string tableName : {"System"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel   varchar(50) NOT NULL,";
        cmd += " SystemName      varchar(50) NOT NULL,";
        cmd += " TotalBiomass    varchar(50) NULL,";
        cmd += " FH_FirstYear    int(11)     NULL,";
        cmd += " FH_LastYear     int(11)     NULL,";
        cmd += " NumSpInter      int(11)     NULL,";
        cmd += " Owt             int(11)     NULL,";
        cmd += " LogNorm         double      NULL,";
        cmd += " MultiResid      double      NULL,";
        cmd += " AbundanceDriver varchar(50) NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 9] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 18 of 29: PredatorPreyPreferredRatio
    // 19 of 29: PredatorPreyVarianceLTRatio
    // 20 of 29: PredatorPreyVarianceGTRatio
    // 21 of 29: PredatorPreyInteractions
    // 22 of 29: PredatorPreyVulnerability - From Eq'n (5) Curti Dissertation
    for (std::string tableName : {"PredatorPreyPreferredRatio",
                                  "PredatorPreyVarianceLTRatio",
                                  "PredatorPreyVarianceGTRatio",
                                  "PredatorPreyInteractions",
                                  "PredatorPreyVulnerability"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " PredatorName  varchar(50) NOT NULL,";
        cmd += " PreyName      varchar(50) NOT NULL,";
        cmd += " Value         float       NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,PredatorName,PreyName))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 8] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 23 of 29: PredatorPreyInteractionsVec
    for (std::string tableName : {"PredatorPreyInteractionsVec"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " PredValue     int(11) NOT NULL,";
        cmd += " PreyValue     int(11) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,PredValue,PreyValue))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 9] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 24 of 29: Covariates
    for (std::string tableName : {"Covariates"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel   varchar(50) NOT NULL,";
        cmd += " SystemName      varchar(50) NOT NULL,";
        cmd += " SpeName         varchar(50) NOT NULL,";
        cmd += " CovariateNumber int(11)     NOT NULL,";
        cmd += " CovariateName   varchar(50) NOT NULL,";
        cmd += " Year            int(11)     NOT NULL,";
        cmd += " Value           float       NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,CovariateNumber,CovariateName,Year))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 9] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 25 of 29: Fleets
    for (std::string tableName : {"Fleets"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel   varchar(50) NOT NULL,";
        cmd += " SystemName      varchar(50) NOT NULL,";
        cmd += " SpeName         varchar(50) NOT NULL,";
        cmd += " FleetNumber     int(11)     NOT NULL,";
        cmd += " FleetName       varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,FleetNumber))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 10] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 26 of 29: Age-Length Key
    for (std::string tableName : {"AgeLengthKey"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel   varchar(50) NOT NULL,";
        cmd += " SystemName      varchar(50) NOT NULL,";
        cmd += " SpeName         varchar(50) NOT NULL,";
        cmd += " Year            int(11)     NOT NULL,";
        cmd += " Age             int(11)     NOT NULL,";
        cmd += " Value           float       NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Year,Age))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 11] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 27 of 29: CatchAtLengthFishery
    for (std::string tableName : {"CatchAtLengthFishery"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Fleet         varchar(50) NOT NULL,";
        cmd += " Year          int(11)     NOT NULL,";
        cmd += " BinNumber     int(11)     NOT NULL,"; // Useful for sorting by bin since bin names can be: "10-20","21-30"
        cmd += " BinName       varchar(50) NOT NULL,";
        cmd += " Value         float       NOT NULL,";
        cmd += " Units         varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Fleet,Year,BinNumber,BinName,Units))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 12] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 28 of 29: SimulationParametersSpecies
    for (std::string tableName : {"SimulationParametersSpecies"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " Algorithm     varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " ParameterName varchar(50) NOT NULL,";
        cmd += " Value         double      NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,Algorithm,SpeName,ParameterName))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 13] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 29 of 29: SimulationParametersYearly
    for (std::string tableName : {"SimulationParametersYearly"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(MohnsRhoLabel varchar(50) NOT NULL,";
        cmd += " SystemName    varchar(50) NOT NULL,";
        cmd += " SpeName       varchar(50) NOT NULL,";
        cmd += " Year          int(11)     NOT NULL,";
        cmd += " ParameterName varchar(50) NOT NULL,";
        cmd += " Value         float       NOT NULL,";
        cmd += " PRIMARY KEY (MohnsRhoLabel,SystemName,SpeName,Year,ParameterName))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 14] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
    }

    // 30 of 30: Application (contains name of application - used to assure app is using correct database)
    for (std::string tableName : {"Application"})
    {
        fullTableName = db + "." + tableName;
        cmd  = "CREATE TABLE IF NOT EXISTS " + fullTableName;
        cmd += "(Name varchar(50) NOT NULL,";
        cmd += " PRIMARY KEY (Name))";
        errorMsg = m_databasePtr->nmfUpdateDatabase(cmd);
        if (errorMsg != " ") {
            nmfUtils::printError("[Error 15] CreateTables: Create table " + fullTableName + " error: ", errorMsg);
            okToCreateMoreTables = false;
        } else {
            m_logger->logMsg(nmfConstants::Normal,"Created table: "+fullTableName);
            progressDlg->setValue(++pInc);
            progressDlg->update();
        }
        if (! okToCreateMoreTables)
            return;
        m_databasePtr->saveApplicationTable(Setup_Tabs,m_logger,fullTableName);
    }

    delete progressDlg;

}

