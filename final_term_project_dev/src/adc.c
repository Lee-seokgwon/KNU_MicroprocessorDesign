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
