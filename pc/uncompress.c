#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_RAM 502

void main()
{
  unsigned char input[MAX_RAM];
  uint16_t i=0;
  uint16_t len=0;
  uint16_t len_net=0;

  len=read(0,input,MAX_RAM+1);
  if (len<1)
  {
    perror("read(): ");
  } else if (len==MAX_RAM+1) {
    printf("File to big");
  }
  
  len_net= *((uint16_t *) input);
  len = ntohs(len_net);
  //very unsave!!!
  for(i=2*8; i<len+2*8;i++)
  {
    if ( input[i/8] & (1<< (i%8)))
      printf("1");
    else
      printf("0");
  }
}
