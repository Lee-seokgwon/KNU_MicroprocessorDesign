#include "S32K144.h"
#include <stdint.h>

// PCC Register
#define PCC_FTM2    *((volatile unsigned*)(PCC_BASE + 0xE8))
#define PCC_ADC0    *((volatile unsigned*)(PCC_BASE + 0xEC))
#define PCC_PORTB   *((volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC   *((volatile unsigned*)(PCC_BASE + 0x12C)) // [변경] PORTC 클럭 제어 레지스터
#define PCC_PORTD   *((volatile unsigned*)(PCC_BASE + 0x130))

#define PCS_BITS    24
#define CGC_BIT     30

// FTM2 Registers
#define FTM2_SC     *((volatile unsigned*)(FTM2_BASE + 0x0))
#define FTM2_MOD    *((volatile unsigned*)(FTM2_BASE + 0x8))
#define FTM2_C0SC   *((volatile unsigned*)(FTM2_BASE + 0xC))
#define FTM2_C0V    *((volatile unsigned*)(FTM2_BASE + 0x10))
#define FTM2_CNTIN  *((volatile unsigned*)(FTM2_BASE + 0x4C))

#define PWMEN0_BIT  16
#define PWMEN1_BIT  17
#define CLKS_BITS   3
#define PS_BITS     0
#define MSB_BIT     5
#define MSA_BIT     4
#define ELSB_BIT    3
#define ELSA_BIT    2

// ADC0 Registers
#define ADC0_SC1A   *((volatile unsigned*)(ADC0_BASE + 0x0))
#define ADC0_CFG1   *((volatile unsigned*)(ADC0_BASE + 0x40))
#define ADC0_CFG2   *((volatile unsigned*)(ADC0_BASE + 0x44))
#define ADC0_RA     *((volatile unsigned*)(ADC0_BASE + 0x48))
#define ADC0_SC2    *((volatile unsigned*)(ADC0_BASE + 0x90))

#define ADCH_BITS   0
#define COCO_BIT    7
#define MODE_BITS   2
#define ADIV_BITS   5
#define SMPLTS_BITS 0
#define ADTRG_BIT   6

// [변경] ADC 채널 정의 (PTC14 = ADC0_SE12)
#define ADC0_SE12   12

// GPIO Registers (PORT B) - 모터 방향 제어용
#define PTB_BASE_ADDR 0x400FF040
#define PTB_PDDR    *((volatile unsigned*)(PTB_BASE_ADDR + 0x14))
#define PTB_PSOR    *((volatile unsigned*)(PTB_BASE_ADDR + 0x04))
#define PTB_PCOR    *((volatile unsigned*)(PTB_BASE_ADDR + 0x08))

// PORT Registers
// PORTB는 모터 제어(GPIO)용으로 유지
#define PORTB_PCR10 *((volatile unsigned*)(PORTB_BASE + 0x28))
#define PORTB_PCR11 *((volatile unsigned*)(PORTB_BASE + 0x2C))

// [변경] PORTC_BASE 및 PCR14 정의
#define PORTC_BASE  0x4004B000
#define PORTC_PCR14 *((volatile unsigned*)(PORTC_BASE + 0x38)) // PTC14

#define PORTD_PCR10 *((volatile unsigned*)(PORTD_BASE + 0x28))
#define MUX_BITS    8

void SOSC_init_8MHz(void) {
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) | SCG_SOSCDIV_SOSCDIV2(1);
    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(2) | SCG_SOSCCFG_EREFS_MASK;
    while (SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK);
    SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;
    while (!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));
}

void SPLL_init_160MHz(void) {
    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);
    SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(2) | SCG_SPLLDIV_SPLLDIV2(3);
    SCG->SPLLCFG = SCG_SPLLCFG_MULT(24);
    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
    while (!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK));
}

void NormalRUNmode_80MHz(void) {
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1) | SCG_SIRCDIV_SIRCDIV2(1);
    SCG->RCCR = SCG_RCCR_SCS(6) | SCG_RCCR_DIVCORE(0b01) | SCG_RCCR_DIVBUS(0b01) | SCG_RCCR_DIVSLOW(0b10);
    while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) != 6) {}
}

void ADC0_init(void) {
    PCC_ADC0 &= ~(1<<CGC_BIT); PCC_ADC0 &= ~((0b111)<<PCS_BITS); PCC_ADC0 |= ((0b001)<<PCS_BITS); PCC_ADC0 |= (1<<CGC_BIT);

    ADC0_SC1A |= ((0b111111)<<ADCH_BITS); // 초기 비활성화

    ADC0_CFG1 &= ~((0b11)<<ADIV_BITS); ADC0_CFG1 &= ~((0b11)<<MODE_BITS); ADC0_CFG1 |= ((0b01)<<MODE_BITS);
    ADC0_CFG2 &= ~(255<<SMPLTS_BITS); ADC0_CFG2 |= (12<<SMPLTS_BITS);
    ADC0_SC2 &= ~(1<<ADTRG_BIT);
}

void FTM2_CH0_PWM(void) {
    PCC_FTM2 &= ~(1<<CGC_BIT); PCC_FTM2 &= ~((0b111)<<PCS_BITS); PCC_FTM2 |= ((0b010)<<PCS_BITS); PCC_FTM2 |= (1<<CGC_BIT);
    FTM2_SC |= (1<<PWMEN1_BIT); FTM2_SC &= ~((0b111)<<PS_BITS); FTM2_SC |= ((0b001)<<PS_BITS);
    FTM2_MOD = 16000 - 1; FTM2_CNTIN = 0;
    FTM2_C0SC |= (1<<MSB_BIT); FTM2_C0SC |= (1<<ELSB_BIT); FTM2_C0SC &= ~(1<<ELSA_BIT);
    FTM2_C0V = 0; FTM2_SC |= ((0b11)<<CLKS_BITS);
}

void PORT_init(void)
{
    // PORTB 클럭 (모터 방향 제어용 B10, B11)
    PCC_PORTB |= (1<<CGC_BIT);

    // [변경] PORTC 클럭 (ADC 입력용 C14)
    PCC_PORTC |= (1<<CGC_BIT);

    // PORTD 클럭 (PWM 출력용 D10)
    PCC_PORTD |= (1<<CGC_BIT);

    // [변경] PTC14를 ADC 기능으로 설정 (ALT0 = Analog)
    PORTC_PCR14 &= ~((0b111)<<MUX_BITS);

    // B10, B11 GPIO 설정 (모터 방향)
    PORTB_PCR10 &= ~((0b111)<<MUX_BITS); PORTB_PCR10 |= ((0b001)<<MUX_BITS);
    PORTB_PCR11 &= ~((0b111)<<MUX_BITS); PORTB_PCR11 |= ((0b001)<<MUX_BITS);

    PTB_PDDR |= (1 << 10) | (1 << 11);
    PTB_PCOR |= (1 << 10) | (1 << 11);

    // D10 PWM 설정
    PORTD_PCR10 &= ~((0b111)<<MUX_BITS); PORTD_PCR10 |= ((0b010)<<MUX_BITS);
}

void adc_start(void)
{
    // [변경] 채널 선택을 SE12(PTC14)로 설정
    ADC0_SC1A &= ~((0b111111)<<ADCH_BITS);
    ADC0_SC1A |= (ADC0_SE12<<ADCH_BITS);
}

uint32_t read_adc_chx(void)
{
    while ((ADC0_SC1A & (1<<COCO_BIT)) == 0) {}
    return ADC0_RA;
}

int main(void)
{
    uint32_t adcResult = 0;
    uint32_t pwm_value = 0;
    uint32_t speed_raw = 0; // 계산된 속도 비례값

    // 초기화
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    PORT_init();      // GPIO 및 핀 설정
    ADC0_init();
    FTM2_CH0_PWM();

    for(;;)
    {
        // 1. ADC 값 읽기 (PTC14)
        adc_start();
        adcResult = read_adc_chx(); // 범위: 0 ~ 4095

        // 2. 범위에 따른 제어 로직
        if (adcResult <= 1000)
        {
            // === [전진 구간] (0 ~ 1000) ===
            // 0일 때 최대 속도, 1000일 때 정지

            // 방향: 전진 (B10 High, B11 Low)
            PTB_PSOR |= (1 << 10);
            PTB_PCOR |= (1 << 11);

            // 속도 계산: 1000에서 ADC 값을 뺌
            // adc=0 -> speed_raw=1000 (최대)
            // adc=1000 -> speed_raw=0 (정지)
            speed_raw = 1000 - adcResult;

            // PWM 변환 (최대값 1000을 16000으로 매핑)
            // 비례식: output = input * 16000 / 1000
            // 즉, output = input * 16
            pwm_value = speed_raw * 16;
        }
        else if (adcResult >= 2000)
        {
            // === [후진 구간] (2000 ~ 4095) ===
            // 2000일 때 정지, 4095일 때 최대 속도

            // 방향: 후진 (B10 Low, B11 High)
            PTB_PCOR |= (1 << 10);
            PTB_PSOR |= (1 << 11);

            // 속도 계산: ADC 값에서 2000을 뺌
            // adc=2000 -> speed_raw=0 (정지)
            // adc=4095 -> speed_raw=2095 (최대)
            speed_raw = adcResult - 2000;

            // PWM 변환 (최대값 2095를 16000으로 매핑)
            // 비례식: output = (input * 16000) / 2095
            pwm_value = (speed_raw * 16000) / 2095;
        }
        else
        {
            // === [정지 구간] (1001 ~ 1999) ===

            // 모터 드라이버 정지 (B10 Low, B11 Low)
            PTB_PCOR |= (1 << 10);
            PTB_PCOR |= (1 << 11);

            pwm_value = 0; // PWM 끄기
        }

        // 3. PWM 적용 (최대값 제한 안전장치)
        if(pwm_value > 16000) pwm_value = 16000;
        FTM2_C0V = pwm_value;

        // 딜레이
        for(volatile uint32_t i = 0; i < 10000; i++);
    }
}
