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
    ultrasonic_port_init();             //초음파 - 주석해제시 스탭모터 힘만 받고 안돎
    lcd_port_init();                     //lcd - 주석해제시 스탭모터 힘만 받고 안돎
    sequential_turn_light_led_init();   // LED
    sequential_turn_light_switch_init();// SW
    sequential_turn_light_port_init();  // Port multiplexer

    piezo_port_init();                  // Piezo buzzer GPIO init
    step_motor_port_init();             // Step motor control pins
    NVIC_init_IRQs();                   // Interrupt controller init

    FTM0_CH1_PWM();                     // Optional: PWM channel init

    // lcd 초기화
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Debug Mode");
    lcd_set_cursor(1, 0);
    lcd_print_string("Initialized");
    //lcd_debug_delay_ms(500); // 이거 주석해제하면 바로 피에조 삑 소리 지르기만함.

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
            run_left_sequential();
        }
        else if (g_right_request)
        {
            run_right_sequential();
        }

        //초음파 , LCD 테스트
        uint32_t distance = ultrasonic_measure_distance_cm();
        // LCD에 거리 표시
        lcd_set_cursor(0, 8);
        lcd_print_string(" Dist:");
        if (distance < 100)
        {
            lcd_data(' ');
            lcd_data('0' + (distance / 10) % 10);
            lcd_data('0' + distance % 10);
        }
        else
        {
            lcd_data('0' + (distance / 100) % 10);
            lcd_data('0' + (distance / 10) % 10);
            lcd_data('0' + distance % 10);
        }
        lcd_print_string("cm");

        
        //VR
        uint16_t pot_value = read_potentiometer();
        // LCD에 가변저항 값 표시
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Pot:");
        
        // 4자리 숫자 표시
        lcd_data('0' + (pot_value / 1000) % 10);
        lcd_data('0' + (pot_value / 100) % 10);
        lcd_data('0' + (pot_value / 10) % 10);
        lcd_data('0' + pot_value % 10);
        
        // 3단계 구분 (0~4095 범위)
        uint8_t mode = 0;  // 0:전진, 1:중립, 2:후진
        if (pot_value < 1365)      mode = 0;  // 0~1365: 전진
        else if (pot_value < 2730) mode = 1;  // 1365~2730: 중립
        else                        mode = 2;  // 2730~4095: 후진
        
        // REVERSE 모드로 진입할 때 "엘리제를 위하여" 재생 (main_3.c 방식)
        if (mode == 2 && prev_mode != 2)  // REVERSE 모드로 진입
        {
            // 재생 중에는 모터 정지 상태 유지
            GPIOB_PCOR |= (1 << PTB10);
            GPIOB_PCOR |= (1 << PTB11);
            FTM2_C0V = 0;
            
            // LCD에 재생 중 메시지 표시
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print_string("Playing...");
            lcd_set_cursor(1, 0);
            lcd_print_string("Elije");
            
            // 엘리제를 위하여 재생 (블로킹 - 재생 완료까지 대기)
            piezo_playElije();
        }
    }

    return 0;
}
