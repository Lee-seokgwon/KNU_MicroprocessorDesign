#include "common.h"
#include "adc.h"
#include "cds.h"

// CdS가 연결된 채널(PTC17, ADC0_SE15)
#define ADC_CH_CDS 14

uint16_t read_cds()
{
    uint16_t cdsResult = ADC0_ReadChannel();
    return cdsResult;
}
