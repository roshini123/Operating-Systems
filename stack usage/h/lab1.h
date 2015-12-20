#define LINUXSCHED 1
#define MULTIQSCHED 2
extern int realhead;
extern int realtail;
extern int  getschedclass();
extern void setschedclass(int);
extern int  scheduleclass;
