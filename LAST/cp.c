// Mike Berger
// CS460
// cp.c

#include "ucode.c"


int main(int argc, char* argv[])
{
    int sfd, dfd, i, n, count;
    char srcFilename[256], destFilename[256], c[1], buf[1024];

    // a current and new filename must be passed for this function to work
    if (argc != 3)
    {
        printf("cp usage: cp <src> <dest>\n");
        return 0;
    }
    else
    {
        strcpy(srcFilename, argv[1]);
        strcpy(destFilename, argv[2]);

        // open source file for read
        sfd = open(srcFilename, O_RDONLY);

        // make dest file and open it for write
        creat(destFilename);
        dfd = open(destFilename, O_WRONLY);

        if (sfd == 0 || dfd == 0)
        {
            printf("Error opening files\n");
            exit(1);
        }

        // while read from source, write to dest
        count = 0;
        while (n = read(sfd, buf, 1024))
        {
            write(dfd, buf, n);
            count += n;
        }
        printf("%d bytes copied to %s\n", count, destFilename);
        return 0;
    }
    
}