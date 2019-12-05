/************* kbd.c file **************/

#include "keymap"
#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;
int count;

int kbd_init()
{
  char scode;
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;
  count = 0;  // number of KBD interrupts
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;
  color = YELLOW;
  scode = *(kp->base + KDATA);
  count++;
  printf("scanCode = %x count = %d\n", scode, count);
  
}

int kgetc()
{
}

int kgets()
{
}
