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
#define VA(x) (0x80000000 + (u32)x)

char *tab = "0123456789ABCDEF";
int BASE;
int color;

#include "kbd.c"
#include "vid.c"
#include "exceptions.c"
#include "queue.c"
#include "kernel.c"
#include "fork.c"
#include "timer.c"
#include "sdc.c"
#include "svc.c"
#include "wait.c"

#include "load.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

int mkPtable()
{
  int i;
  u32 *ut = (u32 *)0x4000;   // at 16KB
  u32 entry = 0 | 0x412;     // 0x412;// AP=01 (Kmode R|W; Umode NO) domaian=0
  for (i=0; i<4096; i++)
    ut[i] = 0;
  for (i=0; i<512; i++){
    ut[i] = entry;
    entry += 0x100000;
  }
}

int kprintf(char *fmt, ...);

void irq_chandler()
{
    int vicstatus, sicstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if (vicstatus & 0x0010){
         timer0_handler();
	 // kprintf("TIMER "); // verify timer handler return to here
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

void data_chandler(void) 
{
  u32 fault_status, fault_addr, domain, status;
  int spsr = get_spsr();
  int oldcolor = color;
  color = RED;
  kprintf("data_abort exception in ");
  if ((spsr & 0x1F)==0x13)
    kprintf("SVC mode\n");
  if ((spsr & 0x1F)==0x10)
    kprintf("USER mode\n");

  fault_status = get_fault_status();
  fault_addr   = get_fault_addr();
  // fault_status = 7654 3210
  //                doma status
  domain = (fault_status & 0xF0) >> 4;
  status = fault_status & 0xF;
  kprintf("domain=%x status=%x addr=%x\n", domain, status, fault_addr);
  //kprintf("data_abort handler return\n");
  color = oldcolor;
}


int main()
{ 
   char line[128]; 
   
   color = WHITE;
   row = col = 0; 
   BASE = 10;
      
   fbuf_init();
   kprintf("                     Welcome to WANIX in Arm\n");
   kprintf("LCD display initialized : fbuf = %x\n", fb);
   color = CYAN;
   kbd_init();

   VIC_INTENABLE |= (1<<4);  // timer0,1 at 4 
   VIC_INTENABLE |= (1<<12); // UART0 at 12
   VIC_INTENABLE |= (1<<13); // UART1 at 13

   VIC_INTENABLE |= (1<<31);    // SIC to VIC's IRQ31
   
   /* enable KBD IRQ */
   SIC_ENSET   |= (1<<3);   // KBD int=3 on SIC
   SIC_ENSET   |= (1<<22);  // SDC int=22 on SIC

   timer_init();
   timer_start(0);

   sdc_init();
   
   kernel_init();

   kfork("u1");
   kfork("u2");
   
   color = CYAN;
   kprintf("P0 switch to P1, enter a key : ");
   kgetc();
   tswitch();  // switch to run P1 ==> never return again
}
