#include "S32K144.h"
#include "common.h"
#include "lcd.h"
#include "humid.h"



int main(void)
{
    uint32_t hum = 0u;
    uint32_t temp = 0u;

    /* Clock 초기화 */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();

    /* DHT11 초기화 */
    humid_init();

    /* LCD 초기화 */
    lcd_port_init();

    /* 초기화 메시지 표시 */
    lcd_clear();
    lcd_set_cursor(1, 0);
    lcd_print_string("Initializing..");
    delay_ms(2000u);

    /* 센서 안정화 대기 (최소 2초) */
    delay_ms(2000u);

    /* Main Loop */
    for (;;)
    {
        /* DHT11 데이터 읽기 (매번 새로운 데이터 읽기) */
        uint8_t success = humid_get_data(&hum, &temp);
        
        /* 읽기 성공한 경우에만 LCD 업데이트 */
        if (success == 0)
        {
            /* 체크섬 오류 - 에러 메시지 표시 */
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print_string("Read Error!");
            lcd_set_cursor(1, 0);
            lcd_print_string("Retrying...");
            
            /* 2초 대기 후 다시 시도 */
            volatile uint32_t del = 0u;
            while (del < 1000000u)
            {
                del++;
            }
            continue;  /* 다시 읽기 시도 */
        }

        /* LCD에 습도 표시 */
        lcd_clear();
        lcd_set_cursor(0, 0);
        lcd_print_string("Humidity: ");

        /* 습도 값 표시 (2자리 숫자) */
        if (hum < 10u)
        {
            lcd_data(' ');
            lcd_data('0' + (uint8_t)hum);
        }
        else
        {
            lcd_data('0' + (uint8_t)((hum / 10u) % 10u));
            lcd_data('0' + (uint8_t)(hum % 10u));
        }
        lcd_print_string("%");

        /* LCD에 온도 표시 */
        lcd_set_cursor(1, 0);
        lcd_print_string("Temp: ");

        /* 온도 값 표시 (2자리 숫자) */
        if (temp < 10u)
        {
            lcd_data(' ');
            lcd_data('0' + (uint8_t)temp);
        }
        else
        {
            lcd_data('0' + (uint8_t)((temp / 10u) % 10u));
            lcd_data('0' + (uint8_t)(temp % 10u));
        }
        lcd_print_string("C");

        /* 2초 대기 (DHT11은 최소 2초 간격으로 읽어야 함) */
        /* 이 딜레이 후 루프가 다시 시작되어 새로운 데이터를 읽습니다 */
        delay_ms(2000u);
    }

    return 0;
}
