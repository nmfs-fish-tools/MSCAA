#include "VerboseDebugLevelDialog.h"
#include <QStandardItemModel>
#include <QTableWidget>

VerboseDebugLevelDialog::VerboseDebugLevelDialog(QWidget* parent, QLineEdit* debugLE) :
    QDialog(parent)
{
    MainLAYT       = new QVBoxLayout();
    BtnLAYT        = new QHBoxLayout();
    MainLBL        = new QLabel("<b>Select Verbose Debug Level:</b>");
    CancelPB       = new QPushButton("Cancel");
    OkPB           = new QPushButton("OK");
    VerboseLevelLW = new QListWidget();
    DebugLE        = debugLE;

    OkPB->setToolTip("Set a new verbose debug level");
    OkPB->setStatusTip("Set a new verbose debug level");

    MainLAYT->addWidget(MainLBL);
    MainLAYT->addWidget(VerboseLevelLW);
    BtnLAYT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    BtnLAYT->addWidget(CancelPB);
    BtnLAYT->addWidget(OkPB);
    BtnLAYT->addSpacerItem(new QSpacerItem(2,1,QSizePolicy::Expanding,QSizePolicy::Fixed));
    MainLAYT->addLayout(BtnLAYT);
    this->setLayout(MainLAYT);
    VerboseLevelLW->setWhatsThis("<html><head/><body><p align=\"center\"><span style=\" font-weight:600;\">Debug Level</span></p><p>The user may select a debug verbose level. Each level prints out a set of debug statements based upon its description. A debug value of 0 will cause no debug information to be printed out. The debug values are grouped as follows:</p><p>0's = Main body of program<br/>10's = Initial states function<br/>20's = Suitability function<br/>30's = Predation mortality function<br/>40's = Population dynamics function<br/>50's = Survey abundance function<br/>60's = Log likelihood function<br/>70's = Food habits function<br/>80's = Penalty functions</p></body></html>");
    VerboseLevelLW->setToolTip("List of verbose debug levels");
    VerboseLevelLW->setStatusTip("List of verbose debug levels");

    connect(CancelPB, SIGNAL(clicked()), this, SLOT(close()));
    connect(OkPB,     SIGNAL(clicked()), this, SLOT(callback_OkPB()));
    connect(VerboseLevelLW, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,           SLOT(callback_ItemDoubleClicked(QListWidgetItem*)));

    setWindowTitle("Verbose Debug Level");
    loadWidgets();
}

void
VerboseDebugLevelDialog::loadWidgets()
{

 QStringList verboseLevels = {
/*  0 */  "0's = Main body of program",
/*  1 */  "0:   No debug statements printed",
/*  2 */  "1:   Exits after data section",
/*  3 */  "2:   Exits after parameter section",
/*  4 */  "3:   Exits at end of procedure section",
/*  5 */  "4:   Prints checkpoints after each function in the procedure section, except those within the year loop",
/*  6 */  "5:   Prints checkpoints after each function within the year loop",
/*  7 */  "6:   Prints parameter estimates after each iteration",
/*  8 */  "7:   Prints pop dy variables after each iteration",
/*  9 */  "8:   Prints trophic matrices at end of year loop, then exits",
/* 10 */  "9:   Prints out predicted indices that go into the objective function after each iteration",
/* 11 */  "10's = Initial states function",
/* 12 */  "10:  Outputs food-selection parameters at the end of the function and then exits",
/* 13 */  "11:  Outputs food selection paramters at the end of each iteration",
/* 14 */  "12:  Outputs fishery and survey selectivity matrices at the end of the function and then exits",
/* 15 */  "13:  Outputs abundance and biomass arrays and then exits",
/* 16 */  "14:  Outputs Yr1, Age1 and iFt matrices to ensure 'means + devt'ns' parameterized correctly and then exits",
/* 17 */  "20's = Suitability function",
/* 18 */  "20:  Output at end of function",
/* 19 */  "21:  Output at end of function, then exits",
/* 20 */  "22:  Outputs suitability and scaled suitability for each predator sp and then exits",
/* 21 */  "23:  Outputs Eta, Sigma, WtRatio, G for pred, prey combos and then exits",
/* 22 */  "30's = Predation mortality function",
/* 23 */  "30:  Prints checkpoints throughout the function",
/* 24 */  "31:  Prints intermediate arrays (Avail, scAv, Consum) for each predator sp and then exits",
/* 25 */  "32:  Prints intermediate arrays for each prey sp (Consum, sumCon) and then exits",
/* 26 */  "33:  Prints sumCon and B right before M2 is calculated, and M2 after it is calculated",
/* 27 */  "34:  Debug 31 but does not exit",
/* 28 */  "35:  Prints nf and Other Food in every year",
/* 29 */  "40's = Population dynamics function",
/* 30 */  "40:  Outputs N, C_hat and Cprop_hat at end of function",
/* 31 */  "41:  Outputs N, C_hat and Cprop_hat at end of function and then exits",
/* 32 */  "42:  Outputs mortality components for each species in each year and exits at end of year loop after trophic =1",
/* 33 */  "43:  Outputs mortality components at end of function and then exits",
/* 34 */  "50's = Survey abundance function",
/* 35 */  "50:  Prints intermediate arrays for species where survey data is one contiguous time series and then exits",
/* 36 */  "51:  Prints intermediate arrays for species where the survey data is split into multiple segments and then exits",
/* 37 */  "52:  Prints predicted q, FICs, FIC_hat and N for each species and then exits",
/* 38 */  "53:  Prints estimated q matrix at the end of each iteration",
/* 39 */  "60's = Log likelihood function",
/* 40 */  "60:  Prints checkpoints after each likelihood component",
/* 41 */  "61:  Prints checkpoints for multinomial components within the year loop",
/* 42 */  "62:  Prints predicted and log predicted indices for TotC and TotFIC",
/* 43 */  "63:  Prints predicted and log predicted indices for Cprop",
/* 44 */  "64:  Prints predicted and log predicted indices for Sprop",
/* 45 */  "65:  FHprop, when added",
/* 46 */  "66:  Prints summary of objective function components at end of each iteration",
/* 47 */  "70's = Food habits function",
/* 48 */  "70:  Prints Avpd (scAv) and FHtmp for each predator, year and then exits",
/* 49 */  "71:  Prints Avpd, Avpy for each prey species within Avpd, the colsum of Avpy, and FHtmp; exits after all predator species",
/* 50 */  "72:  Prints bin years, FHtmp, FHsum, and average FH, FH_hat, for each FH bin, and exits after all predator species",
/* 51 */  "73:  Prints total %W for each pred age, summed across prey sp, for each pred sp and FH bin.  This value should always == 1.",
/* 53 */  "80's = Penalty functions",
/* 54 */  "80:  Biomass penalty function: Prints pre- and post- B for every species and year, regardless of whether a penalty is imposed",
/* 55 */  "81:  Biomass penalty function: Prints pre- and post- biomass and assorted arrays when biomass falls below threshold",
/* 56 */  "82:  Yr1 penalty function: Prints avgZ, thYr1, Yr1 and Ypen arrays and then exits at end of function",
/* 57 */  "83:  Recruitment penalty function: Prints Age1 parameter estimates, calculated CV and recruitment penalty"
 };
 VerboseLevelLW->addItems(verboseLevels);

 // Make bold and inactive the group list widget items
 std::vector<int> groupItems = {0,11,17,22,29,34,39,47,52};
 QFont font = VerboseLevelLW->font();
 font.setBold(true);
 for (unsigned i=0; i<groupItems.size(); ++i) {
     QListWidgetItem* item = VerboseLevelLW->item(groupItems[i]);
     item->setFont(font);
     item->setFlags(item->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled);
 }

 this->resize(750,500);

}

void
VerboseDebugLevelDialog::callback_OkPB()
{
    DebugLE->setText(getSelectedString());
    close();
}

void
VerboseDebugLevelDialog::callback_ItemDoubleClicked(QListWidgetItem* sel)
{
    DebugLE->setText(getSelectedString());
    close();
}

QString
VerboseDebugLevelDialog::getSelectedString()
{
    QString retv = "0";

    QList<QListWidgetItem*> items = VerboseLevelLW->selectedItems();
    if (items.size() > 0) {
        retv = items[0]->text().split(":")[0];
    }
    return retv;
}
