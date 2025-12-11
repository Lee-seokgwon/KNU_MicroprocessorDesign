#include "common.h"
#include "adc.h"
#include "pwm.h"

int main(void)
{
    uint32_t adcResult = 0;
    uint32_t pwm_value = 0;
    uint32_t speed_raw = 0; // 계산된 속도 비례값

    // ============================
    // Clock & System Init
    // ============================
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();

    // ============================
    // Peripheral Init
    // ============================
    // PORTB 클럭 (모터 방향 제어용 B10, B11)
    PCC_PORTB |= (1 << CGC_BIT);
    
    // PORTC 클럭 (ADC 입력용 C14)
    PCC_PORTC |= (1 << CGC_BIT);
    
    // PORTD 클럭 (PWM 출력용 D10)
    PCC_PORTD |= (1 << CGC_BIT);

    // PTC14를 ADC 기능으로 설정 (MUX=000: Analog)
    PORTC_PCR(PTC14) &= ~((0b111) << MUX_BITS);

    // B10, B11 GPIO 설정 (모터 방향)
    PORTB_PCR(PTB10) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(PTB10) |=  ((0b001) << MUX_BITS);  // GPIO
    PORTB_PCR(PTB11) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(PTB11) |=  ((0b001) << MUX_BITS);  // GPIO

    GPIOB_PDDR |= (1 << PTB10) | (1 << PTB11);
    GPIOB_PCOR |= (1 << PTB10) | (1 << PTB11);

    // D10 PWM 설정 (원래 코드: PORTD_PCR10)
    PORTD_PCR(10) &= ~((0b111) << MUX_BITS);
    PORTD_PCR(10) |=  ((0b010) << MUX_BITS);  // FTM2_CH0

    ADC0_init();        // ADC 초기화
    FTM2_CH0_PWM();     // FTM2 채널 0 PWM 초기화

    for(;;)
    {
        // 1. ADC 값 읽기 (PTC14)
        adc_start_potentiometer();
        adcResult = ADC0_ReadChannel(); // 범위: 0 ~ 4095

        // 2. 범위에 따른 제어 로직
        if (adcResult <= 2000)
        {
            // === [전진 구간] (0 ~ 2000) ===
            // 0일 때 최대 속도, 2000일 때 정지

            // 방향: 전진 (B10 High, B11 Low)
            GPIOB_PSOR |= (1 << PTB10);
            GPIOB_PCOR |= (1 << PTB11);

            // 속도 계산: 1000에서 ADC 값을 뺌 (원래 코드 로직)
            // adc=0 -> speed_raw=1000 (최대)
            // adc=1000 -> speed_raw=0 (정지)
            speed_raw = 1000 - adcResult;
            if (speed_raw > 1000) speed_raw = 0;  // 음수 방지

            // PWM 변환 (최대값 1000을 16000으로 매핑)
            // 비례식: output = input * 16000 / 1000
            // 즉, output = input * 16
            pwm_value = speed_raw * 16;
        }
        else if (adcResult >= 3000)
        {
            // === [후진 구간] (3000 ~ 4095) ===
            // 3000일 때 정지, 4095일 때 최대 속도

            // 방향: 후진 (B10 Low, B11 High)
            GPIOB_PCOR |= (1 << PTB10);
            GPIOB_PSOR |= (1 << PTB11);

            // 속도 계산: ADC 값에서 2000을 뺌 (원래 코드 로직)
            // adc=2000 -> speed_raw=0 (정지)
            // adc=4095 -> speed_raw=2095 (최대)
            speed_raw = adcResult - 2000;

            // PWM 변환 (최대값 2095를 16000으로 매핑)
            // 비례식: output = (input * 16000) / 2095
            pwm_value = (speed_raw * 16000) / 2095;
        }
        else
        {
            // === [정지 구간] (2001 ~ 2999) ===

            // 모터 드라이버 정지 (B10 Low, B11 Low)
            GPIOB_PCOR |= (1 << PTB10);
            GPIOB_PCOR |= (1 << PTB11);

            pwm_value = 0; // PWM 끄기
        }

        // 3. PWM 적용 (최대값 제한 안전장치)
        if(pwm_value > 16000) pwm_value = 16000;
        FTM2_C0V = pwm_value;

        // 딜레이
        for(volatile uint32_t i = 0; i < 10000; i++);
    }
}
