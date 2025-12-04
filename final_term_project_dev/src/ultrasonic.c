#include "common.h"
#include "ultrasonic.h"

#define TRIG_PIN 9 //D9
#define ECHO_PIN 11 //D11

static void ultrasonic_short_delay(volatile uint32_t cnt)
{
    while(cnt--) __asm("NOP");
}


void ultrasonic_port_init(void)
{
    /* PORTD clock enable */
    PCC_PORTD |= (1u << CGC_BIT);

    /* MUX 설정 */
    PORTD_PCR(TRIG_PIN)  = (1u << MUX_BITS);
    PORTD_PCR(ECHO_PIN)  = (1u << MUX_BITS);

    /* Echo Pull-down */
    PORTD_PCR(ECHO_PIN) |= (1u << 1);   // PE
    PORTD_PCR(ECHO_PIN) &= ~(1u << 0);  // PD

    /* 방향 설정 */
    GPIOD_PDDR |=  (1u << TRIG_PIN);
    GPIOD_PDDR &= ~(1u << ECHO_PIN);
}

/* 트리거 펄스 */
void ultrasonic_send_trig_pulse(void)
{
    GPIOD_PCOR = (1u << TRIG_PIN);
    ultrasonic_short_delay(800);

    GPIOD_PSOR = (1u << TRIG_PIN);
    ultrasonic_short_delay(4000);

    GPIOD_PCOR = (1u << TRIG_PIN);
}

/* 거리 측정 */
uint32_t ultrasonic_measure_distance_cm(void)
{
    uint32_t cnt = 0;

    ultrasonic_send_trig_pulse();

    /* Echo HIGH 기다림 */
    while((GPIOD_PDIR & (1u << ECHO_PIN)) == 0u) {
        if(cnt++ > 3000000) return 999;   // 아무 Echo도 없으면 999cm
    }

    /* Echo HIGH 유지시간 측정 */
    cnt = 0;
    while((GPIOD_PDIR & (1u << ECHO_PIN)) != 0u) {
        cnt++;
        ultrasonic_short_delay(5);   // 약 0.5µs
        if(cnt > 5000000) return 999;
    }

    return cnt / 58;
}