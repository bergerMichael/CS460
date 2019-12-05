typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#include "string.c"
#include "uio.c"
#include "ucode.c"

int main()
{
  int pid, ppid;
  char line[64];
  u32 mode,  *up;

  mode = getcsr();
  mode = mode & 0x1F;
  printf("CPU MODE=%x\n", mode);

  pid = getpid();
  ppid = getppid();
  
  while(1){
    printf("THIS IS %d IN UMODE AT %x  PARENT=%d\n", pid, getPA(),ppid);
    umenu();
    printf("INPUT A COMMAND : ");
    ugetline(line); 
    uprintf("\n"); 
 
    if (strcmp(line, "getpid")==0)
       ugetpid();
    if (strcmp(line, "getppid")==0)
       ugetppid();
    if (strcmp(line, "ps")==0)
       ups();  
    if (strcmp(line, "chname")==0)
       uchname();
    if (strcmp(line, "switch")==0)
       uswitch();
   if (strcmp(line, "fork") == 0)
      ufork();
   if (strcmp(line, "exit") == 0)
      uexit();
   if (strcmp(line, "wait") == 0)
      uwait();
  }
}

