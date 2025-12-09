#include "common.h"
#include "adc.h"
#include "pwm.h"
#include "piezo_buzzer.h"
#include "ultrasonic.h"
#include "dcmotor.h"
#include "lcd.h"

// 디버깅용 지연 함수 (80MHz 기준)
void debug_delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    while (ms--)
    {
        count = 80000;  // 80MHz 기준 1ms
        while (count--) __asm("nop");
    }
}

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
    // PORT 초기화 (ADC용 PTC14 설정) - 예제 코드처럼 별도로 처리
    PCC_PORTC |= (1 << CGC_BIT);  // PORTC 클럭 활성화
    PORTC_PCR(PTC14) &= ~((0b111) << MUX_BITS);  // MUX 클리어
    PORTC_PCR(PTC14) |=  ((0b000) << MUX_BITS);  // MUX=000: ADC 기능 (Analog)
    
    ADC0_init();                    // ADC 초기화
    piezo_port_init();              // 피에조 부저 초기화
    ultrasonic_port_init();         // 초음파 센서 초기화
    DcMotor_init();                 // DC 모터 초기화 (FTM2 포함)
    lcd_port_init();                // LCD 초기화
    
    FTM0_CH1_PWM();                 // 피에조용 PWM (FTM0)

    /* ============================
       디버깅 테스트 시작
       ============================ */
    
    // 초기화 완료 메시지
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Debug Mode");
    lcd_set_cursor(1, 0);
    lcd_print_string("Initialized");
    debug_delay_ms(2000);

    /* ============================================
       테스트 1: LCD 테스트
       ============================================ */
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("LCD Test OK");
    lcd_set_cursor(1, 0);
    lcd_print_string("Line 2 Works");
    debug_delay_ms(2000);

    /* ============================================
       테스트 2: RCCA 기능 테스트
       - 가변저항 값이 REVERSE(4095)일 때 DC 모터 후진
       - 초음파 센서 30cm 이하 감지 시 급제동
       ============================================ */
    // 첫 번째 읽기로 ADC 초기화 (더미 읽기)
    read_potentiometer();
    debug_delay_ms(100);
    
    // 초기 상태
    uint8_t motor_running = 0;  // 모터 동작 상태
    uint8_t prev_mode = 255;    // 이전 모드 추적 (초기값: 255 = 없음)
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("RCCA Test Mode");
    debug_delay_ms(1000);
    
    while(1)
    {
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
        
        // REVERSE 모드로 진입할 때 "엘리제를 위하여" 재생
        if (mode == 2 && prev_mode != 2)  // REVERSE 모드로 진입
        {
            piezo_playElije();  // 엘리제를 위하여 재생
        }
        
        // REVERSE 모드일 때 DC 모터 후진
        if (mode == 2)  // REVERSE 모드
        {
            // 초음파 센서로 거리 측정
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
            
            // 30cm 이하일 때 급제동
            if (distance <= 30)
            {
                DcMotor_stop();  // 급제동
                motor_running = 0;
                
                lcd_set_cursor(1, 0);
                lcd_print_string("EMERGENCY STOP!");
            }
            else
            {
                // 30cm 이상이면 후진 계속
                DcMotor_run_reverse(8000);  // 후진 (50% 듀티: 16000 중 8000)
                motor_running = 1;
                
                lcd_set_cursor(1, 0);
                lcd_print_string("REVERSE");
            }
        }
        else
        {
            // REVERSE 모드가 아니면 모터 정지
            DcMotor_stop();
            motor_running = 0;
            
            lcd_set_cursor(1, 0);
            if (mode == 0) 
            {
                lcd_print_string("FORWARD");
            }
            else 
            {
                lcd_print_string("NEUTRAL");
            }
        }
        
        // 이전 모드 업데이트
        prev_mode = mode;
        
        debug_delay_ms(100);  // 0.1초마다 업데이트 (빠른 반응)
    }
    
    /* ============================================
       테스트 3: 초음파 센서 테스트 (주석 해제하여 사용)
       ============================================ */
    /*
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Ultrasonic");
    
    while(1)
    {
        uint32_t distance = ultrasonic_measure_distance_cm();
        
        lcd_set_cursor(1, 0);
        lcd_print_string("Dist: ");
        
        // 3자리 숫자 표시
        if (distance >= 100)
        {
            lcd_data('0' + (distance / 100) % 10);
            lcd_data('0' + (distance / 10) % 10);
            lcd_data('0' + distance % 10);
        }
        else if (distance >= 10)
        {
            lcd_data(' ');
            lcd_data('0' + (distance / 10) % 10);
            lcd_data('0' + distance % 10);
        }
        else
        {
            lcd_data(' ');
            lcd_data(' ');
            lcd_data('0' + distance % 10);
        }
        lcd_print_string(" cm");
        
        debug_delay_ms(500);
    }
    */
    
    /* ============================================
       테스트 4: DC 모터 테스트 (주석 해제하여 사용)
       ============================================ */
    /*
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("DC Motor Test");
    
    // 전진 테스트
    lcd_set_cursor(1, 0);
    lcd_print_string("Forward");
    DcMotor_run_forward(8000);  // 50% 듀티 (16000 중 8000)
    debug_delay_ms(2000);  // 2초
    
    // 정지
    DcMotor_stop();
    lcd_set_cursor(1, 0);
    lcd_print_string("Stop      ");
    debug_delay_ms(1000);
    
    // 후진 테스트
    lcd_set_cursor(1, 0);
    lcd_print_string("Reverse");
    DcMotor_run_reverse(8000);
    debug_delay_ms(2000);
    
    // 정지
    DcMotor_stop();
    lcd_set_cursor(1, 0);
    lcd_print_string("Stop      ");
    debug_delay_ms(1000);
    
    // 무한 루프로 멈춤
    while(1);
    */
    
    /* ============================================
       테스트 5: 피에조 부저 테스트 (주석 해제하여 사용)
       ============================================ */
    /*
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Piezo Test");
    lcd_set_cursor(1, 0);
    lcd_print_string("Elije Song");
    
    piezo_playElije();
    
    debug_delay_ms(1000);
    
    lcd_set_cursor(1, 0);
    lcd_print_string("Airplane  ");
    piezo_playAirplane();
    
    debug_delay_ms(1000);
    
    while(1);
    */
    
    /* ============================================
       테스트 6: 통합 테스트 - RCCA 시뮬레이션
       (주석 해제하여 사용)
       ============================================ */
    /*
    uint8_t prev_mode = 255;  // 이전 모드 추적
    
    while(1)
    {
        // 가변저항 읽기
        uint16_t pot_value = read_potentiometer();
        
        // 3단계 구분
        uint8_t mode = 0;
        if (pot_value < 1365)      mode = 0;  // 전진
        else if (pot_value < 2730) mode = 1;  // 중립
        else                        mode = 2;  // 후진
        
        // 모드 변경 시에만 처리
        if (mode != prev_mode)
        {
            lcd_clear();
            lcd_set_cursor(0, 0);
            
            if (mode == 0)
            {
                lcd_print_string("FORWARD");
                DcMotor_run_forward(6000);  // 전진
            }
            else if (mode == 1)
            {
                lcd_print_string("NEUTRAL");
                DcMotor_stop();  // 정지
            }
            else  // mode == 2 (후진)
            {
                lcd_print_string("REVERSE");
                piezo_playElije();  // 엘리제를 위하여 재생
                DcMotor_run_reverse(6000);  // 후진 시작
            }
            
            prev_mode = mode;
        }
        
        // 후진 모드일 때 초음파 센서로 급제동 체크
        if (mode == 2)
        {
            uint32_t distance = ultrasonic_measure_distance_cm();
            
            lcd_set_cursor(1, 0);
            lcd_print_string("Dist: ");
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
            lcd_print_string(" cm");
            
            // 30cm 이하일 때 급제동
            if (distance <= 30)
            {
                DcMotor_stop();  // 급제동
                lcd_set_cursor(1, 0);
                lcd_print_string("EMERGENCY STOP!");
                debug_delay_ms(1000);
            }
        }
        
        debug_delay_ms(100);
    }
    */
    
    return 0;
}

