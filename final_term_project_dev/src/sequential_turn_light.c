#include "inc/common.h"
#include "inc/sequential_turn_light.h"

bool g_left_request  = false;
bool g_right_request = false;

void sequential_turn_light_switch_init (void)
{
    PCC_PORTC |= (1 << CGC_BIT);

    //PTC12 GPIO input & Interrupt (좌측 깜빡이 스위치)
    PORTC_PCR(PTC12) &= ~(0b111 << MUX_BITS);
    PORTC_PCR(PTC12) |=  (1 << MUX_BITS);
    GPIOC_PDDR       &= ~(1 << PTC12);
    PORTC_PCR(PTC12) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC_PCR(PTC12) &= ~(0b1111 << IRQC_BITS);
    PORTC_PCR(PTC12) |=  (IRQC_FALLING_EDGE << IRQC_BITS);
    PORTC_PCR(PTC12) |= (1 << ISF_BIT);

    //PTC13 GPIO input & Interrupt (우측 깜빡이 스위치)
    PORTC_PCR(PTC13) &= ~(0b111 << MUX_BITS);
    PORTC_PCR(PTC13) |=  (1 << MUX_BITS);
    GPIOC_PDDR       &= ~(1 << PTC13);
    PORTC_PCR(PTC13) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC_PCR(PTC13) &= ~(0b1111 << IRQC_BITS);
    PORTC_PCR(PTC13) |=  (IRQC_FALLING_EDGE << IRQC_BITS);
    PORTC_PCR(PTC13) |= (1 << ISF_BIT);
}

void sequential_turn_light_led_init (void)
{
    PCC_PORTD |= (1 << CGC_BIT);

    //PTD0 ~ PTD7 GPIO output (깜빡이 LED 8개)
    for (int pin = 0; pin <= 7; ++pin)
    {
        PORTD_PCR(pin) &= ~(0b111 << MUX_BITS);
        PORTD_PCR(pin) |=  (1 << MUX_BITS);
        GPIOD_PDDR     |=  (1 << pin);
    }

    //LED 안켜지게 초기화
    GPIOD_PDOR |= 0xFF;
}

void sequential_turn_light_port_init(void)
{
    sequential_turn_light_switch_init();
    sequential_turn_light_led_init();
}

