static int HARDTICKval;
static NBENCOUNTEREDINT;
#define THRESHOLD 400
#undef HardClock

int HardTick()
{
int inter;
inter = HardClock();
if (inter-HARDTICKval > THRESHOLD) NBENCOUNTEREDINT ++;
HARDTICKval= inter;
return(inter);
}
int INTERRUPT(int i)
{
if (NBENCOUNTEREDINT <i) return(1);
NBENCOUNTEREDINT=0;
return(0);
}