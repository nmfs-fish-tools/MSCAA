/**
 * @file nmfSetupTab01.h
 * @brief This file contains the class definition for the GUI elements and functionality for the 1st tabbed Setup pane
 *
 * This file contains the GUI definition for the 1st Setup Tabbed window.
 *
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
#ifndef NMFSETUPTAB1_H
#define NMFSETUPTAB1_H

#include <QTextEdit>
#include <QLabel>

/**
 * @brief The class containing the interactions for the 1st Setup tabbed pane
 */
class nmfSetup_Tab1: public QObject
{
    Q_OBJECT

    QTabWidget  *Setup_Tabs;
    QWidget     *Setup_Tab1_Widget;
    QTextEdit   *Setup_Tab1_RequirementsTE;
    QLabel      *Setup_Tab1_RequirementsLBL;

public:
    /**
     * @brief class constructor for the 1st Setup pane
     * @param tab : parent widget into which this Setup tab will be placed
     */
    nmfSetup_Tab1(QTabWidget *tab);
    virtual ~nmfSetup_Tab1();

    /**
     * @brief The method isn't currently implemented but is here as a placeholder for any future implementation.
     */
    void loadWidgets();

};

#endif // NMFSETUPTAB1_H
