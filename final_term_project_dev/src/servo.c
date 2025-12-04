#include "common.h"
#include "pwm.h"

#define SERVO_MIN_TICKS 1200
#define SERVO_MAX_TICKS 4800
#define SERVO_PERIOD_TICKS 40000

//servo PTA11(FTM1_CH5)
void servo_run_90()
{
    FTM1_C5V = 4800;
}

void servo_run_reverse_90()
{
    FTM1_C5V = 1200;
}

