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

int body(), goUmode();
char *istring = "init start";

PROC *kfork(char *filename)
{
  int i;
  char line[8], *cp, *addr;
  int *ptable, pentry;  
  int *ustacktop, *usp;
  u32 BA, Btop, Busp;

  PROC *p = dequeue(&freeList);
  if (p==0){
    kprintf("kfork failed\n");
    return (PROC *)0;
  }
  p->ppid = running->pid;
  p->parent = running;
  p->status = READY;
  p->priority = 1;
  p->cpsr = (int *)0x10;

  // build pgtable for p at 6MB + pid*16KB
  p->pgdir = (int *)(0x600000 + p->pid*0x4000); // must be on 16KB boundary  

  ptable = p->pgdir;
  for (i=0; i<4096; i++)
    ptable[i] = 0;
  
  // Kmode: ptable[0]-[257] ID map to 258 PA
  pentry = 0xC12;
  for (i=0; i<258; i++){
    ptable[i] = pentry;
    pentry += 0x100000;
  }
  // Umode: ptable[2048] map to 1MB PA of process at 8MB, 9MB, etc by pid
  ptable[2048] = 0x800000 + (p->pid - 1)*0x100000 | 0xC12;    // offset by 2MB?
  ptable[2049] = 0x1600000 + (p->pid - 1)*0x100000 | 0xC12;

  // set kstack to resume to goUmode
  for (i=1; i<29; i++)  // all 28 cells = 0
    p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-15] = (int)goUmode;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-28]);

  // kstack must contain a resume frame FOLLOWed by a goUmode frame
  //  ksp  
  //  -|-----------------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 fp ip pc|
  //  -------------------------------------------
  //  28 27 26 25 24 23 22 21 20 19 18  17 16 15
  //  
  //   usp      NOTE: r0 is NOT saved in svc_entry()
  // -|-----goUmode--------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 ufp uip upc|
  //-------------------------------------------------
  //  14 13 12 11 10 9  8  7  6  5  4   3    2   1
  /********************

  // to go Umode, must set new PROC's Umode cpsr to Umode=10000
  // this was done in ts.s dring init the mode stacks ==> 
  // user mode's cspr was set to IF=00, mode=10000  

  ***********************/
  // must load filename to Umode image area at 8MB+(pid-1)*1MB
  addr = (char *)(0x700000 + (p->pid)*0x100000);

  if (filename)
     load(filename, p); // p->PROC containing pid, pgdir, etc
 
  // must fix Umode ustack for it to goUmode: how did the PROC come to Kmode?
  // by swi # from VA=0 in Umode => at that time all CPU regs are 0
  // we are in Kmode, p's ustack is at its Uimage (8mb+(pid-1)*1mb) high end
  // from PROC's point of view, it's a VA at 1MB (from its VA=0)
  // but we in Kmode must access p's Ustack directly

  /***** this sets each proc's ustack differently, thinking each in 8MB+
  ustacktop = (int *)(0x800000+(p->pid)*0x100000 + 0x100000);
  TRY to set it to OFFSET 1MB in its section; regardless of pid
  **********************************************************************/
  // p's physical begin address is at BA=p->pgdir[2048] & 0xFFF00000
  // its ustacktop is at BA+1MB = BA + 0x100000;
  // put istring in it, let p->usp be its VA pointing at the istring  

  BA = p->pgdir[2048] & 0xFFF00000;
  Btop = BA + 0x100000;
  // Btop = BA + 0x100000 + 0x100000;   ????
  Busp = Btop - 32;

  cp = (char *)Busp;
  strcpy(cp, istring);

  p->kstack[SSIZE-14] = (int)(0x80100000 - 32); 

  p->usp = (int *)(0x80100000 - 32);
  p->upc = (int *)0x80000000;  // need this in goUmode() 
  p->cpsr = (int *)0x10;

  p->kstack[SSIZE-1] = (int)0x80000000;
  // -|-----goUmode-------------------------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 ufp uip upc|string       |
  //----------------------------------------------------------------
  //  14 13 12 11 10 9  8  7  6  5  4   3    2   1 |             |

  enqueue(&readyQueue, p);

  kprintf("proc %d kforked a child %d\n", running->pid, p->pid); 
  printQ(readyQueue);

  return p;
}

int fork()
{
  // fork a child proc with identical Umode image=> same u1 file
  int *ptable, pentry;
  int i, PA, CA;
  PROC *p = dequeue(&freeList);
  if (p==0){
    kprintf("fork failed\n");
    return -1;
  }
  p->ppid = running->pid;
  p->parent = running;
  p->status = READY;
  p->priority = 1;

  // build pgtable for p at 6MB + pid*16KB
  p->pgdir = (int *)(0x600000 + p->pid*0x4000); // must be on 16KB boundary  

  ptable = p->pgdir;
  // zero out ptable entries first
  for (i=0; i<4096; i++)
    ptable[i] = 0;
  
  // Kmode: ptable[0]-[257] ID map to 258 PA
  pentry = 0xC12;
  for (i=0; i<258; i++){
    ptable[i] = pentry;
    pentry += 0x100000;
    // pentry += 0x200000;
  }
  // Umode: ptable[2048] map to 1MB PA of process at 8MB, 9MB, etc by pid
  ptable[2048] = 0x800000 + (p->pid - 1)*0x100000 | 0xC12;
  ptable[2049] = (2 * 0x800000) + (p->pid - 1)*0x100000 | 0xC12;

  printf("running usp=%x linkR=%x\n", running->usp, running->upc);

  PA = (running->pgdir[2048] & 0xFFFf0000);
  CA = (p->pgdir[2048] & 0xFFFF0000);
  printf("FORK: child  %d uimage at %x\n", p->pid, CA);
  printf("copy Umode image from %x to %x\n", PA, CA);
  // copy 1MB of Umode image
  memcpy((char *)CA, (char *)PA, 0x100000); // THIS SHOULD BE done a second time

  PA = (running->pgdir[2049] & 0xFFFf0000);
  CA = (p->pgdir[2049] & 0xFFFF0000);
  printf("copy Umode image from %x to %x\n", PA, CA);
  memcpy((char *)CA, (char *)PA, 0x100000); // THIS SHOULD BE done a second time

  //  p->upc = running->upc;
  p->usp = running->usp;   // both should be VA in their sections
  p->cpsr = running->cpsr;

  // the hard part: child must resume to the same place as parent
  // child kstack must contain |parent kstack|goUmode stack|=> copy kstack
  printf("copy kernel mode stack\n");
  //j = &running->kstack[SSIZE] - running->ksp;
  // printf("j=%d\n", j);
 
  //  this frame must be copied from parent's kstack, except PC, r0=0
  //  1  2  3  4   5  6  7  8  9  10 11 12 13 14
  // ----------------------------------------------
  //  PC ip fp r10 r9 r8 r7 r6 r5 r4 r3 r2 r1 r0  |
  //-----------------------------------------------
  //   15    16 17 18  19 20 21 22 23 24 25 26 27 28
  //-----------------------------------------------------
  // goUmode ip fp r10 r9 r8 r7 r6 r5 r4 r3 r2 r1 r0 |
  //  --------------------------------------------|ksp----
  for (i=1; i <= 14; i++){
     p->kstack[SSIZE-i] = running->kstack[SSIZE-i];
  }
  for (i=15; i<=28; i++)
    p->kstack[SSIZE-i] = 0;
  printf("FIX UP child resume PC to %x\n", running->upc);
  p->kstack[SSIZE - 14] = 0; // child return pid=0
  p->kstack[SSIZE-15] = (int)goUmode;
  p->ksp = &(p->kstack[SSIZE-28]);
 
  enqueue(&readyQueue, p);

  kprintf("KERNEL: proc %d forked a child %d\n", running->pid, p->pid); 
  printQ(readyQueue);

  PA = (running->pgdir[2049] & 0xFFFf0000);
  CA = (p->pgdir[2049] & 0xFFFF0000);
  memcpy((char *)CA, (char *)PA, 0x100000);

  return p->pid;
}
