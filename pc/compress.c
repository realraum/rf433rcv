#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_RAM 502

void main(int argc, char ** argv )
{
  unsigned char input[MAX_RAM*8+1];
  unsigned char output[MAX_RAM];
  uint16_t i=0;
  uint16_t len=0;
  uint16_t len_net=0;
  for(i=0; i<MAX_RAM;i++)
  {
    output[i] = 0;
  }

  len=read(0,input,MAX_RAM*8+1);
  if (len<1)
  {
    perror("read(): ");
  } else if (len==MAX_RAM*8+1) {
    printf("File to big");
  }
  for(i=0; i<len;i++)
  {
    if (input[i]=='1')
      output[i/8] |= 1<< (i%8);
  }
  len_net=htons(len);
  if (argc==1)
    write(1,&len_net,sizeof(len_net));
  write(1,output, (len%8)?len/8+1:len/8);
}
