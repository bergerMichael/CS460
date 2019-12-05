// Mike Berger
// CS460
// sh.c

#include "ucode.c"

// must support io redirection ( < > >> ) and pipes ( cmd1 | cmd2 )

// embedded systems pg 228:
/*
int pid, status;
while(1)
{
    display executable commands in /bin directory
    prompt for a command line cmdline = "cmd a1 a2 .... an"
    if (!strcmp(cmd,"exit"))
        exit(0);
    // fork a child process to execute the cmd line
    pid = fork();
    if (pid)    // parent sh waits for child to die
        pid = wait(&status);
    else    // child exec cmdline
        exec(cmdline);  // exec("cmd a1 a2 ... an");
}
*/

int main(int argc, char *argv[])
{
    int pid, i, n, m;
    char cmd[256], cmdline[1024], c;

    while (1)
    {
        printf("sh #%d", -1); // need pid? -1 is a placeholder
        strcpy(cmdline, "");    // clear cmdline to start

        // read a line from stdin
        gets(cmdline);
        /*
        for(i = 0; n = read(0, c, 1); i++)
        {
            write(1, c, 1);
            if (c == '\r' || c == '\n') // continue on '\n' or '\n' character
            {
                cmdline[i] = '\0';
                break;
            }
            cmdline[i] = c;
        }
        */

        // at this point, cmdline contains the command and arguments (including | < > >>)
        // from here, need to tokenize cmdline and handle each token appropriately
        token(cmdline); // according to crt0.c, token will tokenize the command line and write the tokens to *argv[]
        // the following loop tests this
        n = 10;
        for (i = 0; i < n; i++)
        {
            printf("%s ", argv[i]);
        }

        if (strcmp(cmd, "exit") == 0)
            exit(0);

        pid = fork();
    }
}