  PIX0MQ *p;
  TCanvas *c;
  TH1F *hrate;
  TGraph *grate;
void PlotRate(uint64_t chipid=207, uint64_t ch=16, uint64_t gth=22, uint64_t pixtrim=16)
{
  c=new TCanvas();
 // c->SetLogy(1);
  c->SetGridx(1);
 c->SetGridy(1);
  uint64_t th;
  uint32_t mask;
  Int_t pt;
  Float_t rate, rate_prev=1e7;
  p=new PIX0MQ("192.168.2.102:5555");
//p->ASIC_pixel_trim_dac(207,0,0);
p->ASIC_channel_mask(207,0xffffffff);
p->ASIC_channel_mask(250,0xffffffff);
p->ASIC_channel_mask(249,0xffffffff);

p->ASIC_reset_cycles(207,4096); 
p->ASIC_reset_cycles(250,4096); 
p->ASIC_reset_cycles(249,4096); 


p->ASIC_reg47(207,0,0,1,0); //enable periodic reset
p->ASIC_reg47(250,0,0,1,0); //enable periodic reset
p->ASIC_reg47(249,0,0,1,0); //enable periodic reset

p->ASIC_reg33(207,1,0,1,1); //enable internal bypass
p->ASIC_reg33(250,1,0,1,1); //enable internal bypass
p->ASIC_reg33(249,1,0,1,1); //enable internal bypass

  p->ASIC_threshold_global(chipid,gth);
  p->ASIC_pixel_trim_dac(chipid,ch,pixtrim);

  mask=1<<ch;
  p->ASIC_channel_mask(chipid,(~mask));

  char nm[256];
  sprintf(nm,"Pixel %lld rate vs time, gth=%lld, th=%lld",ch,gth,th); 
  hrate=new TH1F("hrate",nm,32,0,32);
  hrate->GetXaxis()->SetTitle("time, au"); 
  hrate->GetYaxis()->SetTitle("Pixel event rate, Hz");
  hrate->SetMaximum(95000);
  hrate->SetMinimum(0.1);
  grate=new TGraph(32); 
  hrate->Draw("hist");


  for(pt=0;pt<1000;pt++)

{
if(chipid==207) rate=p->GetRate(0,100); else rate=p->GetRate(1,100);
  grate->SetPoint(pt,pt,rate);
  grate->Draw("AL"); 
  c->Update();
  printf( "Pixel %d Threshold %lld rate %f\n",ch,th,rate);

}

  p->~PIX0MQ();
}
