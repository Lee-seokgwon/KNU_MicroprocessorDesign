//시퀀셜 턴라이트 시그널 + 스탭모터 제어 코드
#include <stdint.h>
#include <stdbool.h>
#include "S32K144.h"

#define TURN_REPEAT_COUNT 5
#define STEPPER_STEPS_PER_PHASE  30
#define STEPPER_DELAY_PER_STEP   10000U

#define PCC_BASE   (0x40065000U)

#define PCC_PORTA       (*(volatile unsigned*)(PCC_BASE + 0x124))
#define PCC_PORTB       (*(volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC       (*(volatile unsigned*)(PCC_BASE + 0x12C))
#define PCC_PORTD       (*(volatile unsigned*)(PCC_BASE + 0x130))
#define PCC_PORTE       (*(volatile unsigned*)(PCC_BASE + 0x134))

#define CGC_BIT    30

#define PORTA_BASE (0x40049000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTD_BASE (0x4004C000U)
#define PORTE_BASE (0x4004D000U)

#define PORTA_PCR(n)    (*(volatile unsigned*)(PORTA_BASE + (n)*4))
#define PORTB_PCR(n)    (*(volatile unsigned*)(PORTB_BASE + (n)*4))
#define PORTC_PCR(n)    (*(volatile unsigned*)(PORTC_BASE + (n)*4))
#define PORTD_PCR(n)    (*(volatile unsigned*)(PORTD_BASE + (n)*4))
#define PORTE_PCR(n)    (*(volatile unsigned*)(PORTE_BASE + (n)*4))

#define MUX_BITS        8

#define PORT_PCR_PE_MASK  (1U << 1)
#define PORT_PCR_PS_MASK  (1U << 0)

#define IRQC_BITS       16
#define ISF_BIT         24

#define PTC12 12
#define PTC13 13

#define PTD0 0
#define PTD1 1
#define PTD2 2
#define PTD3 3
#define PTD4 4
#define PTD5 5
#define PTD6 6
#define PTD7 7

#define PTB0 0
#define PTB1 1
#define PTB2 2
#define PTB3 3

#define IRQC_RISING_EDGE   0b1001
#define IRQC_FALLING_EDGE  0b1010
#define IRQC_EITHER_EDGE   0b1011

#define GPIOA_BASE (0x400FF000U)
#define GPIOB_BASE (0x400FF040U)
#define GPIOC_BASE (0x400FF080U)
#define GPIOD_BASE (0x400FF0C0U)
#define GPIOE_BASE (0x400FF100U)

#define GPIOA_PDOR      (*(volatile unsigned*)(GPIOA_BASE + 0x0))
#define GPIOA_PDDR      (*(volatile unsigned*)(GPIOA_BASE + 0x14))

#define GPIOB_PDOR      (*(volatile unsigned*)(GPIOB_BASE + 0x0))
#define GPIOB_PDDR      (*(volatile unsigned*)(GPIOB_BASE + 0x14))

#define GPIOC_PDOR      (*(volatile unsigned*)(GPIOC_BASE + 0x0))
#define GPIOC_PDIR      (*(volatile unsigned*)(GPIOC_BASE + 0x10))
#define GPIOC_PDDR      (*(volatile unsigned*)(GPIOC_BASE + 0x14))

#define GPIOD_PDOR      (*(volatile unsigned*)(GPIOD_BASE + 0x0))
#define GPIOD_PDDR      (*(volatile unsigned*)(GPIOD_BASE + 0x14))

#define GPIOE_PDOR      (*(volatile unsigned*)(GPIOE_BASE + 0x0))
#define GPIOE_PDDR      (*(volatile unsigned*)(GPIOE_BASE + 0x14))

#define NVIC_ISER_BASE   (0xE000E100U)
#define NVIC_ICPR_BASE   (0xE000E280U)
#define NVIC_IPR_BASE    (0xE000E400U)

#define NVIC_ISER1   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x04))
#define NVIC_ICPR1   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x04))
#define NVIC_IPR61  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3D))

const unsigned char SEQUENTIAL_LIGHT_PATTERN_LEFT[9] = {
    0b00000001,
    0b00000011,
    0b00000111,
    0b00001111,
    0b00011111,
    0b00111111,
    0b01111111,
    0b11111111,
    0b00000000
};

const unsigned char SEQUENTIAL_LIGHT_PATTERN_RIGHT[9] = {
    0b10000000,
    0b11000000,
    0b11100000,
    0b11110000,
    0b11111000,
    0b11111100,
    0b11111110,
    0b11111111,
    0b00000000
};

const unsigned char STEPPER_SEQ[4] = {
    0b0001,
    0b0010,
    0b0100,
    0b1000
};

volatile int g_step_idx = 0;
volatile bool g_left_request  = false;
volatile bool g_right_request = false;

void PORT_init(void);
void STEPPER_init(void);
void NVIC_init_IRQs(void);
void delay(volatile uint32_t d);
void run_left_sequential(void);
void run_right_sequential(void);
void stepper_step_left(void);
void stepper_step_right(void);

int main(void)
{
    PORT_init();
    STEPPER_init();
    NVIC_init_IRQs();

    while (1)
    {
        if (g_left_request)
        {
            g_left_request = false;
            run_left_sequential();
        }

        if (g_right_request)
        {
            g_right_request = false;
            run_right_sequential();
        }
    }
}

void PORT_init (void)
{
    PCC_PORTD |= (1 << CGC_BIT);
    PCC_PORTC |= (1 << CGC_BIT);

    //PTC12 GPIO input & Interrupt (좌측 깜빡이 스위치)
    PORTC_PCR(PTC12) &= ~(0b111 << MUX_BITS);
    PORTC_PCR(PTC12) |=  (1 << MUX_BITS);
    GPIOC_PDDR       &= ~(1 << PTC12);
    PORTC_PCR(PTC12) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC_PCR(PTC12) &= ~(0b1111 << IRQC_BITS);
    PORTC_PCR(PTC12) |=  (IRQC_FALLING_EDGE << IRQC_BITS);
    PORTC_PCR(PTC12) |= (1 << ISF_BIT);

    //PTC13 GPIO input & Interrupt (우측 깜빡이 스위치)
    PORTC_PCR(PTC13) &= ~(0b111 << MUX_BITS);
    PORTC_PCR(PTC13) |=  (1 << MUX_BITS);
    GPIOC_PDDR       &= ~(1 << PTC13);
    PORTC_PCR(PTC13) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC_PCR(PTC13) &= ~(0b1111 << IRQC_BITS);
    PORTC_PCR(PTC13) |=  (IRQC_FALLING_EDGE << IRQC_BITS);
    PORTC_PCR(PTC13) |= (1 << ISF_BIT);

    //PTD0 ~ PTD7 GPIO output (깜빡이 LED 8개)
    for (int pin = 0; pin <= 7; ++pin)
    {
        PORTD_PCR(pin) &= ~(0b111 << MUX_BITS);
        PORTD_PCR(pin) |=  (1 << MUX_BITS);
        GPIOD_PDDR     |=  (1 << pin);
    }

    //LED 안켜지게 초기화
    GPIOD_PDOR |= 0xFF;
}

//PTB0~PTB3 GPIO output (스탭모터 제어용 출력핀)
void STEPPER_init(void)
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

void NVIC_init_IRQs(void)
{
    NVIC_ICPR1 |= (1 << (61 % 32));
    NVIC_ISER1 |= (1 << (61 % 32));
    NVIC_IPR61 = 10;
}

void delay(volatile uint32_t d) {
    while(d--) __asm("NOP");
}

void stepper_step_left(void)
{
    g_step_idx++;
    if (g_step_idx >= 4)
        g_step_idx = 0;

    unsigned char seq = STEPPER_SEQ[g_step_idx];

    GPIOB_PDOR = (GPIOB_PDOR & ~0x0F) | (seq & 0x0F);
}

void stepper_step_right(void)
{
    g_step_idx--;
    if (g_step_idx < 0)
        g_step_idx = 3;

    unsigned char seq = STEPPER_SEQ[g_step_idx];

    GPIOB_PDOR = (GPIOB_PDOR & ~0x0F) | (seq & 0x0F);
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
                stepper_step_left();
                delay(STEPPER_DELAY_PER_STEP);
            }
        }
    }

    GPIOD_PDOR |= 0xFF;
    GPIOB_PDOR &= ~0x0F;
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
                stepper_step_right();
                delay(STEPPER_DELAY_PER_STEP);
            }
        }
    }

    GPIOD_PDOR |= 0xFF;
    GPIOB_PDOR &= ~0x0F;
}

void PORTC_IRQHandler(void)
{
    if (PORTC_PCR(PTC12) & (1 << ISF_BIT))
    {
        PORTC_PCR(PTC12) |= (1 << ISF_BIT);

        if ((GPIOC_PDIR & (1 << PTC12)) == 0)
        {
            g_left_request = true;
        }
    }

    if (PORTC_PCR(PTC13) & (1 << ISF_BIT))
    {
        PORTC_PCR(PTC13) |= (1 << ISF_BIT);

        if ((GPIOC_PDIR & (1 << PTC13)) == 0)
        {
            g_right_request = true;
        }
    }
}
