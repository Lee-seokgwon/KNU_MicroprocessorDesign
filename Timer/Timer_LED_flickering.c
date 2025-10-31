#include "S32K144.h"
#define NVIC_BASE (0xE000E100)
#define NVIC_ISER1 *((volatile unsigned*)(NVIC_BASE + 0x4))
#define NVIC_ICPR1 *((volatile unsigned*)(NVIC_BASE + 0x184))
#define NVIC_IPR48 *((volatile unsigned*)(NVIC_BASE + 0x330))

//#ifndef (S32k144.h에 있어서 중복됨)
//#define PCC_BASE (0x40065000)
//#endif
#define PCC_LPIT (*(volatile unsigned*)(PCC_BASE + 0xDC))
#define PCC_PORTD (*(volatile unsigned*)(PCC_BASE + 0x130))

#define PCS_BITS 24
#define CGC_BIT 30

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

//#ifndef (S32k144.h에 있어서 중복됨)
//#define PORTD_BASE (0x4004C000)
//#endif
#define PORTD_PCR0 *((volatile unsigned*)(PORTD_BASE + 0x0))

#define MUX_BITS 8

#define GPIOD_BASE (0x400FF0C0)
#define GPIOD_PTOR *((volatile unsigned*)(GPIOD_BASE + 0xC))
#define GPIOD_PDDR *((volatile unsigned*)(GPIOD_BASE + 0x14))

#define PTD0 0

void PORT_init(void);
void NVIC_init_IRQs(void);
void LPIT0_init(void);
void SOSC_init_8MHz(void);
void SPLL_init_160MHz(void);
void NormalRUNmode_80MHz(void);

int main(void)
{
    PORT_init();
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    NVIC_init_IRQs();
    LPIT0_init();

    for(;;)
    {
        //empty
    }
}

void PORT_init(void)
{
    PCC_PORTD |= (1<<CGC_BIT);

    PORTD_PCR0 &= ~((0b111)<<MUX_BITS);
    PORTD_PCR0 |= (1<<MUX_BITS);

    GPIOD_PDDR |= (1<<PTD0);
}

void NVIC_init_IRQs(void)
{
    NVIC_ICPR1 |= (1<<(48 % 32));
    NVIC_ISER1 |= (1<<(48 % 32));
    NVIC_IPR48 &= ~(255<<(48 % 4));
    NVIC_IPR48 |= (10<<(48 % 4));
}

void LPIT0_init(void)
{
    PCC_LPIT &= ~((0b111)<<PCS_BITS);
    PCC_LPIT |= ((0b110)<<PCS_BITS);
    PCC_LPIT |= (1<<CGC_BIT);

    LPIT_MCR |= (1<<M_CEN_BIT);

    LPIT_MIER |= (1<<TIE0_BIT);

    LPIT_TVAL0 = 40000000;

    LPIT_TCTRL0 &= ~((0b11)<<MODE_BITS);
    LPIT_TCTRL0 |= (1<<T_EN_BIT);
}

void LPIT0_Ch0_IRQHandler(void)
{
    GPIOD_PTOR |= (1<<PTD0);
    LPIT_MSR |= (1<<TIF0_BIT);
}

void SOSC_init_8MHz(void)
{
    /*!
     * SOSC Initialization (8 MHz):
     * ===================================================
     */
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) |
                   SCG_SOSCDIV_SOSCDIV2(1);    /* Divide by 1 */

    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(2) |       /* Medium frequency range (8MHz~40MHz) */
                   SCG_SOSCCFG_EREFS_MASK;      /* External crystal */

    while (SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK); /* Wait until unlocked */

    SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;     /* Enable system oscillator */

    while (!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)); /* Wait until SOSC is valid */
}

void SPLL_init_160MHz(void)
{
    /*!
     * SPLL Initialization (160 MHz):
     * ===================================================
     */
    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);     /* Ensure SPLL control register is unlocked */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;       /* Disable SPLL before configuration */

    SCG->SPLLDIV |= SCG_SPLLDIV_SPLLDIV1(2) |       /* Divide by 2 */
                    SCG_SPLLDIV_SPLLDIV2(3);        /* Divide by 3 */

    SCG->SPLLCFG = SCG_SPLLCFG_MULT(24);            /* Multiply factor = 24 (160 MHz output) */

    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);     /* Wait until register is unlocked */
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;        /* Enable SPLL */

    while (!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); /* Wait until SPLL is valid */
}


void NormalRUNmode_80MHz(void)
{

    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1) |
                   SCG_SIRCDIV_SIRCDIV2(1);

    SCG->RCCR = SCG_RCCR_SCS(6) |            /* Select PLL as clock source */
                SCG_RCCR_DIVCORE(0b01) |     /* DIVCORE=1 → divide by 2 → Core clock = 80 MHz */
                SCG_RCCR_DIVBUS(0b01)  |     /* DIVBUS=1 → divide by 2 → Bus clock = 40 MHz */
                SCG_RCCR_DIVSLOW(0b10);      /* DIVSLOW=2 → divide by 3 → SCG slow, flash clock = 26.6 MHz */

    while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) != 6)
    {
        /* Wait until PLL is selected as clock source */
    }
}
