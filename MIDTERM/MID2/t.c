
#include "type.h"
#include "queue.c"
//#include "kbd.c"
#include "uart.c"
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
//#include "timer.c"
#include "pipe.c" // pipe implementation
#include "wait.c"
#include "string.c"

PIPE *kpipe;

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

int pipe_writer() // pipe writer task code
{
    struct uart *up = &uart[0];
    ufprintf(up, "UART port %d ready : \n", 0);
    char line[128];
    while(1)
    {
        ufprintf(up, "Enter a line for task1 to get : ");
        printf("task%d waits for line from UART0\n", running->pid);
        ugets(up, line);
        uprints(up, "\r\n");
        tswitch();
        color = 2;
        ufprintf(up, "task%d writes line=[%s] to pipe\n", running->pid, line);
        color = 1;
        write_pipe(kpipe, line, strlen(line));
    }
}

int pipe_reader() // pipe reader task code
{
    char line[128];
    int i, n = 0;
    while(1)
    {
        printf("task%d reading from pipe\n", running->pid);
        n = read_pipe(kpipe, line, PSIZE);     // you cannot pass an n that is less than PSIZE (I think)
        color = 5;
        printf("task%d read n=%d bytes from pipe : [", running->pid, n);
        for (i=0; i<n; i++)
            kputc(line[i]);
        printf("]\n");
        color = 1;
    }
}

int main()
{
    fbuf_init();
    kprintf("Welcome to Wanix in ARM\n");
    uart_init();
    kbd_init();
    pipe_init();
    // initialize PIPEs
    kpipe = create_pipe(); // create global kpipe
    init();
    kprintf("P0 kfork tasks:\n");
    kfork((int)pipe_writer, 1); // pipe writer process
    kfork((int)pipe_reader, 2); // pipe reader process
    printList("readyQueue", readyQueue);
    while(1)
    {
        if (readyQueue)
        {
            tswitch();            
        }
    }
}