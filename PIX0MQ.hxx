//#include <vector>

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "TObject.h"
//______________________________________________________________________________
class LarpixConfig : public TObject
  {
  public:

   LarpixConfig();
   LarpixConfig(int chip0, int chan0, int tile0=0);
   virtual ~LarpixConfig();
   virtual void Copy(LarpixConfig &h) const { memcpy( (void*)this, (void*)&h, sizeof(LarpixConfig) ); }
   void PrintMe();
//   void PrintInline();
   void WriteText(const char *file=0);
//   void WriteInline(const char *file=0);
   int ReadText(const char *file=0);

   uint16_t tile; //Tile number for this ASIC
   uint8_t chan; //UART channel (1-4) for this ASIC
   uint8_t chip; //ASIC ID (0-255)
uint32_t threshold_global;
uint8_t pixel_trim_dac[32];
//REG33:
uint8_t csa_gain,  csa_bypass_enable,  internal_bias,  internal_bypass;
uint32_t csa_bypass_select;
uint32_t csa_monitor_select;
uint32_t csa_testpulse_enable;
uint8_t csa_testpulse_dac;
//REG47:
uint8_t test_mode,  cross_trigger,  enable_periodic_reset,  fifo_diagnostic_en;
uint8_t sample_cycles;
uint16_t test_burst_length;
uint8_t adc_burst_length;
uint32_t channel_mask;
uint32_t external_trigger_mask;
uint32_t reset_cycles;
  

    ClassDef(LarpixConfig, 1) // LarpixConfig
};



//______________________________________________________________________________
class PIX0MQ : public TObject
  {
  public:
  int nchips;
  int chipchannel[256];

int Verbose=0;

void *context=0;
void *subscriber=0;
void *requester=0;
int ActiveChannel=-1;
int GlobTimeout=1; //timeout for socket comm in seconds
    PIX0MQ(){}; // default constructor
    PIX0MQ(const char * iface){ControlSocketConnect(iface);};  //main constructor
    ~PIX0MQ(); //destructor
int  InitChipList(const char *list);
void  WriteDefaultConfigs(); //produce config files for chips in the current chip list
int ConfigureChip(LarpixConfig* conf); //send configuration to corresponding ASIC with validation

int  DataStreamConnect(const char * iface); 
void DataStreamDisconnect(); 
int  DataStreamGetMessage(uint8_t*  buf, int bufmaxlen, int timeout); 
int  ControlSocketConnect(const char * iface); 
void ControlSocketDisconnect(); 
int  ControlSocketSendWord(uint64_t w); 
int  ControlSocketSetClock(int freq);
int  ControlSocketSetTestPulseDivider(int div); 
int  ControlSocketSendConfig(uint64_t chipid, uint64_t reg, uint64_t value);
int  ControlSocketRequestConfig(uint64_t chipid, uint64_t reg);
int  ControlSocketSystemReset(); 
int  ControlSocketGetStats(int chan, uint64_t* starts, uint64_t* stops);  
int  ControlSocketPing();  

int  ASIC_threshold_global(uint64_t chipid, uint64_t arg);
//LSB=VDDA/256 mV, for VDDA=1.8V LSB=7 mV
int  ASIC_pixel_trim_dac(uint64_t chipid, uint64_t chan, uint64_t arg);
//LSB=1.2mV 
int  ASIC_reg33(uint64_t chipid, uint64_t csa_gain, uint64_t csa_bypass_enable, uint64_t internal_bias, uint64_t internal_bypass);

int  ASIC_csa_bypass_select(uint64_t chipid, uint64_t arg);
int  ASIC_csa_monitor_select(uint64_t chipid, uint64_t arg);
int  ASIC_csa_testpulse_enable(uint64_t chipid, uint64_t arg);
int  ASIC_csa_testpulse_dac(uint64_t chipid, uint64_t arg);
int  ASIC_reg47(uint64_t chipid, uint64_t test, uint64_t cross_trigger, uint64_t enable_periodic_reset, uint64_t fifo_diagnostic_en);
int  ASIC_sample_cycles(uint64_t chipid, uint64_t arg);
int  ASIC_test_burst_length(uint64_t chipid, uint64_t arg);
int  ASIC_adc_burst_length(uint64_t chipid, uint64_t arg);
int  ASIC_channel_mask(uint64_t chipid, uint64_t arg);
int  ASIC_external_trigger_mask(uint64_t chipid, uint64_t arg);
int  ASIC_reset_cycles(uint64_t chipid, uint64_t arg);

void  PrintStats();
Float_t  GetRate(uint64_t chan, int period_ms);

 
uint64_t Parity(uint64_t w);   

    ClassDef(PIX0MQ, 1) // PIX0MQ
};


