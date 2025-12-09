#ifndef DCMOTOR_H
#define DCMOTOR_H

void DcMotor_init(void);
void DcMotor_run_forward(uint32_t accel);
void DcMotor_run_reverse(uint32_t accel);
void DcMotor_stop(void);

#endif


