#include "inc/common.h"
#include "inc/step_motor.h"


extern int g_step_idx = 0;

const unsigned char STEPPER_SEQ[4] = {
    0b0001,
    0b0010,
    0b0100,
    0b1000
};

//PTB4~PTB7 GPIO output (스탭모터 제어용 출력핀)
void step_motor_port_init(void)
{
    PCC_PORTB |= (1 << CGC_BIT);

    PORTB_PCR(PTB4) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB4) |=  (1 << MUX_BITS);
    PORTB_PCR(PTB5) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB5) |=  (1 << MUX_BITS);
    PORTB_PCR(PTB6) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB6) |=  (1 << MUX_BITS);
    PORTB_PCR(PTB7) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB7) |=  (1 << MUX_BITS);

    GPIOB_PDDR |= (1 << PTB4) | (1 << PTB5) | (1 << PTB6) | (1 << PTB7);

    GPIOB_PDOR &= ~0xF0;
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

    GPIOB_PDOR = (GPIOB_PDOR & ~0xF0) | (seq & 0x0F);
}

void step_motor_step_right(void)
{
    g_step_idx--;
    if (g_step_idx < 0)
        g_step_idx = 3;

    unsigned char seq = STEPPER_SEQ[g_step_idx];

    GPIOB_PDOR = (GPIOB_PDOR & ~0xF0) | (seq & 0x0F);
}