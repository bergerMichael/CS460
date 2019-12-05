// Mike Berger
// CS460
// ls.c
// This program references code found on page 299-300 of embedded systems textbook

#include "ucode.c"

// permission bits
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
struct stat mystat, *sp;
char buffer[1024], cwd[256];

int ls_file(char *fileName)     // lists a single file
{
    struct stat fstat, *sp = &fstat;
    int r, i;
    char sbuf[4096];

    r = stat(fileName, sp);    // populate the stat pointer by lstatting the file
    
    if (sp->st_mode == S_ISDIR)   // check if the file is a directory
        printf("%c", 'd');
    if (sp->st_mode == S_ISREG)      // check if the file is a regular file
        printf("%c", '-');
    if (sp->st_mode == S_ISLNK)   // check if the file is a symbolic link
        printf("%c", 'l');

    for (i = 8; i >= 0; i--)
    {
        if (sp->st_mode & (1<<i))
            printf("%c", (char*)t1[i]);    // print permission bit of r w or x
        else
            printf("%c", (char*)t2[i]);    // print permission bit -
    }

    printf(" %d ", sp->st_nlink);   // print link count
    printf("%d ", sp->st_uid);     // print uid
    printf("%d ", sp->st_size);    //print file size

    printf("%s", fileName);   // print file basename
    
    if (sp->st_mode == S_ISLNK)   // if the file is a symbolic link
    {
        r = readlink(fileName, sbuf);
        printf(" -> %s", sbuf); // print the linked pathname
    }

    mputc('\n');
    //printf("\n");
}

int ls_dir(char *dirName)   // list a directory
{
    char name[256];
    char *cp;
    DIR *dp, *ep;
    int dirFD;

    // open directory to read names
    dirFD = open(dirName, O_RDONLY);    // open the directory for read
    read(dirFD, buffer, 1024);     // read the directory into buf
    cp = buffer;    // point cp to buffer
    dp = (DIR *) buffer;    // cast buffer to a director struct and point dp to it

    // ls each entry in the directory
    while (cp < 1024 + buffer)
    {
        strcpy(name, dp->name);
        ls_file(name);

        // point to the next entry
        cp += dp->rec_len;
        dp = (DIR *) cp;
    }
}

int main(int argc, char *argv[])
{
    struct stat mstat, *sp;
    int r;
    char *s;
    char filename[1024], cwd[1024];

    printf("------------ Mike B's ls! ------------");
    mputc('\n');

    s = argv[1];
    if (argc < 2)   // if no path is passed
    {
        getcwd(cwd);
        strcpy(filename, cwd);
        ls_dir(filename);        // ls cwd
    }

    else
    {
        sp = &mystat;
        if ((r = stat(s, sp)) < 0)
        {
            printf("ERROR - ls");
            exit(1);
        }
        ls_file(filename);
    }
    printf("------------ Mike B's ls! ------------");
    mputc('\n');
}