/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#include "type.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int BASE;
int color;

#include "uart.c"
#include "kbd.c"
#include "timer.c"
#include "vid.c"
#include "interrupts.c"
#include "queue.c"
#include "kernel.c"
#include "wait.c"
#include "fork.c"
#include "exec.c"
#include "svc.c"
#include "sdc.c"
//#include "load.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

// L1 entry:|31          10|9|8765|4|32|10|
//          |     addr     |0|DOM |1|00|01|
//          |              |0|0000|1|00|01|
//                          0    1     1   = addr OR 0x11        
int mkPtable()
{
  int i;
  u32 *ut = (u32 *)0x4000;   // at 16KB
  //u32 entry = 0 | 0x41E;     // 0x412;// AP=01 (Kmode R|W; Umode NO) domaian=0
  u32 entry = 0 | 0x412;     // 0x412;// AP=01 (Kmode R|W; Umode NO) domaian=0
  for (i=0; i<4096; i++)
    ut[i] = 0;
  for (i=0; i<512; i++){
    ut[i] = entry;
    entry += 0x100000;
  }
}


int kprintf(char *fmt, ...);
void timer0_handler();

void data_handler()
{
  printf("data exception\n");
}

// IRQ interrupts handler entry point
//void __attribute__((interrupt)) kc_handler()

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);

    if (vicstatus & 0x0010){
         timer0_handler();
	 // kprintf("TIMER "); // verify timer handler return to here
    }
    if (vicstatus & 0x1000){
         uart_handler(&uart[0]);
	 // kprintf("U0 "); 
    }
    if (vicstatus & 0x2000){
         uart_handler(&uart[1]);
    }
    if (vicstatus & 0x80000000){
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
      if (sicstatus & (1<<22)){
          sdc_handler();
       }
    }
}

extern char _binary_ramdisk_start, _binary_ramdisk_end;

int main()
{ 
   int i,a; 
   char string[32]; 
   char line[128]; 
   int size = sizeof(int);
   UART *up;
   
   color = RED;
   row = col = 0; 
   BASE = 10;
      
   fbuf_init();
   kprintf("                     Welcome to WANIX in Arm\n");
   kprintf("LCD display initialized : fbuf = %x\n", fb);
   color = CYAN;
   kbd_init();

   /* enable UART0 IRQ */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at 4 
   VIC_INTENABLE |= (1<<12); // UART0 at 12
   VIC_INTENABLE |= (1<<13); // UART1 at 13
   VIC_INTENABLE |= (1<<31);    // SIC to VIC's IRQ31

   //VIC_INTENABLE |= (1<<5);  // timer3,4 at 5 

   /* enable UART0 RXIM interrupt */
   UART0_IMSC = 1<<4;
      
   /* enable UART1 RXIM interrupt */
   UART1_IMSC = 1<<4;
  
   /* enable KBD IRQ */
   SIC_ENSET |= (1<<3);  // KBD int=3 on SIC
   SIC_PICENSET |= (1<<3);  // KBD int=3 on SIC
   SIC_ENSET |= (1<<22);  // KBD int=3 on SIC
   SIC_PICENSET |= (1<<22);  // KBD int=3 on SIC

   timer_init();
   timer_start(0);
   uart_init();
   up = &uart[0];
   sdc_init();
   
   init();

   kfork("u1");

   kprintf("P0 switch to P1 : enter a line : ");
   kgetline(string);

   while(1){
     unlock();
     if (readyQueue)
       tswitch();
   }
}
