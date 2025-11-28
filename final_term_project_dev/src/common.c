#include "inc/common.h"

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