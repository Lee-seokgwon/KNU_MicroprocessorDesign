#include "inc/common.h"
#include "inc/adc.h"
#include "inc/cds.h"

// CdS가 연결된 채널(PTC17, ADC0_SE12)
#define ADC_CH_CDS 12

uint16_t read_cds()
{
    uint16_t cdsResult = ADC0_ReadChannel(ADC_CH_CDS);
    return cdsResult;
}