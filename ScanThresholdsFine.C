  PIX0MQ *p;
  TCanvas *c;
  TH1F *hrate;
  TGraph *grate;
#include "InitBoard.h"

void ScanThresholdsFine(uint64_t chipid=80, uint64_t gth=22, int kHz=10000)
{
  c=new TCanvas();
  c->SetLogy(1);
  c->SetGridx(1);
 c->SetGridy(1);
  uint64_t th;
  uint32_t mask;
  Float_t rate, rate_prev=1e7;
  p=new PIX0MQ("192.168.2.102:5555");
//p->ASIC_pixel_trim_dac(207,0,0);
p->ControlSocketSetClock(kHz);
    if(InitBoard("CONF/chips.list")==0) return;
    if(chipchannel[chipid]<0) {printf("Nonexisting chip!\n"); return;}
  p->ActiveChannel=p->chipchannel[chipid];

  p->ASIC_threshold_global(chipid,gth);

  char nm[256];
  sprintf(nm,"ASIC %lu pix rate vs pix trim threshold, Glob_th=%lu, Fx2=%d kHz",chipid,gth,kHz); 
  hrate=new TH1F("hrate",nm,32,0,32);
  hrate->GetXaxis()->SetTitle("pixel_trim_dac"); 
  hrate->GetYaxis()->SetTitle("Pixel event rate, Hz");
  hrate->SetMaximum(200000);
  hrate->SetMinimum(0.1);
  grate=new TGraph(0); 
  mask=1;
  hrate->Draw("hist");

  for(int ch=0; ch<32; ch++) p->ASIC_pixel_trim_dac(chipid,ch,0x1f); //max threshold everywhere

  for(int ch=0; ch<32; ch++)
{ 
  hrate->Reset();
  grate->Clear();
  grate->Set(0);
  rate_prev=190000;
  for(th=0; th<32; th++)
  {
  p->ASIC_pixel_trim_dac(chipid,ch,th);
//  p->ASIC_channel_mask(207,0xfffffffe);
  p->ASIC_channel_mask(chipid,(~mask));
  rate=p->GetRate(p->chipchannel[chipid],200); 
 // if(chipid==207) rate=p->GetRate(0,200); else rate=p->GetRate(1,200);
 // if(rate==0) {p->ASIC_pixel_trim_dac(chipid,ch,th); p->ASIC_channel_mask(chipid,(~mask)); usleep(100);rate=p->GetRate(p->chipchannel[chipid],200); }
 // if(rate==0) {p->ASIC_pixel_trim_dac(chipid,ch,th); p->ASIC_channel_mask(chipid,(~mask)); usleep(200); rate=p->GetRate(p->chipchannel[chipid],200); }
// if(rate>rate_prev*1.5) { th=th-2; if(th<0) th=0;}
//  if(rate_prev>1000 && rate<rate_prev/100.) rate=p->GetRate(0,100);
//  if(rate_prev>1000 && rate<rate_prev/100.) rate=p->GetRate(0,100);
 // if(rate_prev>1000 && rate<rate_prev/100.) rate=p->GetRate(0,100);
//  hrate->Fill(gth,rate);
  grate->SetPoint(th,th,rate);
  grate->Draw("lsame"); 
//  if(ch==0) hrate->Draw("hist"); else hrate->Draw("samehist");
  c->Update();
  p->ASIC_channel_mask(chipid,0xffffffff);
  printf( "Pixel %d Threshold %lu rate %f\n",ch,th,rate);
  if(rate==0 && rate_prev==0) break; //to save time
  rate_prev=rate;
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
