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
#include "lcd.h"

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
    // PORT 초기화 (ADC용 PTC14 설정)
    PCC_PORTC |= (1 << CGC_BIT);  // PORTC 클럭 활성화
    PORTC_PCR(PTC14) &= ~((0b111) << MUX_BITS);  // MUX 클리어
    PORTC_PCR(PTC14) |=  ((0b000) << MUX_BITS);  // MUX=000: ADC 기능 (Analog)

    // PORTB 클럭 (모터 방향 제어용 B10, B11)
    PCC_PORTB |= (1 << CGC_BIT);

    // PORTD 클럭 (PWM 출력용 D10)
    PCC_PORTD |= (1 << CGC_BIT);

    // B10, B11 GPIO 설정 (모터 방향)
    PORTB_PCR(PTB10) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(PTB10) |=  ((0b001) << MUX_BITS);  // GPIO
    PORTB_PCR(PTB11) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(PTB11) |=  ((0b001) << MUX_BITS);  // GPIO

    GPIOB_PDDR |= (1 << PTB10) | (1 << PTB11);
    GPIOB_PCOR |= (1 << PTB10) | (1 << PTB11);

    // D10 PWM 설정 (FTM2_CH0)
    PORTD_PCR(10) &= ~((0b111) << MUX_BITS);
    PORTD_PCR(10) |=  ((0b010) << MUX_BITS);  // FTM2_CH0

    ADC0_init();                        // ADC
    ultrasonic_port_init();             //초음파 - 주석해제시 스탭모터 힘만 받고 안돎
    lcd_port_init();                     //lcd - 주석해제시 스탭모터 힘만 받고 안돎
    sequential_turn_light_led_init();   // LED
    sequential_turn_light_switch_init();// SW
    sequential_turn_light_port_init();  // Port multiplexer

    piezo_port_init();                  // Piezo buzzer GPIO init
    step_motor_port_init();             // Step motor control pins
    NVIC_init_IRQs();                   // Interrupt controller init

    FTM0_CH1_PWM();                     // 피에조용 PWM (FTM0)
    FTM2_CH0_PWM();                     // DC 모터용 PWM (FTM2_CH0)

    // 초음파 센서 안정화 대기
    volatile uint32_t delay_count;
    for(delay_count = 0; delay_count < 8000000; delay_count++);  // 약 100ms

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
    uint8_t prev_mode = 255;    // 이전 모드 추적 (초기값: 255 = 없음)
    uint8_t motor_running = 0;  // 모터 동작 상태

    // 첫 번째 읽기로 ADC 초기화 (더미 읽기)
    read_potentiometer();
    for(delay_count = 0; delay_count < 8000000; delay_count++);  // 약 100ms

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

        //VR
        uint16_t pot_value = read_potentiometer();

        // 3단계 구분 (0~4095 범위)
        uint8_t mode = 0;  // 0:전진, 1:중립, 2:후진
        if (pot_value < 1365)      mode = 0;  // 0~1365: 전진
        else if (pot_value < 2730) mode = 1;  // 1365~2730: 중립
        else                        mode = 2;  // 2730~4095: 후진

        // REVERSE 모드로 진입할 때 "엘리제를 위하여" 재생 (main_2.c 방식)
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

        // 모터 제어 로직 (main_2.c 방식 적용)
        uint32_t pwm_value = 0;
        uint32_t distance = 0;

        // LCD에 가변저항 값 표시
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Pot:");

        // 4자리 숫자 표시
        lcd_data('0' + (pot_value / 1000) % 10);
        lcd_data('0' + (pot_value / 100) % 10);
        lcd_data('0' + (pot_value / 10) % 10);
        lcd_data('0' + pot_value % 10);

        if (mode == 0)  // 전진 모드
        {
            // 방향: 전진 (B10 High, B11 Low)
            GPIOB_PSOR |= (1 << PTB10);
            GPIOB_PCOR |= (1 << PTB11);

            // 속도 설정 (50% 듀티: 16000 중 8000)
            pwm_value = 8000;

            lcd_set_cursor(1, 0);
            lcd_print_string("FORWARD");
        }
        else if (mode == 2)  // REVERSE 모드
        {
            // 방향: 후진 (B10 Low, B11 High)
            GPIOB_PCOR |= (1 << PTB10);
            GPIOB_PSOR |= (1 << PTB11);

            // 초음파 센서 측정 전 딜레이 (센서 안정화)
            for(delay_count = 0; delay_count < 4000000; delay_count++);  // 약 50ms

            // 초음파 센서로 거리 측정
            distance = ultrasonic_measure_distance_cm();

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

            // 30cm 이하일 때 급제동
            if (distance <= 30)
            {
                // 모터 드라이버 정지 (B10 Low, B11 Low)
                GPIOB_PCOR |= (1 << PTB10);
                GPIOB_PCOR |= (1 << PTB11);
                pwm_value = 0;  // PWM 끄기
                motor_running = 0;

                lcd_set_cursor(1, 0);
                lcd_print_string("EMERGENCY STOP!");
            }
            else
            {
                // 30cm 이상이면 후진 계속 (50% 듀티: 16000 중 8000)
                pwm_value = 8000;
                motor_running = 1;

                lcd_set_cursor(1, 0);
                lcd_print_string("REVERSE");
            }

            // 초음파 센서 측정 후 딜레이 (센서 안정화)
            for(delay_count = 0; delay_count < 4000000; delay_count++);  // 약 50ms
        }
        else  // 중립 모드 (mode == 1)
        {
            // 모터 드라이버 정지 (B10 Low, B11 Low)
            GPIOB_PCOR |= (1 << PTB10);
            GPIOB_PCOR |= (1 << PTB11);
            pwm_value = 0;  // PWM 끄기
            motor_running = 0;

            lcd_set_cursor(1, 0);
            lcd_print_string("NEUTRAL");
        }

        // PWM 적용 (최대값 제한 안전장치)
        if(pwm_value > 16000) pwm_value = 16000;
        FTM2_C0V = pwm_value;

        // 이전 모드 업데이트
        prev_mode = mode;

        // 모드에 따라 다른 딜레이 (후진 모드에서는 초음파 센서 측정 시간 고려)
        if (mode == 2)
        {
            for(delay_count = 0; delay_count < 12000000; delay_count++);  // 약 150ms
        }
        else
        {
            for(delay_count = 0; delay_count < 8000000; delay_count++);  // 약 100ms
        }
    }

    return 0;
}
