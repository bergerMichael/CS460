int umenu()
{
  uprintf("----------------------------------------------\n");
  uprintf("getpid getppid ps chname switch fork wait exit\n");
  uprintf("----------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}

int getPA()
{
  return syscall(92,0,0,0);
}

int ufork()
{
  int pid, mypid;

  pid = getpid();
  if (pid == 1)
  {
    pid = syscall(8,"u1",0,0);
  }
  else
  {
    pid = syscall(8,"u2",0,0);
  }
  

  if (pid > 0)
    uprintf("parent %d forked a child %d\n", getpid(), pid);

  if (pid == 0)
    uprintf("child %d return from fork(), pid=%d\n", getpid(), pid);

  if (pid < 0)
    uprintf("%d fork failed\n", getpid());
}

int uwait()
{
  int pid, status;

  uprintf("proc %d syscalls 5 to wait for a ZOMBIE child\n", getpid());

  pid = syscall(5,&status,0,0);

  uprintf("proc %d waited for a ZOMBIE child=%d ", getpid(), pid);

  if (pid>0)
    uprintf("status=%x %d", status, status);

  uprintf("\n");
}

int uexit()
{
  int value;

  uprintf("enter an exit value : ");
  printf("\n");

  value = geti();
  syscall(9,value,0,0);
}