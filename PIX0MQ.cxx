#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zmq.h>
#include <time.h>


#include "TObject.h"

#include "PIX0MQ.hxx"



ClassImp(PIX0MQ)
ClassImp(LarpixConfig)

int  PIX0MQ::InitChipList(const char *list) 
{
  FILE *fp1;
  char line[128];
 int chip, chan, na, x,y;
 nchips=0;
 fp1=fopen(list,"r");
 if(fp1<=0) {printf("PIX0MQ::InitChipList Can't open file %s. Exiting.\n",list); return 0;}
 for(int i=0; i<256; i++) chipchannel[i]=-1;
 while(fgets(line,128,fp1))
  {
    na=sscanf(line,"%d %d %d %d",&chip, &chan, &x, &y);
    if(na!=4 || chip<0 || chip > 255 || chan<0 ||chan>3) continue;
    chipchannel[chip]=chan;
    printf("Adding chip %d at UART channel %d...",chip,chan);
    nchips++;
    printf("done.\n");
  }
  printf("List filled for %d ASICs.\n",nchips);
  fclose(fp1);
  return nchips;
}

void  PIX0MQ::WriteDefaultConfigs() //produce config files for chips in the current chip list
{
  LarpixConfig *conf=new LarpixConfig();
  for(int i=0; i<256; i++) if(chipchannel[i]>-1)
  {
    conf->chip=i;
    conf->chan=chipchannel[i];
    conf->WriteText();
  } 
  delete conf;
}

int PIX0MQ::DataStreamConnect(const char * iface) 
{
char str[64];
sprintf(str,"tcp://%s", iface);
if(context==0) context = zmq_ctx_new();
//  Socket to talk to server
printf ("Connecting to pixlar_server data stream at %s\n",str);
subscriber = zmq_socket(context, ZMQ_SUB);
if(subscriber<=0) return 0;
if(zmq_connect(subscriber, str)<0) return 0;
//zmq_connect (subscriber, "ipc://stats");
if(zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, NULL, 0)<0) return 0;
return 1;
}

int PIX0MQ::DataStreamGetMessage(uint8_t*  buf, int bufmaxlen, int timeout)
{
if(subscriber==NULL) {printf("Socket not yet initialized! Use DataStreamConnect first.\n"); return 0;}
time_t t0,t1;
int dt,dt0;
 int tocopy=bufmaxlen;
 int received=0;
t0=time(NULL);
zmq_msg_t reply;
zmq_msg_init(&reply);

if(timeout==0)  zmq_msg_recv (&reply, subscriber, ZMQ_DONTWAIT);
else while(zmq_msg_recv (&reply, subscriber, ZMQ_DONTWAIT)==-1)
 {
  dt=time(NULL)-t0; 
  if(dt>timeout) {
        printf("No data from driver for %d seconds!\n",dt); 
	zmq_msg_close(&reply);
        return 0;
  }
 };
received=zmq_msg_size (&reply);
//printf ("%s", (char*)zmq_msg_data (&reply));
if(tocopy > received) tocopy=received;
memcpy(buf,(uint8_t*)zmq_msg_data(&reply),tocopy);
zmq_msg_close (&reply);
return received;
}

void PIX0MQ::DataStreamDisconnect() 
{
  if(subscriber!=NULL) zmq_close (subscriber); 
  subscriber=NULL;
}

int  PIX0MQ::ControlSocketConnect(const char * iface) 
{
int rv=0;
  char str[64];
sprintf(str,"tcp://%s", iface);
if(context==0) context = zmq_ctx_new();
//  Socket to talk to server
printf ("Connecting to pixlar_server control socket at %s\n",str);
requester = zmq_socket (context, ZMQ_REQ);
rv=zmq_connect (requester, str); 
if(rv<0) {printf("Connection to %s failed!\n",str); return 0;}
return 1;
}


void  PIX0MQ::ControlSocketDisconnect() 
{
if(requester!=NULL) zmq_close (requester);
requester=NULL;
}

int  PIX0MQ::ControlSocketSendWord(uint64_t w) 
{
time_t t0=time(NULL);
int dt=0;
if(requester==NULL) {printf("Socket not yet initialized! Use ControlSocketConnect first.\n"); return 0;}
char cmd[256];
sprintf(cmd,"SNDWORD 0x%016llx %d",(long long unsigned int)w, ActiveChannel);
zmq_msg_t request;
zmq_msg_init_size (&request, strlen(cmd)+1);
memcpy(zmq_msg_data (&request), cmd,strlen(cmd)+1);
if(Verbose>0) printf ("Sending command %s...", cmd);
zmq_msg_send (&request, requester, 0);
zmq_msg_close (&request);
zmq_msg_t reply;
zmq_msg_init (&reply);
while(zmq_msg_recv (&reply, requester, ZMQ_DONTWAIT)==-1) { dt=time(NULL)-t0; if(dt>GlobTimeout) break;}

if(Verbose>0 && dt<=GlobTimeout) printf ("Received reply: %s\n", (char*)zmq_msg_data (&reply));
zmq_msg_close (&reply);
if(dt>GlobTimeout) return -1;
return 1;
}

/*
if(timeout==0)  zmq_msg_recv (&reply, subscriber, ZMQ_DONTWAIT);
else while(zmq_msg_recv (&reply, subscriber, ZMQ_DONTWAIT)==-1)
 {
  dt=time(NULL)-t0; 
  if(dt>timeout) {
        printf("No data from driver for %d seconds!\n",dt); 
	zmq_msg_close(&reply);
        return 0;
  }
 };

time_t t0=time(NULL);
int dt=0;
while(zmq_msg_recv (&reply, requester, ZMQ_DONTWAIT)==-1) { dt=time(NULL)-t0; if(dt>GlobTimeout) break;}

if(Verbose>0 && dt<=GlobTimeout) printf ("Received reply: %s\n", (char*)zmq_msg_data (&reply));
zmq_msg_close (&reply);
if(dt>GlobTimeout) return -1;
return 1;


*/

int  PIX0MQ::ControlSocketSetClock(int freq) 
{
time_t t0=time(NULL);
int dt=0;
if(requester==NULL) {printf("Socket not yet initialized! Use ControlSocketConnect first.\n"); return 0;}
char cmd[256];
sprintf(cmd,"SETFREQ 0x%x",freq); 
zmq_msg_t request;
zmq_msg_init_size (&request, strlen(cmd)+1);
memcpy(zmq_msg_data (&request), cmd,strlen(cmd)+1);
printf ("Sending command %s...", cmd);
zmq_msg_send (&request, requester, 0);
zmq_msg_close (&request);
zmq_msg_t reply;
zmq_msg_init (&reply);
zmq_msg_recv (&reply, requester, 0);

while(zmq_msg_recv (&reply, requester, ZMQ_DONTWAIT)==-1) { dt=time(NULL)-t0; if(dt>GlobTimeout) break;}

if(Verbose>0 && dt<=GlobTimeout) printf ("Received reply: %s\n", (char*)zmq_msg_data (&reply));
zmq_msg_close (&reply);
if(dt>GlobTimeout) return -1;
return 1;

}

int  PIX0MQ::ControlSocketSetTestPulseDivider(int div) 
{
time_t t0=time(NULL);
int dt=0;
if(requester==NULL) {printf("Socket not yet initialized! Use ControlSocketConnect first.\n"); return 0;}
char cmd[256];
sprintf(cmd,"SETFTST 0x%x",div); 
zmq_msg_t request;
zmq_msg_init_size (&request, strlen(cmd)+1);
memcpy(zmq_msg_data (&request), cmd,strlen(cmd)+1);
printf ("Sending command %s...", cmd);
zmq_msg_send (&request, requester, 0);
zmq_msg_close (&request);
zmq_msg_t reply;
zmq_msg_init (&reply);
while(zmq_msg_recv (&reply, requester, ZMQ_DONTWAIT)==-1) { dt=time(NULL)-t0; if(dt>GlobTimeout) break;}

if(Verbose>0 && dt<=GlobTimeout) printf ("Received reply: %s\n", (char*)zmq_msg_data (&reply));
zmq_msg_close (&reply);
if(dt>GlobTimeout) return -1;
return 1;


}


int  PIX0MQ::ControlSocketSendConfig(uint64_t chipid, uint64_t reg, uint64_t value)
{
 int rv=0;
 uint64_t cw=2;
 uint64_t parity=0;
 cw=cw | ((chipid & 0xff)<<2); 
 cw=cw | ((reg & 0xff)<<10); 
 cw=cw | ((value & 0xff)<<18); 
 parity=Parity(cw);
 cw=cw | ((parity & 0x1)<<53); 
  
 ControlSocketSendWord(cw);
 return rv;
} 

int  PIX0MQ::ControlSocketRequestConfig(uint64_t chipid, uint64_t reg)
{
 int rv=0;
 uint64_t value=0;
 uint64_t cw=3;
 uint64_t parity=0;
 cw=cw | ((chipid & 0xff)<<2); 
 cw=cw | ((reg & 0xff)<<10); 
 cw=cw | ((value & 0xff)<<18); 
 parity=Parity(cw);
 cw=cw | ((parity & 0x1)<<53); 
  
 ControlSocketSendWord(cw);
 return rv;
} 

uint64_t PIX0MQ::Parity(uint64_t w)
{
uint64_t p=1;
for(int i=0;i<64;i++) p=p+((w>>i) & 0x1);
return (p & 0x1);
}

int  PIX0MQ::ControlSocketGetStats(int chan, uint64_t* starts, uint64_t* stops)
{
time_t t0=time(NULL);
int dt=0;
if(requester==NULL) {printf("Socket not yet initialized! Use ControlSocketConnect first.\n"); return 0;}
char cmd[256];
sprintf(cmd,"GETSTAT %d",chan); 
zmq_msg_t request;
zmq_msg_init_size (&request, strlen(cmd)+1);
memcpy(zmq_msg_data (&request), cmd,strlen(cmd)+1);
if(Verbose==1) printf ("Sending command %s...", cmd);
zmq_msg_send (&request, requester, 0);
zmq_msg_close (&request);
zmq_msg_t reply;
zmq_msg_init (&reply);

while(zmq_msg_recv (&reply, requester, ZMQ_DONTWAIT)==-1) { dt=time(NULL)-t0; if(dt>GlobTimeout) break;}

sscanf((char*)zmq_msg_data (&reply),"%lld %lld",(long long int*)starts,(long long int*)stops);

if(Verbose>0 && dt<=GlobTimeout) printf ("Received reply: %s\n", (char*)zmq_msg_data (&reply));
zmq_msg_close (&reply);
if(dt>GlobTimeout) return -1;
return 1;

}  


int PIX0MQ::ControlSocketSystemReset() 
{
time_t t0=time(NULL);
int dt=0;
if(requester==NULL) {printf("Socket not yet initialized! Use ControlSocketConnect first.\n"); return 0;}
char cmd[256];
sprintf(cmd,"SYRESET"); 
zmq_msg_t request;
zmq_msg_init_size (&request, strlen(cmd)+1);
memcpy(zmq_msg_data (&request), cmd,strlen(cmd)+1);
printf ("Sending command %s...", cmd);
zmq_msg_send (&request, requester, 0);
zmq_msg_close (&request);
zmq_msg_t reply;
zmq_msg_init (&reply);
while(zmq_msg_recv (&reply, requester, ZMQ_DONTWAIT)==-1) { dt=time(NULL)-t0; if(dt>GlobTimeout) break;}
if(Verbose>0 && dt<=GlobTimeout) printf ("Received reply: %s\n", (char*)zmq_msg_data (&reply));
zmq_msg_close (&reply);
if(dt>GlobTimeout) return -1;
return 1;
}


PIX0MQ::~PIX0MQ(){DataStreamDisconnect(); ControlSocketDisconnect(); zmq_ctx_destroy (context); } //destructor



int  PIX0MQ::ASIC_threshold_global(uint64_t chipid, uint64_t arg)
{
 return  ControlSocketSendConfig(chipid,32, (arg & 0xff));
}

int  PIX0MQ::ASIC_pixel_trim_dac(uint64_t chipid, uint64_t chan, uint64_t arg)
{
 return  ControlSocketSendConfig(chipid,chan, (arg & 0x1f));
}


int  PIX0MQ::ASIC_reg33(uint64_t chipid, uint64_t csa_gain, uint64_t csa_bypass_enable, uint64_t internal_bias, uint64_t internal_bypass)
{
 uint64_t arg=0;
 arg=arg | (csa_gain & 1);
 arg=arg | ((csa_bypass_enable & 1)<<2);
 arg=arg | ((internal_bias & 1)<<3);
 arg=arg | ((internal_bypass & 1)<<4);
 
 return  ControlSocketSendConfig(chipid,33, arg);
}




int  PIX0MQ::ASIC_csa_bypass_select(uint64_t chipid, uint64_t arg)
{
 ControlSocketSendConfig(chipid, 34, arg);
 ControlSocketSendConfig(chipid, 35, arg>>8);
 ControlSocketSendConfig(chipid, 36, arg>>16);
 return  ControlSocketSendConfig(chipid, 37, arg>>24);
}
int  PIX0MQ::ASIC_csa_monitor_select(uint64_t chipid, uint64_t arg)
{
 ControlSocketSendConfig(chipid, 38, arg);
 ControlSocketSendConfig(chipid, 39, arg>>8);
 ControlSocketSendConfig(chipid, 40, arg>>16);
 return  ControlSocketSendConfig(chipid, 41, arg>>24);
}
int  PIX0MQ::ASIC_csa_testpulse_enable(uint64_t chipid, uint64_t arg)
{
 ControlSocketSendConfig(chipid, 42, arg);
 ControlSocketSendConfig(chipid, 43, arg>>8);
 ControlSocketSendConfig(chipid, 44, arg>>16);
 return  ControlSocketSendConfig(chipid, 45, arg>>24);
}
int  PIX0MQ::ASIC_csa_testpulse_dac(uint64_t chipid, uint64_t arg)
{
 return  ControlSocketSendConfig(chipid,46, arg);
}

int  PIX0MQ::ASIC_reg47(uint64_t chipid, uint64_t test, uint64_t cross_trigger, uint64_t enable_periodic_reset, uint64_t fifo_diagnostic_en)
{
 uint64_t arg=0;
 arg=arg | (test & 3);
 arg=arg | ((cross_trigger & 1)<<2);
 arg=arg | ((enable_periodic_reset & 1)<<3);
 arg=arg | ((fifo_diagnostic_en & 1)<<4);
 
 return  ControlSocketSendConfig(chipid,47, arg);
}

int  PIX0MQ::ASIC_sample_cycles(uint64_t chipid, uint64_t arg)
{
 return  ControlSocketSendConfig(chipid,48, arg);
}
int  PIX0MQ::ASIC_test_burst_length(uint64_t chipid, uint64_t arg)
{
 ControlSocketSendConfig(chipid,49, arg);
 return ControlSocketSendConfig(chipid, 50, arg>>8);
}
int  PIX0MQ::ASIC_adc_burst_length(uint64_t chipid, uint64_t arg)
{
 return  ControlSocketSendConfig(chipid,51, arg);
}
int  PIX0MQ::ASIC_channel_mask(uint64_t chipid, uint64_t arg)
{
 ControlSocketSendConfig(chipid, 52, arg);
 ControlSocketSendConfig(chipid, 53, arg>>8);
 ControlSocketSendConfig(chipid, 54, arg>>16);
 return  ControlSocketSendConfig(chipid, 55, arg>>24);
}
int  PIX0MQ::ASIC_external_trigger_mask(uint64_t chipid, uint64_t arg)
{
 ControlSocketSendConfig(chipid, 56, arg);
 ControlSocketSendConfig(chipid, 57, arg>>8);
 ControlSocketSendConfig(chipid, 58, arg>>16);
 return  ControlSocketSendConfig(chipid, 59, arg>>24);
}
int  PIX0MQ::ASIC_reset_cycles(uint64_t chipid, uint64_t arg)
{
 ControlSocketSendConfig(chipid,60, arg);
 ControlSocketSendConfig(chipid, 61, arg>>8);
 return ControlSocketSendConfig(chipid, 62, arg>>16);
}

Float_t  PIX0MQ::GetRate(uint64_t chan, int period_ms)
{
  uint64_t starts0, stops0;
  uint64_t starts, stops;
  Float_t rate;
  ControlSocketGetStats(chan,&starts0,&stops0);
  usleep(period_ms*1000);
  ControlSocketGetStats(chan,&starts,&stops);
  rate=1000.*(starts-starts0)/period_ms;
  return rate;
 }


void  PIX0MQ::PrintStats()
{
  int period_ms=1000;
  uint64_t starts0, stops0;
  uint64_t starts, stops;
  Float_t rate;
  long long int errors, errors_total;

  ControlSocketGetStats(0,&starts0,&stops0);
  usleep(period_ms*1000);
  ControlSocketGetStats(0,&starts,&stops);
  rate=1000.*(starts-starts0)/period_ms;
  errors=((starts-starts0)-(stops-stops0)); 
  errors_total=starts-stops; 
  printf("UART0 words: total %lld; rate %f Hz; total lost %lld; lost in %d ms %lld\n",(long long int)starts,rate,errors_total, period_ms, errors);  
 
  ControlSocketGetStats(1,&starts0,&stops0);
  usleep(period_ms*1000);
  ControlSocketGetStats(1,&starts,&stops);
  rate=1000.*(starts-starts0)/period_ms;
  errors=((starts-starts0)-(stops-stops0)); 
  errors_total=starts-stops; 
  printf("UART1 words: total %lld; rate %f Hz; total lost %lld; lost in %d ms %lld\n",(long long int)starts,rate,errors_total, period_ms, errors);  
  
}

int PIX0MQ::ControlSocketPing()
{
time_t t0=time(NULL);
int dt=0;
char *repl; 
int rv=0;
 if(requester==NULL)  return 0;
char cmd[256];
sprintf(cmd,"PING_HB");
zmq_msg_t request;
zmq_msg_init_size (&request, strlen(cmd)+1);
memcpy(zmq_msg_data (&request), cmd,strlen(cmd)+1);
if(Verbose>0) printf ("Sending command %s...", cmd);
zmq_msg_send (&request, requester, 0);
zmq_msg_close (&request);
zmq_msg_t reply;
zmq_msg_init (&reply);
while(zmq_msg_recv (&reply, requester, ZMQ_DONTWAIT)==-1) { dt=time(NULL)-t0; if(dt>GlobTimeout) break;}
if(Verbose>0 && dt<=GlobTimeout) printf ("Received reply: %s\n", (char*)zmq_msg_data (&reply));
if(dt>GlobTimeout) {zmq_msg_close (&reply); return -1;}
repl=(char*)zmq_msg_data (&reply);
if(strcmp(repl,"OK")==0) rv=1; else rv=0;
zmq_msg_close (&reply);
return rv;
}  

int PIX0MQ::ConfigureChip(LarpixConfig* conf)
{ //send configuration to corresponding ASIC with validation
 printf("Configuring ASIC %d at UART %d tile %d..\n",conf->chip,conf->chan,conf->tile);
uint64_t chipid=conf->chip;
uint64_t arg;
ActiveChannel=conf->chan;

for(int c=0;c<32;c++) {arg=conf->pixel_trim_dac[c]; ASIC_pixel_trim_dac(chipid, c, arg);}

arg=conf->threshold_global;
ASIC_threshold_global(chipid, arg);

uint64_t csa_gain=conf->csa_gain; 
uint64_t csa_bypass_enable=conf->csa_bypass_enable; 
uint64_t internal_bias=conf->internal_bias; 
uint64_t internal_bypass=conf->internal_bypass;
ASIC_reg33(chipid, csa_gain, csa_bypass_enable, internal_bias, internal_bypass);

arg=conf->csa_bypass_select ;
ASIC_csa_bypass_select(chipid, arg);

arg=conf->csa_monitor_select ;
ASIC_csa_monitor_select(chipid, arg);

arg=conf->csa_testpulse_enable ;
ASIC_csa_testpulse_enable(chipid, arg);

arg=conf->csa_testpulse_dac ;
ASIC_csa_testpulse_dac(chipid, arg);

uint64_t test=conf->test_mode;
uint64_t cross_trigger=conf->cross_trigger;
uint64_t enable_periodic_reset=conf->enable_periodic_reset;
uint64_t fifo_diagnostic_en=conf->fifo_diagnostic_en;
ASIC_reg47(chipid, test, cross_trigger, enable_periodic_reset, fifo_diagnostic_en);

arg=conf-> sample_cycles;
ASIC_sample_cycles(chipid, arg);

arg=conf-> test_burst_length;
ASIC_test_burst_length(chipid, arg);

arg=conf->adc_burst_length ;
ASIC_adc_burst_length(chipid, arg);

arg=conf->channel_mask ;
ASIC_channel_mask(chipid, arg);

arg=conf->external_trigger_mask ;
ASIC_external_trigger_mask(chipid, arg);

arg=conf->reset_cycles ;
ASIC_reset_cycles(chipid, arg);

return 1;
}

LarpixConfig::LarpixConfig()
{
 tile=0; //tile number for this ASIC
 chan=0; //UART channel (1-4) for this ASIC
 chip=1; //ASIC ID (0-255)
 threshold_global=50;
 for(int c=0;c<32;c++) pixel_trim_dac[c]=20;
//REG33:
csa_gain=1;
csa_bypass_enable=0;
internal_bias=0;  
internal_bypass=1;

csa_bypass_select=0;
csa_monitor_select=0;
csa_testpulse_enable=0xffffffff;
csa_testpulse_dac=0;
//REG47:
test_mode=0;  
cross_trigger=0;
enable_periodic_reset=1;  
fifo_diagnostic_en=0;

sample_cycles=1;
test_burst_length=0x00FF;
adc_burst_length=0;
channel_mask=0xFFFFFFFF; //all disabled
external_trigger_mask=0xFFFFFFFF; //all disabled
reset_cycles=4096;

}

LarpixConfig::LarpixConfig(int chip0, int chan0, int tile0)
{
   LarpixConfig();
   tile=tile0;
   chip=chip0;
   chan=chan0;
}


LarpixConfig::~LarpixConfig()
{
}


void LarpixConfig::PrintMe()
{
printf("\tLARPIX ASIC CONFIGURATION\n");
printf("%d \t'ASIC ID (0-255)\n",chip);  
printf("%d \t'at tile Nr\n",tile);  
printf("%d \t'connected to UART channel (0-3)\n",chan);  
printf("%d \t'threshold_global (0-255)\n",threshold_global);  
for(int c=0; c<32; c++)  printf("%d ",pixel_trim_dac[c]);
printf("\t'pixel_trim_dac (0-31)\n");
printf("%d \t'csa_gain: 0=45uV/e, 1=4uV/e\n",csa_gain);  
printf("%d \t'csa_bypass_enable: 0 or 1; 1-Enables injection of ADC_TEST_IN to selected ADC\n",csa_bypass_enable);  
printf("0x%08x \t'csa_bypass_select - 32 bits, HIGH bits select channel for injection \n",csa_bypass_select);  
printf("%d \t'internal_bias: 0 or 1; UNUSED\n",internal_bias);  
printf("%d \t'internal_bypass: 0 or 1; 1-Internal capacitor(?) on power line\n",internal_bypass);  
printf("0x%08x \t'csa_monitor_select: 32 bits, only one HIGH to select channel to connect to analog MONITOR output\n",csa_monitor_select);  
printf("0x%08x \t'csa_testpulse_enable:  32 bits, LOW connects chan inputs to test pulse generator \n",csa_testpulse_enable);  
printf("%d \t'csa_testpulse_dac (0-255)\n",csa_testpulse_dac);  
printf("%d \t'test_mode: 0 - normal DAQ, 1 - UART test, 2 - FIFO burst\n",test_mode);  
printf("%d \t'cross_trigger: 0 or 1, 1- all not masked channels digitize when one is triggered\n",cross_trigger);  
printf("%d \t'enable_periodic_reset: 0 or 1, 1=periodic reset after reset_cycles\n",enable_periodic_reset);  
printf("%d \t'reset_cycles (0-2^24), in Master clock/4 cycles\n",reset_cycles);
printf("%d \t'fifo_diagnostic_en: 0 or 1, Embeds FIFO counter to output event\n",fifo_diagnostic_en);
printf("%d \t'sample_cycles, 0-255. 0 or 1 - one system clock period used for sampling\n",sample_cycles);
printf("%d \t'test_burst_length, 0-2^16, for FIFO burst test\n",test_burst_length);
printf("%d \t'adc_burst_length: 0-255, number of ADC conversions per pix hit, 0 or 1 = one sample per hit\n",adc_burst_length);
printf("0x%08x \t'channel_mask: 32 bits, HIGH to disable channel\n",channel_mask);
printf("0x%08x \t'external_trigger_mask: 32 bits, HIGH to disable external trigger to channel\n",external_trigger_mask);  
}


void LarpixConfig::WriteText(const char *file)
{
 char fname[256];
 FILE *fp=0;
 sprintf(fname,"CONF/T%dC%dASIC%d.conf",tile,chan,chip);
 if(file!=0) fp=fopen(file,"w");
 else fp=fopen(fname,"w");
 printf("Writing config to file %s\n",fname);
//fprintf(fp,"ANNOTATED\n");
fprintf(fp,"%d \t'ASIC ID (0-255)\n",chip);  
fprintf(fp,"%d \t'at tile Nr\n",tile);  
fprintf(fp,"%d \t'connected to UART channel (0-3)\n",chan);  
fprintf(fp,"%d \t'threshold_global (0-255)\n",threshold_global);  
//fprintf(fp,"%d \t'pixel_trim_dac (0-31)\n",pixel_trim_dac);  
for(int c=0; c<32; c++)  fprintf(fp,"%d ",pixel_trim_dac[c]);
fprintf(fp,"\t'pixel_trim_dac (0-31)\n");
fprintf(fp,"%d \t'csa_gain: 0=45uV/e, 1=4uV/e\n",csa_gain);  
fprintf(fp,"%d \t'csa_bypass_enable: 0 or 1; 1-Enables injection of ADC_TEST_IN to selected ADC\n",csa_bypass_enable);  
fprintf(fp,"0x%08x \t'csa_bypass_select - 32 bits, HIGH bits select channel for injection \n",csa_bypass_select);  
fprintf(fp,"%d \t'internal_bias: 0 or 1; UNUSED\n",internal_bias);  
fprintf(fp,"%d \t'internal_bypass: 0 or 1; 1-Internal capacitor(?) on power line\n",internal_bypass);  
fprintf(fp,"0x%08x \t'csa_monitor_select: 32 bits, only one HIGH to select channel to connect to analog MONITOR output\n",csa_monitor_select);  
fprintf(fp,"0x%08x \t'csa_testpulse_enable:  32 bits, LOW connects chan inputs to test pulse generator \n",csa_testpulse_enable);  
fprintf(fp,"%d \t'csa_testpulse_dac (0-255)\n",csa_testpulse_dac);  
fprintf(fp,"%d \t'test_mode: 0 - normal DAQ, 1 - UART test, 2 - FIFO burst\n",test_mode);  
fprintf(fp,"%d \t'cross_trigger: 0 or 1, 1- all not masked channels digitize when one is triggered\n",cross_trigger);  
fprintf(fp,"%d \t'enable_periodic_reset: 0 or 1, 1=periodic reset after reset_cycles\n",enable_periodic_reset);  
fprintf(fp,"%d \t'reset_cycles (0-2^24), in Master clock/4 cycles\n",reset_cycles);
fprintf(fp,"%d \t'fifo_diagnostic_en: 0 or 1, Embeds FIFO counter to output event\n",fifo_diagnostic_en);
fprintf(fp,"%d \t'sample_cycles, 0-255. 0 or 1 - one system clock period used for sampling\n",sample_cycles);
fprintf(fp,"%d \t'test_burst_length, 0-2^16, for FIFO burst test\n",test_burst_length);
fprintf(fp,"%d \t'adc_burst_length: 0-255, number of ADC conversions per pix hit, 0 or 1 = one sample per hit\n",adc_burst_length);
fprintf(fp,"0x%08x \t'channel_mask: 32 bits, HIGH to disable channel\n",channel_mask);
fprintf(fp,"0x%08x \t'external_trigger_mask: 32 bits, HIGH to disable external trigger to channel\n",external_trigger_mask);  

  fclose(fp);
}


int LarpixConfig::ReadText(const char *file)
{
 char fname[256];
  char line[1024];
  int a[23];
  int ta[32];
int offset=0;
int dof=0;
// int FormatInline=1;
 FILE *fp;
 if(file!=0) 
  {
    sprintf(fname,"%s",file);
    fp=fopen(fname,"r"); 
    if(fp<=0) { printf("Can't open file %s!\n",fname); return 0;}
  }
 else 
  {
      sprintf(fname,"CONF/T%dC%dASIC%d.conf",tile,chan,chip);
      printf("Trying default Annotated file %s...\n",fname);
      fp=fopen(fname,"r");
      if(fp<=0) {printf("Can't find default config file %s!\n",fname); return 0;} 
  }
  printf("Reading configuration from %s\n",fname);
for(int i=0;i<23;i++)
 {
  if(!feof(fp)) fgets(line,1024,fp); else {printf("EOF encountered too early, bailing out!"); return 0;}
  if(i==7 || i==10 || i==11 || i==21 || i==22) sscanf(line,"0x%08x ",&a[i]);
  else if (i==4) for(int c=0; c<32;c++) { sscanf(line+offset,"%d%n ",&ta[c],&dof); offset=offset+dof; }//array for pixel trim DAC values  
  else sscanf(line,"%d ",&a[i]); 
 }
       chip=a[0];  
       tile=a[1];  
       chan=a[2];  
       threshold_global=a[3];  
        for(int c=0; c<32;c++) pixel_trim_dac[c]=ta[c];  
       csa_gain=a[5];  
       csa_bypass_enable=a[6];  
       csa_bypass_select=a[7];  
       internal_bias=a[8];  
       internal_bypass=a[9];  
       csa_monitor_select=a[10];  
       csa_testpulse_enable=a[11];  
       csa_testpulse_dac=a[12];  
       test_mode=a[13];  
       cross_trigger=a[14];  
       enable_periodic_reset=a[15];  
       reset_cycles=a[16];
       fifo_diagnostic_en=a[17];
       sample_cycles=a[18];
       test_burst_length=a[19];
       adc_burst_length=a[20];
       channel_mask=a[21];
       external_trigger_mask=a[22];  

 return 1;
}

