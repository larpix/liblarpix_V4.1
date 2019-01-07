#include <stdio.h>
  int nchips;
  int chipchannel[256];
  FILE *fp1;
  char line[128];

int InitBoard(const char *list)
{
 int chip, chan; //, na, x,y;
/* nchips=0;
 fp1=fopen(list,"r");
 if(fp1<=0) {printf("Can't open file %s. Exiting.\n",list); return 0;}
 for(int i=0; i<256; i++) chipchannel[i]=-1;
 while(fgets(line,128,fp1))

  {
    na=sscanf(line,"%d %d %d %d",&chip, &chan, &x, &y);
    if(na!=4 || chip<0 || chip > 255 || chan<0 ||chan>3) continue;
    chipchannel[chip]=chan;
*/
 p->InitChipList(list);
 if(p->nchips<=0) return 0;

 for(chip=0; chip<256; chip++) if(p->chipchannel[chip]>=0)  
{
    chan=p->chipchannel[chip];
    printf("Configuring Chip %d channel %d...\n",chip,chan);
    p->ActiveChannel=chan;
    p->ASIC_csa_monitor_select(chip, 0);    
    p->ASIC_channel_mask(chip,0xffffffff);
    p->ASIC_threshold_global(chip,250); 
    p->ASIC_external_trigger_mask(chip,0xffffffff);
    p->ASIC_reset_cycles(chip,4096); 
    p->ASIC_reg47(chip,0,0,1,0); //enable periodic reset
    p->ASIC_reg33(chip,1,0,1,1); //enable internal bypass
    p->ASIC_adc_burst_length(chip, 1); // 1 conversion per trigger
  for(int ch=0; ch<32; ch++) p->ASIC_pixel_trim_dac(chip,ch,0x0); //max threshold everywhere
}
/*
    nchips++;
    printf("done.\n");
  }
  printf("Configured %d ASICs.\n",nchips);
  fclose(fp1);
*/
  return 1;
}

void dump(unsigned char * ptr)
{
   int i;
     for (i = 0; i < 8; ++i)
        printf("%02x", *(unsigned char*)(ptr+7-i));
        printf("\n");
 
}



void dumpd(uint64_t word)
{
   int bc=0;
      printf("(");
      for (bc = 0; bc < 64; bc++)
     { 
       if(bc==2) printf(") id:");
       if(bc==10) printf(" ch:");
       if(bc==17) printf(" ts:");
       if(bc==41) printf(" adc:");
       if(bc==51) printf(" fifo/2:");
       if(bc==52) printf(" ovfl:");
       if(bc==53) printf(" par:");
       if(bc==54) printf(" ");
       if( ((word>>bc) & 1) > 0 ) printf("1"); else printf("0");
      
     }
     //   printf("%02x", *(unsigned char*)(ptr+7-i));
        printf("\n");
 
}

void dumpc(uint64_t word)
{
   int bc=0;
      printf("(");

      for (bc = 0; bc < 64; bc++)
     { 
       if(bc==2) printf(") id:");
       if(bc==10) printf(" addr:");
       if(bc==18) printf(" data:");
       if(bc==26) printf(" zeros:");
       if(bc==53) printf(" parity:");
       if(bc==54) printf(" ");
       if( ((word>>bc) & 1) > 0 ) printf("1"); else printf("0");
      
     }
     //   printf("%02x", *(unsigned char*)(ptr+7-i));
        printf("\n");
 
}

void dump_decoded(uint64_t word)
{
  if((word & 0x0000000000000003) == 0) {printf("DATA "); dumpd(word); }
  if((word & 0x0000000000000003) == 2) {printf("CONF_W "); dumpc(word); }
  if((word & 0x0000000000000003) == 1) {printf("TEST "); dumpd(word); }
  if((word & 0x0000000000000003) == 3) {printf("CONF_R "); dumpc(word); }
}


