int ksleep(int event)   /*************VERIFY************/
{
  int sr = int_off();
  /*
  sleep(int event)
  {
    record value in running PROC.event
    change running PROC status to Sleep
    switch process
  }
  */
  printf("proc %d going to sleep on event=%x\n", running->pid, event);
  
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);

  printList("sleepList", sleepList);

  tswitch();

  int_on(sr);
}

int kwakeup(int event)        /*************VERIFY************/
{
  int sr = int_off();   // turns off irq interrupts

  /*
  for each PROC *p do
  {
    if (p->status == SLEEP && p->event == event)
    {
        change p->status to READY
        enter p into readyQueue
    }
  }
  */

  PROC *tmp = 0;
  PROC *p;

  if (tmp->status == SLEEP && tmp->event == event)  // first proc wakes up
  {
    tmp->status = READY;    
  }

  while(p = (PROC *)dequeue(&sleepList))
  {
    
    if (p->status == SLEEP && p->event == event)
    {
      p->status = READY;
      printf("Wake up [%d %d]\n", p->pid, p->priority);
      enqueue(&readyQueue, p);
    }
    else
      enqueue(&tmp, p);
  }
  sleepList = tmp;
  printList("sleepList", sleepList);
  int_on(sr);     // turns on irq interrupts
}

      
int kexit(int exitCode)
{
  int i;
  PROC *tmp, *rp, *rpn;  

  /*
  kexit algorithm:
  kexit(int exitValue)
  {
      1. erase process user-mode context
      2. dispose of children processes if any
      3. record exitValue in PROC.exitCode for parent to get
      4. become a ZOMBIE (but do not free the PROC)
      5. wakeup parent and, if needed, also the INIT process P1
      6. switch process to give up CPU
  }
  */

  if (running->pid == 1)
    return -1;

  if(running->child)  // if the running process has children they will have to be disposed of
  {
    tmp = running->child;

    running->child = 0;
    while(tmp)  // while there are still children to free up
    {
      // send them to the parent process
      // append to the end of the parent process' child list
      tmp->parent = running->parent;
      PROC *tmpCont = tmp;
      tmp = tmp->sibling;
      printf("Sending child p%d to p%d\n", tmpCont->pid, running->parent->pid);
      sendChild(running->parent, tmpCont);
    }

    running->exitCode = exitCode;
    running->status = ZOMBIE;    
    running->priority = 0;
    kwakeup((int)running->parent);
    tswitch();
    return 0;
  }

  // otherwise just kill the process and switch like normal
  running->exitCode = exitCode;
  running->status = ZOMBIE;
  running->priority = 0;
  kwakeup((int)running->parent);
  tswitch();
  return 0;
}

int kwait(int *status)    /******************FIX ME***********************/
{

  /*
  int kwait(int *status)
  {
      if (caller has no child)
          return -1 for error
      while(1)
      {
          search for a (any) ZOMBIE child
          if (found a ZOMBIE child)
          {
              get ZOMBIE child pid
              copy ZOMBIE child exitCode to *status
              release child proc to freeList as free
              return ZOMBIE child pid
          }
          ksleep(running)  sleep on its PROC address
      }
  }
  */

  int i;
  PROC *p, *tmp;

  if (!running->child)  // if caller has no child
  {
    printf("wait error: no child\n");
    return -1;    // return error
  }

  tmp = running->child;
  p = running->child->sibling;

  if(tmp->status == ZOMBIE) // if the first child is a zombie
  {
    running->child = p;
    *status = tmp->exitCode;
    enqueue(&freeList, tmp);
    return tmp->pid;
  }

  while(1)  // search for any ZOMBIE child
  {
    if (p->status == ZOMBIE)  // if found
    {
      tmp->sibling = p->sibling;
      *status = p->exitCode;
      enqueue(&freeList, p);
      return p->pid;
    }
    ksleep((int)running);
    tmp = p;
    p = p->sibling;
  }

}


