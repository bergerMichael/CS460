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
int ktswitch()
{
  tswitch();
}

int svc_handler(int a, int b, int c, int d)
{
  int r;
  
  // printf("svc_handler: a=%d b=%d c=%d d=%d\n",a,b,c,d);
  switch(a){
     case 0: r = kgetpid(); break;
     case 1: r = kgetppid(); break;
     case 2: r = kps(); break;
     case 3: r = kchname((char *)b); break;
     case 4: r = ktswitch(); break;

      case 5: r = kwait((int*)b);     break;
	    case 6: r = ksleep(b);          break;
	    case 7: r = kwakeup(b);         break;
	    case 8: r = kfork((char*)b);	 break;
	    case 9: r = kexit(b);           break;

     case 90: r = kgetc() & 0x7F;  break;
     case 91: r = kputc(b); break;
     case 92: r = kgetPA(); break;
     default: printf("invalid syscall %d\n", a);
  }

  return r; // return to to goUmode: which will replace r0 in Kstack with r
}

