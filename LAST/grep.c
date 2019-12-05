// Michael Berger
// CS 460
// grep.c

#include "ucode.c"

/*
grep with filename:
    1) open filename for read
    2) read 1024 bytes into buf1
    3) for each line in buf1 (denoted by n or \r)
        4) read that line into buf2
        5) search for pattern is buf 2
        6) if it is found, print buf2 to terminal
        7) otherwise, continue
    8) when EOF is reached, close the file and return
grep without filename:
    1) read stdin
    2) if the pattern is found on a line, print it to terminal
*/

int main(int argc, char *argv[])
{
    int fd, i, j, k, n, patternLen, lineLen, matchCount, matchFound;
    char buf1[1024], buf2[1024], pattern[256], filename[256], c[1];

    printf("-------- Mike B's grep --------");
    mputc('\n');

    strcpy(pattern, argv[1]);
    patternLen = strlen(pattern);

    if (argc == 2)  // no filename is passed
    {
        matchCount = 0;
        matchFound = 0;
        i = 0;
        while (n = read(0, c, 1))  // read stdin
        {            
            buf2[i] = c[0];

            if (matchCount == patternLen)
            {
                matchFound = 1;
            }
            if (pattern[matchCount] == c[0])
            {
                matchCount++;
            }
            else
                matchCount = 0;  
            i++;

            if (c[0] == '\r' || c[0] == '\n')
            {
                i = 0;                
                if (matchFound == 1)
                {
                    printf("\n%c%s\n", buf2[0], buf2);                    
                    matchFound = 0;
                }
                else
                    mputc('\n');
                
                memset(buf2, 0, 1024);
            }
            
            mputc(c[0]);
        }
    }

    else
    {        
        strcpy(filename, argv[2]);        

        fd = open(filename, O_RDONLY);

        while(n = read(fd, buf1, 1024)) // read 1024 bytes of the file at a time
        {
            strcpy(buf2, "");   // clear buf2 for a new string
            j = 0;  // j is needed to track separate indexing on buf, which holds lines extracted from buf1
            for(i = 0; i < n; i++)  // for each read, find one line at a time
            {
                if (buf1[i] == '\r' || buf1[i] == '\n') // if end of line is reached
                {                                        
                    // search for pattern in buf2
                    lineLen = strlen(buf2);
                    matchCount = 0;
                    for (k = 0; k < lineLen; k++)
                    {
                        if (matchCount == patternLen)
                        {
                            printf("%s", buf2);
                            //mputc('\n');
                            break;
                        }
                        if (pattern[matchCount] == buf2[k])
                        {
                            matchCount++;
                        }
                        else
                            matchCount = 0;                        
                    }
                    // clear buf2 at the end
                    matchCount = 0;
                    memset(buf2, 0, 1024);
                    j = 0;
                }
                buf2[j] = buf1[i];
                j++;
            }
        }
        close(fd);
    }

    mputc('\n');
    printf("-------- Mike B's grep --------");
    mputc('\n');
}
