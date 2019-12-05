#include "keymap"
#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

#define CAPSLOCK         0x3A
// #define LSHIFT           0x2A
#define LSHIFT           0x12   // keymap 2
#define RSHIFT           0x36
// #define CONTROL          0x1D
#define CONTROL          0x14   // keymap 2

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

// state variables
int shift;		// left and right shift key state 
int control;		// control key state 
int capslock;		// caps lock key state 
int release;            // key release

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;

  shift   = 0;
  control = 0;
  capslock = 0;
  release = 0;
}

void kbd_handler()
{
  //kprintf("entered kbd_handler\n");
  unsigned char code, c;
  int i;
  KBD *kp = &kbd;

  code = *(kp->base + KDATA);  

  if (code & 0x80){ // key release: ONLY catch release of shift, control
    code &= 0x7F;   // mask out bit 7
    if (code == LSHIFT || code == RSHIFT) // released the shift key
    {
      if (shift == 0) // need to toggle shift to support caps lock
      {
        shift = 1;
      }
      else
      {
        shift = 0;
      }      
    }
    if (code == CONTROL)
      control = 0;  // released the Control key
    return;
  }

  // from here on, must be key press 
  if (code == LSHIFT || code == RSHIFT){
      if (shift == 0) // need to toggle shift to support caps lock
      {
        shift = 1;
      }
      else
      {
        shift = 0;
      }
    return;
  }
  if (code == CONTROL){
    control = 1;
    return;
  }

  if (code == 0x3A){
    // handle capslock
      if (shift == 0) // toggle shift
      {
        shift = 1;
      }
      else
      {
        shift = 0;
      }
    return;
  }

  c = (shift ? utab[code] : ltab[code]);

  /* (1). write code to handle caps lock key *******/
  

  /* (2). write code to recognize Control-C key *******/

  if (control && ltab[code] == 'c'){    // Control-C keys on PC, these are 2 keys
     kprintf("Control-c key\n");
     c = '\n'; // force a line, let proc handle #2 signal when exit Kmode
  }
  
  /* (3). write code to recognize Control-D key *******/
  if (control && ltab[code] == 'd'){  // Control-D, these are 2 keys
     kprintf("Control-d key\n");
     c = 4;   // Control-D
  }

  // Store char in buf[] for task to get 

  if (kp->data == 128)  // buf FULL, ignore current key
    return;

  if(release)
  {
    release = 0;
    return;  
  }
  kprintf("%c", c);
  release = 1;
  kp->buf[kp->head++] = c;
  kp->head %= 128;

  kp->data++;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  //unlock();
  while(kp->data == 0);

  //lock();
    c = kp->buf[kp->tail++];
    kp->tail %= 128;
    kp->data--; kp->room++;
  //unlock();
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    if (c=='\b'){
      s--;
      continue;
    }
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}