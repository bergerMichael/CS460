#include "type.h"

/*
(1). Write YOUR own
  enqueue(PROC **queue, PROC *p), which enters p into queue by priority

  PROC *dequeue(PROC **queue), which removes and return the first PROC in queue

  printList(char *name, PROC *list), which prints a list or queue

Use YOUR queue.c file in t.c (replace queue.obj during linking)
*/

int enqueue(PROC **queue, PROC *p)  // enter the proc p into queue by priority
{
  PROC *tmp = *queue; // create a temp pointer to keep track of position in queue
  if (tmp == 0) // if the queue is empty, insert p in the first position of the queue
  {
    *queue = p;
    p->next = tmp;  // point p's the next position to empty
    return;
  }
  if(tmp->priority < p->priority) // if the first process has a lower priority than p
  {
    *queue = p; // insert p ahead of tmp and move tmp to p's next position
    p->next = tmp;
    return;
  }
  while (tmp->next->priority >= p->priority && tmp->next)
  {
    // while the queue still has PROCs of higher priority and we haven't reached the end, continue iterating
    tmp = tmp->next;
  }
  // here tmp has iterated to one of two positions.
  // Position 1, where tmp->next has a lower priority that p or,
  // Position 2, where tmp has reached the end of the list
  // either way, p can be safely inserted here.
  p->next = tmp->next;
  tmp->next = p;
}

PROC *dequeue(PROC **queue) // Remove and return the first PROC in queue
{
  PROC *tmp = *queue; // point a temp PROC pointer at the beginning of the queue
  if (tmp)  // if a Process does exist at the front of the queue
  {
    *queue = tmp->next; // move the beginning of the queue to the next process
  }
  return tmp; // and return the captured process.
}

int printList(char *name, PROC *list) // prints a list or queue
{
  kprintf("%s:", name);
  PROC *tmp = list;
  while(tmp)  // loop through the process list printing each PROC's pid and priority along the way
  {
    kprintf("[%d%d]->", tmp->pid, tmp->priority);
    tmp = tmp->next;
  }
  kprintf("Null\n");
}