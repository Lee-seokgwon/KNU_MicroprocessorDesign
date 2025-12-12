// S32K144 7-Segment Digital Clock (HH:MM:SS)
// 세그먼트: A,B,C,D = PTB4,5,6,7 / E,F,G = PTA11,12,13
// COLON: PTA14 (active-low, 항상 ON)
// 자릿수(COM): PTC3,6,7,8,9,10

#include "S32K144.h"
#include "common.h"
#include "clock.h"

// 코어 클럭 주파수 (Hz)
// - 아무 클럭 초기화 안 했으면 기본 FIRC 48MHz일 가능성이 큼 → 48000000U
// - 만약 직접 80MHz로 올렸으면 80000000U로 바꾸세요.
#define CORE_CLOCK_HZ   (48000000U)

int main(void)
{
    // GPIO 초기화
    clock_gpio_init();

    // 시계 시작값 설정
    clock_set_time(0, 0, 0);

    // SysTick 1ms 타이머 초기화
    clock_systick_init(CORE_CLOCK_HZ);

    uint32_t last_scan_ms = 0U;   // 디지트 스캔 기준 시간
    uint32_t last_sec_ms  = 0U;   // 1초 기준 시간

    for (;;) {
        uint32_t now = clock_get_ms_ticks();

        // 1) 디스플레이 동적 스캔 (대략 1ms마다 한 자릿수씩)
        if ((now - last_scan_ms) >= 1U) {
            last_scan_ms = now;
            clock_display_update();
        }

        // 2) 정확히 1000ms마다 시간 +1초
        if ((now - last_sec_ms) >= 1000U) {
            last_sec_ms += 1000U;   // 드리프트 방지용: += 1000U 로 누적
            clock_increment_one_second();
        }
    }

    // 여긴 도달하지 않음
    // return 0;
}
