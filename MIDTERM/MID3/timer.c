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
volatile TIMER timer; //4 timers; 2 per unit; at 0x00 and 0x20
void timer_init()
{
    int i; TIMER *tp;
    printf("timer_init()\n");
    tp = &timer;
    tp->base = (u32 *)0x101E2000;
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
void timer_handler(int n) 
{
    int i;
    TIMER *t = &timer;
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
void timer_start() // timer_start(0), 1, etc.
{
    TIMER *tp = &timer;
    kprintf("timer_start base=%x\n", tp->base);
    *(tp->base+TCNTL) |= 0x80;
    // set enable bit 7
}
int timer_clearInterrupt() // timer_start(0), 1, etc.
{
    TIMER *tp = &timer;
    *(tp->base+TINTCLR) = 0xFFFFFFFF;
}
void timer_stop()
{
    // stop a timer
    TIMER *tp = &timer;
    *(tp->base+TCNTL) &= 0x7F; // clear enable bit 7
}