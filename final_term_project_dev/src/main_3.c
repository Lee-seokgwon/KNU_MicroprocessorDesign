#include "common.h"
#include "adc.h"
#include "cds.h"
#include "interrupt.h"
#include "pwm.h"
#include "piezo_buzzer.h"
#include "sequential_turn_light.h"
#include "step_motor.h"
#include "step_motor_and_turn_light_controller.h"
#include "ultrasonic.h"

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

    ultrasonic_port_init();             // ultrasonic port init
    NVIC_init_IRQs();                   // Interrupt controller init

    FTM0_CH1_PWM();                     // Optional: PWM channel init

    /* ============================
       Main Loop
       ============================ */
    int is_bright_prev = -1; // 초기 상태

    while (1)
    {
        adc_start();
        uint16_t cds = read_cds();
        int is_bright_current = (cds > 2000); // 밝음: 1, 어두움: 0

        // [1] 현재 깜빡이/스텝모터 요청이 없을 때만 노래 재생
        if (!g_left_request && !g_right_request)
        {
            if (is_bright_current != is_bright_prev)
            {
                if (is_bright_current)
                {
                    piezo_playAirplane();   // 블로킹 OK: 모터 안 돌 때만 실행
                }
                else
                {
                    piezo_playElije();
                }
                is_bright_prev = is_bright_current;
            }
        }

        // [2] 스텝 모터 / 방향지시등 제어
        if (g_left_request)
        {
            run_left_sequential();      // 내부에서 끝나면 g_left_request = false; 해주는 구조 추천
        }
        else if (g_right_request)
        {
            run_right_sequential();
        }
    }

    return 0;
}
