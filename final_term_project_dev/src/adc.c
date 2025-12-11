#include "common.h"
#include "adc.h"

void ADC0_init(void)
{
    // 1. ADC0 클럭 선택 & Enable
    PCC_ADC0 &= ~(PCC_CGC_MASK);
    PCC_ADC0 &= ~((0b111U)<<PCS_BITS);
    PCC_ADC0 |=  ((0b001U)<<PCS_BITS);   // PCS = SOSCDIV2 (예: 8MHz/2 등)
    PCC_ADC0 |=  PCC_CGC_MASK;

    // 2. SC1A 초기값: 채널 disable (ADCH=31, 0x1F)
    ADC0_SC1A |= ((0b111111)<<ADCH_BITS);

    // 3. CFG1: 분주=1(기본), 12비트 모드
    ADC0_CFG1 &= ~((0b11U)<<ADIV_BITS);
    ADC0_CFG1 &= ~((0b11U)<<MODE_BITS);
    ADC0_CFG1 |=  ((0b01U)<<MODE_BITS);  // MODE=01 → 12bit

    // 4. CFG2: 샘플 타임 설정
    ADC0_CFG2 &= ~(255<<SMPLTS_BITS);
    ADC0_CFG2 |=  (12U<<SMPLTS_BITS);    // 12 ADC clock 사이 샘플

    // 5. SC2: SW trigger 사용 (ADTRG=0)
    ADC0_SC2 &= ~(1U<<ADTRG_BIT);
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