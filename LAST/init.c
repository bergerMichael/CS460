// Michael Berger
// CS 460
// init.c

#include "ucode.c"

int console;
int s0, s1; // need to support login on console and two serial

int parent()    // P1's code
{
    int pid, status;
    while(1)
    {
        printf("INIT : wait for ZOMBIE child\n");
        pid = wait(&status);
        if (pid==console)
        {
            // if console login process died
            printf("INIT: forks a new console login\n");
            console = fork(); // fork another one
            if (console)
                continue;
            else
                exec("login /dev/tty0"); // new console login process
        }


        if (pid == s0) //multi user login
        {
            s0 = fork();
            if (s0)
            {
                continue;
            }
            else
            {
                printf("login /dev/ttyS0");
                exec("login /dev/ttyS0");
            }
        }
        if (pid == s1)
        {
            s1 = fork();
            if (s1)
            {
                continue;
            }
            else
            {
                printf("login /dev/ttyS1");
                exec("login /dev/ttyS1");
            }
        }

        printf("INIT: I just buried an orphan child proc %d\n", pid);
    }
}

main()
{
    int in, out;    // file descriptors for terminal I/O
    in = open("/dev/tty0", O_RDONLY); // file descriptor 0
    out = open("/dev/tty0", O_WRONLY); // for display to console
    printf("INIT : fork a login proc on console\n");
    console = fork();

    if (console)    // if console != 0 then it is the parent
    {
        s0 = fork();
        if (s0)
        {
            s1 = fork();
            if (s1)
            {
                parent();
            }
            else
            {
                exec("login /dev/ttyS1");
            }
        }
        else
        {
            exec("login /dev/ttyS0");
        }
    }
    else    // child: exec to login on tty0
        exec("login /dev/tty0");
}