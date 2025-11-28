#include "inc/common.h"
#include "inc/pwm.h"

void FTM0_CH1_PWM(void)
{
    PCC_FTM0 &= ~(PCC_CGC_MASK);
    PCC_FTM0 &= ~((0b111)<<PCS_BITS);
    PCC_FTM0 |= ((0b010)<<PCS_BITS);
    PCC_FTM0 |= PCC_CGC_MASK;

    FTM0_SC |= (1<<PWMEN1_BIT);
    FTM0_SC &= ~((0b111)<<PS_BITS);

    FTM0_MOD = 10000 - 1;
    FTM0_CNTIN = 0;

    FTM0_C1SC |= (1<<MSB_BIT);
    FTM0_C1SC |= (1<<ELSA_BIT);

    FTM0_C1V = 1023;

    FTM0_SC |= ((0b11)<<CLKS_BITS);
}