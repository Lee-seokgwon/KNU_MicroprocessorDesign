#include "clock.h"

int main(void) {
    init_gpio_hardware();
    clear_all_segments();

    // 초기 시각 설정 (필요에 맞게 조정)
    g_hour = 0;
    g_minute = 0;
    g_second = 0;

    colon_on();    // 콜론 ON
    colon_state = 1;
    clock_running = 1;

    unsigned int soft_sec_counter = 0;  // 소프트웨어 초 카운터

    for (;;) {
        // 디스플레이 동적 스캔 (화면 유지)
        for (int scan_count = 0; scan_count < 200; scan_count++) {
            display_time_dynamic();
            for (volatile int delay = 0; delay < 800; delay++);  // 짧은 지연
        }

        // 대략 1초 만들기
        soft_sec_counter++;
        if (soft_sec_counter >= 100) {   // 정확한 1초는 아님, 필요시 타이머로 교체
            soft_sec_counter = 0;
            increment_time_one_second();
        }
    }

    return 0;
}


