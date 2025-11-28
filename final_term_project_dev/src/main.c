#include "common.h"
#include "adc.h"
#include "cds.h"
#include "interrupt.h"
#include "pwm.h"
#include "piezo_buzzer.h"
#include "sequential_turn_light.h"
#include "step_motor.h"
#include "step_motor_and_turn_light_controller.h"

int main(void)
{
    /* ============================
       Clock & System Init
       ============================ */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();

    /* ============================
       Peripheral Init
       ============================ */
    ADC0_init();                        // ADC
    sequential_turn_light_led_init();   // LED
    sequential_turn_light_switch_init();// SW
    sequential_turn_light_port_init();  // Port multiplexer

    step_motor_port_init();             // Step motor control pins

    piezo_port_init();                  // Piezo buzzer GPIO init

    NVIC_init_IRQs();                   // Interrupt controller init

    FTM0_CH1_PWM();                     // Optional: PWM channel init

    /* ============================
       Main Loop (Empty)
       — 검증 목적이므로 비워둠
       ============================ */
    while (1)
    {
        // nothing here – just test build linking
    }

    return 0;
}
