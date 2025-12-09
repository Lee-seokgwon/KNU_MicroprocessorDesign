#include "common.h"
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
    DcMotor_init();                 // DC 모터 초기화 (FTM2 포함)
    lcd_port_init();                 // LCD 초기화

    /* ============================
       DC 모터 디버깅 테스트
       ============================ */
    
    // 초기화 완료 메시지
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("DC Motor Test");
    lcd_set_cursor(1, 0);
    lcd_print_string("Initialized");
    debug_delay_ms(2000);
    
    // 초기화 직후 짧은 테스트 (PWM 신호 확인)
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Quick Test");
    lcd_set_cursor(1, 0);
    lcd_print_string("Forward 10%");
    DcMotor_run_forward(1600);  // 10% 듀티 (작은 값으로 테스트)
    debug_delay_ms(1000);
    DcMotor_stop();
    debug_delay_ms(500);

    // 무한 루프로 테스트
    while(1)
    {
        // 1. 전진 테스트
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("FORWARD");
        lcd_set_cursor(1, 0);
        lcd_print_string("Speed: 50%");
        DcMotor_run_forward(8000);  // 50% 듀티 (16000 중 8000)
        debug_delay_ms(3000);  // 3초 전진
        
        // 정지
        DcMotor_stop();
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("STOP");
        debug_delay_ms(1000);
        
        // 2. 후진 테스트
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("REVERSE");
        lcd_set_cursor(1, 0);
        lcd_print_string("Speed: 50%");
        DcMotor_run_reverse(8000);  // 50% 듀티 (16000 중 8000)
        debug_delay_ms(3000);  // 3초 후진
        
        // 정지
        DcMotor_stop();
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("STOP");
        debug_delay_ms(1000);
        
        // 3. 속도 변화 테스트 (전진)
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Speed Test");
        lcd_set_cursor(1, 0);
        lcd_print_string("Forward");
        
        // 낮은 속도 (25%)
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("FORWARD 25%");
        DcMotor_run_forward(4000);  // 25% 듀티
        debug_delay_ms(2000);
        
        // 중간 속도 (50%)
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("FORWARD 50%");
        DcMotor_run_forward(8000);  // 50% 듀티
        debug_delay_ms(2000);
        
        // 높은 속도 (75%)
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("FORWARD 75%");
        DcMotor_run_forward(12000);  // 75% 듀티
        debug_delay_ms(2000);
        
        // 정지
        DcMotor_stop();
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("STOP");
        debug_delay_ms(2000);
    }
    
    return 0;
}

