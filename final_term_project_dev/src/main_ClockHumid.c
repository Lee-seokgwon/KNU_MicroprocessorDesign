#include "S32K144.h"
#include "common.h"
#include "lcd.h"
#include "humid.h"
#include "clock.h"

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

    /* 시계 GPIO 초기화 */
    init_gpio_hardware();
    clear_all_segments();

    /* 초기 시각 설정 */
    g_hour = 18;
    g_minute = 0;
    g_second = 0;

    colon_on();    // 시작 시 콜론 ON
    colon_state = 1;
    clock_running = 1;

    /* 초기화 메시지 표시 */
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Clock & Sensor");
    lcd_set_cursor(1, 0);
    lcd_print_string("Initializing..");
    delay_ms(2000u);

    /* 센서 안정화 대기 (최소 2초) */
    delay_ms(2000u);

    /* 초기 LCD 표시 */
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print_string("Humidity: --%");
    lcd_set_cursor(1, 0);
    lcd_print_string("Temp: --C");

    unsigned int soft_sec_counter = 0;  // 소프트웨어 초 카운터
    uint32_t dht11_second_counter = 0u;  // DHT11 읽기용 초 카운터 (2초마다 읽기)

    /* Main Loop */
    for (;;)
    {
        /* 1) 시계 디스플레이 동적 스캔 (화면 유지) */
        for (int scan_count = 0; scan_count < 200; scan_count++)
        {
            display_time_dynamic();
            for (volatile int delay = 0; delay < 800; delay++);  // 짧은 지연
        }

        /* 2) 소프트웨어로 "대충 1초" 만들기 */
        soft_sec_counter++;
        if (soft_sec_counter >= 100)   // 이 값은 대충 맞춰서 쓰는 용도 (정확한 1초는 아님)
        {
            soft_sec_counter = 0;
            increment_time_one_second();
            
            /* DHT11 읽기 타이밍 카운터 증가 (2초마다 읽기) */
            dht11_second_counter++;
        }

        /* 3) DHT11 읽기 (2초 간격) */
        if (dht11_second_counter >= 2u)
        {
            dht11_second_counter = 0u;

            /* DHT11 데이터 읽기 */
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
            }
            else
            {
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
            }
        }
    }

    return 0;
}

