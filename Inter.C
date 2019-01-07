#include "PIX0MQ.hxx"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraph.h"

  TCanvas *c;
  TH1F *hr;
  TGraph *gr;
#include "InitBoard.h"

PIX0MQ *p=0;
Float_t rate;
uint64_t evcount;
uint64_t word[1024];
  uint64_t starts0, stops0;
  uint64_t starts, stops;
uint64_t chan;
int evs1,evs2;
int pts=0;
int MaxEvs=0;

void Initi(uint64_t chipid, uint64_t gth, uint64_t th);

void Inter(uint64_t chipid, uint64_t gth, uint64_t th, int kHz=10000)
{
 if(c==0)  c=new TCanvas();
  c->SetLogy(1);
  c->SetGridx(1);
 c->SetGridy(1);
 c->Draw();
 Initi(chipid,gth,th);
p->ControlSocketSetClock(kHz);
    if(InitBoard("CONF/chips.list")==0) return;
    if(chipchannel[chipid]<0) {printf("Nonexisting chip!\n"); return;}

  p->ASIC_threshold_global(chipid,gth);

 InitBoard();
}



void Initi(uint64_t chipid, uint64_t gth, uint64_t th)
{
  uint32_t mask=1;
  p=new PIX0MQ();
p->ControlSocketConnect("192.168.2.102:5555");


}
