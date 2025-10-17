#include <stdint.h>

//============================================================================
// PCC for Port Clock Enable
//============================================================================

// PCC 베이스 주소 (datasheet 623p)
#define PCC_BASE   (0x40065000U)

#define PCC_PORTA       (*(volatile unsigned*)(PCC_BASE + 0x124))
#define PCC_PORTB       (*(volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC       (*(volatile unsigned*)(PCC_BASE + 0x12C))
#define PCC_PORTD       (*(volatile unsigned*)(PCC_BASE + 0x130))
#define PCC_PORTE       (*(volatile unsigned*)(PCC_BASE + 0x134))

// PORTn clock enable 마스크, (datasheet 656)
#define PCC_CGC_MASK (1U << 30)

//============================================================================
// PCC for Port Clock Enable
//============================================================================




//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO) , PORTn_BASE = PORTn의 PCR Base 주소
//============================================================================

// PORT 베이스 주소 (LMS exel file)
#define PORTA_BASE (0x40049000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTD_BASE (0x4004C000U)
#define PORTE_BASE (0x4004D000U)

//PORTn PCR (n = Pin number, 핀 마다의 설정)
#define PORTA_PCR(n)    (*(volatile unsigned*)(PORTA_BASE + (n)*4))
#define PORTB_PCR(n)    (*(volatile unsigned*)(PORTB_BASE + (n)*4))
#define PORTC_PCR(n)    (*(volatile unsigned*)(PORTC_BASE + (n)*4))
#define PORTD_PCR(n)    (*(volatile unsigned*)(PORTD_BASE + (n)*4))
#define PORTE_PCR(n)    (*(volatile unsigned*)(PORTE_BASE + (n)*4))

// PORT PCR - MUX
#define MUX_BITS        8

// PORT PCR - PullUp / PullDown / PullEnable
#define PORT_PCR_PE_MASK  (1U << 1) //PULL enable
#define PORT_PCR_PS_MASK  (1U << 0) //PULL select

// PORT PCR - Interrupt
#define IRQC_BITS       16
#define ISF_BIT         24

//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO) , PORTn_BASE = PORTn의 PCR Base 주소
//============================================================================





//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================

//GPIO 베이스 주소 (datasheet 229p)
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

#define NVIC_IPR59  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3B)) // PORTA
#define NVIC_IPR60  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3C)) // PORTB
#define NVIC_IPR61  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3D)) // PORTC
#define NVIC_IPR62  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3E)) // PORTD
#define NVIC_IPR63  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3F)) // PORTE

//============================================================================
// NVIC
//============================================================================

void PORT_init(void);
void delay(volatile uint32_t d);

int main (void)
{
    PORT_init();

    for (;;)
    {
        //스위치(PTA9) 폴링 로직
        if (!(GPIOA_PDIR & (1 << 9)))
        {
        	delay(800000); //debouncing 폴링은 빡세개 줘야함
            //LED(PTE15) 토글 로직
            GPIOE_PTOR |= (1 << 15);
        }
        delay(100000);
    }
}

void PORT_init(void)
{
    //스위치핀 클럭 및 풀업, GPIO 설정 (PTA9)
    PCC_PORTA |= PCC_CGC_MASK;

    PORTA_PCR(9) &= ~(0b111<<MUX_BITS);
    PORTA_PCR(9) |= (1<<MUX_BITS);
    PORTA_PCR(9) |= PORT_PCR_PE_MASK; //pull enable
    PORTA_PCR(9) |= PORT_PCR_PS_MASK; //pull up

    GPIOA_PDDR &= ~(1<<9); //input


    //LED핀 클럭 및 GPIO 설정 (PTE15)
    PCC_PORTE |= PCC_CGC_MASK;

    PORTE_PCR(15) &= ~(0b111<<MUX_BITS);
    PORTE_PCR(15) |= (1<<MUX_BITS);

    GPIOE_PDDR |= (1<<15); //output
}

void delay(volatile uint32_t d) {
    while(d--) __asm("NOP");
}
