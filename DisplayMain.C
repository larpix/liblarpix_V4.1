#include "DisplayFunctions.C"

// Mainframe macro generated from application: /home/kreslo/root6-10/bin/root.exe
// By ROOT version 6.10/06 on 2018-11-17 15:22:27

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGSlider
#include "TGSlider.h"
#endif
#ifndef ROOT_TGDoubleSlider
#include "TGDoubleSlider.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGColorDialog
#include "TGColorDialog.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGFontDialog
#include "TGFontDialog.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGView
#include "TGView.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TRootCanvas
#include "TRootCanvas.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"
#include "TThread.h"
#include "TTimer.h"
#include <stdlib.h>
#include <time.h>

TGTextEntry *fTextEntry2201; //IP:PORT
TGTextEntry *fTextEntry2168; //ASICs list file
TGTextButton *fTextButton2199; //Read ASICs list
TGTextButton *fTextButton2170; //Reset Histos
TGTextButton *fTextButton2200; //Connect
TGTextButton *fTextButton2202; //Disconnect
TGTextButton *fTextButtonQuit; //Quit

TGTextButton *fTextButton4; //set BG
TGTextButton *fTextButton5; //reset BG


TCanvas *c126; //Canvas Tab0: Channels plots
TCanvas *c127; //Canvas Tab1: Pixel plane
TCanvas *c128; //Canvas Tab2: chip rates plot
TCanvas *c125; //left side channel rate canvas
TCanvas *c12; //pixel rates canvas
TCanvas *c123; //3d view
TGStatusBar *fStatusBar; //lower status bar

TGVSlider* vslider; //threshold for display
TGDoubleVSlider * v2slider;
TGTab *fTab2171; //Tab widget
   TGIcon *fIconHBC;
   TGIcon *fIconHBD;
TGLabel *fLabelPE; //parity OK/error
TGListBox *fListBox2189; //ASICs list for tab0
TGTextEdit *fTextEditStats;//statistics
char StatsText[256];
//TGLabel *fLabelStat1; //statistics

int CStreamConnected=0;
int DStreamConnected=0;
int CStreamHeartbeat=0;
int DStreamHeartbeat=0;
int UpdatePeriod=500; //Timer1 period, ms;
int ParityError=0;
//heartbeat times
time_t Dct,Dct0;
time_t Cct,Cct0;
int LastSelectedChip=-1;
LarpixConfig cfg[256];


TThread *worker=0; 
TThread *gui=0; 
TTimer *timer1=0;
void Timer1Update();
void *worker_handle( void *ptr); 
void *gui_handle( void *ptr);
/*
void ChangeThreshold(Int_t thr)
{ 
   htpc_threshold=63-thr;
}
*/
void ChangeThreshold()
{ 
   htpc_Hthreshold=255-v2slider->GetMinPosition();
   htpc_Lthreshold=255-v2slider->GetMaxPosition();
   printf("3D display shows %d =< adc =< %d\n",htpc_Lthreshold,htpc_Hthreshold);
}

void InitChipList();
void DrawHistos();
void Quit();
void PrintPix();
void DisplayMain()
{
   // main frame
   TGMainFrame *fMainFrame2380 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainFrame2380->SetName("fMainFrame2380");
   fMainFrame2380->SetWindowName("LarPix monitor");
   fMainFrame2380->SetLayoutBroken(kTRUE);

   // composite frame
   TGCompositeFrame *fMainFrame1984 = new TGCompositeFrame(fMainFrame2380,1211,758,kVerticalFrame);
   fMainFrame1984->SetName("fMainFrame1984");
   fMainFrame1984->SetLayoutBroken(kTRUE);

   // composite frame
   TGCompositeFrame *fMainFrame921 = new TGCompositeFrame(fMainFrame1984,1211,758,kVerticalFrame);
   fMainFrame921->SetName("fMainFrame921");
   fMainFrame921->SetLayoutBroken(kTRUE);

   // composite frame
   TGCompositeFrame *fMainFrame6033 = new TGCompositeFrame(fMainFrame921,1211,758,kVerticalFrame);
   fMainFrame6033->SetName("fMainFrame6033");
   fMainFrame6033->SetLayoutBroken(kTRUE);

   // composite frame
   TGCompositeFrame *fMainFrame5421 = new TGCompositeFrame(fMainFrame6033,1211,758,kVerticalFrame);
   fMainFrame5421->SetName("fMainFrame5421");
   fMainFrame5421->SetLayoutBroken(kTRUE);

   // status bar
   fStatusBar = new TGStatusBar(fMainFrame5421,968,24);
   fStatusBar->SetName("fStatusBar");
   fStatusBar->SetText("Idle",0);
   fMainFrame5421->AddFrame(fStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsExpandX));
   fStatusBar->MoveResize(240,720,968,24);
   TGLabel *fLabel2167 = new TGLabel(fMainFrame5421,"IP");
   fLabel2167->SetTextJustify(36);
   fLabel2167->SetMargins(0,0,0,0);
   fLabel2167->SetWrapLength(-1);
   fMainFrame5421->AddFrame(fLabel2167, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2167->MoveResize(0,40,64,19);

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t valEntry2168;
   valEntry2168.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry2168.fForeground);
   gClient->GetColorByName("#e8e8e8",valEntry2168.fBackground);
   valEntry2168.fFillStyle = kFillSolid;
   valEntry2168.fFont = ufont->GetFontHandle();
   valEntry2168.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry2168, kTRUE);
   fTextEntry2168 = new TGTextEntry(fMainFrame5421, new TGTextBuffer(31),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
   fTextEntry2168->SetMaxLength(4096);
   fTextEntry2168->SetAlignment(kTextLeft);
   fTextEntry2168->SetText("CONF/chips.list");
   fTextEntry2168->Resize(128,fTextEntry2168->GetDefaultHeight());
   fMainFrame5421->AddFrame(fTextEntry2168, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry2168->MoveResize(64,72,128,23);
   TGLabel *fLabel2169 = new TGLabel(fMainFrame5421,"ASIC list");
   fLabel2169->SetTextJustify(36);
   fLabel2169->SetMargins(0,0,0,0);
   fLabel2169->SetWrapLength(-1);
   fMainFrame5421->AddFrame(fLabel2169, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2169->MoveResize(0,72,64,19);

   fTextButton2170 = new TGTextButton(fMainFrame5421,"Reset histos",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton2170->SetTextJustify(36);
   fTextButton2170->SetMargins(0,0,0,0);
   fTextButton2170->SetWrapLength(-1);
   fTextButton2170->Resize(152,48);
   fMainFrame5421->AddFrame(fTextButton2170, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton2170->MoveResize(40,272,152,48);

   fTextButtonQuit = new TGTextButton(fMainFrame5421,"Quit",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButtonQuit->SetTextJustify(36);
   fTextButtonQuit->SetMargins(0,0,0,0);
   fTextButtonQuit->SetWrapLength(-1);
   fTextButtonQuit->Resize(152,48);
   fMainFrame5421->AddFrame(fTextButtonQuit, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButtonQuit->MoveResize(40,472,152,48);

   fIconHBC = new TGIcon(fMainFrame5421,"./icons/HeartCl.png");
   fIconHBC->SetName("fIconHBC");
   fMainFrame5421->AddFrame(fIconHBC, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fIconHBC->MoveResize(2,320,100,100);

   fIconHBD = new TGIcon(fMainFrame5421,"./icons/HeartDl.png");
   fIconHBD->SetName("fIconHBD");
   fMainFrame5421->AddFrame(fIconHBD, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fIconHBD->MoveResize(102,320,100,100);

   fLabelPE = new TGLabel(fMainFrame5421,"Parity OK");
   fLabelPE->SetTextJustify(36);
   fLabelPE->SetMargins(0,0,0,0);
   fLabelPE->SetWrapLength(-1);
   fLabelPE->SetBackgroundColor(0x66ff66); 
   fMainFrame5421->AddFrame(fLabelPE, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabelPE->MoveResize(102,420,80,20);
   //fLabelPE->Disable(1);

// embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas804 = new TRootEmbeddedCanvas(0,fMainFrame5421,192,128,kSunkenFrame);
   fRootEmbeddedCanvas804->SetName("fRootEmbeddedCanvas804");
   Int_t wfRootEmbeddedCanvas804 = fRootEmbeddedCanvas804->GetCanvasWindowId();
   c125 = new TCanvas("c125", 10, 10, wfRootEmbeddedCanvas804);
   fRootEmbeddedCanvas804->AdoptCanvas(c125);
   fMainFrame5421->AddFrame(fRootEmbeddedCanvas804, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fRootEmbeddedCanvas804->MoveResize(2,530,192,170);

   // tab widget
   fTab2171 = new TGTab(fMainFrame5421,1008,712);

   // container of "Channel plots"
   TGCompositeFrame *fCompositeFrame2174;
   fCompositeFrame2174 = fTab2171->AddTab("Channel plots"); //Tab0
   fCompositeFrame2174->SetLayoutManager(new TGVerticalLayout(fCompositeFrame2174));

   // horizontal frame
   TGHorizontalFrame *fHorizontalFrame2175 = new TGHorizontalFrame(fCompositeFrame2174,1000,682,kHorizontalFrame);

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas2176 = new TRootEmbeddedCanvas(0,fHorizontalFrame2175,890,670,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas2176 = fRootEmbeddedCanvas2176->GetCanvasWindowId();
   c126 = new TCanvas("c126", 10, 10, wfRootEmbeddedCanvas2176);
   fRootEmbeddedCanvas2176->AdoptCanvas(c126);
   fHorizontalFrame2175->AddFrame(fRootEmbeddedCanvas2176, new TGLayoutHints(kLHintsRight | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,10,2));

   // list box
   fListBox2189 = new TGListBox(fHorizontalFrame2175,-1,kSunkenFrame);
  // fListBox2189->AddEntry("0",1);
   fListBox2189->Resize(102,665);
   fHorizontalFrame2175->AddFrame(fListBox2189, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandY,2,2,10,2));

   fCompositeFrame2174->AddFrame(fHorizontalFrame2175, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));


   // container of "Pixel Plane Amps"
   TGCompositeFrame *fCompositeFrame2198;
   fCompositeFrame2198 = fTab2171->AddTab("Anode Raw Amps"); //Tab1
   fCompositeFrame2198->SetLayoutManager(new TGVerticalLayout(fCompositeFrame2198));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas2211 = new TRootEmbeddedCanvas(0,fCompositeFrame2198,1000,682,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas2211 = fRootEmbeddedCanvas2211->GetCanvasWindowId();
   c127 = new TCanvas("c127", 10, 10, wfRootEmbeddedCanvas2211);
   c127->AddExec("PrintPix","PrintPix()"); // print pixel indices
   fRootEmbeddedCanvas2211->AdoptCanvas(c127);
   fCompositeFrame2198->AddFrame(fRootEmbeddedCanvas2211, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));

   fTextButton4 = new TGTextButton(fCompositeFrame2198,"Set as BG",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton4->SetTextJustify(36);
   fTextButton4->SetMargins(0,0,0,0);
   fTextButton4->SetWrapLength(-1);
   fTextButton4->Resize(252,48);
   fCompositeFrame2198->AddFrame(fTextButton4, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton4->SetCommand("hplaneBG->Add(hplane,1);");

   fTextButton5 = new TGTextButton(fCompositeFrame2198,"Reset BG",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton5->SetTextJustify(36);
   fTextButton5->SetMargins(0,0,0,0);
   fTextButton5->SetWrapLength(-1);
   fTextButton5->Resize(252,48);
   fCompositeFrame2198->AddFrame(fTextButton5, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton5->SetCommand("hplaneBG->Reset();");


  // fTextButton4->MoveResize(100,100,40,20);
  // fTextButton5->MoveResize(100,130,40,20);


   // container of "Pixel Rates"
   TGCompositeFrame *fCompositeFrame21;
   fCompositeFrame21 = fTab2171->AddTab("Anode Rates"); //Tab2
   fCompositeFrame21->SetLayoutManager(new TGVerticalLayout(fCompositeFrame21));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas22 = new TRootEmbeddedCanvas(0,fCompositeFrame21,1000,682,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas22 = fRootEmbeddedCanvas22->GetCanvasWindowId();
   c12 = new TCanvas("c12", 10, 10, wfRootEmbeddedCanvas22);
   c12->AddExec("PrintPix","PrintPix()"); // print pixel indices
   fRootEmbeddedCanvas22->AdoptCanvas(c12);
   fCompositeFrame21->AddFrame(fRootEmbeddedCanvas22, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));


   // container of "Chip rates"
   TGCompositeFrame *fCompositeFrame21981;
   fCompositeFrame21981 = fTab2171->AddTab("Chip rates"); ///Tab3
   fCompositeFrame21981->SetLayoutManager(new TGVerticalLayout(fCompositeFrame21981));
   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas22111 = new TRootEmbeddedCanvas(0,fCompositeFrame21981,1000,682,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas22111 = fRootEmbeddedCanvas22111->GetCanvasWindowId();
   c128 = new TCanvas("c128", 10, 10, wfRootEmbeddedCanvas22111);
   fRootEmbeddedCanvas22111->AdoptCanvas(c128);
   fCompositeFrame21981->AddFrame(fRootEmbeddedCanvas22111, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));


   // container of "Statistics"
   TGCompositeFrame *fCompositeFrame219819;
   fCompositeFrame219819 = fTab2171->AddTab("Statistics"); //Tab4
   fCompositeFrame219819->SetLayoutManager(new TGVerticalLayout(fCompositeFrame219819));
/*
   fLabelStat1 = new TGLabel(fCompositeFrame219819,"Some stats");
   fLabelStat1->SetTextJustify(36);
   fLabelStat1->SetMargins(0,0,0,0);
   fLabelStat1->SetWrapLength(-1);
  // fLabelStat1->SetBackgroundColor(0x66ff66); 
   fCompositeFrame219819->AddFrame(fLabelStat1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabelStat1->MoveResize(102,420,80,20);
*/
   fTextEditStats = new TGTextEdit(fCompositeFrame219819,900,600);
  // fTextEdit934->LoadFile("TxtEdit934");
   fTextEditStats->SetReadOnly(kTRUE);
   fTextEditStats->Clear();
   fCompositeFrame219819->AddFrame(fTextEditStats, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));



   // container of "3D"
   TGCompositeFrame *fCompositeFrame213;
   fCompositeFrame213 = fTab2171->AddTab("3D view"); //Tab5
   fCompositeFrame213->SetLayoutManager(new TGVerticalLayout(fCompositeFrame213));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas223 = new TRootEmbeddedCanvas(0,fCompositeFrame213,1000,682,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas223 = fRootEmbeddedCanvas223->GetCanvasWindowId();
   c123 = new TCanvas("c123", 10, 10, wfRootEmbeddedCanvas223);
   fRootEmbeddedCanvas223->AdoptCanvas(c123);
   fCompositeFrame213->AddFrame(fRootEmbeddedCanvas223, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
 /*  vslider = new TGVSlider(fCompositeFrame213,680,kSlider1|kScaleDownRight,101);
   vslider->SetRange(0,63);
   vslider->SetPosition(0);
   vslider->Connect("PositionChanged(Int_t)",0,0,"ChangeThreshold(Int_t)");
*/
   v2slider = new TGDoubleVSlider(fCompositeFrame213,680,kDoubleScaleBoth,101);
   v2slider->SetRange(0,255);
   v2slider->SetPosition(0,255);
   v2slider->Connect("PositionChanged()",0,0,"ChangeThreshold()");



   fTab2171->SetTab(0);

   fTab2171->Resize(fTab2171->GetDefaultSize());
   fMainFrame5421->AddFrame(fTab2171, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTab2171->MoveResize(200,2,1008,712);
   fTextButton2199 = new TGTextButton(fMainFrame5421,"Read ASIC list",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton2199->SetTextJustify(36);
   fTextButton2199->SetMargins(0,0,0,0);
   fTextButton2199->SetWrapLength(-1);
   fTextButton2199->Resize(152,48);
   fMainFrame5421->AddFrame(fTextButton2199, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton2199->MoveResize(40,104,152,48);

   fTextButton2200 = new TGTextButton(fMainFrame5421,"Connect to Data Stream",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton2200->AllowStayDown(1);
   fTextButton2200->SetDown(0);
   fTextButton2200->SetTextJustify(36);
   fTextButton2200->SetMargins(0,0,0,0);
   fTextButton2200->SetWrapLength(-1);
   fTextButton2200->Resize(152,48);
   fMainFrame5421->AddFrame(fTextButton2200, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton2200->MoveResize(40,160,152,48);

   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   // graphics context changes
   GCValues_t valEntry2201;
   valEntry2201.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry2201.fForeground);
   gClient->GetColorByName("#e8e8e8",valEntry2201.fBackground);
   valEntry2201.fFillStyle = kFillSolid;
   valEntry2201.fFont = ufont->GetFontHandle();
   valEntry2201.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry2201, kTRUE);
   fTextEntry2201 = new TGTextEntry(fMainFrame5421, new TGTextBuffer(31),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
   fTextEntry2201->SetMaxLength(4096);
   fTextEntry2201->SetAlignment(kTextLeft);
 // fTextEntry2201->SetText("192.168.2.102");
  fTextEntry2201->SetText("130.92.139.27");
    fTextEntry2201->Resize(128,fTextEntry2201->GetDefaultHeight());
   fMainFrame5421->AddFrame(fTextEntry2201, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry2201->MoveResize(64,40,128,23);
   fTextButton2202 = new TGTextButton(fMainFrame5421,"Disconnect Data Stream",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton2202->SetTextJustify(36);
   fTextButton2202->SetMargins(0,0,0,0);
   fTextButton2202->SetWrapLength(-1);
   fTextButton2202->Resize(152,48);
   fMainFrame5421->AddFrame(fTextButton2202, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton2202->MoveResize(40,216,152,48);

   ufont = gClient->GetFont("-bitstream-courier 10 pitch-bold-r-normal--17-120-100-100-m-0-adobe-standard");

   // graphics context changes
   GCValues_t vall2203;
   vall2203.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#ff0000",vall2203.fForeground);
   gClient->GetColorByName("#e8e8e8",vall2203.fBackground);
   vall2203.fFillStyle = kFillSolid;
   vall2203.fFont = ufont->GetFontHandle();
   vall2203.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&vall2203, kTRUE);

   ULong_t ucolor=kGreen;        // will reflect user color changes
   TGLabel *fLabel2203 = new TGLabel(fMainFrame5421,"LArPix Monitor",uGC->GetGC(),ufont->GetFontStruct(),kChildFrame,ucolor);
   fLabel2203->SetTextJustify(36);
   fLabel2203->SetMargins(0,0,0,0);
   fLabel2203->SetWrapLength(-1);
   fMainFrame5421->AddFrame(fLabel2203, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2203->MoveResize(16,8,168,19);
   TGLabel *fLabel2204 = new TGLabel(fMainFrame5421,"(c) I. Kreslo Uni-Bern 2018");
   fLabel2204->SetTextJustify(36);
   fLabel2204->SetMargins(0,0,0,0);
   fLabel2204->SetWrapLength(-1);
   fMainFrame5421->AddFrame(fLabel2204, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2204->MoveResize(8,728,176,19);

   fMainFrame6033->AddFrame(fMainFrame5421, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fMainFrame5421->MoveResize(0,0,1211,758);

   fMainFrame921->AddFrame(fMainFrame6033, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fMainFrame6033->MoveResize(0,0,1211,758);

   fMainFrame1984->AddFrame(fMainFrame921, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fMainFrame921->MoveResize(0,0,1211,758);

   fMainFrame2380->AddFrame(fMainFrame1984, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fMainFrame1984->MoveResize(0,0,1211,758);

   fMainFrame2380->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainFrame2380->MapSubwindows();

   fMainFrame2380->Resize(fMainFrame2380->GetDefaultSize());
   fMainFrame2380->MapWindow();
   fMainFrame2380->Resize(1211,758);
c126->SetGrid(1,1);
c127->SetGrid(1,1);
c128->SetGrid(1,1); //chip rates plot
//c125->SetGrid(1,1); //left side channel rate canvas

   gStyle->SetPalette(1);
   p=new PIX0MQ();
   InitChipList(); fListBox2189->Resize(102,665);
   InitHistos();
   ResetHistos();
int x,y;

                  for(int i=0;i<256;i++) if (chipchannel[i]>=0) for(int pix=0;pix<32;pix++) if(cfg[i].channel_mask & (1<<pix))
                  { 
                   GetPixelCoordinates(i, pix, &x, &y);
                   hmaskedraw->Fill(x,y,-1);
                  }

   DrawHistos();
   fTextButton2199->SetCommand("InitChipList()");
fTextButton2170->SetCommand("ResetHistos(); DrawHistos();"); //Reset Histos
fTextButton2200->SetCommand("ConnectStream(); "); //Connect
fTextButton2202->SetCommand("DisconnectStream()"); //Disconnect
fTextButtonQuit->SetCommand("Quit()"); //DisconnectAllAndExit
 fTab2171->SetCommand("DrawHistos()"); //Redraw on tab switch
 fListBox2189->Connect("Selected(Int_t, Int_t)", 0, 0,  "DrawHistos()"); //Redraw on selection change

gStyle->SetOptStat("ne");
sprintf(line,"%s:5555",fTextEntry2201->GetText());
//printf("debug1\n");
if(p->ControlSocketConnect(line)<=0) {printf("Failed!\n"); CStreamConnected=0;}
else {printf("Connected.\n"); CStreamConnected=1;};
//start working thread
worker=new TThread("worker",worker_handle,(void*) 0, TThread::kHighPriority);
worker->Run();
timer1=new TTimer("Timer1Update()",UpdatePeriod);
Cct=time(NULL);
Cct0=Cct;
Elt0=Cct;
timer1->TurnOn();
//start gui updater thread
//gui=new TThread("gui",gui_handle,(void*) 0, TThread::kNormalPriority);
//gui->Run();
//TThread::Ps();

}  
//void *gui_handle( void *ptr)
//{
//  while(1)
//  {
//  } 
//}
void *worker_handle( void *ptr)
{
  uint64_t word[1024];
int First=1;
int recvd;
uint64_t par,pbit;
int adc,pix,chip,tstamp;
int x,y,z;
Float_t val;
Dct=time(NULL);
Dct0=Dct;
int tstamp0;

  TThread::Printf("Starting worker thread..\n");
  while(1)
  {
   if(DStreamConnected==1) 
   {
   recvd=p->DataStreamGetMessage((uint8_t*)(word), 1024, 0);
   if(recvd==3) if(strcmp((char*)(word),"HB")==0) { /*printf("DStream HB received\n"); */ Dct0=time(NULL);}
   if(recvd>=8) Dct0=time(NULL); //likely data received
   for(int wc=0; wc<recvd/8; wc++)
     {
       adc= (word[wc] >> 41) & 0x3FF; //10 bits of ADC field
       adc=(adc >> 1) & 0xFF; //discard sticky LSB and take only 8 significant bits
    //   printf("pix=%lu chip=%lu adc=%d\n",((word[wc] >> 10) & 0x7F),((word[wc] >> 2) & 0xFF), adc);
       if( (word[wc]  & 0x3) != 0x00) continue;  //check if valid data packet
       pix=(word[wc] >> 10) & 0x7F; 
       tstamp=(word[wc] >> 17) & 0xFFFFFF;
//if(pix<0 || pix>31) {TThread::Printf("Data received for unknown pixel #%d! Skipping\n",pix); continue;} //likely comm error 
if(pix<0 || pix>31) continue;
       chip=(word[wc] >> 2) & 0xFF; 
//if( chip<0 || chip>255) {TThread::Printf("Data received for unknown chip #%d! Skipping\n",chip); continue; }//likely comm error 
if( chip<0 || chip>255) continue;
  //     if( chipchannel[chip]<0 || chipchannel[chip]>3 ) {TThread::Printf("Data received for unknown chip #%d! Skipping\n",chip); continue;} //likely comm error
if( chipchannel[chip]<0 || chipchannel[chip]>3 ) continue;
       val=VCM+adc*mVperADC;
//       hp->Fill(pix,adc);
       hp[chip]->Fill(pix,val);
       evc_perchip[chip]++;
       if(chipchannel[chip]==0) dataA++;
       if(chipchannel[chip]==1) dataB++;
       if(chipchannel[chip]==2) dataC++;
       if(chipchannel[chip]==3) dataD++;
       evcount++;
       GetPixelCoordinates(chip, pix, &x, &y);
       hplane->Fill(x,y,val);
       z=tstamp%TPCHDEPTH;
       if(First==1) {tstamp0=tstamp; First=0;}
       if(adc>=htpc_Lthreshold && adc<=htpc_Hthreshold) { htpc->Fill(x,y,z,(char)adc); }
 //      if(adc>=htpc_Lthreshold && adc<=htpc_Hthreshold && abs(tstamp-tstamp0) < 10000) { htpc->Fill(x,y,z,(char)adc); tstamp0=tstamp;}
       hpixrates->Fill(x,y,1);
       par=parity(word[wc] & 0x001FFFFFFFFFFFFF);
       pbit=(word[wc]>>53) & 0x1;
       if (pbit != par) { 
             ParityError=1; 
        if(chipchannel[chip]==0) nparersA++;
       if(chipchannel[chip]==1) nparersB++;
       if(chipchannel[chip]==2) nparersC++;
       if(chipchannel[chip]==3) nparersD++;
         //   printf("word=%llx par=%ld, pbit=%ld\n",word[wc], par,pbit);
          }
    //   printf("word=%llx par=%ld, pbit=%ld\n",word[wc], par,pbit);

     }
   }
  }
}

void Quit()
{
    worker->Kill();
  //  gui->Kill();
    uint64_t word[1024];
    if(DStreamConnected==1) 
    {
    DStreamConnected=0;
    p->DataStreamGetMessage((uint8_t*)(word), 1024, 0); //flush buffer
    p->DataStreamDisconnect();
    printf("Disconnected Data Stream Socket.\n");
    }
    if(CStreamConnected==1)
    {
      CStreamConnected=0;
      p->ControlSocketDisconnect();
      printf("Disconnected Control Socket.\n");
    }
    printf("Exiting.\n");
    exit(0);
}

void DrawHistos()
{
 //  printf("DrawHistos called\n");
   int i=0;
   char ttl[128];
   if(fTab2171->GetCurrent()==0)
   {
   //  printf("Tab %d redraw\n",fTab2171->GetCurrent());
     TGLBEntry * e=fListBox2189->GetSelectedEntry();
     if(!e) return;
   //  printf("Draw ASIC %d\n",i);
     i=atoi(e->GetTitle());
     LastSelectedChip=i;
     c126->cd();
     hp[i]->Draw("colz");
     if(pr) pr->Reset();
     pr=hp[i]->ProfileX();
     pr->Draw("same");
     c126->Update();
   }
/*
   if(fTab2171->GetCurrent()==1)
   {
     c127->cd();
     hplane->Draw("colz");
     hmasked->Draw("boxsame");
     c127->Update();
   }
*/
   if(fTab2171->GetCurrent()==1)
   {
     c127->cd();
     hplaneSIG->Reset();
     hplaneSIG->Add(hplane,1);
     hplaneSIG->Add(hplaneBG, -1);
     hplaneSIG->SetMinimum(0);
     hplaneSIG->Draw("colz");
     hmasked->Draw("boxsame");
     c127->Update();
   }

Float_t factor=1;
   if(fTab2171->GetCurrent()==2)
   {
     c12->cd();
     hpixrates->Draw("colz");
     factor=hpixrates->GetMaximum();
     hmasked->Reset();
     hmasked->Add(hmaskedraw,factor);
     hmasked->Draw("boxsame");
     c12->Update();
   }


   if(fTab2171->GetCurrent()==3)
   {
     c128->cd();
     hmaxrates->Draw("hist");
     hrates->Draw("histsame");
     c128->Update();
   }
/*
   stopsA0=stopsA;
   stopsB0=stopsB;
   stopsC0=stopsC;
   stopsD0=stopsD;
   dataA0=dataA;
   dataB0=dataB;
   dataC0=dataC;
   dataD0=dataD;

uint64_t nparersA;
uint64_t nparersB;
uint64_t nparersC;
uint64_t nparersD;

*/
float lostA,lostB,lostC,lostD;

   if(fTab2171->GetCurrent()==4)
   {
     fTextEditStats->Clear();
     sprintf(StatsText,"Elapsed time,s                     %ld ",time(NULL)-Elt0);
     fTextEditStats->AddLineFast(StatsText);
     sprintf(StatsText,"                                                 A                B                C                D");
     fTextEditStats->AddLineFast(StatsText);
     sprintf(StatsText,"UART Received words since start   %16lu %16lu %16lu %16lu", stopsA,stopsB,stopsC,stopsD);
     fTextEditStats->AddLineFast(StatsText);
     sprintf(StatsText,"UART Received words               %16lu %16lu %16lu %16lu", stopsA-relstopsA,stopsB-relstopsB,stopsC-relstopsC,stopsD-relstopsD);
     fTextEditStats->AddLineFast(StatsText);
     sprintf(StatsText,"Data words on 0MQ                 %16lu %16lu %16lu %16lu",dataA,dataB,dataC,dataD);
     fTextEditStats->AddLineFast(StatsText);
     if((stopsA-relstopsA) > dataA) lostA=100.0*((stopsA-relstopsA)-dataA)/(stopsA-relstopsA); else lostA=0;
     if((stopsB-relstopsB) > dataB) lostB=100.0*((stopsB-relstopsB)-dataB)/(stopsB-relstopsB); else lostB=0;
     if((stopsC-relstopsC) > dataC) lostC=100.0*((stopsC-relstopsC)-dataC)/(stopsC-relstopsC); else lostC=0;
     if((stopsD-relstopsD) > dataD) lostD=100.0*((stopsD-relstopsD)-dataD)/(stopsD-relstopsD); else lostD=0;
 //    sprintf(StatsText,"Lost words                      %16.2f%%%16.2f%%%16.2f%%%16.2f%%",100.0*((stopsA-relstopsA)-dataA)/(stopsA-relstopsA),100.0*((stopsB-relstopsB)-dataB)/(stopsB-relstopsB),100.0*((stopsC-relstopsC)-dataC)/(stopsC-relstopsC),100.0*((stopsD-relstopsD)-dataD)/(stopsD-relstopsD));
     sprintf(StatsText,"Lost words                      %16.2f%%%16.2f%%%16.2f%%%16.2f%%",lostA,lostB,lostC,lostD);
     fTextEditStats->AddLineFast(StatsText);
     sprintf(StatsText,"Parity errors                     %7lu(%6.4f%%) %7lu(%6.4f%%) %7lu(%6.4f%%) %7lu(%6.4f%%)",nparersA,100.0*nparersA/dataA,nparersB,100.0*nparersB/dataB,nparersC,100.0*nparersC/dataC,nparersD,100.0*nparersD/dataD);
     fTextEditStats->AddLineFast(StatsText);
     fTextEditStats->Update();
   }


   if(fTab2171->GetCurrent()==5)
   {
     c123->cd();
     htpc->Draw("");
     c123->Update();
   }


}

void InitChipList()
{
  int chipIndex;
  int x,y;
  if( ReadChipList(fTextEntry2168->GetText())==0)
  {
    sprintf(line,"ASICs list reading from %s failed. Keeping old list.",fTextEntry2168->GetText());
    fStatusBar->SetText(line,0);
    return;
  }
chipIndex=0;
fListBox2189->RemoveAll();
for(int i=0;i<256;i++)
{
    if(chipchannel[i]>=0) { 
                chipIndex++;  
		sprintf(line,"%d",i); 
		//fListBox2189->AddEntry(line,chipIndex);
		fListBox2189->NewEntry(line);
                  cfg[i].chip=i;
                  cfg[i].chan=chipchannel[i];
                  cfg[i].ReadText();
		}
evc_perchip[i]=0;
}
p->InitChipList(fTextEntry2168->GetText());
sprintf(line,"ASICs list initialized with %d ASICs.",chipIndex);
fStatusBar->SetText(line,0);

}


void ConnectStream()
{
sprintf(line,"%s:5556",fTextEntry2201->GetText());
printf("Connecting data stream at %s..\n",line);
if(p->DataStreamConnect(line)<=0) { printf("Failed!\n"); DStreamConnected=0; return;}
printf("Connected.\n"); 
DStreamConnected=1;
fTextButton2200->SetDown(1);

//if(DStreamConnected==1) timer1->TurnOn();
//else timer1->TurnOff();
}

void DisconnectStream()
{
sprintf(line,"%s:5556",fTextEntry2201->GetText());
if(DStreamConnected==1)
 {
 DStreamConnected=0;
 p->DataStreamDisconnect();
 printf("Data stream at %s disconnected.\n",line);
 }
fTextButton2200->SetDown(0);

//if(DStreamConnected==1) timer1->TurnOn();
//else timer1->TurnOff();

}


void Timer1Update()
{
//  printf("Timer1 update called\n");
//heartbeat handling
Dct=time(NULL);
if(Dct-Dct0 > 2) {} //printf("No heartbeat from Data Stream!\n");
else { if(fIconHBD->GetHeight()==100) fIconHBD->MoveResize(102,320,80,80); else fIconHBD->MoveResize(102,320,100,100); }
//printf("debug1\n");

if(p->ControlSocketPing()==1) Cct0=time(NULL);
//printf("debug2\n");
Cct=time(NULL);
if(Cct-Cct0 > 2) {} //printf("No heartbeat from Command Stream!\n");
else { if(fIconHBC->GetHeight()==100) fIconHBC->MoveResize(2,320,80,80); else fIconHBC->MoveResize(2,320,100,100); }

if(ParityError==1)
{   
fLabelPE->SetText("Parity ERR");
fLabelPE->SetBackgroundColor(0x990000); 
}
else 
{
fLabelPE->SetText("Parity OK");
fLabelPE->SetBackgroundColor(0x66ff66); 
}

if(ParityError==1 && DStreamConnected==1) 
{
  ParityError=0;
}

  rate=1.0*(evcount-evcount_prev)/UpdatePeriod; //kHz
  evcount_prev=evcount;
  if(DStreamConnected==1)
  {
  hrates->Reset();
  for(int i=0;i<nchips;i++) 
      { 
        hrates->Fill(i,1.0*(evc_perchip[chipids[i]]-evc_perchip_prev[chipids[i]])/UpdatePeriod); 
        evc_perchip_prev[chipids[i]]=evc_perchip[chipids[i]]; 
        if(hrates->GetBinContent(i+1) > hmaxrates->GetBinContent(i+1)) hmaxrates->SetBinContent(i+1, hrates->GetBinContent(i+1));
      }
  DrawHistos();
  sprintf(line,"Data stream connected, %f kEvents/s at ZMQ backend",rate);
  fStatusBar->GetBarPart(0)->SetBackgroundColor(0x99ff99);
  fStatusBar->SetText(line,0);
  }
  else { fStatusBar->GetBarPart(0)->SetBackgroundColor(0xaaaaaa); fStatusBar->SetText("Data stream disconnected",0);   }

//always update rates
   hABCD_uart_starts->Reset();
   hABCD_uart_stops->Reset();
   hABCD_data->Reset();
  if(CStreamConnected==1)
  {
   p->ControlSocketGetStats(0, &startsA, &stopsA);  
   p->ControlSocketGetStats(1, &startsB, &stopsB);  
   p->ControlSocketGetStats(2, &startsC, &stopsC);  
   p->ControlSocketGetStats(3, &startsD, &stopsD);
   hABCD_uart_starts->Fill(0.,1.0*(startsA-startsA0)/UpdatePeriod);
   hABCD_uart_starts->Fill(1.,1.0*(startsB-startsB0)/UpdatePeriod);
   hABCD_uart_starts->Fill(2.,1.0*(startsC-startsC0)/UpdatePeriod);
   hABCD_uart_starts->Fill(3.,1.0*(startsD-startsD0)/UpdatePeriod);
   hABCD_uart_stops->Fill(0.,1.0*(stopsA-stopsA0)/UpdatePeriod);
   hABCD_uart_stops->Fill(1.,1.0*(stopsB-stopsB0)/UpdatePeriod);
   hABCD_uart_stops->Fill(2.,1.0*(stopsC-stopsC0)/UpdatePeriod);
   hABCD_uart_stops->Fill(3.,1.0*(stopsD-stopsD0)/UpdatePeriod);
   hABCD_data->Fill(0.,1.0*(dataA-dataA0)/UpdatePeriod);
   hABCD_data->Fill(1.,1.0*(dataB-dataB0)/UpdatePeriod);
   hABCD_data->Fill(2.,1.0*(dataC-dataC0)/UpdatePeriod);
   hABCD_data->Fill(3.,1.0*(dataD-dataD0)/UpdatePeriod);
   startsA0=startsA;
   startsB0=startsB;
   startsC0=startsC;
   startsD0=startsD;
   stopsA0=stopsA;
   stopsB0=stopsB;
   stopsC0=stopsC;
   stopsD0=stopsD;
   dataA0=dataA;
   dataB0=dataB;
   dataC0=dataC;
   dataD0=dataD;
   }
   c125->cd();
   hABCD_uart_starts->Draw("hist");
   hABCD_uart_stops->Draw("histsame");
   hABCD_data->Draw("histsame");
   c125->Update();
   
}


void PrintPix()
{
   // get event information
   int event = gPad->GetEvent();
   int px    = gPad->GetEventX();
   int py    = gPad->GetEventY();

   // some magic to get the coordinates...
   double xd = gPad->AbsPixeltoX(px);
   double yd = gPad->AbsPixeltoY(py);
   int x = gPad->PadtoX(xd);
   int y = gPad->PadtoY(yd);
   int xp,yp;
   
   if (event==1) { // left mouse button click
    for(int chip=0; chip<256; chip++) if(chipchannel[chip]>-1) for(int pix=0; pix<32; pix++)
     {
       GetPixelCoordinates(chip, pix, &xp, &yp);
       if(xp==x && yp==y) { printf("Pixel clicked at (%d %d) => UART %d    ASIC %d    pix %d\n",x,y,chipchannel[chip],chip,pix); return;}
     }
    printf("Canvas clicked at (%d %d) but no corresponding physical pixel found..\n",x,y);
   }
  
}


