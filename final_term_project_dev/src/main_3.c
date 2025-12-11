#include "common.h"
#include "piezo_buzzer.h"
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
    piezo_port_init();              // 피에조 부저 초기화
    lcd_port_init();                 // LCD 초기화
    
    FTM0_CH1_PWM();                  // 피에조용 PWM (FTM0)

    /* ============================
       엘리제를 위하여 테스트
       ============================ */
    
    // 초기화 완료 메시지
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Piezo Test");
    lcd_set_cursor(1, 0);
    lcd_print_string("Elije Song");
    debug_delay_ms(2000);

    // 무한 루프로 반복 재생
    while(1)
    {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Playing...");
        lcd_set_cursor(1, 0);
        lcd_print_string("Elije");
        
        // 엘리제를 위하여 재생
        piezo_playElije();
        
        // 재생 완료 후 1초 대기
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Finished");
        debug_delay_ms(1000);
    }
    
    return 0;
}

