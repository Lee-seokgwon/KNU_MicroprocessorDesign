#include "clock.h"


// ---------- 메인 ----------
int main(void) {
    init_gpio_hardware();
    clear_all_segments();

    // 초기 시각 설정 (원하면 바꿔도 됨)
    g_hour = 18;
    g_minute = 0;
    g_second = 0;

    colon_on();    // 시작 시 콜론 ON
    colon_state = 1;
    clock_running = 1;

    unsigned int soft_sec_counter = 0;  // 소프트웨어 초 카운터

    for(;;) {
        // 1) 스위치 처리
        // handle_switch_polling();

        // 2) 디스플레이 동적 스캔 (화면 유지)
        for (int scan_count = 0; scan_count < 200; scan_count++) {
            display_time_dynamic();
            for (volatile int delay = 0; delay < 800; delay++);  // 짧은 지연
        }

        // 3) 소프트웨어로 "대충 1초" 만들기
        soft_sec_counter++;
        if (soft_sec_counter >= 100) {   // 이 값은 대충 맞춰서 쓰는 용도 (정확한 1초는 아님)
            soft_sec_counter = 0;

            // 시간 +1초
            increment_time_one_second();

        }
    }

    return 0;
}
