#include "common.h"
#include "piezo_buzzer.h"
#include "lcd.h"
#include "pwm.h"

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
    // piezo_port_init() 내부에서 이미 FTM0_CH1_PWM()을 호출하므로
    // 중복 호출하지 않음
    piezo_port_init();              // 피에조 부저 초기화 (FTM0 포함)
    lcd_port_init();                 // LCD 초기화

    /* ============================
       떳다떳다 비행기 테스트
       ============================ */
    
    // 초기화 완료 메시지
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Piezo Test");
    lcd_set_cursor(1, 0);
    lcd_print_string("Airplane Song");
    debug_delay_ms(2000);

    // 간단한 테스트: 단일 음 재생 (피에조 동작 확인)
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Test Tone 1");
    lcd_set_cursor(1, 0);
    lcd_print_string("440Hz (A4)");
    piezo_playTone(440);  // A4 음
    debug_delay_ms(1000);  // 1초 재생
    piezo_noTone();
    debug_delay_ms(500);
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Test Tone 2");
    lcd_set_cursor(1, 0);
    lcd_print_string("659Hz (E5)");
    piezo_playTone(659);  // E5 음 (미)
    debug_delay_ms(1000);  // 1초 재생
    piezo_noTone();
    debug_delay_ms(500);

    // 단일 음 테스트 (떳다떳다 비행기 첫 음)
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Single Note");
    lcd_set_cursor(1, 0);
    lcd_print_string("E5 (200ms)");
    piezo_playNote(659, 200);  // E5, 200ms
    debug_delay_ms(500);
    
    // 무한 루프로 반복 재생
    while(1)
    {
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Playing...");
        lcd_set_cursor(1, 0);
        lcd_print_string("Airplane");
        
        // 떳다떳다 비행기 재생
        piezo_playAirplane();
        
        // 재생 완료 후 2초 대기
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Finished");
        lcd_set_cursor(1, 0);
        lcd_print_string("Wait 2s...");
        debug_delay_ms(2000);
    }
    
    return 0;
}

