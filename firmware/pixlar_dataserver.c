
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
time_t ct, ct0, ct_ts;

//uint8_t evbuf[EVLEN];
#define LARPIX_WORD_SIZE 8 //bytes
#define LARPIX_BUFFER_SIZE (1024*LARPIX_WORD_SIZE) //bytes

volatile bool bufbusy[6]={0,0,0,0,0,0}; //A,B,C,D,timestamp,heartbeat

void transfer_complete (void *data, void *hint) //call back from ZMQ sent function, hint points to subbufer index
{
bufbusy[*(uint8_t*)hint]=0;
// printf("buf transmission complete on %i.\n", *(uint8_t*)hint);
}


void sendout(void * evbuf, int bytes, uint8_t *channel)
{
int rv;
zmq_msg_t msg;
zmq_msg_init_data (&msg, evbuf, bytes , transfer_complete, channel);
//printf("sendout() buf for sending..\n");
rv=zmq_msg_send (&msg, publisher, ZMQ_DONTWAIT);
if(rv==-1) {rv=zmq_errno(); printf("zmq_msg_send ERRNO=%d\n",rv);}
//printf("zmq_msg_send returns %d\n",rv);
//zmq_msg_close (&msg); - according to manual not needed
}

/*
 * A basic format for the ZMQ messages (reserves first word of message).
 *
 * All messages:
 * byte[0] = major version
 * byte[1] = minor version
 * byte[2] = message type
 *
 * MSGTYPE_LARPIX_DATA:
 * byte[2] = 'D'
 * byte[3] = io channel
 * byte[4:7] = *unused*
 *
 * MSGTYPE_TIMESTAMP_DATA:
 * byte[2] = 'T'
 * byte[3:7] = *unused*
 *
 * MSGTYPE_HB_DATA:
 * byte[2] = 'H'
 * byte[3] = 'H'
 * byte[4] = 'B'
 * byte[5] = '\x00'
 * byte[6:7] = *unused*
 *
 */
static char MSGTYPE_LARPIX_DATA = 'D';
static char MSGTYPE_TIMESTAMP_DATA = 'T';
static char MSGTYPE_HB_DATA = 'H';
void send_formatted(uint64_t * buf, int nbytes, uint8_t *channel, uint8_t msg_type)
{
    uint8_t version_major = 1;
    uint8_t version_minor = 0;
    uint8_t prefix[8];
    prefix[0] = version_major;
    prefix[1] = version_minor;
    prefix[2] = msg_type;
    if (msg_type == MSGTYPE_LARPIX_DATA) {
        prefix[3] = *channel;
    } else if (msg_type == MSGTYPE_TIMESTAMP_DATA) {
        ;
    } else if (msg_type == MSGTYPE_HB_DATA) {
        prefix[3] = 'H';
        prefix[4] = 'B';
        prefix[5] = '\x00';
    }
    buf[0] = 0;
    for(int i = 0; i < 8; i++)
        buf[0] = (uint64_t)prefix[i] << 8*i | buf[0];
    sendout(buf, nbytes+8, channel);
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
// create larpix data buffers, +1 is to allow word 0 to be a header word
uint64_t buf_A[LARPIX_BUFFER_SIZE/LARPIX_WORD_SIZE+1];
uint64_t buf_B[LARPIX_BUFFER_SIZE/LARPIX_WORD_SIZE+1];
uint64_t buf_C[LARPIX_BUFFER_SIZE/LARPIX_WORD_SIZE+1];
uint64_t buf_D[LARPIX_BUFFER_SIZE/LARPIX_WORD_SIZE+1];
uint64_t buf_heartbeat[1];
uint64_t buf_timestamp[2];

uint8_t channel0 = 0;
uint8_t channel1 = 1;
uint8_t channel2 = 2;
uint8_t channel3 = 3;
uint8_t channel_hb = 5;
uint8_t channel_ts = 4;

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
for ( int i = 0; i < 6; i++)
    bufbusy[i]=0;


int fd[4];
fd[0] = open("/dev/uart640", O_RDWR); printf("open /dev/uart640 fd=%d\n",fd[0]);
fd[1] = open("/dev/uart641", O_RDWR); printf("open /dev/uart641 fd=%d\n",fd[1]);
fd[2] = open("/dev/uart642", O_RDWR); printf("open /dev/uart642 fd=%d\n",fd[2]);
fd[3] = open("/dev/uart643", O_RDWR); printf("open /dev/uart643 fd=%d\n",fd[3]);

int recvd_A=0;
int recvd_B=0;
int recvd_C=0;
int recvd_D=0;
int recvd=0;
ct0=time(NULL);
ct_ts=ct0;
while(1) //main loop
{
    if(!(bufbusy[0] || bufbusy[1] || bufbusy[2] || bufbusy[3])){
      // Read from uart into 1:N words of buffer, word 0 is reserved for the message header
        recvd_A=read(fd[0],buf_A+1,LARPIX_BUFFER_SIZE); recvd+=recvd_A;
        recvd_B=read(fd[1],buf_B+1,LARPIX_BUFFER_SIZE); recvd+=recvd_B;
        recvd_C=read(fd[2],buf_C+1,LARPIX_BUFFER_SIZE); recvd+=recvd_C;
        recvd_D=read(fd[3],buf_D+1,LARPIX_BUFFER_SIZE); recvd+=recvd_D;

        rec_wA+=recvd_A/LARPIX_WORD_SIZE;
        rec_wB+=recvd_B/LARPIX_WORD_SIZE;
        rec_wC+=recvd_C/LARPIX_WORD_SIZE;
        rec_wD+=recvd_D/LARPIX_WORD_SIZE;
        if(recvd_A>0) {
            bufbusy[0]=1;
            send_formatted(buf_A, recvd_A, &channel0, MSGTYPE_LARPIX_DATA);
        }
        if(recvd_B>0) {
            bufbusy[1]=1;
            send_formatted(buf_B, recvd_B, &channel1, MSGTYPE_LARPIX_DATA);
        }
        if(recvd_C>0) {
            bufbusy[2]=1;
            send_formatted(buf_C, recvd_C, &channel2, MSGTYPE_LARPIX_DATA);
        }
        if(recvd_D>0) {
            bufbusy[3]=1;
            send_formatted(buf_D, recvd_D, &channel3, MSGTYPE_LARPIX_DATA);
        }
        if(recvd>0) {
            ct0=time(NULL);
        }
    }

    if(recvd>0) printf("Received words:%d  A:%lld B:%lld C:%lld D:%lld \n", recvd/LARPIX_WORD_SIZE, rec_wA, rec_wB,rec_wC, rec_wD);
    recvd = 0;

    // Heartbeat generation if no data
    ct=time(NULL);
    if(ct-ct0 > 1 && bufbusy[5]==0) {
      bufbusy[5] = 1;
      ct0=ct;
      send_formatted(buf_heartbeat, 0, &channel_hb, MSGTYPE_HB_DATA);
    }
    // Timestamp at ~1Hz
    if(ct-ct_ts > 0 && bufbusy[4]==0) {
      ct_ts = time(NULL);
      ct0=ct;
      bufbusy[4] = 1;
      buf_timestamp[1] = (uint64_t)ct_ts;
      send_formatted(buf_timestamp, 8, &channel_ts, MSGTYPE_TIMESTAMP_DATA);
    }
}

close(fd[0]);
close(fd[1]);
close(fd[2]);
close(fd[3]);
return 0;
}



