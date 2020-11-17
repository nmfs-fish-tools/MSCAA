/**
 * @file VerboseDebugLevelDialog.h
 * @brief Creates the dialog asking the user to select the desired verbose debug level.
 *
 */
#pragma once

#include "nmfDatabase.h"

#include <QListWidget>
#include <QTableWidget>
#include <QComboBox>

/**
 * @brief Verbose Debug Level Dialog
 *
 * This dialog allows the user to set the verbose debug level for a run.
 * Different debug levels will cause different sets of print commands to
 * be written to the summary window.
 *
 */
class VerboseDebugLevelDialog : public QDialog
{
    Q_OBJECT

    nmfDatabase* m_databasePtr;
    QHBoxLayout* BtnLAYT;
    QPushButton* CancelPB;
    QLineEdit*   DebugLE;
    QVBoxLayout* MainLAYT;
    QLabel*      MainLBL;
    QPushButton* OkPB;
    QListWidget* VerboseLevelLW;

    void loadWidgets();

public:
    /**
     * @brief VerboseDebugLevelDialog : class constructor
     * @param parent : the parent widget (e.g., the main window)
     * @param debugLE : the pointer to the QLineEdit in which the selected value will be written
     */
    VerboseDebugLevelDialog(QWidget* parent, QLineEdit* debugLE);
    virtual ~VerboseDebugLevelDialog() {}

    /**
     * @brief Gets the selected item from the list widget
     * @return Returns the QString of the item selected
     */
    QString getSelectedString();


private Q_SLOTS:
    void callback_OkPB();
    void callback_ItemDoubleClicked(QListWidgetItem* sel);

};

