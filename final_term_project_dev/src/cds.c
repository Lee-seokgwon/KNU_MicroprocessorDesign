#include "inc/common.h"
#include "inc/adc.h"
#include "inc/cds.h"

// CdS가 연결된 채널(PTB0, ADC0_SE4)
#define ADC_CH_CDS 4

uint16_t read_cds()
{
    uint16_t cdsResult = ADC0_ReadChannel(ADC_CH_CDS);
    return cdsResult;
}