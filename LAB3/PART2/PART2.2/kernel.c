/****** in type.h file **************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];
}PROC;
************************************/
#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);

int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;
  printList("freeList", freeList);
  
  // creat P0 as initial running process
  p = (PROC *)dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0;
  running = p;
  kprintf("running = %d\n", running->pid);
}

void kexit()
{
  printf("proc %d kexit\n", running->pid);
  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running);
  tswitch();
}
  
PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = (PROC *)dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = (PROC *)dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
} 

/*
(3). In the posted kernel.c code, the process body function is defined as
        int body()
        {
           printf("proc %d resume to body()\n", running->pid);
            ..............
        }

     Modify it to

	int body(char *myname, int myage)
        {
           printf("proc %d: myname=%s myage=%d\n", running->pid, myname, myage);
	   ........
	}
*/
int body(char *myname, int myage)
{
  char command[64];
  int pid;
  kprintf("proc %d: myname=%s myage=%d\n", running->pid, myname, myage);

  while(1){

    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=WHITE;
    if (pid==2) color=GREEN;
    if (pid==3) color=CYAN;
    if (pid==4) color=YELLOW;
    if (pid==5) color=WHITE;
    if (pid==6) color=GREEN;   
    if (pid==7) color=CYAN;
    if (pid==8) color=YELLOW;

    printList("freeList", freeList);
    printList("readyQueue", readyQueue);
    kprintf("proc %d running : command = [switch|fork|exit] : ", running->pid);

    kgets(command);
    kprintf("Received command: %s\n", command);

    if (strcmp(command, "switch")==0)
       tswitch();
    if (strcmp(command, "fork")==0)
       kfork((int)body, 1);
    if (strcmp(command, "exit")==0)
       kexit();
  }
}
