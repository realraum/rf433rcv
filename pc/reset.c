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
	// C-based example is 16C0:0480:FFAB:0200
	int r = rawhid_open(1, 0x16C0, 0x0480, 0xFFAB, 0x0200);
	if (r <= 0) {
		// Arduino-based example is 16C0:0486:FFAB:0200
		r = rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200);
		if (r <= 0) {
			printf("no rawhid device found\n");
			return -1;
		}
	}
  sendstr("r"); // clear the buffer  
  return 0;
}
