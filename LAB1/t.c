/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD *gp;
INODE *ip;
DIR *dp;
u32 *up;

int color = 0x20;

int prints(char *s)
{
  while(*s)
    {
      putc(*s++);
    }
}

int gets(char *s)
{
  while((*s = getc()) != '\r')
    {
      getc(*s++);
    }
    *s = 0;
}


u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
}

u16 search(INODE *ip, char *name)
{  
  u16 i;
  char temp;
  //search for name in the data block of INODE; 
  //return its inumber if found
  //else error();
  for (i = 0; i < 12; i++)
  {
    //prints("i = "); putc(i + '0'); prints("\n\r");
    if ((u16)ip->i_block[i])
    {
      getblk((u16)ip->i_block[i], buf2);
      dp = (DIR *)buf2;
      while ((char *)dp < &buf2[BLK])
      {
        temp = dp->name[dp->name_len];
        dp->name[dp->name_len] = 0;
        prints(dp->name);
        putc(' ');
        if (strcmp(dp->name, name) == 0)
        {
          //prints(dp->name);prints("\n\r");
          return ((u16)dp->inode);
        }

        dp->name[dp->name_len] = temp;
        dp = (char *)dp + dp->rec_len;        
      }
    }
    error();
  }
}

main()
{ 
/*
1. Write YOUR C code to get the INODE of /boot/mtx
   INODE *ip --> INODE

   if INODE has indirect blocks: get i_block[12] int buf2[  ]


2. setes(0x1000);  // MTX loading segment = 0x1000

3. load 12 DIRECT blocks of INODE into memory beginning at segment 0x1000

4. load INDIRECT blocks, if any, into memory

   prints("go?"); getc();
*/
  u16 iblk, i;
  u8 ino;
  char *name[2];
  name[0] = "boot";
  name[1] = "mtx";

  // read group descriptor into gp
  getblk((u16)2, buf1);
  gp = (GD *)buf1;
  iblk = (u16)gp->bg_inode_table;

  // point ip to the root inode (ino #2)
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1;

  // now that ip theoretically points to the root inode, search the root inode's blocks for the boot directory
  for (i = 0; i < 2; i++)
  {
    ino = search(ip, name[i]) - 1;
    getblk(iblk + (ino/8), buf1);
    ip = (INODE *)buf1 + (ino % 8);
  }

  if ((u16)ip->i_block[12])
  {
    getblk((u16)ip->i_block[12], buf2);
  }
  
  setes(0x1000);

  for (i = 0; i < 12; i++)
  {
    getblk(ip->i_block[i], 0);
    inces();
  }

  if ((u16)ip->i_block[12])
  {
    up = (u32 *)buf2;
    while (*up)
    {
      getblk((u16)*up, 0);
      inces();
      up++;
    }
  }
  // 4. print file names in the root directory /
  prints("r?\n");getc();
}  
