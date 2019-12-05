

typedef struct pipe{
    char buf[PSIZE]; // circular data buffer
    int head, tail; // circular buf index
    int data, room; // number of data & room in pipe
    int status; // FREE or BUSY
}PIPE;

PIPE pipe[NPIPE];   // global PIPE objects

int pipe_init()
{
    // Initialize all PIPE objects to FREE
    PIPE *p;
    for (int i = 0; i < NPIPE; i++)
    {
        p = &pipe[i];
        p->status = FREE;
    }
}

PIPE* create_pipe()
{
    // PIPE *create_pipe(): this creates a PIPE object in the (shared) address space of all the processes. 
    // It allocates a free PIPE object, initializes it and returns a pointer to the created PIPE object.
    PIPE *p;
    for (int i = 0; i < NPIPE; i++)
    {
        p = &pipe[i];
        if (pipe[i].status == FREE)
        {
            printf("Allocating pipe[%d]\n", i);
            p->head = p->tail = 0;
            p->data = 0; 
            p->room = PSIZE;
            p->status = BUSY;
            return p;
        }
    }

    printf("create_pipe error: No free pipes\n");
    return -1;

}

int print_pipe()
{
    PIPE *p = &pipe;
    printf("Contents of pipe: [");
    for (int i = 0; i<p->data; i++)
        printf("%c", p->buf[p->tail+i]);
    printf("]\n");
}

/*---------- Algorithm of pipe_read-------------*/
int read_pipe(PIPE *p, char *buf, int n)    //  try to read n bytes from the pipe p into buf
{
    int r = 0;
    if (n<=0)
    {
        printf("I guess there's nothing to read\n");
        return 0;
    }

    if (p->status == FREE)
    {
        // validate PIPE pointer p; 
        // p->status must not be FREE
        printf("read_pipe validation failed\n");
        return -1;
    }

    while(n)    // while there are still bytes to read
    {
        while(p->data)  // and while there is still data in the pipe
        {   
            *buf++ = p->buf[p->tail++]; // read a byte to buf
            p->tail %= PSIZE;  // p->tail? modulus assignment operator returns a remainder and assigns to left operand
            p->data--;  // update to reflect less data in pipe and more room
            p->room++; 
            r++;    // this is the return value. It is incremented every time a byte is read
            n--;   
            if (n == 0)   // when n == 0, all data has been read from the pipe
            {
                break;
            }
            if (p->data == 0)
                break;
        }
        kwakeup(&p->room); // wakeup writers
        if (r) // if has read some data
        {
            return r; // pipe has no data
        }
        printf("reader going to sleep\n");
        ksleep(&p->data); // sleep for data
    }
}

/*---------- Algorithm of write_pipe -----------*/
int write_pipe(PIPE *p, char *buf, int n)   // n is the number of bytes to write to the pipe p from buf
{
    int r = 0;
    if (n<=0)   // write no data
        return 0;

    if (p->status == FREE)
    {
        // validate PIPE pointer p; 
        // p->status must not be FREE
        printf("write_pipe validation failed\n");
        return -1;
    }

    while(n)    // while there is still data to write
    {
        while(p->room)      // and there is still room in the pipe
        {
            p->buf[p->head++] = *buf++; // write a byte to pipe;
            p->head %= PSIZE;
            p->data++; 
            p->room--; 
            r++; 
            n--;
            if (n == 0)   
            {
                printf("There is room for %d more bytes in the pipe\n", p->room);
                break;
            }
        }
        print_pipe();
        kwakeup(&p->data); // wakeup readers, if any.
        if (n==0)
            return r;   // finished writing n bytes
        // still has data to write but pipe has no room
        ksleep(&p->room); // sleep for room
    }
}

// (3). When a pipe is no longer needed, it may be freed by destroy_pipe(PIPE *pipePtr), which deallocates the PIPE object and
// wake up all the sleeping processes on the pipe.