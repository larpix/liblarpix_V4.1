
#include <zmq.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <termios.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <sys/timeb.h>
#include "pixlar.c"
#include <time.h>


void *context = NULL;

//  Socket to send data to clients
void *publisher = NULL;

struct timeb mstime0, mstime1;
time_t ct,ct0;

uint8_t evbuf[EVLEN];

volatile int bufbusy=0;


void transfer_complete (void *data, void *hint) //call back from ZMQ sent function, hint points to subbufer index
{
bufbusy=0;
// printf("buf transmission complete.\n");
}


void sendout(void * evbuf, int bytes)
{
int rv;
zmq_msg_t msg;
zmq_msg_init_data (&msg, evbuf, bytes , transfer_complete, NULL);
//printf("sendout() buf for sending..\n");
rv=zmq_msg_send (&msg, publisher, ZMQ_DONTWAIT);
if(rv==-1) {rv=zmq_errno(); printf("zmq_msg_send ERRNO=%d\n",rv);}
//printf("zmq_msg_send returns %d\n",rv);
//zmq_msg_close (&msg); - according to manual not needed
}

/*
 * A basic format for the ZMQ messages.
 *
 * "<major_version>.<minor_version>!<IO chain index>!<UART data>"
 */
void send_formatted(uint64_t * buf, int nbytes, int channel)
{
    int version_major = 1;
    int version_minor = 0;
    char prefix[64];
    size_t prefix_length;
    prefix_length = sprintf(prefix, "%d.%d!%d!", version_major, version_minor, channel);
    // standard buffer size for dataserver is 1024, plus 8 for max
    // length of prefix
    uint64_t bigbuf[1032];
    memcpy(bigbuf, prefix, prefix_length);
    memcpy(bigbuf+prefix_length, buf, nbytes);
    sendout(bigbuf, nbytes+prefix_length);
}

void printdate()
{
    char str[64];
    time_t result=time(NULL);
    sprintf(str,"%s", asctime(gmtime(&result))); 
    str[strlen(str)-1]=0; //remove CR simbol
    printf("%s ", str); 
}

/*
ZMQ_TCP_KEEPALIVE -> 1,
ZMQ_TCP_KEEPALIVE_CNT -> 4,
ZMQ_TCP_KEEPALIVE_INTVL -> 15,
ZMQ_TCP_KEEPALIVE_IDLE -> 60,
ZMQ_SNDHWM -> 10000,
ZMQ_SNDBUF -> 256*10000,
ZMQ_LINGER -> 0,
ZMQ_SNDTIMEO - > 3000,
*/


int main (int argc, char **argv)
{

 //   set_conio_terminal_mode();
char roll[4]={95,92,124,47};
int rv;
uint64_t buf[1024];
char heartbeat[128];
sprintf(heartbeat,"HB");

uint64_t w64;
context = zmq_ctx_new();
long long unsigned rec_wA=0, rec_wB=0, rec_wC=0, rec_wD=0;
//  Socket to send data to clients
publisher = zmq_socket (context, ZMQ_PUB);
/*
int64_t affinity;
affinity = 1;
rc = zmq_setsockopt (socket, ZMQ_AFFINITY, &affinity, sizeof affinity);
assert (rc);
*/
int64_t hwm = 100;
zmq_setsockopt (publisher, ZMQ_SNDHWM, &hwm, sizeof hwm);
int64_t sndbuf = 200*EVLEN;
zmq_setsockopt (publisher, ZMQ_SNDBUF, &sndbuf, sizeof sndbuf);
int ling = 1;
zmq_setsockopt (publisher, ZMQ_LINGER, &ling, sizeof ling);
int timeout = 250;
zmq_setsockopt (publisher, ZMQ_SNDTIMEO, &timeout, sizeof timeout);

rv = zmq_bind (publisher, "tcp://*:5556");
if(rv<0) {printdate(); printf("Can't bind tcp socket for data! ERRNO=%d. Exiting.\n",errno); return 0;}
printdate(); printf ("pixlar_server: data publisher at tcp://5556\n");
bufbusy=0;


    int fd[4];
    fd[0] = open("/dev/uart640", O_RDWR); printf("open /dev/uart640 fd=%d\n",fd[0]);
    fd[1] = open("/dev/uart641", O_RDWR); printf("open /dev/uart641 fd=%d\n",fd[1]);
    fd[2] = open("/dev/uart642", O_RDWR); printf("open /dev/uart642 fd=%d\n",fd[2]);
    fd[3] = open("/dev/uart643", O_RDWR); printf("open /dev/uart643 fd=%d\n",fd[3]);

int recvd=0;
ct0=time(NULL);
while(1) //main loop
{
    if(bufbusy==0){    recvd=read(fd[0],buf,1024*8);    rec_wA=rec_wA+recvd/8; if(recvd>0) {   bufbusy=1;   send_formatted(buf,recvd, 0); ct0=time(NULL);}}
    if(bufbusy==0){    recvd=read(fd[1],buf,1024*8);    rec_wB=rec_wB+recvd/8; if(recvd>0) {   bufbusy=1;   send_formatted(buf,recvd, 1); ct0=time(NULL);}}
    if(bufbusy==0){    recvd=read(fd[2],buf,1024*8);    rec_wC=rec_wC+recvd/8; if(recvd>0) {   bufbusy=1;   send_formatted(buf,recvd, 2); ct0=time(NULL);}}
    if(bufbusy==0){    recvd=read(fd[3],buf,1024*8);    rec_wD=rec_wD+recvd/8; if(recvd>0) {   bufbusy=1;   send_formatted(buf,recvd, 3); ct0=time(NULL);}}
     
//    if(recvd>0) printf("Received words:  A:%lld B:%lld C:%lld D:%lld \n",rec_wA, rec_wB,rec_wC, rec_wD);

//Heartbeat generation if no data
ct=time(NULL);
if(ct-ct0 > 1 && bufbusy==0) {ct0=ct; sendout(heartbeat,3);}

}

close(fd[0]);
close(fd[1]);
close(fd[2]);
close(fd[3]);
return 0;
}



