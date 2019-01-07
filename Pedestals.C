//#include "PIX0MQ.hxx"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TProfile.h"


  TCanvas *c;
  TCanvas *c2;
  TH1F *hr;
  TGraph *gr;
  TH2F *hp;
//ADC parameters:
Double_t VCM=330.0; //330mV
Double_t VREF=1000.0;
int BITS=128;
Double_t mVperADC= (VREF-VCM)/BITS;

PIX0MQ *p=0;
Float_t rate;
uint64_t evcount;
uint64_t word[1024];

#include "InitBoard.h"


void Measure(uint64_t chipid, uint64_t gth, uint32_t pixel, uint64_t nevs);
void Initi(uint64_t Clock);

void Pedestals(uint64_t chipid, uint64_t gth, uint64_t Clock=10000, int points=100)
{
char nm[128];
Initi(Clock);
   if(InitBoard("CONF/chips.list")==0) return;
    if(p->chipchannel[chipid]<0) {printf("Nonexisting chip!\n"); return;}
 gStyle->SetPalette(1);
 if(c==0)  c=new TCanvas();
// if(c2==0)  c2=new TCanvas();
 // c->SetLogy(1);
  c->SetGridx(1);
 c->SetGridy(1);
 c->Draw();
//  c2->SetGridx(1);
// c2->SetGridy(1);
// c2->Draw();
  sprintf(nm,"ASIC %d, Fx2=%d kHz, pedestals",chipid,Clock);
 //if(hp==0) hp=new TH2F("hp","Pedestals",32,0,32,128,0,128); else hp->Reset();
// if(hp==0) hp=new TH2F("hp",nm,32,0,32,128,VCM,VREF); else hp->Reset();
 if(hp==0) hp=new TH2F("hp",nm,32,0,32,128,0,VREF); else hp->Reset();
 hp->GetXaxis()->SetTitle("pixel");
 hp->GetYaxis()->SetTitle("Value, mV");
 
p->DataStreamConnect("192.168.2.102:5556");
  for(int pix=0; pix<32; pix++)
   {
printf("M\n");
     Measure(chipid,gth,pix,points);
printf("2\n");
     c->cd();
     hp->Draw("colz");
     hp->ProfileX("pfz",1,-1,"s")->Draw("same");
printf("3\n");
  //  hp->Draw("sameprofs");
     c->Update();
printf("4\n");
   }
p->DataStreamDisconnect();
}

void Initi(uint64_t Clock)
{
p=new PIX0MQ();
p->ControlSocketConnect("192.168.2.102:5555");

p->ControlSocketSetClock(Clock);
//p->ASIC_threshold_global(207,30); 
//p->ASIC_threshold_global(250,30); 
//p->ASIC_threshold_global(249,30); 

//p->~PIX0MQ();
}

void Measure(uint64_t chipid, uint64_t gth, uint32_t pix, uint64_t nevs)
{
  uint32_t mask=1;
int recvd;
int adc;
Float_t val;
//  p=new PIX0MQ();
//p->ControlSocketConnect("192.168.2.102:5555");

mask=mask<<pix; //activate channel
//rate=p->GetRate(0,100)+p->GetRate(1,100);
printf("Connecting data stream for pixel %d\n",pix);
//p->DataStreamConnect("192.168.2.102:5556");
p->ASIC_threshold_global(chipid,gth);
p->ASIC_channel_mask(chipid,(~mask));
p->ASIC_external_trigger_mask(chipid,(~mask));
evcount=0;
int stopped=0;
p->DataStreamGetMessage((uint8_t*)(word), 1024, 0); //flush buffer
do 
{
  recvd=p->DataStreamGetMessage((uint8_t*)(word), 1024, 0);
   for(int wc=0; wc<recvd/8; wc++)
     {
       adc= (word[wc] >> 41) & 0x3FF; //10 bits of ADC field
       adc=(adc >> 1) & 0xFF; //discard sticky LSB and take only 8 significant bits
       printf("pix=%d chip=%d adc=%d\n",((word[wc] >> 10) & 0x7F),((word[wc] >> 2) & 0xFF), adc);
       if( (word[wc]  & 0x3) != 0x00) continue; 
       if( ((word[wc] >> 10) & 0x7F) != pix) continue; 
       if( ((word[wc] >> 2) & 0xFF) != chipid) continue; 
       val=VCM+adc*mVperADC;
//       hp->Fill(pix,adc);
       hp->Fill(pix,val);
       evcount++;
     }

 // printf("recvd=%d ",recvd);
 // evcount=evcount+recvd/8;
  if(stopped==0 && evcount>=nevs){stopped=1; p->ASIC_channel_mask(chipid,0xffffffff); }
//} while(recvd>0);
} while(stopped==0);
printf("0\n");

p->ASIC_channel_mask(chipid,0xffffffff);
p->ASIC_external_trigger_mask(chipid,0xffffffff);
//p->DataStreamDisconnect();
printf("1\n");
//p->~PIX0MQ();
}
