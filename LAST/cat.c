// Mike Berger
// CS460
// cat.c
#include "ucode.c"

/*
==========================  HOW TO cat ======================================
cat filename:
   char mybuf[1024], dummy = 0;  // a null char at end of mybuf[ ]
   int n;
1. int fd = open filename for READ;
2. while( n = read(fd, mybuf[1024], 1024)){
       mybuf[n] = 0;             // as a null terminated string
       // printf("%s", mybuf);   <=== THIS works but not good
       spit out chars from mybuf[ ] but handle \n properly;
   } 
3. close(fd);
*/
char buf[1024], cwd[256], filename[1024], c[1];
int n;

int main(int argc, char *argv[])
{
    int fd, fd2, i;
    char *s;

    printf("------------ Mike B's cat! ------------");
    mputc('\n');

    s = argv[1];
    if (argc == 1)   // if no path is passed
    {
        // copy stdin to stdout
        i = 0;
        strcpy(buf, ""); // make sure buf is clear
        while(n = read(0, c, 1))
        {
            if (c[0] == '\r' || c[0] == '\n') // if a return carrige or newline is encountered, print and continue
            {
                //strcat(buf, c);
                printf("\n%s\n", buf);
                strcpy(buf, "");
                continue;
            }

            write(1, c, 1);   // display incoming characters 
            
            if (c == 3) // if ^C is encountered, return
                return 0;

            if (i >= 1024)
            {
                printf("\n%s", buf);
                strcpy(buf, "");
                continue;
            }
            
            strcat(buf, c); // append to buf so we can print everything at once
            i++;
        }
    }

    else    // cat filename
    {
        strcpy(filename, argv[1]);
        printf("Executing MB cat on: %s\n", filename);
        fd = open(filename, O_RDONLY);
        while (n = read(fd, buf, 1024))
        {
            for(i = 0; i < n; i++)
            {
                mputc(buf[i]);
            }
            strcpy(buf, "");
        }
        close(fd);
    }
    printf("------------ Mike B's cat! ------------");
    mputc('\n');
}