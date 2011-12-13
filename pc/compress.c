#include <unistd.h>


void main()
{
  unsigned char c,out=0;
  int count =0;
  while (read(0,&c,1))
  {
    out<<=1;
    if (c=='1')
      out |=1;
    count++;  
    if (count ==8)
    {
      write(1,&out,1);
      count=0;
      out=0;
    }
  }
  out<<=8-count;
  out |= 0xff>>count;
  write(1,&out,1);
}
