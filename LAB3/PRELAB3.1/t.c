#include "defines.h" // LCD, TIMER and UART addresses
#include "string.c" // strcmp, strlen, etc
// #include "vid.c" // LCD driver file
//#include "exceptions.c" // other exception handlers

// timer register u32 offsets from base address
#define TLOAD 0x0
#define TVALUE 0x1
#define TCNTL 0x2
#define TINTCLR 0x3
#define TRIS 0x4
#define TMIS 0x5
#define TBGLOAD 0x6
typedef volatile struct timer
{
    u32 *base;
    // timer's base address; as u32 pointer
    int tick, hh, mm, ss; // per timer data area
    char clock[16];
}TIMER;

int color;

void copy_vectors(void) // copy vector table in ts.s to 0x0
{
    extern u32 vectors_start, vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while (vectors_src < &vectors_end)
    *vectors_dst++ = *vectors_src++;
}

volatile TIMER timer[4]; //4 timers; 2 per unit; at 0x00 and 0x20
void timer_init()
{
    int i; TIMER *tp;
    printf("timer_init()\n");
    for (i=0; i<4; i++)
    {
        tp = &timer[i];
        if (i==0) tp->base = (u32 *)0x101E2000;
        if (i==1) tp->base = (u32 *)0x101E2020;
        if (i==2) tp->base = (u32 *)0x101E3000;
        if (i==3) tp->base = (u32 *)0x101E3020;
        *(tp->base+TLOAD) = 0x0;
        // reset
        *(tp->base+TVALUE)= 0xFFFFFFFF;
        *(tp->base+TRIS) = 0x0;
        *(tp->base+TMIS) = 0x0;
        *(tp->base+TLOAD) = 0x100;
        // CntlReg=011-0010=|En|Pe|IntE|-|scal=01|32bit|0=wrap|=0x66
        *(tp->base+TCNTL) = 0x66;
        *(tp->base+TBGLOAD) = 0x1C00; // timer counter value
        tp->tick = tp->hh = tp->mm = tp->ss = 0; // initialize wall clock
        strcpy((char *)tp->clock, "00:00:00");
    }
}
void timer_handler(int n) 
{
    int i;
    TIMER *t = &timer[n];
    t->tick++;
    // Assume 120 ticks per second
    if (t->tick==120)
    {
        t->tick = 0; t->ss++;
        if (t->ss == 60)
        {
            t->ss = 0; t->mm++;
            if (t->mm == 60)
            {
                t->mm = 0; t->hh++; // no 24 hour roll around
            }
        }

        // clear last tick's text
        for (i=0; i< 8; i++)
        {
            unkpchar(t->clock[i], n, 70+i);
        }

        t->clock[7]='0'+(t->ss%10); t->clock[6]='0'+(t->ss/10);
        t->clock[4]='0'+(t->mm%10); t->clock[3]='0'+(t->mm/10);
        t->clock[1]='0'+(t->hh%10); t->clock[0]='0'+(t->hh/10);
    }
    color = n;
    // display in different color
    for (i=0; i<8; i++)
    {
        kpchar(t->clock[i], n, 70+i); // to line n of LCD
    }
    timer_clearInterrupt(n); // clear timer interrupt
}
void timer_start(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    kprintf("timer_start %d base=%x\n", n, tp->base);
    *(tp->base+TCNTL) |= 0x80;
    // set enable bit 7
}
int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
    TIMER *tp = &timer[n];
    *(tp->base+TINTCLR) = 0xFFFFFFFF;
}
void timer_stop(int n)
{
    // stop a timer
    TIMER *tp = &timer[n];
    *(tp->base+TCNTL) &= 0x7F; // clear enable bit 7
}

TIMER *tp[4]; // 4 TIMER structure pointers
void IRQ_handler() // IRQ interrupt handler in C
{
    // read VIC status registers to determine interrupt source
    int vicstatus = VIC_STATUS;
    // VIC status BITs: timer0,1=4, uart0=13, uart1=14
    if (vicstatus & (1<<4))
    {
        // bit4=1:timer0,1
        if (*(tp[0]->base+TVALUE)==0) // timer 0
        timer_handler(0);
        if (*(tp[1]->base+TVALUE)==0) // timer 1
        timer_handler(1);
    }
    if (vicstatus & (1<<5))
    {
        // bit5=1:timer2,3
        if (*(tp[2]->base+TVALUE)==0) // timer 2    
        timer_handler(2);
        if (*(tp[3]->base+TVALUE)==0)// timer 3    
        timer_handler(3);
    }
}

int main()
{
    int i;
    color = RED; // int color in vid.c file
    fbuf_init(); // initialize LCD driver
    printf("main starts\n");
    /* enable VIC for timer interrupts */
    VIC_INTENABLE = 0;
    VIC_INTENABLE |= (1<<4); // timer0,1 at VIC.bit4
    VIC_INTENABLE |= (1<<5);
    timer_init(); // timer2,3 at VIC.bit5
    for (i=0; i<4; i++) // start all 4 timers
    { 
        tp[i] = &timer[i];
        timer_start(i);
    }
    printf("Enter while(1) loop, handle timer interrupts\n");
}