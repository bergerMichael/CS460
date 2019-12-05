//#include "type.h"

int enqueue(PROC **queue, PROC *p)
{
  PROC *q  = *queue;
  if (q==0 || p->priority > q->priority){
    *queue = p;
    p->next = q;
    return;
  }
  while (q->next && p->priority <= q->next->priority){
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
}

PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  if (p)
    *queue = p->next;
  return p;
}

int printList(char *name, PROC *list)
{
  kprintf("%s=", name);
  PROC *p = list;
  while(p)
  {
    kprintf("[%d%d]->", p->pid, p->priority);
    p = p->next;
  }
  kprintf("NULL\n");
}
