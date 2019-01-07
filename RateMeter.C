#include "PIX0MQ.hxx"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGraph.h"

  TCanvas *c;
  TH1F *hr;
  TGraph *gr;

PIX0MQ *p=0;
Float_t rate;
uint64_t evcount;
uint64_t word[1024];

#include "InitBoard.h"

  uint64_t starts0, stops0;
  uint64_t starts, stops;
uint64_t chan;
int evs1,evs2;
int pts=0;
int MaxEvs=0;

void Measure(uint64_t chipid, uint64_t gth, uint64_t th, int Clock, int Burst);
void Initi(uint64_t Clock);

void RateMeter(uint64_t chipid, uint64_t gth, uint64_t th0, uint64_t th1, int Clock=10000, int Burst=1)
{
 
 if(c==0)  c=new TCanvas();
  c->SetLogy(1);
  c->SetGridx(1);
 c->SetGridy(1);
 c->Draw();
 if(gr==0) gr=new TGraph();
  for(int i=th0; i<=th1; i++)
   {
     Measure(chipid,gth,i,Clock,Burst);
     if(evs1<evcount) evs1=evcount+1;
     gr->SetPoint(pts,rate, 1.*(evs1-evcount)/evs1);
     pts++;
     gr->Draw("AL");
     c->Update();
   }
}

void Measure(uint64_t chipid, uint64_t gth, uint64_t th, int Clock, int Burst)
{
  uint32_t mask=1;
Initi(Clock);
   if(InitBoard("CONF/chips.list")==0) return;
    if(p->chipchannel[chipid]<0) {printf("Nonexisting chip!\n"); return;}
p->ActiveChannel=p->chipchannel[chipid];
mask=mask<<3; //activate channel 3
//p->ASIC_threshold_global(chipid,0); //low threshold - max rate
 p->ASIC_adc_burst_length(chipid, Burst); //simulate N-samples long track
p->ASIC_threshold_global(chipid,gth); 
p->ASIC_pixel_trim_dac(chipid,3,th);
p->ASIC_channel_mask(chipid,(~mask));

 rate=p->GetRate(p->chipchannel[chipid],100);
  printf( "Rate  %f\n",rate);
  if(rate>10000.) MaxEvs=1e5;
  else MaxEvs=rate*10;
 //rate=p->GetRate(1,100);
 // printf( "Rate B %f\n",rate);

evcount=0;
p->ASIC_channel_mask(chipid,0xffffffff);


p->DataStreamConnect("192.168.2.102:5556");
p->ControlSocketGetStats(p->chipchannel[chipid],&starts0,&stops0);
p->ASIC_channel_mask(chipid,(~mask));
int recvd=0;
int stopped=0;
do 
{
  recvd=p->DataStreamGetMessage((uint8_t*)(word), 1024, 1);
//  printf("%d ",recvd);
  evcount=evcount+recvd/8;
  if(stopped==0 && evcount>=MaxEvs){stopped=1; p->ASIC_channel_mask(chipid,0xffffffff); }
} while(recvd>0);

p->ASIC_channel_mask(chipid,0xffffffff);
p->ControlSocketGetStats(p->chipchannel[chipid],&starts,&stops);
p->DataStreamDisconnect();
 p->ASIC_adc_burst_length(chipid, 1); 
evs1=starts-starts0;
printf("UART54 received %lld events, Socket received %lld events\n",evs1,evcount); 

p->~PIX0MQ();
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

