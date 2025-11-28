#include "inc/common.h"
#include "inc/step_motor.h"


extern int g_step_idx = 0;

const unsigned char STEPPER_SEQ[4] = {
    0b0001,
    0b0010,
    0b0100,
    0b1000
};

//PTB0~PTB3 GPIO output (스탭모터 제어용 출력핀)
void step_motor_port_init(void)
{
    PCC_PORTB |= (1 << CGC_BIT);

    PORTB_PCR(PTB0) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB0) |=  (1 << MUX_BITS);
    PORTB_PCR(PTB1) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB1) |=  (1 << MUX_BITS);
    PORTB_PCR(PTB2) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB2) |=  (1 << MUX_BITS);
    PORTB_PCR(PTB3) &= ~(0b111 << MUX_BITS);
    PORTB_PCR(PTB3) |=  (1 << MUX_BITS);

    GPIOB_PDDR |= (1 << PTB0) | (1 << PTB1) | (1 << PTB2) | (1 << PTB3);

    GPIOB_PDOR &= ~0x0F;
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

    GPIOB_PDOR = (GPIOB_PDOR & ~0x0F) | (seq & 0x0F);
}

void step_motor_step_right(void)
{
    g_step_idx--;
    if (g_step_idx < 0)
        g_step_idx = 3;

    unsigned char seq = STEPPER_SEQ[g_step_idx];

    GPIOB_PDOR = (GPIOB_PDOR & ~0x0F) | (seq & 0x0F);
}