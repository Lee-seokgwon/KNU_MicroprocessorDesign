#ifndef STEPMOTORANDTURNLIGHTCONTROLLER_H
#define STEPMOTORANDTURNLIGHTCONTROLLER_H

#define TURN_REPEAT_COUNT 5
#define STEPPER_STEPS_PER_PHASE  30
#define STEPPER_DELAY_PER_STEP   10000U

void run_left_sequential(void);
void run_right_sequential(void);
void run_sequential_delay(volatile uint32_t d);
#endif
