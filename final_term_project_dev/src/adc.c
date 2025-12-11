#include "common.h"
#include "adc.h"

void ADC0_init(void)
{
    // 예제 코드와 완전히 동일한 방식으로 초기화
    // 1. ADC0 클럭 설정
    PCC_ADC0 &= ~(1 << CGC_BIT);  // 먼저 클럭 비활성화
    PCC_ADC0 &= ~((0b111) << PCS_BITS);
    PCC_ADC0 |=  ((0b001) << PCS_BITS);   // SOSC 선택
    PCC_ADC0 |=  (1 << CGC_BIT);  // 클럭 활성화

    // 2. ADC 비활성화 (채널 disable)
    ADC0_SC1A |= ((0b111111) << ADCH_BITS);

    // 3. 12비트 해상도, 클럭 분주
    ADC0_CFG1 &= ~((0b11) << ADIV_BITS);
    ADC0_CFG1 &= ~((0b11) << MODE_BITS);
    ADC0_CFG1 |=  ((0b01) << MODE_BITS);  // 12-bit 해상도

    // 4. 샘플링 시간 설정
    ADC0_CFG2 &= ~(255 << SMPLTS_BITS);
    ADC0_CFG2 |=  (12 << SMPLTS_BITS);

    // 5. 소프트웨어 트리거 선택
    ADC0_SC2 &= ~(1 << ADTRG_BIT);
}


void adc_start(void)
{
    ADC0_SC1A &= ~((0b1111111)<<ADCH_BITS);
    ADC0_SC1A |= (14<<ADCH_BITS);
}

uint16_t ADC0_ReadChannel(void)
{
    // 변환 완료(COCO=1)될 때까지 대기
    while ((ADC0_SC1A & (1U << COCO_BIT)) == 0U) {
        // wait
    }

    return (uint16_t)ADC0_RA;   // 0 ~ 4095 (12bit)
}

// 가변저항 읽기 (PTC14/C14 핀 → ADC0_SE12 사용)
// 예제 코드와 완전히 동일한 방식
void adc_start_potentiometer(void)
{
    // 예제 코드와 동일: 채널 설정 및 변환 시작
    // PTC14 (C14 핀) = ADC0_SE12 (채널 12)
    ADC0_SC1A &= ~((0b111111) << ADCH_BITS);  // 6비트 클리어
    ADC0_SC1A |= (ADC0_SE12 << ADCH_BITS);    // ADC0_SE12 = 채널 12
}

uint16_t read_potentiometer(void)
{
    // 예제 코드와 완전히 동일한 방식
    adc_start_potentiometer();

    // 변환 완료 대기 (예제 코드와 동일)
    while ((ADC0_SC1A & (1 << COCO_BIT)) == 0) {}

    // 변환 결과 읽기 (12bit ADC: 0~4095)
    return (uint16_t)ADC0_RA;
}
