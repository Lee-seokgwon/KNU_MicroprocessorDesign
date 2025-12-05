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
    #define CDS_THRESHOLD 1000
    bool prev_dark = false;  // 이전 상태 (1000 이상이면 true)
    
    while (1)
    {
        // CDS 센서 값 읽기
        uint16_t cds_value = read_cds();
        bool is_dark = (cds_value >= CDS_THRESHOLD);
        
        // 상태 변화 감지
        if (is_dark && !prev_dark)
        {
            // 1000 이상으로 올라감 → AA 노래 재생
            piezo_playMelody("AA");
            prev_dark = true;
        }
        else if (!is_dark && prev_dark)
        {
            // 1000 이하로 떨어짐 → BB 노래 재생
            piezo_playMelody("BB");
            prev_dark = false;
        }
        


        // 좌깜우깜 버튼 감지
        if (g_left_request)
        {
            g_left_request = false;
            run_left_sequential();
        }

        if (g_right_request)
        {
            g_right_request = false;
            run_right_sequential();
        }



        // 초음파 시퀀셜 검증
        uint32_t distance = ultrasonic_measure_distance_cm();

        if (distance < 50)
        {
            // 50cm 이내면 오른쪽으로
            run_right_sequential();
        }
        else
        {
            // 50cm 이상이면 왼쪽으로
            run_left_sequential();
        }

        // 다음 측정 전 약간의 딜레이
        for (volatile uint32_t i = 0; i < 1000000; i++);


    }

    return 0;
}

