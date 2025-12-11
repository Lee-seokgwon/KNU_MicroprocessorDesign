#include "common.h"
#include "ultrasonic.h"
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
    ultrasonic_port_init();         // 초음파 센서 초기화
    lcd_port_init();                // LCD 초기화

    /* ============================
       초음파 센서 디버깅 테스트
       ============================ */
    
    // 초기화 완료 메시지
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Ultrasonic");
    lcd_set_cursor(1, 0);
    lcd_print_string("Test Mode");
    debug_delay_ms(2000);
    
    // 초음파 센서 안정화 대기
    debug_delay_ms(100);

    // 무한 루프로 거리 측정 및 표시
    while(1)
    {
        // 초음파 센서로 거리 측정 (main_2.c와 동일한 방식)
        uint32_t distance = ultrasonic_measure_distance_cm();
        
        // LCD에 거리 표시 (main_2.c 방식)
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Ultrasonic");
        
        lcd_set_cursor(1, 0);
        lcd_print_string("Dist: ");
        
        // 3자리 숫자 표시 (main_2.c와 동일한 방식)
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
        
        // main_2.c와 동일한 딜레이 (0.1초마다 업데이트)
        debug_delay_ms(100);
    }
    
    return 0;
}

