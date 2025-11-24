//스위치 인터럽트 + 시퀀셜 턴 라이트

#include <stdint.h>
#include <stdbool.h>
#include "S32K144.h"

#define TURN_REPEAT_COUNT 5

//============================================================================
// PCC for Port Clock Enable
//============================================================================

#define PCC_BASE   (0x40065000U)

#define PCC_PORTA       (*(volatile unsigned*)(PCC_BASE + 0x124))
#define PCC_PORTB       (*(volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC       (*(volatile unsigned*)(PCC_BASE + 0x12C))
#define PCC_PORTD       (*(volatile unsigned*)(PCC_BASE + 0x130))
#define PCC_PORTE       (*(volatile unsigned*)(PCC_BASE + 0x134))

#define PCC_CGC_MASK (1U << 30)
#define CGC_BIT 30

//============================================================================
// PORT BASE / PCR
//============================================================================

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

#define IRQC_RISING_EDGE   0b1001
#define IRQC_FALLING_EDGE  0b1010
#define IRQC_EITHER_EDGE   0b1011

//============================================================================
// GPIO BASE
//============================================================================

#define GPIOA_BASE (0x400FF000U)
#define GPIOB_BASE (0x400FF040U)
#define GPIOC_BASE (0x400FF080U)
#define GPIOD_BASE (0x400FF0C0U)
#define GPIOE_BASE (0x400FF100U)

#define GPIOA_PDOR      (*(volatile unsigned*)(GPIOA_BASE + 0x0))
#define GPIOA_PSOR      (*(volatile unsigned*)(GPIOA_BASE + 0x4))
#define GPIOA_PCOR      (*(volatile unsigned*)(GPIOA_BASE + 0x8))
#define GPIOA_PTOR      (*(volatile unsigned*)(GPIOA_BASE + 0xC))
#define GPIOA_PDIR      (*(volatile unsigned*)(GPIOA_BASE + 0x10))
#define GPIOA_PDDR      (*(volatile unsigned*)(GPIOA_BASE + 0x14))

#define GPIOB_PDOR      (*(volatile unsigned*)(GPIOB_BASE + 0x0))
#define GPIOB_PSOR      (*(volatile unsigned*)(GPIOB_BASE + 0x4))
#define GPIOB_PCOR      (*(volatile unsigned*)(GPIOB_BASE + 0x8))
#define GPIOB_PTOR      (*(volatile unsigned*)(GPIOB_BASE + 0xC))
#define GPIOB_PDIR      (*(volatile unsigned*)(GPIOB_BASE + 0x10))
#define GPIOB_PDDR      (*(volatile unsigned*)(GPIOB_BASE + 0x14))

#define GPIOC_PDOR      (*(volatile unsigned*)(GPIOC_BASE + 0x0))
#define GPIOC_PSOR      (*(volatile unsigned*)(GPIOC_BASE + 0x4))
#define GPIOC_PCOR      (*(volatile unsigned*)(GPIOC_BASE + 0x8))
#define GPIOC_PTOR      (*(volatile unsigned*)(GPIOC_BASE + 0xC))
#define GPIOC_PDIR      (*(volatile unsigned*)(GPIOC_BASE + 0x10))
#define GPIOC_PDDR      (*(volatile unsigned*)(GPIOC_BASE + 0x14))

#define GPIOD_PDOR      (*(volatile unsigned*)(GPIOD_BASE + 0x0))
#define GPIOD_PSOR      (*(volatile unsigned*)(GPIOD_BASE + 0x4))
#define GPIOD_PCOR      (*(volatile unsigned*)(GPIOD_BASE + 0x8))
#define GPIOD_PTOR      (*(volatile unsigned*)(GPIOD_BASE + 0xC))
#define GPIOD_PDIR      (*(volatile unsigned*)(GPIOD_BASE + 0x10))
#define GPIOD_PDDR      (*(volatile unsigned*)(GPIOD_BASE + 0x14))

#define GPIOE_PDOR      (*(volatile unsigned*)(GPIOE_BASE + 0x0))
#define GPIOE_PSOR      (*(volatile unsigned*)(GPIOE_BASE + 0x4))
#define GPIOE_PCOR      (*(volatile unsigned*)(GPIOE_BASE + 0x8))
#define GPIOE_PTOR      (*(volatile unsigned*)(GPIOE_BASE + 0xC))
#define GPIOE_PDIR      (*(volatile unsigned*)(GPIOE_BASE + 0x10))
#define GPIOE_PDDR      (*(volatile unsigned*)(GPIOE_BASE + 0x14))

//============================================================================
// NVIC (for interrupt)
//============================================================================

#define NVIC_ISER_BASE   (0xE000E100U)   // Interrupt Set Enable
#define NVIC_ICER_BASE   (0xE000E180U)   // Interrupt Clear Enable
#define NVIC_ISPR_BASE   (0xE000E200U)   // Interrupt Set Pending
#define NVIC_ICPR_BASE   (0xE000E280U)   // Interrupt Clear Pending
#define NVIC_IABR_BASE   (0xE000E300U)   // Interrupt Active Bit
#define NVIC_IPR_BASE    (0xE000E400U)   // Interrupt Priority

#define NVIC_ISER0   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x00))
#define NVIC_ISER1   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x04))
#define NVIC_ISER2   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x08))
#define NVIC_ISER3   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x0C))

#define NVIC_ICER0   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x00))
#define NVIC_ICER1   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x04))
#define NVIC_ICER2   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x08))
#define NVIC_ICER3   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x0C))

#define NVIC_ISPR0   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x00))
#define NVIC_ISPR1   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x04))
#define NVIC_ISPR2   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x08))
#define NVIC_ISPR3   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x0C))

#define NVIC_ICPR0   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x00))
#define NVIC_ICPR1   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x04))
#define NVIC_ICPR2   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x08))
#define NVIC_ICPR3   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x0C))

#define NVIC_IABR0   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x00))
#define NVIC_IABR1   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x04))
#define NVIC_IABR2   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x08))
#define NVIC_IABR3   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x0C))

#define NVIC_IPR48  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x30)) // LPIT0
#define NVIC_IPR59  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3B)) // PORTA
#define NVIC_IPR60  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3C)) // PORTB
#define NVIC_IPR61  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3D)) // PORTC
#define NVIC_IPR62  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3E)) // PORTD
#define NVIC_IPR63  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3F)) // PORTE


//============================================================================
// LPIT Timer
//============================================================================

#define PCC_LPIT        (*(volatile unsigned*)(PCC_BASE + 0xDC))
#define PCS_BITS 24

#define LPIT_BASE (0x40037000)
#define LPIT_MCR (*(volatile unsigned*)(LPIT_BASE + 0x8))
#define LPIT_MSR (*(volatile unsigned*)(LPIT_BASE + 0xC))
#define LPIT_MIER (*(volatile unsigned*)(LPIT_BASE + 0x10))
#define LPIT_TVAL0 (*(volatile unsigned*)(LPIT_BASE + 0x20))
#define LPIT_TCTRL0 (*(volatile unsigned*)(LPIT_BASE + 0x28))

#define M_CEN_BIT 0
#define TIF0_BIT 0
#define TIE0_BIT 0
#define MODE_BITS 2
#define T_EN_BIT 0


//============================================================================
// ADC
//============================================================================

#define PCC_ADC0 *((volatile unsigned*)(PCC_BASE + 0xEC))
#define PCC_ADC1 *((volatile unsigned*)(PCC_BASE + 0x9C))

#define ADC0_BASE (0x4003B000)
#define ADC1_BASE (0x40027000)

#define ADC0_SC1A *((volatile unsigned*)(ADC0_BASE + 0x0)) //SC1B SC1C 는 4바이트 간격
#define ADC0_CFG1 *((volatile unsigned*)(ADC0_BASE + 0x40))
#define ADC0_CFG2 *((volatile unsigned*)(ADC0_BASE + 0x44))
#define ADC0_RA *((volatile unsigned*)(ADC0_BASE + 0x48)) //RB RC .. 는 4바이트 간격
#define ADC0_SC2 *((volatile unsigned*)(ADC0_BASE + 0x90))
#define ADC0_SC3 *((volatile unsigned*)(ADC0_BASE + 0x94))

#define ADC1_SC1A *((volatile unsigned*)(ADC1_BASE + 0x0)) //SC1B SC1C .. 는 4바이트 간격
#define ADC1_CFG1 *((volatile unsigned*)(ADC1_BASE + 0x40))
#define ADC1_CFG2 *((volatile unsigned*)(ADC1_BASE + 0x44))
#define ADC1_RA *((volatile unsigned*)(ADC1_BASE + 0x48)) //RB RC .. 는 4바이트 간격
#define ADC1_SC2 *((volatile unsigned*)(ADC1_BASE + 0x90))
#define ADC1_SC3 *((volatile unsigned*)(ADC1_BASE + 0x94))

#define ADCH_BITS 0
#define COCO_BIT 7
#define MODE_BITS 2
#define ADIV_BITS 5
#define SMPLTS_BITS 0
#define ADTRG_BIT 6

#define ADC0_SE12 12
#define ADC0_SE4 4

//============================================================================
// PWM (FTM + PWM) , PWM생성시에는 Flexible Timer 사용해야함
//============================================================================

#define PCC_FTM0 *((volatile unsigned*)(PCC_BASE + 0xE0))
#define PCC_FTM2  (*(volatile unsigned*)(PCC_BASE + 0xE8))

#define FTM0_BASE (0x40038000)
#define FTM0_SC *((volatile unsigned*)(FTM0_BASE + 0x0))
#define FTM0_MOD *((volatile unsigned*)(FTM0_BASE + 0x8))
#define FTM0_C1SC *((volatile unsigned*)(FTM0_BASE + 0x14))
#define FTM0_C1V *((volatile unsigned*)(FTM0_BASE + 0x18))
#define FTM0_CNTIN *((volatile unsigned*)(FTM0_BASE + 0x4C))

#define FTM2_BASE (0x4003A000)
#define FTM2_SC     (*(volatile unsigned*)(FTM2_BASE + 0x0))
#define FTM2_MOD    (*(volatile unsigned*)(FTM2_BASE + 0x8))
#define FTM2_C0SC   (*(volatile unsigned*)(FTM2_BASE + 0x0C))
#define FTM2_C0V    (*(volatile unsigned*)(FTM2_BASE + 0x10))
#define FTM2_CNTIN  (*(volatile unsigned*)(FTM2_BASE + 0x4C))

#define PWMEN1_BIT 17
#define PWMEN0_BIT 16
#define CLKS_BITS 3
#define PS_BITS 0
#define MSB_BIT 5
#define MSA_BIT 4
#define ELSB_BIT 3
#define ELSA_BIT 2

//============================================================================
// Sequential Pattern
//============================================================================

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

volatile bool g_left_request  = false;
volatile bool g_right_request = false;

void PORT_init(void);
void NVIC_init_IRQs(void);
void delay(volatile uint32_t d);
void run_left_sequential(void);
void run_right_sequential(void);

int main(void)
{
    PORT_init();
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

    PORTC_PCR(PTC12) &= ~(0b111 << MUX_BITS);
    PORTC_PCR(PTC12) |=  (1 << MUX_BITS);
    GPIOC_PDDR       &= ~(1 << PTC12);
    PORTC_PCR(PTC12) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC_PCR(PTC12) &= ~(0b1111 << IRQC_BITS);
    PORTC_PCR(PTC12) |=  (IRQC_FALLING_EDGE << IRQC_BITS);
    PORTC_PCR(PTC12) |= (1 << ISF_BIT);

    PORTC_PCR(PTC13) &= ~(0b111 << MUX_BITS);
    PORTC_PCR(PTC13) |=  (1 << MUX_BITS);
    GPIOC_PDDR       &= ~(1 << PTC13);
    PORTC_PCR(PTC13) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC_PCR(PTC13) &= ~(0b1111 << IRQC_BITS);
    PORTC_PCR(PTC13) |=  (IRQC_FALLING_EDGE << IRQC_BITS);
    PORTC_PCR(PTC13) |= (1 << ISF_BIT);

    for (int pin = 0; pin <= 7; ++pin)
    {
        PORTD_PCR(pin) &= ~(0b111 << MUX_BITS);
        PORTD_PCR(pin) |=  (1 << MUX_BITS);
        GPIOD_PDDR     |=  (1 << pin);
    }

    GPIOD_PDOR |= 0xFF;
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

void run_left_sequential(void)
{
    for (int rep = 0; rep < TURN_REPEAT_COUNT; rep++)
    {
        for (int i = 0; i < 9; i++)
        {
            unsigned char pattern = SEQUENTIAL_LIGHT_PATTERN_LEFT[i];
            unsigned char out = (~pattern) & 0xFF;
            GPIOD_PDOR = (GPIOD_PDOR & ~0xFF) | out;
            delay(500000);
        }
    }

    GPIOD_PDOR |= 0xFF;
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
            delay(500000);
        }
    }

    GPIOD_PDOR |= 0xFF;
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
