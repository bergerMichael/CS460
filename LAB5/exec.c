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

int exec(char *cmdline) // cmdline=VA in Uspace
{
  int i, upa, usp;
  char *cp, kline[128], file[32], filename[32];
  PROC *p = running;
  kstrcpy(kline, cmdline); // fetch cmdline into kernel space
  // get first token of kline as filename
  cp = kline;
  i = 0;
  while (*cp != '\0')
  {
    filename[i] = *cp;
    i++;
    cp++;
  }
  filename[i] = 0;
  file[0] = 0;
  // if (filename[0] != '/')
  //   // if filename relative
  //   kstrcpy(file, "/bin/"); // prefix with /bin/
  kstrcat(file, filename);
  upa = p->pgdir[2048] & 0xFFFF0000; // PA of Umode image
  upa = p->pgdir[2049] & 0xFFFF0000; // PA of Umode image
  // loader return 0 if file non-exist or non-executable
  if (!load(file, p))
    return -1;
  // copy cmdline to high end of Ustack in Umode image
  usp = upa + 0x100000 - 128;
  // assume cmdline len < 128
  kstrcpy((char *)usp, kline);
  p->usp = (int *)VA(0x200000 - 128);
  // fix syscall frame in kstack to return to VA=0 of new image
  for (i = 2; i < 14; i++)
  {
    // clear Umode regs r1-r12
    p->kstack[SSIZE - i] = 0;
  }

  p->kstack[SSIZE - 1] = (int)VA(0);
  // return uLR = VA(0)
  return (int)p->usp; // will replace saved r0 in kstack
}