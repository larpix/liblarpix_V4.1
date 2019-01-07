  PIX0MQ *p;
  TCanvas *c;
  TH1F *hrate;
  TGraph *grate;

#include "InitBoard.h"

void ScanThresholds(uint64_t chipid=3, uint64_t gth_start=0,   uint64_t gth_end=255, int kHz=10000)
{
  c=new TCanvas();
  c->SetLogy(1);
  uint64_t gth,th;
  uint32_t mask;
  Float_t rate, rate_prev=1e7;
  p=new PIX0MQ("192.168.2.102:5555");
  p->ControlSocketSetClock(kHz);
  if(InitBoard("CONF/chips.list")==0) return;
    if(p->chipchannel[chipid]<0) {printf("Nonexisting chip!\n"); return;}
  p->ActiveChannel=p->chipchannel[chipid];

  hrate=new TH1F("hrate","Pixel rate vs global threshold",256,0,256);
  hrate->GetXaxis()->SetTitle("threshold_global"); 
  hrate->GetYaxis()->SetTitle("Pixel event rate, Hz");
  hrate->SetMaximum(95000);
  hrate->SetMinimum(0.1);
  grate=new TGraph(256); 
  mask=1;
  hrate->Draw("hist");
  for(int ch=0; ch<32; ch++)
{ 
  hrate->Reset();
  grate->Clear();
  rate_prev=90000;
  for(gth=gth_start; gth<=gth_end; gth++)
  {
  p->ASIC_threshold_global(chipid,gth);
//  p->ASIC_channel_mask(207,0xfffffffe);
  p->ASIC_channel_mask(chipid,(~mask));

//  if(chipid==207) rate=p->GetRate(0,100); else rate=p->GetRate(1,100);
    rate=p->GetRate(p->chipchannel[chipid],100);
 
//  if(rate_prev>1000 && rate<rate_prev/100.) rate=p->GetRate(0,100);
//  if(rate_prev>1000 && rate<rate_prev/100.) rate=p->GetRate(0,100);
 // if(rate_prev>1000 && rate<rate_prev/100.) rate=p->GetRate(0,100);
  rate_prev=rate;
//  hrate->Fill(gth,rate);
  grate->SetPoint(gth,gth,rate);
  grate->Draw("lsame"); 
//  if(ch==0) hrate->Draw("hist"); else hrate->Draw("samehist");
  c->Update();
  p->ASIC_channel_mask(chipid,0xffffffff);
  printf( "Pixel %d Threshold %lld rate %f\n",ch,gth,rate);
  }
//  hrate->SetLineColor(1+ch%5);
 // hrate->DrawClone("samehist");
  grate->SetLineColor(1+ch%5);
  grate->DrawClone("lsame");
  mask=mask<<1;
}
  p->ActiveChannel=-1;
  p->~PIX0MQ();
}
