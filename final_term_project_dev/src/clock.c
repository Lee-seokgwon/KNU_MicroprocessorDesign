#include "common.h"
#include "clock.h"

//============================================================================
// 전역 변수
//============================================================================

static uint8_t current_digit_display = 0;

// 시각 정보
static uint8_t g_hour   = 0;   // 0~23
static uint8_t g_minute = 0;   // 0~59
static uint8_t g_second = 0;   // 0~59

// SysTick 기반 ms 카운터
static volatile uint32_t g_ms_ticks = 0;

//============================================================================
// 숫자 → 세그먼트 패턴 테이블
//============================================================================

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

//============================================================================
// 내부 함수 선언
//============================================================================

static void clear_all_segments(void);
static void set_digit_pattern(uint8_t digit);
static void select_digit_position(uint8_t position);
static void display_time_dynamic(void);
static void colon_always_on(void);

//============================================================================
// 세그먼트 & DIGIT 제어
//============================================================================

/**
 * @brief 모든 세그먼트/자릿수 끄기 (COLON 핀 PTA14는 건드리지 않음)
 */
static void clear_all_segments(void)
{
    // PORTB: A,B,C,D OFF
    GPIOB_PCOR |= (SEG_A | SEG_B | SEG_C | SEG_D);

    // PORTA: E,F,G OFF
    GPIOA_PCOR |= (SEG_E | SEG_F | SEG_G);

    // 디지트 OFF
    GPIOC_PCOR |= DIGIT_ALL;
}

/**
 * @brief 특정 숫자 패턴 출력 (0~9, 공통 애노드 기준)
 */
static void set_digit_pattern(uint8_t digit)
{
    // 먼저 A~D, E~G 모두 OFF
    GPIOB_PCOR |= (SEG_A | SEG_B | SEG_C | SEG_D);
    GPIOA_PCOR |= (SEG_E | SEG_F | SEG_G);

    if (digit < 10U) {
        uint32_t pattern = kDigitPatterns[digit];

        // A~D → PORTB
        GPIOB_PSOR |= (pattern & (SEG_A | SEG_B | SEG_C | SEG_D));

        // E~G → PORTA
        GPIOA_PSOR |= (pattern & (SEG_E | SEG_F | SEG_G));
    }
}

/**
 * @brief 특정 자릿수 선택 (0~5)
 */
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

/**
 * @brief HH:MM:SS 를 6자리로 표시하는 동적 스캔
 * @brief 호출될 때마다 다음 자릿수를 짧게 켜줌
 */
static void display_time_dynamic(void)
{
    uint8_t digits[6];

    uint8_t h = g_hour;
    uint8_t m = g_minute;
    uint8_t s = g_second;

    digits[0] = (uint8_t)((h / 10U) % 10U);
    digits[1] = (uint8_t)(h % 10U);
    digits[2] = (uint8_t)((m / 10U) % 10U);
    digits[3] = (uint8_t)(m % 10U);
    digits[4] = (uint8_t)((s / 10U) % 10U);
    digits[5] = (uint8_t)(s % 10U);

    select_digit_position(current_digit_display);
    set_digit_pattern(digits[current_digit_display]);

    current_digit_display = (uint8_t)((current_digit_display + 1U) % 6U);
}

/**
 * @brief COLON 항상 ON (active-low 가정)
 */
static void colon_always_on(void)
{
    // PTA14 출력 설정 및 active-low ON
    GPIOA_PDDR |= COLON_PIN;   // 출력
    GPIOA_PCOR |= COLON_PIN;   // LOW → 콜론 LED ON
}

//============================================================================
// 시간 관련
//============================================================================

/**
 * @brief 시간 1초 증가
 */
void clock_increment_one_second(void)
{
    g_second++;
    if (g_second >= 60U) {
        g_second = 0U;
        g_minute++;
        if (g_minute >= 60U) {
            g_minute = 0U;
            g_hour++;
            if (g_hour >= 24U) {
                g_hour = 0U;
            }
        }
    }
}

/**
 * @brief 현재 시간 가져오기
 */
void clock_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    if (hour != NULL)   *hour   = g_hour;
    if (minute != NULL) *minute = g_minute;
    if (second != NULL) *second = g_second;
}

/**
 * @brief 시간 설정
 */
void clock_set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    if (hour < 24U)   g_hour   = hour;
    if (minute < 60U) g_minute = minute;
    if (second < 60U) g_second = second;
}

//============================================================================
// GPIO 초기화
//============================================================================

/**
 * @brief 7-Segment 디스플레이 GPIO 초기화
 */
void clock_gpio_init(void)
{
    // 클럭 활성화
    PCC_PORTA |= (1U << CGC_BIT);
    PCC_PORTB |= (1U << CGC_BIT);
    PCC_PORTC |= (1U << CGC_BIT);

    // PORTB (A,B,C,D) MUX 설정
    PORTB_PCR(PTB4) = (PORTB_PCR(PTB4) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTB_PCR(PTB5) = (PORTB_PCR(PTB5) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTB_PCR(PTB8) = (PORTB_PCR(PTB8) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTB_PCR(PTB9) = (PORTB_PCR(PTB9) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);

    // PORTA (E,F,G + COLON) MUX 설정
    PORTA_PCR(PTA11) = (PORTA_PCR(PTA11) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTA_PCR(PTA12) = (PORTA_PCR(PTA12) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTA_PCR(PTA13) = (PORTA_PCR(PTA13) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTA_PCR(PTA14) = (PORTA_PCR(PTA14) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);

    // PORTB 출력 설정 (A,B,C,D)
    GPIOB_PDDR |= (SEG_A | SEG_B | SEG_C | SEG_D);

    // PORTA 출력 설정 (E,F,G + COLON)
    GPIOA_PDDR |= (SEG_E | SEG_F | SEG_G | COLON_PIN);

    // PORTC (Digits) MUX 설정
    PORTC_PCR(PTC3)  = (PORTC_PCR(PTC3)  & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTC_PCR(PTC6)  = (PORTC_PCR(PTC6)  & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTC_PCR(PTC7)  = (PORTC_PCR(PTC7)  & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTC_PCR(PTC8)  = (PORTC_PCR(PTC8)  & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTC_PCR(PTC9)  = (PORTC_PCR(PTC9)  & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);
    PORTC_PCR(PTC10) = (PORTC_PCR(PTC10) & ~(0x7U << MUX_BITS)) | (1U << MUX_BITS);

    // PORTC 출력 설정 (Digits)
    GPIOC_PDDR |= DIGIT_ALL;

    // 초기화: 모든 세그먼트 끄기
    clear_all_segments();

    // 콜론 항상 ON (active-low)
    colon_always_on();
}

//============================================================================
// SysTick 설정
//============================================================================

/**
 * @brief SysTick 타이머 초기화 (1ms 주기)
 */
void clock_systick_init(uint32_t core_clock_hz)
{
    // 1ms마다 인터럽트 발생하도록 reload 값 계산
    uint32_t reload = (core_clock_hz / 1000U) - 1U;

    SYST_RVR = reload;                     // 리로드 값 설정
    SYST_CVR = 0U;                         // 현재 카운터 클리어
    SYST_CSR =  SYST_CSR_CLKSOURCE        // 프로세서 클럭 사용
             |  SYST_CSR_TICKINT          // 인터럽트 활성화
             |  SYST_CSR_ENABLE;          // SysTick 시작
}

/**
 * @brief SysTick 인터럽트 핸들러 (1ms마다 호출)
 */
void SysTick_Handler(void)
{
    g_ms_ticks++;
}

/**
 * @brief 현재 ms 틱 값 가져오기
 */
uint32_t clock_get_ms_ticks(void)
{
    return g_ms_ticks;
}

//============================================================================
// 디스플레이 업데이트
//============================================================================

/**
 * @brief 디스플레이 동적 스캔 (매번 호출 시 다음 자릿수 표시)
 */
void clock_display_update(void)
{
    display_time_dynamic();
}

