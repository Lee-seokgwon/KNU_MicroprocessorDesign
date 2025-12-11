//LED랑 StepMotor를 통합하여 시퀀셜턴 시그널 및 그에따른 스탭모터 작동을 구현
#include "common.h"
#include "sequential_turn_light.h"
#include "step_motor.h"
#include "step_motor_and_turn_light_controller.h"

void run_sequential_delay(volatile uint32_t d) {
    while(d--) __asm("NOP");
}

void run_left_sequential(void)
{
    for (int rep = 0; rep < TURN_REPEAT_COUNT; rep++)
    {
        for (int i = 0; i < 9; i++)
        {
            unsigned char pattern = SEQUENTIAL_LIGHT_PATTERN_LEFT[i];
            unsigned char out = (~pattern) & 0xFF;

            GPIOD_PDOR = (GPIOD_PDOR & ~0xFF) | out;

            for (int s = 0; s < STEPPER_STEPS_PER_PHASE; s++)
            {
                step_motor_step_left();
                run_sequential_delay(STEPPER_DELAY_PER_STEP);
            }
        }
    }

    GPIOD_PDOR |= 0xFF;
    GPIOB_PDOR &= ~0xF0;

    g_left_request = false; //수행 후 플래그 초기화
}

void run_right_sequential(void)
{
    for (int rep = 0; rep < TURN_REPEAT_COUNT; rep++)
    {
        for (int i = 0; i < 9; i++)
        {
            unsigned char pattern = SEQUENTIAL_LIGHT_PATTERN_RIGHT[i];
            unsigned char out = (~pattern) & 0xFF;

            GPIOD_PDOR = (GPIOD_PDOR & ~0xFF) | out;

            for (int s = 0; s < STEPPER_STEPS_PER_PHASE; s++)
            {
                step_motor_step_right();
                run_sequential_delay(STEPPER_DELAY_PER_STEP);
            }
        }
    }

    GPIOD_PDOR |= 0xFF;
    GPIOB_PDOR &= ~0xF0;
    g_right_request = false; //수행 후 플래그 초기화
}
