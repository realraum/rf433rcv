#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if defined(OS_LINUX) || defined(OS_MACOSX)
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(OS_WINDOWS)
#include <conio.h>
#endif

#include "hid.h"


static char get_keystroke(void);

void sendstr(char * tosend)
{
  rawhid_send(0, tosend, strlen(tosend),1000);
}

int main (int argc, char *argv[])
{
	int i, r, num;
	char c, buf[64];
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
    sendstr("s\x10"); // send 4 times
    len = rawhid_recv(0, buf, 64, 255);
    for(i=0;i<len;i++)
    {
      printf("%02x ",(unsigned char) buf[i]);
    }  
    printf("\n");
    return 0;
  } else {
    while (1) {
      // check if any Raw HID packet has arrived
      num = rawhid_recv(0, buf, 64, 220);
      if (num < 0) {
        printf("\nerror reading, device went offline\n");
        rawhid_close(0);
        return 0;
      }
      if (num == 64) {
      //	printf("\nrecv %d bytes:\n", num);
          for (i=0; i<64*8; i++) {
            if (buf[i/8] & 0x80)
            {
              printf("1");
            } else {
              printf("0");
            }
            printf(",");
            buf[i/8]<<=1;
          }
      }
      // check if any input on stdin
      while ((c = get_keystroke()) >= 32) {
        printf("\ngot key '%c', sending...\n", c);
        buf[0] = c;
        for (i=1; i<64; i++) {
          buf[i] = 0;
        }
        rawhid_send(0, buf, 64, 100);
      }
    }
  }  
}

#if defined(OS_LINUX) || defined(OS_MACOSX)
// Linux (POSIX) implementation of _kbhit().
// Morgan McGuire, morgan@cs.brown.edu
static int _kbhit() {
	static const int STDIN = 0;
	static int initialized = 0;
	int bytesWaiting;

	if (!initialized) {
		// Use termios to turn off line buffering
		struct termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = 1;
	}
	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}
static char _getch(void) {
	char c;
	if (fread(&c, 1, 1, stdin) < 1) return 0;
	return c;
}
#endif


static char get_keystroke(void)
{
	if (_kbhit()) {
		char c = _getch();
		if (c >= 32) return c;
	}
	return 0;
}


