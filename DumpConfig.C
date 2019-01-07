//#include "PIX0MQ.hxx"
PIX0MQ *p=0;
#include "InitBoard.h"
uint64_t word[1024];
uint64_t chan;
int recvd;

void DumpConfig(uint64_t chipid,int kHz=10000)
{
 p=new PIX0MQ("192.168.2.102:5555");
p->ControlSocketSetClock(kHz);
p->DataStreamConnect("192.168.2.102:5556");
    if(InitBoard("CONF/chips.list")==0) return;
    if(chipchannel[chipid]<0) {printf("Nonexisting chip!\n"); return;}
p->ActiveChannel=chipchannel[chipid];

//p->DataStreamGetMessage((uint8_t*)(word), 1024, 0); //flush buffer
recvd=1;
while(recvd>0)
{
recvd=p->DataStreamGetMessage((uint8_t*)(word), 1024, 0);
}

p->ControlSocketRequestConfig(chipid,33);
recvd=1;
while(recvd>0)
{
recvd=p->DataStreamGetMessage((uint8_t*)(word), 1024, 1);
 //  printf("recvd=%d\n",recvd); 
   for(int wc=0; wc<recvd/8; wc++)
     {
    //   if( (word[wc]  & 0x3) != 0x11) continue; 
    //   if( ((word[wc] >> 2) & 0xFF) != chipid) continue;
       dump_decoded(word[wc]); 
     }
}


p->DataStreamDisconnect();
}




