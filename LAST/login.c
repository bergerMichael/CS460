// Michael Berger
// CS 460
// login.c : Upon entry, argv[0]=login, argv[1]=/dev/ttyX
#include "ucode.c"

/******Contents of /etc/passwd*******
root:12345:0:0:super user:/:sh
kcw:abcde:1:1:kcw user:/user/kcw:sh
guest:mercy:2:2:guest user:/user/guest:sh
buster:123:3:3:buster dog:/user/buster:sh
*/

int in, out, err;
int pw_file;   // file descriptor for password file
int i, j, k, success;
char username[128],password[128], activeTerm[128];
char accountTokens[64][128];

char *userTok, *passTok, *fullnameTok, *homeTok, *progTok;
int uidTok, gidTok;

int read_file(int fd, char *buf)
{
    int readBytes = read(fd, buf, 151); // nbytes = 151 because that is the size of the file.
    // TODO: adjust this function to support variable file lengths
    return 0;
}

int tokenize(char* line)
{
    // save each token into account tokens
    int correctUser, correctPass, count;
    int length = strlen(line);
    char tok[128];
    j = 0;
    k = 0;
    count = 0;
    correctUser = 0;
    correctPass = 0;
    for (i = 0; i < length; i++)
    {
        if (line[i] != ':' && line[i] != '\r' && line[i] != '\n' && line[i] != 0 && line[i] != EOF)
        {
            tok[j] = line[i];
            j++;
        }
        else
        {
            // append '\0' to tok
            tok[j] = '\0';
            // save the token
            strcpy(accountTokens[k], tok);
            // clear tok
            j = 0;
            // increment k
            k++;
        }    
    }
}

int check_credentials()
{
    for(i = 0; i < 128; i++)
    {
        if (strcmp(username, accountTokens[i]) == 0) // matching user
        {
            if (strcmp(password, accountTokens[i + 1]) == 0) // matching password
            {
                chuid(atoi(accountTokens[i + 2]), atoi(accountTokens[i + 3]));
                chdir(accountTokens[i + 5]);
                close(pw_file);
                exec(accountTokens[i + 6]);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    /*****Algorithm of login from embedded systems textbook*****/
    /// (1). close file descriptors 0,1 inherited from INIT.
    /// (2). open argv[1] 3 times as in(0), out(1), err(2).
    /// (3). settty(argv[1]); // set tty name string in PROC.tty
    /// (4). open /etc/passwd file for READ;
    ///      while(1)
    ///      {
    /// (5).     printf("login:");
    ///          gets(name);
    ///          printf("password:"); gets(password);
    ///          for each line in /etc/passwd file do
    ///          {
    ///              tokenize user account line;  
    /// (6).         if (user has a valid account)
    ///              { 
    /// (7).             change uid, gid to user's uid, gid; // chuid()
    ///                  change cwd to user's home DIR // chdir()
    ///                  close opened /etc/passwd file // close()
    /// (8).             exec to program in user account // exec()
    ///              }
    ///       }
    ///       printf("login failed, try again\n");

    char fileBuf[151];
    int count;

    close(0); close(1); // close file descriptors 0,1 inherited from INIT.

    // open argv[1] 3 times as in(0), out(1), err(2).
    in = open(argv[1], O_RDONLY);   // O_RDONLY == 0
    out = open(argv[1], O_WRONLY);  // O_WRONLY == 1
    err = open(argv[1], O_RDWR);    // O_RDWR == 2

    // ensure the correct terminal is obtained below:
    gettty(activeTerm);
    settty(argv[1]); // set tty name string in PROC.tty

    while (1)
    {
        pw_file = open("/etc/passwd", O_RDONLY); // open /etc/passwd file for READ;
        printf("\nMB login:");
        gets(username);
        printf("\nMB password:");
        gets(password);

        // account line format:
        // username:password:gid:uid:fullname:home dir:program

        read(pw_file, fileBuf, 151); // nbytes = 151 because that is the size of the file.
        //printf("\n%s\n", fileBuf);
        // now that fileBuf contains the contents of the credentials file, begin tokenizing
        // with delimiter ':'

        tokenize(fileBuf);
        // check each token against username
        // if there is a match, check the next token against password.
        // if there is a match, use the next 5 tokens to set things up

        printf("Checking credentials...");
        mputc('\n');
        check_credentials();

        printf("login failed, try again");
        mputc('\n');
    }
}