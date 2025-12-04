#include "common.h"
#include "pwm.h"
#include "dcmotor.h"

void DcMotor_run_forward(uint32_t accel)
{
    FTM2_C0V = accel;
}