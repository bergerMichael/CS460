/************* t.c file **************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define N 10
int v[] = {1,2,3,4,5,6,7,8,9,10};
int sum;

char *tab = "0123456789ABCDEF";

#include "string.c"
#include "uart.c"
int main()
{
  int i;
  char line[128]; 
  UART *up;

  uart_init();

  
  for (i=0; i<4; i++)
  {
    ufprintf(up, "i = %d \n", i);
    up = &uart[i];
    ufprintf(up, "UART port %d ready : \n", i);
  
    while(1)
    {
      ufprintf(up, "Enter a line from this UART : ");
      ugets(up, line);
      ufprintf(up, "    line = %s\n", line);
      if (strcmp(line, "quit")==0)
         break;
    }
    ufprintf(up, "Broke from while(1) \n");
    ufprintf(up, "i = %d \n", i);
  }

  ufprintf(up, "Compute sum of array\n");
  sum = 0;
  for (i=0; i<N; i++)
    sum += v[i];
  ufprintf(up, "sum = %d\n", sum);
  ufprintf(up, "END OF UART DEMO\n");
  ufprintf(up, "Enter Control-a, then x to exit QEMU\n");
}
