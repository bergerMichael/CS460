int color;

#include "type.h"
#include "string.c"
// #include "queue.c"  // use provided queue.obj  during linking
// #include "kbd.c"    // use provided kbd.obj    during linking
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

int sendChild(PROC *parent, PROC *p)  // this function inserts a process at the end of the given child list
{
  PROC *q  = parent->child;
  if(!q)    // if the parent has no children, insert at the first child position
  {
     printf("Apparently p%d has no children so we're gonna give him p%d\n", parent->pid, p->pid);
     parent->child = p;
     p->sibling = 0;
     return 0;
  }

  while(q->sibling)  // find the last child of parent
  {
     q = q->sibling;
  }
  q->sibling = p; // insert p there
  printf("p%d is now the sibling of p%d\n", p->pid, q->pid);
  p->sibling = 0;
  return 0;
}

int body();

int main()
{ 
   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kbd_init();
   
   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4 
   VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5 
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

   /* enable KBD IRQ */
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
 
   kprintf("Welcome to WANIX in Arm\n");
   init();
   kfork((int)body, 1);
   while(1){
     if (readyQueue)
        tswitch();
   }
}
