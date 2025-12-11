#ifndef ADC_H
#define ADC_H
void ADC0_init(void);
void adc_start(void);
uint16_t ADC0_ReadChannel(void);
void adc_start_potentiometer(void);
uint16_t read_potentiometer(void);
#endif
