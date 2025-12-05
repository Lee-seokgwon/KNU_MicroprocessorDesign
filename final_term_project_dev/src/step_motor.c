#include "common.h"
#include "step_motor.h"


int g_step_idx = 0;

const unsigned char STEPPER_SEQ[4] = {
    0b0001,
    0b0010,
    0b0100,
    0b1000
};

//PTA6~PTA9 GPIO output (스탭모터 제어용 출력핀)
void step_motor_port_init(void)
{
    PCC_PORTA |= (1 << CGC_BIT);

    PORTA_PCR(PTA6) &= ~(0b111 << MUX_BITS);
    PORTA_PCR(PTA6) |=  (1 << MUX_BITS);
    PORTA_PCR(PTA7) &= ~(0b111 << MUX_BITS);
    PORTA_PCR(PTA7) |=  (1 << MUX_BITS);
    PORTA_PCR(PTA8) &= ~(0b111 << MUX_BITS);
    PORTA_PCR(PTA8) |=  (1 << MUX_BITS);
    PORTA_PCR(PTA9) &= ~(0b111 << MUX_BITS);
    PORTA_PCR(PTA9) |=  (1 << MUX_BITS);

    GPIOA_PDDR |= (1 << PTA6) | (1 << PTA7) | (1 << PTA8) | (1 << PTA9);

    GPIOA_PDOR &= ~((1U << PTA6) | (1U << PTA7) | (1U << PTA8) | (1U << PTA9));
}

void step_motor_delay(volatile uint32_t d) {
    while(d--) __asm("NOP");
}

void step_motor_step_left(void)
{
    g_step_idx++;
    if (g_step_idx >= 4)
        g_step_idx = 0;

    unsigned char seq = STEPPER_SEQ[g_step_idx];

    uint32_t pattern = 0;

    if (seq & 0b0001) pattern |= (1 << PTA6);
    if (seq & 0b0010) pattern |= (1 << PTA7);
    if (seq & 0b0100) pattern |= (1 << PTA8);
    if (seq & 0b1000) pattern |= (1 << PTA9);

    GPIOA_PDOR =
        (GPIOA_PDOR & ~((1 << PTA6) | (1 << PTA7) | (1 << PTA8) | (1 << PTA9)))
        | pattern;

    step_motor_delay(5000);   // 3~5ms 권장
}

void step_motor_step_right(void)
{
    g_step_idx--;
    if (g_step_idx < 0)
        g_step_idx = 3;

    unsigned char seq = STEPPER_SEQ[g_step_idx];

    uint32_t pattern = 0;

    if (seq & 0b0001) pattern |= (1 << PTA6);
    if (seq & 0b0010) pattern |= (1 << PTA7);
    if (seq & 0b0100) pattern |= (1 << PTA8);
    if (seq & 0b1000) pattern |= (1 << PTA9);

    GPIOA_PDOR =
        (GPIOA_PDOR & ~((1 << PTA6) | (1 << PTA7) | (1 << PTA8) | (1 << PTA9)))
        | pattern;

    step_motor_delay(5000);   // 3~5ms 권장
}
