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

// vid.c file: implement fbuf for the ARM PL110 LCD display
/**************** Reference: ARM PL110 and DUI02241 ********************
Color LCD base address: 0x10120000 - 0x1012FFFF
00    timing0
04    timing1
08    timing2
0C    timing3
10    upperPanelframeBaseAddressRegister // use only ONE panel
14    lowerPanelFrameBaseAddressRegister // some display allows 2 panels
18    interruptMaskClrregister
1C    controlRegister
20    interruptStatusReg
etc
************************************************************************/
#include "font0"  // NOTE: font0 is NOT a bitmap but  char fonts0={ char map }

extern int color;  // define this in your t.c file
extern char *tab;  // in your t.c file: char *tab = "0123456789ABCDEF";

u8 cursor;
int volatile *fb;
unsigned char *font;
int row, col, scrow_row = 4;
int WIDTH = 640;

extern uprintf(char *fmt, ...);  // write this function to print to UART 0

int fbuf_init()
{
  int x; int i;
  /**** for SVGA 800X600 these values are in ARM DUI02241 *********
  *(volatile unsigned int *)(0x1000001c) = 0x2CAC; // 800x600
  *(volatile unsigned int *)(0x10120000) = 0x1313A4C4;
  *(volatile unsigned int *)(0x10120004) = 0x0505F6F7;
  *(volatile unsigned int *)(0x10120008) = 0x071F1800;
  *(volatile unsigned int *)(0x10120010) = (1*1024*1024);  *(volatile unsigned t *)(0x10120018) = 0x82B;
  ***************************************************************/

  /********* for VGA 640x480 ************************/
  *(volatile unsigned int *)(0x1000001c) = 0x2C77;        // LCDCLK SYS_OSCCLK
  *(volatile unsigned int *)(0x10120000) = 0x3F1F3F9C;    // time0
  *(volatile unsigned int *)(0x10120004) = 0x090B61DF;    // time1
  *(volatile unsigned int *)(0x10120008) = 0x067F1800;    // time2
  *(volatile unsigned int *)(0x10120010) = (6*1024*1024); // panelBaseAddress
  *(volatile unsigned int *)(0x10120018) = 0x82B;         // control register
  
  /******** yet to figure out HOW TO use these palletes *************/
  fb = (int *)(6*1024*1024);  // at 6MB area; enough for 800x600 SVGA
  font = fonts0;              // use fonts0 for char bit patterns 

  // for 640x480 VGA mode display
  for (x=0; x<640*480; x++)
    fb[x] = 0x00000000;    // clean screen; all pixels are BLACK
  cursor = 127; // cursor bit map in font0 at 127
}

int clrpix(int x, int y)
{
  int pix = y*640 + x;
  fb[pix] = 0x00000000;
}

int setpix(int x, int y)
{
  int pix = y*640 + x;
  if (color==RED)
     fb[pix] = 0x000000FF;
  if (color==BLUE)
     fb[pix] = 0x00FF0000;
  if (color==GREEN)
     fb[pix] = 0x0000FF00;
  if (color==WHITE)
     fb[pix] = 0x00FFFFFF;
  if (color==YELLOW)
     fb[pix] = 0x0000FFFF;
  if (color==CYAN)
    fb[pix] = 0x00FFFF00;
}

int dchar(unsigned char c, int x, int y)
{
  int r, bit;
  unsigned char *caddress, byte;

  caddress = font + c*16;

  for (r=0; r<16; r++){
    byte = *(caddress + r);

    for (bit=0; bit<8; bit++){
      if (byte & (1<<bit))
	  setpix(x+bit, y+r);
    }
  }
}

int undchar(unsigned char c, int x, int y)
{
  int row, bit;
  unsigned char *caddress, byte;

  caddress = font + c*16;

  for (row=0; row<16; row++){
    byte = *(caddress + row);

    for (bit=0; bit<8; bit++){
      if (byte & (1<<bit))
	  clrpix(x+bit, y+row);
    }
  }
}

int dstring(char *s, int x, int y)
{
  while(*s){
    dchar(*s, x, y);
    x+=8;
    s++;
  }
}

/************
int scroll() // scroll to top of screen
{
  int i;
  for (i=0; i<640*480-640*16; i++){
    fb[i] = fb[i+ 640*16];
  } 
}
************/
int scroll() // scroll to scrow_row
{
  int i;
  for (i=scrow_row*16*640; i<640*480-640*16; i++){
    fb[i] = fb[i+ 640*16];
  } 
}
  
int kpchar(char c, int ro, int co)
{
   int x, y;
   x = co*8;
   y = ro*16;
   dchar(c, x, y);
   
}

int unkpchar(char c, int ro, int co)
{
   int x, y;
   x = co*8;
   y = ro*16;
   undchar(c, x, y);
}

int erasechar()
{ 
  // erase char at (row,col)
  int r, bit, x, y;
  unsigned char *caddress, byte;

  x = col*8;
  y = row*16;

  for (r=0; r<16; r++){
     for (bit=0; bit<8; bit++){
        clrpix(x+bit, y+r);
    }
  }
} 

int clrcursor()
{
  unkpchar(cursor, row, col);
}

int putcursor(unsigned char c)
{
  kpchar(c, row, col);
}

int kputc(char c)
{
  clrcursor();
  if (c=='\r'){
    col=0;
    //printf("row=%d col=%d\n", row, col);
    putcursor(cursor);
    return;
  }
  if (c=='\n'){
    row++;
    if (row>=25){
      row = 24;
      scroll();
    }
    //printf("row=%d col=%d\n", row, col);
    putcursor(cursor);
    return;
  }
  if (c=='\b'){
    if (col>0){
      col--;
      erasechar();
      putcursor(cursor);
    }
    return;
  }
  // c is ordinary char
  kpchar(c, row, col);
  col++;
  if (col>=80){
    col = 0;
    row++;
    if (row >= 25){
      row = 24;
      scroll();
    }
  }
  putcursor(cursor); 
  //printf("row=%d col=%d\n", row, col);
}

int kprints(char *s)
{
  while(*s){
    kputc(*s);
    s++;
  }
}

int krpx(int x)
{
  char c;
  if (x){
     c = tab[x % 16];
     krpx(x / 16);
  }
  kputc(c);
}

int kprintx(int x)
{
  kputc('0'); kputc('x');
  if (x==0)
    kputc('0');
  else
    krpx(x);
  kputc(' ');
}

int krpu(int x)
{
  char c;
  if (x){
     c = tab[x % 10];
     krpu(x / 10);
  }
  kputc(c);
}

int kprintu(int x)
{
  if (x==0){
    kputc(' ');kputc('0');
  }
  else
    krpu(x);
  kputc(' ');
}

int kprinti(int x)
{
  if (x<0){
    kputc('-');
    x = -x;
  }
  kprintu(x);
}

int kprintf(char *fmt,...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

  while(*cp){
    if (*cp != '%'){
      kputc(*cp);
      if (*cp=='\n')
	kputc('\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': kputc((char)*ip);      break;
    case 's': kprints((char *)*ip);  break;
    case 'd': kprinti(*ip);          break;
    case 'u': kprintu(*ip);          break;
    case 'x': kprintx(*ip);          break;
    }
    cp++; ip++;
  }
}

int show_bmp(char *p, int startRow, int startCol)
{ 
   int h, w, pixel, r1, r2, i, j; 
   unsigned char r, g, b;
   char *pp;
 
   int *q = (int *)(p+14); // skip over 14 bytes file header 
   q++;                    // skip 4 bytes in image header
   w = *q;                 // width in pixels 
   h = *(q + 1);           // height in pixels

   p += 54;                // p point at pixels now 

   // but the picture is up-side DOWN

   r1 = 3*w;
   r2 = 4*((3*w+3)/4);     // row size is a multiple of 4 bytes  
   p += (h-1)*r2;

   for (i=startRow; i<h+startRow; i += 1){
     pp = p;
     for (j=startCol; j<startCol+w; j+=1)
     {
        b = *pp; g = *(pp+1); r = *(pp+2);
        pixel = (b<<16) + (g<<8) + r;
        fb[(i/2)*640 + (j/2)] = pixel;
        pp += 3;    // back pp by 3 bytes
     }
     p -= r2*2;
   }
   //uprintf("\nBMP image height=%d width=%d\n", h, w);
}
