#include <iostream>

int main()
{
  unsigned char c =0;
  unsigned char sym =0;
  unsigned char old_sym =0;
  int counter = 0;
  while( 1)
  {
    std::cin >> c;
    for(int i = 0; i<8; i++)
    {
      sym = c & (1<<7);
      if (sym == old_sym)
      {      
	      counter++;
      } else {
	if (counter > 30 )
	{
	  std::cout << std::endl;
	}
	counter = 0;
      }

      if( sym)
      {
        std::cout << 'X';
      } else {
        std::cout << '_';
      }
      c<<=1;
      old_sym=sym;
    }
  }
}
