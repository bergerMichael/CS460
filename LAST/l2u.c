// Mike Berger
// CS460
// l2u.c

#include "ucode.c"

int main(int argc, char *argv[])
{
    int rfd, wfd, i, n;
    char filename[256], newFilename[256], buf1[1024], buf2[1024], c[1];

    if (argc == 1)
    {
        while(n = read(0, c, 1))
        {
            if (c[0] == '\r' || c[0] == '\n')
            {
                mputc('\n');
                continue;
            }
            else if (c[0] >= 97 && c[0] <= 122)        // ascii values 97 - 122 are all lowercase chars
            {
                c[0] = c[0] - 32; // subtract 32 from these ascii values to convert to upper case
            }
            write(1, c, 1);
        }
    }

    if (argc == 2)  // if only one filename passed
    {
        // do nothing I guess
        return 0;
    }

    if (argc == 3)  // if two filenames are passed
    {
        strcpy(filename, argv[1]);
        strcpy(newFilename, argv[2]);
        // open filename for read
        rfd = open(filename, O_RDONLY);

        // create a new file
        creat(newFilename);
        
        // open new file for write
        wfd = open(newFilename, O_WRONLY);

        while (n = read(rfd, buf1, 1024))   // read the file into a buffer
        {
            for(i = 0; i < n; i++)
            {
                // check each char. If lowercase, conver to upper and save to new file
                if (buf1[i] >= 97 && buf1[i] <= 122)        // ascii values 97 - 122 are all lowercase chars
                {
                    buf2[i] = buf1[i] - 32; // subtract 32 from these ascii values to convert to upper case
                }
                else
                    buf2[i] = buf1[i];
            }
            write(wfd, buf2, n);
            // clear buffers
            memset(buf1, 0, 1024);
            memset(buf2, 0, 1024);
        }
    }
}