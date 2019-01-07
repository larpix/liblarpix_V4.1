// Mainframe macro generated from application: /home/kreslo/root6.10.08/bin/root.exe
// By ROOT version 6.10/08 on 2018-12-12 13:27:11

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
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
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
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
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#include "PIX0MQ.hxx"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TProfile.h"

#include "Riostream.h"

#include "TThread.h"
#include "TTimer.h"
#include <stdlib.h>
#include <time.h>


TGListBox *fListBox711;
TGTextEntry *fTextEntry2201; //IP:PORT
TGTextEntry *fTextEntry2168; //ASICs list file
TGTextButton *fTextButton2199; //Read ASICs list
TGTextButton *fTextButton2191; //SendAllConfigs
TGTextButton *fTextButton2; //MaskAllPixels
TGTextButton *fTextButton3; //set
TGTextButton *fTextButton4; //set
TGNumberEntry *fFreqUart;
TGNumberEntry *fFreqTrig;

TGTextButton *fTextButtonQuit; //Quit

TGTextButton *fCButtonGlobTrim; 
TGTextButton *fCButtonSendOne; 
TGTextButton *fCButtonSaveToFile; 
TGTextButton *fCButtonReadFile;
TGTextButton *fCButtonScanGTH;
TGTextButton *fCButtonScanPTRIM;

TGCheckButton* fCbM[32]; //mask
TGCheckButton* fCbT[32]; //ext trigger
TGCheckButton* fCbTpe[32]; //test pulse enable
TGCheckButton* fCbMon[32]; //analog monitor select


PIX0MQ *p;
  char line[128];
LarpixConfig cfg[256];
int CStreamConnected=0;
int ChipSelected=-1;
   TGNumberEntry *fN[26];
   TGNumberEntry *fTrim[32];
   TGNumberEntry *fGlobTrim;

void InitChipList()
{
int chipIndex=0;
chipIndex=p->InitChipList(fTextEntry2168->GetText());
sprintf(line,"ASICs list initialized with %d ASICs.",chipIndex);
fListBox711->RemoveAll();
for(int i=0; i<256; i++) if(p->chipchannel[i]>-1)
 {
  sprintf(line,"%d",i); 
  fListBox711->NewEntry(line);
  cfg[i].chip=i;
  cfg[i].chan=p->chipchannel[i];
  cfg[i].ReadText();
 }
 else cfg[i].chan=255; //invalid config
}

void Quit()
{
    if(CStreamConnected==1)
    {
      CStreamConnected=0;
      p->ControlSocketDisconnect();
      printf("Disconnected Control Socket.\n");
    }
    printf("Exiting.\n");
    exit(0);
}

void SendAllConfigs()
{
printf("Configuring ASICs from the list..\n");
for(int i=0; i<256; i++) if(p->chipchannel[i]>-1) p->ConfigureChip(&cfg[i]);
printf("Done\n");
}

void UpdateConfigFromFields()
{

  cfg[ChipSelected].chip=fN[0]->GetNumber(); 
  cfg[ChipSelected].tile=fN[1]->GetNumber(); 
  cfg[ChipSelected].chan=fN[2]->GetNumber();
  cfg[ChipSelected].threshold_global=fN[3]->GetIntNumber(); 

  for(int i=0; i<32; i++) cfg[ChipSelected].pixel_trim_dac[i]=fTrim[i]->GetIntNumber();
 
  cfg[ChipSelected].csa_gain=fN[8]->GetNumber(); 
  cfg[ChipSelected].csa_bypass_enable=fN[9]->GetNumber(); 
  cfg[ChipSelected].csa_bypass_select=fN[10]->GetNumber(); 
  cfg[ChipSelected].internal_bias=fN[11]->GetNumber(); 
  cfg[ChipSelected].internal_bypass=fN[12]->GetNumber(); 
  cfg[ChipSelected].csa_monitor_select=fN[13]->GetNumber(); 
  cfg[ChipSelected].csa_testpulse_enable=fN[14]->GetNumber(); 
  cfg[ChipSelected].csa_testpulse_dac=fN[15]->GetNumber(); 
  cfg[ChipSelected].test_mode=fN[16]->GetNumber(); 
  cfg[ChipSelected].cross_trigger=fN[17]->GetNumber(); 
  cfg[ChipSelected].enable_periodic_reset=fN[18]->GetNumber(); 
  cfg[ChipSelected].reset_cycles=fN[19]->GetNumber(); 
  cfg[ChipSelected].fifo_diagnostic_en=fN[20]->GetNumber(); 
  cfg[ChipSelected].sample_cycles=fN[21]->GetNumber(); 
  cfg[ChipSelected].test_burst_length=fN[22]->GetNumber(); 
  cfg[ChipSelected].adc_burst_length=fN[23]->GetNumber(); 
  cfg[ChipSelected].channel_mask=fN[24]->GetNumber(); 
  cfg[ChipSelected].external_trigger_mask=fN[25]->GetNumber(); 
}

void UpdateFields()
{
     TGLBEntry * e=fListBox711->GetSelectedEntry();
     if(!e) return;
     ChipSelected=atoi(e->GetTitle());
  fN[0]->SetIntNumber(cfg[ChipSelected].chip); 
  fN[1]->SetIntNumber(cfg[ChipSelected].tile); 
  fN[2]->SetIntNumber(cfg[ChipSelected].chan); 
  fN[3]->SetIntNumber(cfg[ChipSelected].threshold_global); 

  for(int i=0; i<32; i++) fTrim[i]->SetIntNumber(cfg[ChipSelected].pixel_trim_dac[i]);
  fGlobTrim->SetIntNumber(cfg[ChipSelected].pixel_trim_dac[0]);
 
  fN[8]->SetIntNumber(cfg[ChipSelected].csa_gain); 
  fN[9]->SetIntNumber(cfg[ChipSelected].csa_bypass_enable); 
  fN[10]->SetIntNumber(cfg[ChipSelected].csa_bypass_select); 
  fN[11]->SetIntNumber(cfg[ChipSelected].internal_bias); 
  fN[12]->SetIntNumber(cfg[ChipSelected].internal_bypass); 
  fN[13]->SetIntNumber(cfg[ChipSelected].csa_monitor_select); 
     for(int i=0; i<32; i++) fCbMon[i]->SetOn( (cfg[ChipSelected].csa_monitor_select >> i) & 1);
  fN[14]->SetIntNumber(cfg[ChipSelected].csa_testpulse_enable); 
     for(int i=0; i<32; i++) fCbTpe[i]->SetOn( (cfg[ChipSelected].csa_testpulse_enable >> i) & 1);
  fN[15]->SetIntNumber(cfg[ChipSelected].csa_testpulse_dac); 
  fN[16]->SetIntNumber(cfg[ChipSelected].test_mode); 
  fN[17]->SetIntNumber(cfg[ChipSelected].cross_trigger); 
  fN[18]->SetIntNumber(cfg[ChipSelected].enable_periodic_reset); 
  fN[19]->SetIntNumber(cfg[ChipSelected].reset_cycles); 
  fN[20]->SetIntNumber(cfg[ChipSelected].fifo_diagnostic_en); 
  fN[21]->SetIntNumber(cfg[ChipSelected].sample_cycles); 
  fN[22]->SetIntNumber(cfg[ChipSelected].test_burst_length); 
  fN[23]->SetIntNumber(cfg[ChipSelected].adc_burst_length); 
  fN[24]->SetIntNumber(cfg[ChipSelected].channel_mask); 
     for(int i=0; i<32; i++) fCbM[i]->SetOn( (cfg[ChipSelected].channel_mask >> i) & 1);
  fN[25]->SetIntNumber(cfg[ChipSelected].external_trigger_mask); 
     for(int i=0; i<32; i++) fCbT[i]->SetOn( (cfg[ChipSelected].external_trigger_mask >> i) & 1);

}

/*
60 	'ASIC ID (0-255)
0 	'at tile Nr
0 	'connected to UART channel (0-3)
250 	'threshold_global (0-255)
31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 31 	'pixel_trim_dac (0-31)
1 	'csa_gain: 0=45uV/e, 1=4uV/e
0 	'csa_bypass_enable: 0 or 1; 1-Enables injection of ADC_TEST_IN to selected ADC
0x00000000 	'csa_bypass_select - 32 bits, HIGH bits select channel for injection 
0 	'internal_bias: 0 or 1; UNUSED
1 	'internal_bypass: 0 or 1; 1-Internal capacitor(?) on power line
0x00000000 	'csa_monitor_select: 32 bits, only one HIGH to select channel to connect to analog MONITOR output
0xffffffff 	'csa_testpulse_enable:  32 bits, LOW connects chan inputs to test pulse generator 
0 	'csa_testpulse_dac (0-255)
0 	'test_mode: 0 - normal DAQ, 1 - UART test, 2 - FIFO burst
0 	'cross_trigger: 0 or 1, 1- all not masked channels digitize when one is triggered
1 	'enable_periodic_reset: 0 or 1, 1=periodic reset after reset_cycles
4096 	'reset_cycles (0-2^24), in Master clock/4 cycles
0 	'fifo_diagnostic_en: 0 or 1, Embeds FIFO counter to output event
1 	'sample_cycles, 0-255. 0 or 1 - one system clock period used for sampling
255 	'test_burst_length, 0-2^16, for FIFO burst test
0 	'adc_burst_length: 0-255, number of ADC conversions per pix hit, 0 or 1 = one sample per hit
0x0 	'channel_mask: 32 bits, HIGH to disable channel
0x0 	'external_trigger_mask: 32 bits, HIGH to disable external trigger to channel
*/

void Configurator()
{

   // main frame
   TGMainFrame *fMainFrame1169 = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMainFrame1169->SetName("fMainFrame1169");
   fMainFrame1169->SetWindowName("LarPix configurator");
   fMainFrame1169->SetLayoutBroken(kTRUE);

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





   // horizontal frame
   TGHorizontalFrame *fHorizontalFrame607 = new TGHorizontalFrame(fMainFrame1169,180,755,kHorizontalFrame | kRaisedFrame);
   fHorizontalFrame607->SetName("fHorizontalFrame607");
   fHorizontalFrame607->SetLayoutBroken(kTRUE);


   ULong_t ucolor=kGreen;        // will reflect user color changes
   TGLabel *fLabel2203 = new TGLabel(fHorizontalFrame607,"LArPix Configer",uGC->GetGC(),ufont->GetFontStruct(),kChildFrame,ucolor);


   fLabel2203->SetTextJustify(36);
   fLabel2203->SetMargins(0,0,0,0);
   fLabel2203->SetWrapLength(-1);
   fHorizontalFrame607->AddFrame(fLabel2203, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2203->MoveResize(2,8,168,19);
   TGLabel *fLabel2204 = new TGLabel(fHorizontalFrame607,"(c) I. Kreslo Uni-Bern 2018");
   fLabel2204->SetTextJustify(36);
   fLabel2204->SetMargins(0,0,0,0);
   fLabel2204->SetWrapLength(-1);
   fHorizontalFrame607->AddFrame(fLabel2204, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2204->MoveResize(2,728,176,19);



   TGLabel *fLabel2167 = new TGLabel(fHorizontalFrame607,"IP");
   fLabel2167->SetTextJustify(36);
   fLabel2167->SetMargins(0,0,0,0);
   fLabel2167->SetWrapLength(-1);
   fHorizontalFrame607->AddFrame(fLabel2167, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2167->MoveResize(0,40,64,19);
   fTextEntry2201 = new TGTextEntry(fHorizontalFrame607, new TGTextBuffer(31),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
   fTextEntry2201->SetMaxLength(4096);
   fTextEntry2201->SetAlignment(kTextLeft);
//   fTextEntry2201->SetText("192.168.2.102");
   fTextEntry2201->SetText("130.92.139.27");
   fTextEntry2201->Resize(128,fTextEntry2201->GetDefaultHeight());
   fHorizontalFrame607->AddFrame(fTextEntry2201, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry2201->MoveResize(64,40,100,23);



   fTextEntry2168 = new TGTextEntry(fHorizontalFrame607, new TGTextBuffer(31),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
   fTextEntry2168->SetMaxLength(4096);
   fTextEntry2168->SetAlignment(kTextLeft);
   fTextEntry2168->SetText("CONF/chips.list");
   fTextEntry2168->Resize(128,fTextEntry2168->GetDefaultHeight());
   fHorizontalFrame607->AddFrame(fTextEntry2168, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextEntry2168->MoveResize(64,72,100,23);
   TGLabel *fLabel2169 = new TGLabel(fHorizontalFrame607,"ASIC list");
   fLabel2169->SetTextJustify(36);
   fLabel2169->SetMargins(0,0,0,0);
   fLabel2169->SetWrapLength(-1);
   fHorizontalFrame607->AddFrame(fLabel2169, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel2169->MoveResize(0,72,64,19);

   fTextButton2199 = new TGTextButton(fHorizontalFrame607,"Read ASIC list",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton2199->SetTextJustify(36);
   fTextButton2199->SetMargins(0,0,0,0);
   fTextButton2199->SetWrapLength(-1);
   fTextButton2199->Resize(152,48);
   fHorizontalFrame607->AddFrame(fTextButton2199, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton2199->MoveResize(10,104,152,38);

   fTextButton2191 = new TGTextButton(fHorizontalFrame607,"Send configs to all ASICs",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton2191->SetTextJustify(36);
   fTextButton2191->SetMargins(0,0,0,0);
   fTextButton2191->SetWrapLength(-1);
   fTextButton2191->Resize(152,48);
   fHorizontalFrame607->AddFrame(fTextButton2191, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton2191->MoveResize(10,144,152,38);

   fTextButton2 = new TGTextButton(fHorizontalFrame607,"Mask all ASICs",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton2->SetTextJustify(36);
   fTextButton2->SetMargins(0,0,0,0);
   fTextButton2->SetWrapLength(-1);
   fTextButton2->Resize(152,48);
   fHorizontalFrame607->AddFrame(fTextButton2, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton2->MoveResize(10,184,152,38);


   TGLabel *fLabelUart = new TGLabel(fHorizontalFrame607,"Set CLOCK2X, MHz");
   fLabelUart->SetTextJustify(36);
   fLabelUart->SetMargins(0,0,0,0);
   fLabelUart->SetWrapLength(-1);
   fHorizontalFrame607->AddFrame(fLabelUart, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabelUart->MoveResize(10,264,162,19);


fFreqUart=new TGNumberEntry(fHorizontalFrame607, (Double_t)10,2,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEAPositive,TGNumberFormat::kNELLimitMinMax,1,20);
fHorizontalFrame607->AddFrame(fFreqUart, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
fFreqUart->MoveResize(10,284,100,20);


   fTextButton3 = new TGTextButton(fHorizontalFrame607,"Set",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton3->SetTextJustify(36);
   fTextButton3->SetMargins(0,0,0,0);
   fTextButton3->SetWrapLength(-1);
   fTextButton3->Resize(152,48);
   fHorizontalFrame607->AddFrame(fTextButton3, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton3->MoveResize(122,284,40,20);
   fTextButton3->SetCommand("SetUARTFrequency();");

   TGLabel *fLabelTest = new TGLabel(fHorizontalFrame607,"Set Test Trigger divider");
   fLabelTest->SetTextJustify(36);
   fLabelTest->SetMargins(0,0,0,0);
   fLabelTest->SetWrapLength(-1);
   fHorizontalFrame607->AddFrame(fLabelTest, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabelTest->MoveResize(10,324,162,19);


fFreqTrig=new TGNumberEntry(fHorizontalFrame607, (Double_t)10000,7,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1000000);
fHorizontalFrame607->AddFrame(fFreqTrig, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
fFreqTrig->MoveResize(10,344,100,20);


   fTextButton4 = new TGTextButton(fHorizontalFrame607,"Set",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton4->SetTextJustify(36);
   fTextButton4->SetMargins(0,0,0,0);
   fTextButton4->SetWrapLength(-1);
   fTextButton4->Resize(152,48);
   fHorizontalFrame607->AddFrame(fTextButton4, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButton4->MoveResize(122,344,40,20);
   fTextButton4->SetCommand("SetTestTrigFrequency();");




   fTextButtonQuit = new TGTextButton(fHorizontalFrame607,"Quit",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButtonQuit->SetTextJustify(36);
   fTextButtonQuit->SetMargins(0,0,0,0);
   fTextButtonQuit->SetWrapLength(-1);
   fTextButtonQuit->Resize(152,48);
   fHorizontalFrame607->AddFrame(fTextButtonQuit, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fTextButtonQuit->MoveResize(10,472,152,38);



   fMainFrame1169->AddFrame(fHorizontalFrame607, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fHorizontalFrame607->MoveResize(2,2,180,755);

   // "fGroupFrame820" group frame
   TGGroupFrame *fGroupFrame820 = new TGGroupFrame(fMainFrame1169,"ASIC Configurator");
   fGroupFrame820->SetLayoutBroken(kTRUE);

   // list box
   fListBox711 = new TGListBox(fGroupFrame820,-1,kSunkenFrame);
   fListBox711->SetName("fListBox711");
   fListBox711->AddEntry("Entry 1",0);
   fListBox711->AddEntry("Entry 2",1);
   fListBox711->AddEntry("Entry 3",2);
   fListBox711->AddEntry("Entry 4",3);
   fListBox711->AddEntry("Entry 5",4);
   fListBox711->AddEntry("Entry 6",5);
   fListBox711->AddEntry("Entry 7",6);
   fListBox711->Resize(152,648);
   fGroupFrame820->AddFrame(fListBox711, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fListBox711->MoveResize(5,20,60,720);
   fListBox711->Connect("Selected(Int_t, Int_t)", 0, 0,  "ResetMonAndUpdateFields();"); //Redraw on selection change


   TGLabel *fL[26];
   fL[0]=new TGLabel(fGroupFrame820,"ASIC ID"); 
   fL[1]=new TGLabel(fGroupFrame820,"Tile"); 
   fL[2]=new TGLabel(fGroupFrame820,"UART CH"); 
   fL[3]=new TGLabel(fGroupFrame820,"threshold_global"); 
   fL[4]=new TGLabel(fGroupFrame820,"pixel_trim_dac (0-7)"); 
   fL[5]=new TGLabel(fGroupFrame820,"pixel_trim_dac (8-15)"); 
   fL[6]=new TGLabel(fGroupFrame820,"pixel_trim_dac (16-23)"); 
   fL[7]=new TGLabel(fGroupFrame820,"pixel_trim_dac (24-31)"); 
   fL[8]=new TGLabel(fGroupFrame820,"csa_gain"); 
   fL[9]=new TGLabel(fGroupFrame820,"csa_bypass_enable"); 
   fL[10]=new TGLabel(fGroupFrame820,"csa_bypass_select"); 
   fL[11]=new TGLabel(fGroupFrame820,"internal_bias"); 
   fL[12]=new TGLabel(fGroupFrame820,"internal_bypass"); 
   fL[13]=new TGLabel(fGroupFrame820,"csa_monitor_select"); 
   fL[14]=new TGLabel(fGroupFrame820,"csa_testpulse_enable"); 
   fL[15]=new TGLabel(fGroupFrame820,"csa_testpulse_dac"); 
   fL[16]=new TGLabel(fGroupFrame820,"test_mode"); 
   fL[17]=new TGLabel(fGroupFrame820,"cross_trigger"); 
   fL[18]=new TGLabel(fGroupFrame820,"enable_periodic_reset"); 
   fL[19]=new TGLabel(fGroupFrame820,"reset_cycles"); 
   fL[20]=new TGLabel(fGroupFrame820,"fifo_diagnostic_en"); 
   fL[21]=new TGLabel(fGroupFrame820,"sample_cycles"); 
   fL[22]=new TGLabel(fGroupFrame820,"test_burst_length"); 
   fL[23]=new TGLabel(fGroupFrame820,"adc_burst_length"); 
   fL[24]=new TGLabel(fGroupFrame820,"channel_mask"); 
   fL[25]=new TGLabel(fGroupFrame820,"external_trigger_mask"); 

   fN[0]=new TGNumberEntry(fGroupFrame820, (Double_t)0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,255);
   fN[1]=new TGNumberEntry(fGroupFrame820, (Double_t)0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,255);
   fN[2]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,3);
   fN[3]=new TGNumberEntry(fGroupFrame820, (Double_t)0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,255);
 //  fN[4]=new TGNumberEntry(fGroupFrame820, (Double_t)0,2,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,31); //not used, instead array here!
 //  fN[5]=new TGNumberEntry(fGroupFrame820, (Double_t)0,2,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,31); //not used, instead array here!
 //  fN[6]=new TGNumberEntry(fGroupFrame820, (Double_t)0,2,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,31); //not used, instead array here!
 //  fN[7]=new TGNumberEntry(fGroupFrame820, (Double_t)0,2,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,31); //not used, instead array here!
   fN[8]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1);
   fN[9]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1);
   fN[10]=new TGNumberEntry(fGroupFrame820, (Double_t)0,8,-1,TGNumberFormat::kNESHex,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,0xffffffff);
   fN[11]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1);
   fN[12]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1);
   fN[13]=new TGNumberEntry(fGroupFrame820, (Double_t)0,8,-1,TGNumberFormat::kNESHex,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,0xffffffff);
   fN[14]=new TGNumberEntry(fGroupFrame820, (Double_t)0,8,-1,TGNumberFormat::kNESHex,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,0xffffffff);
   fN[15]=new TGNumberEntry(fGroupFrame820, (Double_t)0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,255);
   fN[16]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,2);
   fN[17]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1);
   fN[18]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1);
   fN[19]=new TGNumberEntry(fGroupFrame820, (Double_t)0,6,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,0xffffff);
   fN[20]=new TGNumberEntry(fGroupFrame820, (Double_t)0,1,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,1);
   fN[21]=new TGNumberEntry(fGroupFrame820, (Double_t)0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,255);
   fN[22]=new TGNumberEntry(fGroupFrame820, (Double_t)0,5,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,0xffff);
   fN[23]=new TGNumberEntry(fGroupFrame820, (Double_t)0,3,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,255);
   fN[24]=new TGNumberEntry(fGroupFrame820, (Double_t)0,8,-1,TGNumberFormat::kNESHex,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,0xffffffff);
   fN[25]=new TGNumberEntry(fGroupFrame820, (Double_t)0,8,-1,TGNumberFormat::kNESHex,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,0xffffffff);

   for(int i=0;i<32;i++) fTrim[i]=new TGNumberEntry(fGroupFrame820, (Double_t)0,2,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,31);
   fGlobTrim=new TGNumberEntry(fGroupFrame820, (Double_t)0,2,-1,TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative,TGNumberFormat::kNELLimitMinMax,0,31);

   for(int i=0;i<26;i++)
   {
     if(i>3 && i<8)
     {
       for(int j=0;j<8;j++) 
       { 
         fGroupFrame820->AddFrame(fTrim[j+8*(i-4)], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
         fTrim[j+8*(i-4)]->MoveResize(270+j*40,30+24*i,40,19);
       }
       continue;
     }  
     fGroupFrame820->AddFrame(fN[i], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
     fN[i]->MoveResize(270,30+24*i,100,19);
   }
 
   fGroupFrame820->AddFrame(fGlobTrim, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGlobTrim->MoveResize(270+8*40+20,30+24*4,40,19);

   fCButtonGlobTrim= new TGTextButton(fGroupFrame820,"Set all pixel_trim_dac",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fGroupFrame820->AddFrame(fCButtonGlobTrim, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCButtonGlobTrim->MoveResize(270+8*40+20,30+24*5,130,19);
   fCButtonGlobTrim->SetCommand("if(ChipSelected>-1) for(int u=0; u<32;u++) cfg[ChipSelected].pixel_trim_dac[u]=fGlobTrim->GetNumber();UpdateFields();");
  
   for(int i=0;i<26;i++)
   {
     fGroupFrame820->AddFrame(fL[i], new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
     fL[i]->SetTextJustify(kTextRight);
     fL[i]->MoveResize(100,30+24*i,157,19);
   }

   fCButtonReadFile = new TGTextButton(fGroupFrame820,"Read config from default file",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fGroupFrame820->AddFrame(fCButtonReadFile, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCButtonReadFile->MoveResize(270+4*40+20,342+24+20,230,30);
   fCButtonReadFile->SetCommand("if(ChipSelected>-1) cfg[ChipSelected].ReadText(); UpdateFields();");

fCButtonScanGTH = new TGTextButton(fGroupFrame820,"Scan and set global threshold",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fGroupFrame820->AddFrame(fCButtonScanGTH, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCButtonScanGTH->MoveResize(270+4*40+20,342+24+20+40,230,30);
   fCButtonScanGTH->SetCommand("if(ChipSelected>-1) ScanGlobalThreshold(ChipSelected); UpdateFields();");

fCButtonScanPTRIM = new TGTextButton(fGroupFrame820,"Scan and set pixel trim DACs",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fGroupFrame820->AddFrame(fCButtonScanPTRIM, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCButtonScanPTRIM->MoveResize(270+4*40+20,342+24+20+80,230,30);
   fCButtonScanPTRIM->SetCommand("if(ChipSelected>-1) ScanFineThresholds(ChipSelected); UpdateFields();");



   fCButtonSaveToFile = new TGTextButton(fGroupFrame820,"Save current config with default name",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fGroupFrame820->AddFrame(fCButtonSaveToFile, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCButtonSaveToFile->MoveResize(270+4*40+20,30+24*22-40,230,30);
   fCButtonSaveToFile->SetCommand("if(ChipSelected>-1) {UpdateConfigFromFields(); cfg[ChipSelected].WriteText(); }");

   fCButtonSendOne = new TGTextButton(fGroupFrame820,"Send config to ASIC (no saving)",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fGroupFrame820->AddFrame(fCButtonSendOne, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fCButtonSendOne->MoveResize(270+4*40+20,30+24*22,230,30);
   fCButtonSendOne->SetCommand("if(ChipSelected>-1)  {UpdateConfigFromFields(); p->ConfigureChip(&cfg[ChipSelected]); }");

   for(int i=0; i<32; i++) { 
         fCbM[31-i] = new TGCheckButton(fGroupFrame820);
         fCbM[31-i]->MoveResize(370+i*14+int(i/8)*5,610,14,14);
         fCbM[31-i]->SetCommand("UpdateMaskFromCbM();");
         fCbT[31-i] = new TGCheckButton(fGroupFrame820);
         fCbT[31-i]->MoveResize(370+i*14+int(i/8)*5,610+24,14,14);
         fCbT[31-i]->SetCommand("UpdateTrigMaskFromCbT();");

         fCbTpe[31-i]= new TGCheckButton(fGroupFrame820);
         fCbTpe[31-i]->MoveResize(370+i*14+int(i/8)*5,342+24,14,14);
         fCbTpe[31-i]->SetCommand("UpdateTpeFromCb();");
         fCbMon[31-i]= new TGCheckButton(fGroupFrame820,(const char*)0,31-i);
         fCbMon[31-i]->MoveResize(370+i*14+int(i/8)*5,342,14,14);
         fCbMon[31-i]->SetCommand("UpdateMonFromCb($PARM1);");

   }


   fGroupFrame820->SetLayoutManager(new TGVerticalLayout(fGroupFrame820));
   fGroupFrame820->Resize(938,757);
   fMainFrame1169->AddFrame(fGroupFrame820, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGroupFrame820->MoveResize(180,0,938,757);

   fMainFrame1169->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainFrame1169->MapSubwindows();

   fMainFrame1169->Resize(fMainFrame1169->GetDefaultSize());
   fMainFrame1169->MapWindow();
   fMainFrame1169->Resize(1100,759);

   fTextButton2199->SetCommand("InitChipList()");
   fTextButton2191->SetCommand("SendAllConfigs()");
   fTextButton2->SetCommand("MaskAllChips()");
   fTextButtonQuit->SetCommand("Quit()"); //DisconnectAllAndExit
   gStyle->SetPalette(1);
   p=new PIX0MQ();
   InitChipList(); 


sprintf(line,"%s:5555",fTextEntry2201->GetText());
if(p->ControlSocketConnect(line)<=0) {printf("Failed!\n"); CStreamConnected=0;}
else {printf("Connected.\n"); CStreamConnected=1;};


}  


void UpdateMaskFromCbM()
{
     cfg[ChipSelected].channel_mask=0;
     for(int i=0; i<32; i++) cfg[ChipSelected].channel_mask+= ((fCbM[i]->IsOn()) << i);
     UpdateFields();
}
void UpdateTrigMaskFromCbT()
{
     cfg[ChipSelected].external_trigger_mask=0;
     for(int i=0; i<32; i++) cfg[ChipSelected].external_trigger_mask+= ((fCbT[i]->IsOn()) << i);
     UpdateFields();
}



void UpdateTpeFromCb()
{
     cfg[ChipSelected].csa_testpulse_enable=0;
     for(int i=0; i<32; i++) cfg[ChipSelected].csa_testpulse_enable+= ((fCbTpe[i]->IsOn()) << i);
     UpdateFields();
}

void ResetMonAndUpdateFields()
{
  if(ChipSelected>=0) { if(cfg[ChipSelected].csa_monitor_select>0) printf("Analog monitor out disconnected.\n"); cfg[ChipSelected].csa_monitor_select = 0; }
  p->ASIC_csa_monitor_select(ChipSelected,0);
  UpdateFields();
}

void UpdateMonFromCb(long id)
{
     
     if(fCbMon[id]->IsOn()) { cfg[ChipSelected].csa_monitor_select = (1 << id); printf("Analog monitor out connected to pixel %ld\n",id);}
     else { cfg[ChipSelected].csa_monitor_select = 0; printf("Analog monitor out disconnected.\n");}
     p->ASIC_csa_monitor_select(ChipSelected,cfg[ChipSelected].csa_monitor_select);
     UpdateFields();
}


void SetUARTFrequency()
{
  int freq=fFreqUart->GetNumber()*1000;
  if(CStreamConnected==1) 
   { 
    p->ControlSocketSetClock(freq);
  //  p->ControlSocketSystemReset();
   } 
}

void SetTestTrigFrequency()
{
  //printf("Not implemented yet!\n");
  int div=fFreqTrig->GetNumber();
  if(CStreamConnected==1) 
   { 
    p->ControlSocketSetTestPulseDivider(div);
   } 
}


void ScanGlobalThreshold(int chipid)
{
  Float_t rate;
  int gth;
  int gth0=255;
  printf("Scanning global threshold for chip %d...\n",chipid);
  p->ActiveChannel=p->chipchannel[chipid];
  p->ASIC_threshold_global(chipid,255);
  p->ASIC_channel_mask(chipid,0);
  p->ASIC_threshold_global(chipid,40); //try normal range right away to save time
  for(int pix=0; pix<32; pix++) p->ASIC_pixel_trim_dac(chipid,pix,31); //max fine threshold everywhere
  if(p->GetRate(p->chipchannel[chipid],100)==0) gth0=40;
 
  for(gth=gth0; gth>0; gth--)
  {
  p->ASIC_threshold_global(chipid,gth);
  rate=p->GetRate(p->chipchannel[chipid],100);
  if(rate>0) break; 
  }
  printf("At gth=%d rate=%f (chip %d, all pixels active)\n",gth,rate, chipid);
  p->ASIC_channel_mask(chipid,0xffffffff);
  if(gth==31) gth=29;
  cfg[chipid].threshold_global=gth+2;
  p->ASIC_threshold_global(chipid,gth+2);
}

void ScanFineThresholds(int chipid)
{
  printf("Scanning global threshold for chip %d...\n",chipid);
//  printf("NOT IMPLEMENTED YET! - ToDo\n");

  Float_t rate;
  int th;
  uint32_t mask=1;

  p->ActiveChannel=p->chipchannel[chipid];
  p->ASIC_channel_mask(chipid,0xffffffff);
  for(int pix=0; pix<32; pix++) p->ASIC_pixel_trim_dac(chipid,pix,31); //max threshold everywhere
  p->ASIC_threshold_global(chipid,cfg[chipid].threshold_global);

  for(int pix=0; pix<32; pix++)
  {
  p->ASIC_channel_mask(chipid,(~mask));   
  for(th=31; th>=0; th--)
   {
   p->ASIC_pixel_trim_dac(chipid,pix,th);
   rate=p->GetRate(p->chipchannel[chipid],100);
   if(rate>0) break; 
   }
  if(th<0) th=0; 
  printf("At th=%d rate=%f (chip %d, pix %d)\n",th,rate, chipid,pix);
  if(th<=0) { printf("pix %d threshold at zero, consider decrementing gth!\n",pix); }
  if(th>=30) { cfg[chipid].pixel_trim_dac[pix]=31; printf("pix %d threshold at max, consider incrementing gth!\n",pix); }
  else cfg[chipid].pixel_trim_dac[pix]=th+2;
  p->ASIC_pixel_trim_dac(chipid,pix,31);  
  mask=mask<<1; 
  }
printf("Complete.\n");
}


void MaskAllChips()
{
  int c;
  printf("Masking all chips all pixels (not saved, not stored in configs!)\n"); 
  for(c=0; c<256; c++)
   {
    if(p->chipchannel[c]<0) continue;
    p->ActiveChannel=p->chipchannel[c];
    p->ASIC_channel_mask(c,0xffffffff);
   }
}
