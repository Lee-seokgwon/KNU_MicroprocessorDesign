#ifndef STEPMOTOR_H
#define STEPMOTOR_H

void step_motor_port_init (void);
void step_motor_delay(volatile uint32_t d);
void step_motor_step_left(void);
void step_motor_step_right(void);
#endif
