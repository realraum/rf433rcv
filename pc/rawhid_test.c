#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#if defined(OS_LINUX) || defined(OS_MACOSX)
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(OS_WINDOWS)
#include <conio.h>
#endif

#include "hid.h"


void sendstr(char * tosend)
{
  rawhid_send(0, tosend, strlen(tosend),1000);
}

int mtime_diff(struct timeval high,struct timeval low)
{
  int result=1000*(high.tv_sec-low.tv_sec);
  result+=high.tv_usec/1000-low.tv_usec/1000;
  return result;
}

int main (int argc, char *argv[])
{
	int i, r, num;
	char buf[64];
	// C-based example is 16C0:0480:FFAB:0200
	r = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
	if (r <= 0) {
		// Arduino-based example is 16C0:0486:FFAB:0200
		r = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
		if (r <= 0) {
			printf("no rawhid device found\n");
			return -1;
		}
	}
//	printf("found rawhid device\n");
  if (argc>1)
  {
    FILE * f = fopen (argv[1], "r");
    if (!f)
      return -3;
    printf("Clearing Buffer\n");  
    sendstr("c"); // clear the buffer  
    buf[0]='f'; 
    size_t len;
    while ( ( len = fread(buf+1, 1, 63, f)  ) )
    {
      
      rawhid_send(0, buf, 64, 1000); //fill the buffer
      printf("Sending Buffer %d bytes\n",len);
      for(i=0;i<len+1;i++)
      {
        printf("%02x ",(unsigned char) buf[i]);
      }  
      printf("\n");
    }
    printf("Executing Send command\n");
    sendstr("s\x20"); // send 32 times
    len = rawhid_recv(0, buf, 64, 255);
    for(i=0;i<len;i++)
    {
      printf("%02x ",(unsigned char) buf[i]);
    }  
    printf("\n");
    return 0;
  } else {
    struct timeval start_time,stop_time;
    sendstr("b");
    gettimeofday(&start_time,NULL);
    gettimeofday(&stop_time,NULL);
    while (mtime_diff(stop_time,start_time)<1000) {
      // check if any Raw HID packet has arrived
      num = rawhid_recv(0, buf, 64, 220);
      if (num < 0) {
        printf("\nerror reading, device went offline\n");
        rawhid_close(0);
        return 0;
      }
      if (num == 64) {
          for (i=0; i<64*8; i++) {
            if (buf[i/8] & 0x80)
            {
              printf("1");
            } else {
              printf("0");
            }
            // printf(",");
            buf[i/8]<<=1;
          }
      }
      gettimeofday(&stop_time,NULL);
    }
    sendstr("e");
    return 0;
  }  
}
