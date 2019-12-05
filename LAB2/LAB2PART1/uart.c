// uart.c file


#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  /*
  ARM versatilepb's 4 UARTs are at the base addresses
      0x101F1000, 0x101F2000, 0x101F3000, 0x10009000
  Write code to initialize the 4 UART structs.
  */
 UART *up;

  // UART0
  up = &uart[0];
  up->base = (char *)(0x101F1000);
  up->n = 0;

  // UART1
  up = &uart[1];
  up->base = (char *)(0x101F2000);
  up->n = 1;

  // UART2
  up = &uart[2];
  up->base = (char *)(0x101F3000);
  up->n = 2;

  // UART3
  up = &uart[3];
  up->base = (char *)(0x10009000);
  up->n = 3;

}

int ugetc(UART *up)
{
  /*
  up points at a UART struct;
  Write code to return an input char from the UART
  */
  while (*(up->base + FR) & RXFE);  // loop until some data is recieved
    return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  /*
  up points at a UART struct;
  Write code to output a char to the UART
  */
  while(*(up->base + FR) & TXFF); // while there is data to transmit
    *(up->base + DR) = c;
}

int ugets(UART *up, char s[ ])
{
  // Write code to input a string from a UART
  while ((*s = (char)ugetc(up)) != '\r')  // while ugetc is returning data (stop at \r)
  {
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  //Write code to print a string s to UART
  while(*s) // while there are still characters in the string
    uputc(up, *s++);
}

int uprinti(UART *up, int x)
{
  if (x<0)
  {
    uputc(up,'-');
    x=-x;
  }
  uprintu(up,x);
}

int urpu(UART *up,int x)
{
  char c;
  if(x){
    c=tab[x%10];
    urpu(up,x/10);
  }
  uputc(up,c);
}

int uprintu(UART *up, int x)
{
  if (x==0)
    uputc(up,'0');
  else
    urpu(up,x);
  uputc(up,' ');
}

int uprx(UART *up, int x)
{
  char c;
  if(x){
    c=tab[x%16];
    uprx(up,x/16);
  }
  uputc(up,c);
}

int uprintx(UART *up, int x)
{
  uputc(up, '0'); 
  uputc(up, 'x');
  if (x==0)
    uputc(up, '0');
  else
    uprx(up, x);
  uputc(up, ' ');
}

// major work:
ufprintf(UART *up, char *fmt, ...)    // this code is from embedded real-time systems pg 42.
{
  // This function takes a string and prints using the functions defined above to put each char
  // when a format token is discovered, the proper funtion is called to handle the type specified.
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;
  while(*cp)
  {
    if (*cp != '%')
    {
      uputc(up, *cp);
      if (*cp=='\n')
      uputc(up, '\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp)
    {
      case 'c': uputc(up, (char)*ip); break;
      case 's': uprints(up, (char *)*ip); break;
      case 'd': uprinti(up, *ip); break;
      case 'u': uprintu(up, *ip); break;
      case 'x': uprintx(up, *ip); break;
    }
    cp++; ip++;
  }

}
