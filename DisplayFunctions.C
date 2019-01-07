#include "PIX0MQ.hxx"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3C.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TProfile.h"

  int nchips;
  int chipchannel[256];
  int chipids[256];
  int chipx[256];
  int chipy[256];
  FILE *fp1;
  char line[128];
Float_t rate=0;
uint64_t evcount=0;
uint64_t evcount_prev=0;
uint64_t evc_perchip[256];
uint64_t evc_perchip_prev[256];

PIX0MQ *p=0;
int GetPixelCoordinates(int chip, int pix, int *x, int *y);

#define TPCHDEPTH 10000  // depth of 3d-histo image for TPC in time stamp units (master clocks)
int htpc_Lthreshold=63; //by default highest possible
int htpc_Hthreshold=63; //by default highest possible
//ADC parameters:
  TH2F *hp[256];
  TH1F *hrates;
  TH2F *hpixrates;
  TH2F *hmasked;
  TH2F *hmaskedraw;
  TH1F *hmaxrates;
  TH1F *hABCD_uart_starts;
  TH1F *hABCD_uart_stops;
  TH1F *hABCD_data;
  TH3C *htpc;
TProfile *pr;

TH2F *hplane;
TH2F *hplaneBG;
TH2F *hplaneSIG;

Double_t VCM=330.0; //330mV
Double_t VREF=800.0;
int BITS=128;
Double_t mVperADC= (VREF-VCM)/BITS;

uint64_t startsA; uint64_t stopsA;
uint64_t startsB; uint64_t stopsB;
uint64_t startsC; uint64_t stopsC;
uint64_t startsD; uint64_t stopsD;
uint64_t startsA0; uint64_t stopsA0;
uint64_t startsB0; uint64_t stopsB0;
uint64_t startsC0; uint64_t stopsC0;
uint64_t startsD0; uint64_t stopsD0;
uint64_t dataA=0; uint64_t dataA0=0;
uint64_t dataB=0; uint64_t dataB0=0;
uint64_t dataC=0; uint64_t dataC0=0;
uint64_t dataD=0; uint64_t dataD0=0;
uint64_t nparersA=0;
uint64_t nparersB=0;
uint64_t nparersC=0;
uint64_t nparersD=0;
uint64_t relstopsA=0,relstopsB=0,relstopsC=0,relstopsD=0;
time_t Elt0;

int ReadChipList(const char *list)
{
 int chip, chan, na,cx,cy;
 nchips=0;
 fp1=fopen(list,"r");
 if(!fp1) {printf("Can't open file %s. Exiting.\n",list); return 0;}
 for(int i=0; i<256; i++) { chipchannel[i]=-1; chipx[i]=-999; chipy[i]=-999; }
 while(fgets(line,128,fp1))
  {
    na=sscanf(line,"%d %d %d %d",&chip, &chan, &cx, &cy);
    if(na!=4 || chip<0 || chip > 255 || chan<0 ||chan>3) continue;
    chipchannel[chip]=chan;
    chipx[chip]=cx; chipy[chip]=cy; 
    printf("Found line for Chip %d channel %d...",chip,chan);
    chipids[nchips]=chip;
    nchips++;
    printf("done.\n");
  }
  printf("Initialized list with %d ASICs.\n",nchips);
  fclose(fp1);
  return 1;
}
void ResetHistos()
{
   for(int i=0; i<256;i++) hp[i]->Reset();
   hrates->Reset();
   hmaxrates->Reset();
   hpixrates->Reset();
   for(int i=0; i<256;i++) evc_perchip[i]=0;
   for(int i=0; i<256;i++) evc_perchip_prev[i]=0;
   hplane->Reset();
   htpc->Reset();
   relstopsA=stopsA;
   relstopsB=stopsB;
   relstopsC=stopsC;
   relstopsD=stopsD;
   dataA=0;
   dataB=0;
   dataC=0;
   dataD=0;
nparersA=0;
nparersB=0;
nparersC=0;
nparersD=0;
Elt0=time(NULL);
}

void InitHistos()
{
 char nm[128];
 char nm1[128];
 for(int i=0; i<256;i++)
 {    
 sprintf(nm,"ASIC %d : all channels",i);
 sprintf(nm1,"hp%d",i);
 if(hp[i]==0) hp[i]=new TH2F(nm1,nm,32,0,32,128,VCM,VREF); else hp[i]->Reset();
 hp[i]->GetXaxis()->SetTitle("pixel");
 hp[i]->GetYaxis()->SetTitle("Value, mV");
 }
 
  hrates=new TH1F("hrates","ASICs event rates",nchips,0,nchips);
 hrates->GetXaxis()->SetTitle("ASIC #");
 hrates->GetYaxis()->SetTitle("Event rate, kHz");
 hrates->SetFillColor(kBlue);
  for( int i=0; i<nchips;i++) {sprintf(line,"%d",chipids[i]); hrates->GetXaxis()->SetBinLabel(i+1,line); }


  hmaxrates=new TH1F("hmaxrates","ASICs event rates",nchips,0,nchips);
 hmaxrates->GetXaxis()->SetTitle("ASIC #");
 hmaxrates->GetYaxis()->SetTitle("Event rate, kHz");
 hmaxrates->SetLineColor(kRed);
  for( int i=0; i<nchips;i++) {sprintf(line,"%d",chipids[i]); hmaxrates->GetXaxis()->SetBinLabel(i+1,line); }


  hABCD_uart_starts=new TH1F("hABCD_uart_starts","UART rates",4,0,4);
 hABCD_uart_starts->GetXaxis()->SetTitle("CHANNEL");
 hABCD_uart_starts->GetYaxis()->SetTitle("Event rate, kHz");
  hABCD_uart_starts->SetFillColor(kRed);
  hABCD_uart_stops=new TH1F("hABCD_uart_stops","UART rates",4,0,4);
 hABCD_uart_stops->GetXaxis()->SetTitle("CHANNEL");
 hABCD_uart_stops->GetYaxis()->SetTitle("Event rate, kHz");
  hABCD_uart_stops->SetFillColor(kBlue);
  hABCD_data=new TH1F("hABCD_data","UART rates",4,0,4);
  hABCD_data->SetFillColor(kGreen);
 hABCD_data->GetXaxis()->SetTitle("CHANNEL");
 hABCD_data->GetYaxis()->SetTitle("Event rate, kHz");
 hABCD_uart_starts->GetXaxis()->SetBinLabel(1,"A");
 hABCD_uart_starts->GetXaxis()->SetBinLabel(2,"B");
 hABCD_uart_starts->GetXaxis()->SetBinLabel(3,"C");
 hABCD_uart_starts->GetXaxis()->SetBinLabel(4,"D");

//pixel plane
hplane=new TH2F("hplane","Pixel plane view",32,0,32,32,0,32);
hplaneSIG=new TH2F("hplaneSIG","Pixel plane view, BG subtracted",32,0,32,32,0,32);
hplaneBG=new TH2F("hplaneBG","Pixel plane view - pedestals",32,0,32,32,0,32);
hpixrates=new TH2F("hpixrates","Pixel events",32,0,32,32,0,32);
hmaskedraw=new TH2F("hmaskedraw","",32,0,32,32,0,32); //masked channels raw
hmasked=new TH2F("hmasked","",32,0,32,32,0,32); //masked channels to draw
hmasked->SetMarkerStyle(5);
hmasked->SetMarkerColor(2);
hmasked->SetMarkerSize(2.);

//tpc volume
htpc=new TH3C("htpc","TPC volume",32,0,32,32,0,32,TPCHDEPTH,0,TPCHDEPTH);
htpc->GetXaxis()->SetTitle("x-pix");
htpc->GetYaxis()->SetTitle("y-pix");
htpc->GetZaxis()->SetTitle("time, clocks");
}
int GetPixelCoordinates(int chip, int pix, int *x, int *y)
{
  int dx=0,dy=0;
  *x=chipx[chip]*8;
  if(int(pix/16)==1) dx=dx+4; 
  *y=chipy[chip]*4;
  dy=int((pix%16)/4);
  if((pix%4)==0) dx=dx+2; 
  if((pix%4)==1) dx=dx+1; 
  if((pix%4)==2) dx=dx+3; 
  if((pix%4)==3) dx=dx+0;
  if(chipchannel[chip]==1) {dx=7-dx;dy=3-dy;}
  *x=*x+dx; 
  *y=*y+dy;
  return 1;
}

uint64_t parity(uint64_t w)
{
  uint64_t retval=1;
  for( int i=0; i<64; i++) retval=retval+((w>>i) & 0x1);
  retval=retval & 0x1;
  return retval;
}


