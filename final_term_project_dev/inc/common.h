#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "S32K144.h" //클럭 생성용 임시 헤더

//============================================================================
// PCC
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
// PCC
//============================================================================




//============================================================================
// PORTn_PCRx MUX setting
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

#define PTA6 6
#define PTA7 7
#define PTA8 8
#define PTA9 9

#define PTB0 0
#define PTB1 1
#define PTB2 2
#define PTB3 3
#define PTB4 4
#define PTB5 5
#define PTB6 6
#define PTB7 7

#define PTC12 12
#define PTC13 13
#define PTC17 17

#define PTD0 0
#define PTD15 15
#define PTD16 16
//============================================================================
// dn_PCRx MUX setting
//============================================================================





//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
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
#define GPIOA_PIDR      (*(volatile unsigned*)(GPIOA_BASE + 0x18))

#define GPIOB_PDOR      (*(volatile unsigned*)(GPIOB_BASE + 0x0))
#define GPIOB_PSOR      (*(volatile unsigned*)(GPIOB_BASE + 0x4))
#define GPIOB_PCOR      (*(volatile unsigned*)(GPIOB_BASE + 0x8))
#define GPIOB_PTOR      (*(volatile unsigned*)(GPIOB_BASE + 0xC))
#define GPIOB_PDIR      (*(volatile unsigned*)(GPIOB_BASE + 0x10))
#define GPIOB_PDDR      (*(volatile unsigned*)(GPIOB_BASE + 0x14))
#define GPIOB_PIDR      (*(volatile unsigned*)(GPIOB_BASE + 0x18))

#define GPIOC_PDOR      (*(volatile unsigned*)(GPIOC_BASE + 0x0))
#define GPIOC_PSOR      (*(volatile unsigned*)(GPIOC_BASE + 0x4))
#define GPIOC_PCOR      (*(volatile unsigned*)(GPIOC_BASE + 0x8))
#define GPIOC_PTOR      (*(volatile unsigned*)(GPIOC_BASE + 0xC))
#define GPIOC_PDIR      (*(volatile unsigned*)(GPIOC_BASE + 0x10))
#define GPIOC_PDDR      (*(volatile unsigned*)(GPIOC_BASE + 0x14))
#define GPIOC_PIDR      (*(volatile unsigned*)(GPIOC_BASE + 0x18))

#define GPIOD_PDOR      (*(volatile unsigned*)(GPIOD_BASE + 0x0))
#define GPIOD_PSOR      (*(volatile unsigned*)(GPIOD_BASE + 0x4))
#define GPIOD_PCOR      (*(volatile unsigned*)(GPIOD_BASE + 0x8))
#define GPIOD_PTOR      (*(volatile unsigned*)(GPIOD_BASE + 0xC))
#define GPIOD_PDIR      (*(volatile unsigned*)(GPIOD_BASE + 0x10))
#define GPIOD_PDDR      (*(volatile unsigned*)(GPIOD_BASE + 0x14))
#define GPIOD_PIDR      (*(volatile unsigned*)(GPIOD_BASE + 0x18))

#define GPIOE_PDOR      (*(volatile unsigned*)(GPIOE_BASE + 0x0))
#define GPIOE_PSOR      (*(volatile unsigned*)(GPIOE_BASE + 0x4))
#define GPIOE_PCOR      (*(volatile unsigned*)(GPIOE_BASE + 0x8))
#define GPIOE_PTOR      (*(volatile unsigned*)(GPIOE_BASE + 0xC))
#define GPIOE_PDIR      (*(volatile unsigned*)(GPIOE_BASE + 0x10))
#define GPIOE_PDDR      (*(volatile unsigned*)(GPIOE_BASE + 0x14))
#define GPIOE_PIDR      (*(volatile unsigned*)(GPIOE_BASE + 0x18))

//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================





//============================================================================
// NVIC (for interrupt)
//============================================================================

#define NVIC_ISER_BASE   (0xE000E100U)
#define NVIC_ICER_BASE   (0xE000E180U)
#define NVIC_ISPR_BASE   (0xE000E200U)
#define NVIC_ICPR_BASE   (0xE000E280U)
#define NVIC_IABR_BASE   (0xE000E300U)
#define NVIC_IPR_BASE    (0xE000E400U)

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

#define IRQC_RISING_EDGE   0b1001
#define IRQC_FALLING_EDGE  0b1010
#define IRQC_EITHER_EDGE   0b1011

//============================================================================
// NVIC
//============================================================================


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
// LPIT Timer
//============================================================================


//============================================================================
// ADC
//============================================================================

#define PCC_ADC0 *((volatile unsigned*)(PCC_BASE + 0xEC))
#define PCC_ADC1 *((volatile unsigned*)(PCC_BASE + 0x9C))

#define ADC0_BASE (0x4003B000)
#define ADC1_BASE (0x40027000)

#define ADC0_SC1A *((volatile unsigned*)(ADC0_BASE + 0x0))
#define ADC0_CFG1 *((volatile unsigned*)(ADC0_BASE + 0x40))
#define ADC0_CFG2 *((volatile unsigned*)(ADC0_BASE + 0x44))
#define ADC0_RA *((volatile unsigned*)(ADC0_BASE + 0x48))
#define ADC0_SC2 *((volatile unsigned*)(ADC0_BASE + 0x90))
#define ADC0_SC3 *((volatile unsigned*)(ADC0_BASE + 0x94))

#define ADC1_SC1A *((volatile unsigned*)(ADC1_BASE + 0x0))
#define ADC1_CFG1 *((volatile unsigned*)(ADC1_BASE + 0x40))
#define ADC1_CFG2 *((volatile unsigned*)(ADC1_BASE + 0x44))
#define ADC1_RA *((volatile unsigned*)(ADC1_BASE + 0x48))
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
// ADC
//============================================================================


//============================================================================
// PWM (FTM + PWM) , PWM생성시에는 Flexible Timer 사용
//============================================================================

#define PCC_FTM0 *((volatile unsigned*)(PCC_BASE + 0xE0))
#define PCC_FTM1 (*(volatile unsigned*)(PCC_BASE + 0xE4))
#define PCC_FTM2  (*(volatile unsigned*)(PCC_BASE + 0xE8))

#define FTM0_BASE (0x40038000)
#define FTM0_SC *((volatile unsigned*)(FTM0_BASE + 0x0))
#define FTM0_MOD *((volatile unsigned*)(FTM0_BASE + 0x8))
#define FTM0_C1SC *((volatile unsigned*)(FTM0_BASE + 0x14))
#define FTM0_C1V *((volatile unsigned*)(FTM0_BASE + 0x18))
#define FTM0_CNTIN *((volatile unsigned*)(FTM0_BASE + 0x4C))

#define FTM1_BASE (0x40039000)
#define FTM1_SC *((volatile unsigned*)(FTM1_BASE + 0x0))
#define FTM1_MOD *((volatile unsigned*)(FTM1_BASE + 0x8))
#define FTM1_C5SC *((volatile unsigned*)(FTM1_BASE + 0x34))
#define FTM1_C5V *((volatile unsigned*)(FTM1_BASE + 0x38))
#define FTM1_CNTIN *((volatile unsigned*)(FTM1_BASE + 0x4C))

#define FTM2_BASE (0x4003A000)
#define FTM2_SC     (*(volatile unsigned*)(FTM2_BASE + 0x0))
#define FTM2_MOD    (*(volatile unsigned*)(FTM2_BASE + 0x8))
#define FTM2_C0SC   (*(volatile unsigned*)(FTM2_BASE + 0x0C))
#define FTM2_C0V    (*(volatile unsigned*)(FTM2_BASE + 0x10))
#define FTM2_C1V    (*(volatile unsigned*)(FTM2_BASE + 0x18))
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
// PWM (FTM + PWM) , PWM생성시에는 Flexible Timer 사용
//============================================================================



//============================================================================
// External Clock
//============================================================================

void SOSC_init_8MHz(void);
void SPLL_init_160MHz(void);
void NormalRUNmode_80MHz(void);

//============================================================================
// External Clock
//============================================================================

#endif
