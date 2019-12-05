/*********  t.c file *********************/

int prints(char *s)
{
  while(*s)
    {
      putc(*s++);
    }
}

int gets(char *s)
{
  while((*s = getc()) != '\r')
    {
      putc(*s++);
    }
  *s = 0;
}

char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
  
