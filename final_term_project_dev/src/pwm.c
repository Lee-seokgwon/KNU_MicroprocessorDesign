#include "common.h"
#include "pwm.h"

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

void FTM1_CH5_PWM(void) //servo PTA11(FTM1_CH5)
{
    static int SERVO_PERIOD_TICKS = 40000;

    // FTM2 클럭 설정
    PCC_FTM1 &= ~(1<<CGC_BIT);
    PCC_FTM1 &= ~((0b111)<<PCS_BITS);
    PCC_FTM1 |= ((0b010)<<PCS_BITS);  // SOSC 8MHz/2 = 4MHz
    PCC_FTM1 |= (1<<CGC_BIT);

    // PWM 설정
    FTM1_SC |= (1<<PWMEN0_BIT);
    FTM1_SC &= ~((0b111)<<PS_BITS);
    FTM1_SC |= ((0b001)<<PS_BITS);  // Prescaler = 2

    FTM1_MOD = SERVO_PERIOD_TICKS - 1;  // PWM 주기 설정
    FTM1_CNTIN = 0;

    // 채널 5 설정 (Edge-aligned PWM)
    FTM1_C5SC |= (1<<MSB_BIT);
    FTM1_C5SC |= (1<<ELSB_BIT);
    FTM1_C5SC &= ~(1<<ELSA_BIT);

    FTM1_C5V = 0;  // 초기 듀티 사이클 0

    // 클럭 소스 선택 (System clock)
    FTM1_SC |= ((0b11)<<CLKS_BITS);
}

void FTM2_CH0_PWM(void)
{
    // FTM2 클럭 설정
    PCC_FTM2 &= ~(1<<CGC_BIT);
    PCC_FTM2 &= ~((0b111)<<PCS_BITS);
    PCC_FTM2 |= ((0b010)<<PCS_BITS);  // SOSC 8MHz/2 = 4MHz
    PCC_FTM2 |= (1<<CGC_BIT);

    // PWM 설정 - 채널 0,1을 사용하므로 PWMEN0_BIT 사용
    FTM2_SC |= (1<<PWMEN0_BIT);  // 채널 0,1의 PWM 활성화
    FTM2_SC &= ~((0b111)<<PS_BITS);
    FTM2_SC |= ((0b001)<<PS_BITS);  // Prescaler = 2

    FTM2_MOD = 16000 - 1;  // PWM 주기 설정
    FTM2_CNTIN = 0;

    // 채널 0 설정 (Edge-aligned PWM)
    FTM2_C0SC |= (1<<MSB_BIT);
    FTM2_C0SC |= (1<<ELSB_BIT);
    FTM2_C0SC &= ~(1<<ELSA_BIT);

    FTM2_C0V = 0;  // 초기 듀티 사이클 0

    // 클럭 소스 선택 (System clock)
    FTM2_SC |= ((0b11)<<CLKS_BITS);
}

void FTM2_CH1_PWM(void)
{
    // FTM2는 이미 FTM2_CH0_PWM에서 초기화되었으므로
    // 클럭 설정은 생략하고 채널 1만 설정
    
    // 채널 1 설정 (Edge-aligned PWM)
    FTM2_C1SC |= (1<<MSB_BIT);
    FTM2_C1SC |= (1<<ELSB_BIT);
    FTM2_C1SC &= ~(1<<ELSA_BIT);

    FTM2_C1V = 0;  // 초기 듀티 사이클 0
}
