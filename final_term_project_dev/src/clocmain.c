// S32K144 7-Segment Digital Clock (HH:MM:SS)
// 세그먼트: PTA6,7,8,9,11,12,13 / COLON: PTA14 (active-low, 항상 ON)
// 자릿수(COM): PTC3,6,7,8,9,10

#include <stdint.h>
#include "S32K144.h"   // SysTick & SystemCoreClock 사용

// ---------- PCC ---------- //
#define PCC_BASE        (0x40065000u)
#define PCC_PORTA       (*((volatile uint32_t *)(PCC_BASE + 0x124u)))
#define PCC_PORTC       (*((volatile uint32_t *)(PCC_BASE + 0x12Cu)))
#define PCC_CGC_BIT     (30u)

// ---------- PORT ---------- //
#define PORTA_BASE      (0x40049000u)
#define PORTC_BASE      (0x4004B000u)

#define PORTA_PCR6      (*((volatile uint32_t *)(PORTA_BASE + 0x18u)))  // PTA6 (A)
#define PORTA_PCR7      (*((volatile uint32_t *)(PORTA_BASE + 0x1Cu)))  // PTA7 (B)
#define PORTA_PCR8      (*((volatile uint32_t *)(PORTA_BASE + 0x20u)))  // PTA8 (C)
#define PORTA_PCR9      (*((volatile uint32_t *)(PORTA_BASE + 0x24u)))  // PTA9 (D)
#define PORTA_PCR11     (*((volatile uint32_t *)(PORTA_BASE + 0x2Cu)))  // PTA11 (E)
#define PORTA_PCR12     (*((volatile uint32_t *)(PORTA_BASE + 0x30u)))  // PTA12 (F)
#define PORTA_PCR13     (*((volatile uint32_t *)(PORTA_BASE + 0x34u)))  // PTA13 (G)
#define PORTA_PCR14     (*((volatile uint32_t *)(PORTA_BASE + 0x38u)))  // PTA14 (COLON)

#define PORTC_PCR3      (*((volatile uint32_t *)(PORTC_BASE + 0x0Cu)))  // 1st digit (H tens)
#define PORTC_PCR6      (*((volatile uint32_t *)(PORTC_BASE + 0x18u)))  // 2nd digit (H ones)
#define PORTC_PCR7      (*((volatile uint32_t *)(PORTC_BASE + 0x1Cu)))  // 3rd digit (M tens)
#define PORTC_PCR8      (*((volatile uint32_t *)(PORTC_BASE + 0x20u)))  // 4th digit (M ones)
#define PORTC_PCR9      (*((volatile uint32_t *)(PORTC_BASE + 0x24u)))  // 5th digit (S tens)
#define PORTC_PCR10     (*((volatile uint32_t *)(PORTC_BASE + 0x28u)))  // 6th digit (S ones)

#define MUX_BITS        (8u)
#define GPIO_MUX_GPIO   (1u << MUX_BITS)

// ---------- GPIO ---------- //
#define GPIOA_BASE      (0x400FF000u)
#define GPIOA_PSOR      (*((volatile uint32_t *)(GPIOA_BASE + 0x04u)))
#define GPIOA_PCOR      (*((volatile uint32_t *)(GPIOA_BASE + 0x08u)))
#define GPIOA_PDDR      (*((volatile uint32_t *)(GPIOA_BASE + 0x14u)))

#define GPIOC_BASE      (0x400FF080u)
#define GPIOC_PSOR      (*((volatile uint32_t *)(GPIOC_BASE + 0x04u)))
#define GPIOC_PCOR      (*((volatile uint32_t *)(GPIOC_BASE + 0x08u)))
#define GPIOC_PDDR      (*((volatile uint32_t *)(GPIOC_BASE + 0x14u)))

// ---------- 세그먼트 / DIGIT 비트 매크로 ---------- //
// Segments (PORTA)
#define SEG_A   (1u << 6)
#define SEG_B   (1u << 7)
#define SEG_C   (1u << 8)
#define SEG_D   (1u << 9)
#define SEG_E   (1u << 11)
#define SEG_F   (1u << 12)
#define SEG_G   (1u << 13)
#define SEG_ALL (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)

// Colon (PORTA)
#define COLON_PIN   (1u << 14)

// Digits (PORTC) - 공통 단자
#define DIGIT_0     (1u << 3)   // H tens
#define DIGIT_1     (1u << 6)   // H ones
#define DIGIT_2     (1u << 7)   // M tens
#define DIGIT_3     (1u << 8)   // M ones
#define DIGIT_4     (1u << 9)   // S tens
#define DIGIT_5     (1u << 10)  // S ones
#define DIGIT_ALL   (DIGIT_0 | DIGIT_1 | DIGIT_2 | DIGIT_3 | DIGIT_4 | DIGIT_5)

// ---------- SysTick (Cortex-M4 공통) ---------- //
// 베이스 주소: 0xE000E010
#define SYST_CSR        (*((volatile uint32_t *)0xE000E010u)) // Control and Status
#define SYST_RVR        (*((volatile uint32_t *)0xE000E014u)) // Reload Value
#define SYST_CVR        (*((volatile uint32_t *)0xE000E018u)) // Current Value

// SYST_CSR 비트
#define SYST_CSR_ENABLE     (1u << 0)  // Enable counter
#define SYST_CSR_TICKINT    (1u << 1)  // Interrupt enable
#define SYST_CSR_CLKSOURCE  (1u << 2)  // Clock source: 0=외부, 1=프로세서 클럭


// ---------- 전역 변수 ---------- //
static uint8_t current_digit_display = 0;

// 시각 정보
static uint8_t g_hour   = 0;   // 0~23
static uint8_t g_minute = 0;   // 0~59
static uint8_t g_second = 0;   // 0~59

// SysTick 기반 ms 카운터
static volatile uint32_t g_ms_ticks = 0;

// ---------- 숫자 → 세그먼트 패턴 테이블 ---------- //
// 공통 애노드, 세그먼트 핀 active-high 기준
static const uint32_t kDigitPatterns[10] = {
    /* 0 */ (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),
    /* 1 */ (SEG_B | SEG_C),
    /* 2 */ (SEG_A | SEG_B | SEG_D | SEG_E | SEG_G),
    /* 3 */ (SEG_A | SEG_B | SEG_C | SEG_D | SEG_G),
    /* 4 */ (SEG_B | SEG_C | SEG_F | SEG_G),
    /* 5 */ (SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),
    /* 6 */ (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),
    /* 7 */ (SEG_A | SEG_B | SEG_C | SEG_F),
    /* 8 */ (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),
    /* 9 */ (SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)
};

// ---------- 함수 선언 ---------- //
static void clear_all_segments(void);
static void set_digit_pattern(uint8_t digit);
static void select_digit_position(uint8_t position);
static void display_time_dynamic(void);
static void init_gpio_hardware(void);
static void increment_time_one_second(void);
static void colon_always_on(void);
static void systick_init(void);

// ---------- 세그먼트 & DIGIT 제어 ---------- //

// 모든 세그먼트/자릿수 끄기 (COLON 핀 PTA14는 건드리지 않음)
static void clear_all_segments(void)
{
    // 세그먼트 OFF
    GPIOA_PCOR |= SEG_ALL;

    // 디지트 OFF
    GPIOC_PCOR |= DIGIT_ALL;
}

// 특정 숫자 패턴 출력 (0~9, 공통 애노드 기준)
static void set_digit_pattern(uint8_t digit)
{
    // 먼저 A~G 모두 OFF
    GPIOA_PCOR |= SEG_ALL;

    if (digit < 10u) {
        GPIOA_PSOR |= kDigitPatterns[digit];
    }
}

// 특정 자릿수 선택 (0~5)
static void select_digit_position(uint8_t position)
{
    // 모든 DIGIT OFF
    GPIOC_PCOR |= DIGIT_ALL;

    switch (position) {
        case 0: GPIOC_PSOR |= DIGIT_0; break; // H tens
        case 1: GPIOC_PSOR |= DIGIT_1; break; // H ones
        case 2: GPIOC_PSOR |= DIGIT_2; break; // M tens
        case 3: GPIOC_PSOR |= DIGIT_3; break; // M ones
        case 4: GPIOC_PSOR |= DIGIT_4; break; // S tens
        case 5: GPIOC_PSOR |= DIGIT_5; break; // S ones
        default: break;
    }
}

// HH:MM:SS 를 6자리로 표시하는 동적 스캔
// 호출될 때마다 다음 자릿수를 짧게 켜줌
static void display_time_dynamic(void)
{
    uint8_t digits[6];

    uint8_t h = g_hour;
    uint8_t m = g_minute;
    uint8_t s = g_second;

    digits[0] = (uint8_t)((h / 10u) % 10u);
    digits[1] = (uint8_t)(h % 10u);
    digits[2] = (uint8_t)((m / 10u) % 10u);
    digits[3] = (uint8_t)(m % 10u);
    digits[4] = (uint8_t)((s / 10u) % 10u);
    digits[5] = (uint8_t)(s % 10u);

    select_digit_position(current_digit_display);
    set_digit_pattern(digits[current_digit_display]);

    current_digit_display = (uint8_t)((current_digit_display + 1u) % 6u);
}

// ---------- COLON 항상 ON (active-low 가정) ---------- //
static void colon_always_on(void)
{
    // PTA14 출력 설정 및 active-low ON
    GPIOA_PDDR |= COLON_PIN;   // 출력
    GPIOA_PCOR |= COLON_PIN;   // LOW → 콜론 LED ON
}

// ---------- 시간 관련 ---------- //
static void increment_time_one_second(void)
{
    g_second++;
    if (g_second >= 60u) {
        g_second = 0u;
        g_minute++;
        if (g_minute >= 60u) {
            g_minute = 0u;
            g_hour++;
            if (g_hour >= 24u) {
                g_hour = 0u;
            }
        }
    }
}

// ---------- GPIO 초기화 ---------- //
static void init_gpio_hardware(void)
{
    // 클럭 활성화
    PCC_PORTA |= (1u << PCC_CGC_BIT);
    PCC_PORTC |= (1u << PCC_CGC_BIT);

    // PORT A (Segments + COLON) MUX 설정
    PORTA_PCR6  = (PORTA_PCR6  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTA_PCR7  = (PORTA_PCR7  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTA_PCR8  = (PORTA_PCR8  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTA_PCR9  = (PORTA_PCR9  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTA_PCR11 = (PORTA_PCR11 & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTA_PCR12 = (PORTA_PCR12 & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTA_PCR13 = (PORTA_PCR13 & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTA_PCR14 = (PORTA_PCR14 & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;  // COLON

    // PORT A 출력 설정 (A~G + COLON)
    GPIOA_PDDR |= SEG_ALL | COLON_PIN;

    // PORT C (Digits) MUX 설정
    PORTC_PCR3  = (PORTC_PCR3  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTC_PCR6  = (PORTC_PCR6  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTC_PCR7  = (PORTC_PCR7  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTC_PCR8  = (PORTC_PCR8  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTC_PCR9  = (PORTC_PCR9  & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;
    PORTC_PCR10 = (PORTC_PCR10 & ~(0x7u << MUX_BITS)) | GPIO_MUX_GPIO;

    // PORT C 출력 설정 (Digits)
    GPIOC_PDDR |= DIGIT_ALL;
}

// ---------- SysTick 설정 (1ms 주기) ---------- //
// 코어 클럭 주파수 (Hz)
// - 아무 클럭 초기화 안 했으면 기본 FIRC 48MHz일 가능성이 큼 → 48000000u
// - 만약 직접 80MHz로 올렸으면 80000000u로 바꾸세요.
#define CORE_CLOCK_HZ   (48000000u)

// ---------- SysTick 설정 (1ms 주기) ---------- //
static void systick_init(void)
{
    // 1ms마다 인터럽트 발생하도록 reload 값 계산
    uint32_t reload = (CORE_CLOCK_HZ / 1000u) - 1u;

    SYST_RVR = reload;                     // 리로드 값 설정
    SYST_CVR = 0u;                         // 현재 카운터 클리어
    SYST_CSR =  SYST_CSR_CLKSOURCE        // 프로세서 클럭 사용
             |  SYST_CSR_TICKINT          // 인터럽트 활성화
             |  SYST_CSR_ENABLE;          // SysTick 시작
}


// SysTick 인터럽트 핸들러 (1ms마다 호출)
void SysTick_Handler(void)
{
    g_ms_ticks++;
}

// ---------- 메인 ---------- //
int main(void)
{
    init_gpio_hardware();
    clear_all_segments();

    // 시계 시작값
    g_hour   = 0u;
    g_minute = 0u;
    g_second = 0u;

    // 콜론 항상 ON (active-low)
    colon_always_on();

    // SysTick 1ms 타이머 초기화
    systick_init();

    uint32_t last_scan_ms = 0u;   // 디지트 스캔 기준 시간
    uint32_t last_sec_ms  = 0u;   // 1초 기준 시간

    for (;;) {
        uint32_t now = g_ms_ticks;

        // 1) 디스플레이 동적 스캔 (대략 1ms마다 한 자릿수씩)
        if ((now - last_scan_ms) >= 1u) {
            last_scan_ms = now;
            display_time_dynamic();
        }

        // 2) 정확히 1000ms마다 시간 +1초
        if ((now - last_sec_ms) >= 1000u) {
            last_sec_ms += 1000u;   // 드리프트 방지용: += 1000u 로 누적
            increment_time_one_second();
        }
    }

    // 여긴 도달하지 않음
    // return 0;
}
