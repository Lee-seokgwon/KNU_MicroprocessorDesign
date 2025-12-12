#include "clock.h"

// 핀 매핑 (포트 위치 표 / clocmain.c 기준)
// 세그먼트: PTA6,7,8,9,11,12,13 / COLON: PTA14
// 자릿수(COM): PTC3,6,7,8,9,10
#define SEG_A_PIN   PTA6
#define SEG_B_PIN   PTA7
#define SEG_C_PIN   PTA8
#define SEG_D_PIN   PTA9
#define SEG_E_PIN   PTA11
#define SEG_F_PIN   PTA12
#define SEG_G_PIN   PTA13
#define SEG_COLON_PIN PTA14

#define DIGIT_1_PIN PTC3   // 1st digit (H tens)
#define DIGIT_2_PIN PTC6   // 2nd digit (H ones)
#define DIGIT_3_PIN PTC7   // 3rd digit (M tens)
#define DIGIT_4_PIN PTC8   // 4th digit (M ones)
#define DIGIT_5_PIN PTC9   // 5th digit (S tens)
#define DIGIT_6_PIN PTC10  // 6th digit (S ones)

// 비트 마스크 (PORTA 세그먼트, PORTC 자릿수)
#define SEG_A      (1u << SEG_A_PIN)
#define SEG_B      (1u << SEG_B_PIN)
#define SEG_C      (1u << SEG_C_PIN)
#define SEG_D      (1u << SEG_D_PIN)
#define SEG_E      (1u << SEG_E_PIN)
#define SEG_F      (1u << SEG_F_PIN)
#define SEG_G      (1u << SEG_G_PIN)
#define SEG_ALL    (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define SEG_COLON  (1u << SEG_COLON_PIN)

#define DIGIT_1    (1u << DIGIT_1_PIN)
#define DIGIT_2    (1u << DIGIT_2_PIN)
#define DIGIT_3    (1u << DIGIT_3_PIN)
#define DIGIT_4    (1u << DIGIT_4_PIN)
#define DIGIT_5    (1u << DIGIT_5_PIN)
#define DIGIT_6    (1u << DIGIT_6_PIN)
#define DIGIT_ALL  (DIGIT_1 | DIGIT_2 | DIGIT_3 | DIGIT_4 | DIGIT_5 | DIGIT_6)

// ---------- 전역 변수 ----------
unsigned char current_digit_display = 0;

// 시각 정보
unsigned int g_hour   = 0;   // 0~23
unsigned int g_minute = 0;   // 0~59
unsigned int g_second = 0;   // 0~59

// 시계 동작 상태
unsigned char clock_running = 1;      // 1: 동작, 0: 일시정지
unsigned char colon_state   = 1;      // 1: ON, 0: OFF

// 스위치 플래그 (현재 미사용)
unsigned char switch_d10_pressed_flag = 0;  // reset
unsigned char switch_d11_pressed_flag = 0;  // +1 hour
unsigned char switch_e10_pressed_flag = 0;  // +1 min
unsigned char switch_e11_pressed_flag = 0;  // pause/resume

// ---------- 세그먼트 & DIGIT 제어 ----------

// 모든 세그먼트/자릿수 끄기 (COLON 핀은 건드리지 않음)
void clear_all_segments(void) {
    GPIOA_PCOR |= SEG_ALL;
    GPIOC_PCOR |= DIGIT_ALL;
}

// 특정 숫자 패턴 출력 (공통 애노드 기준, A~G만 제어)
void set_digit_pattern(unsigned char digit) {
    GPIOA_PCOR |= SEG_ALL;  // 먼저 끄기

    switch(digit) {
        case 0: GPIOA_PSOR |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F; break;
        case 1: GPIOA_PSOR |= SEG_B | SEG_C; break;
        case 2: GPIOA_PSOR |= SEG_A | SEG_B | SEG_D | SEG_E | SEG_G; break;
        case 3: GPIOA_PSOR |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_G; break;
        case 4: GPIOA_PSOR |= SEG_B | SEG_C | SEG_F | SEG_G; break;
        case 5: GPIOA_PSOR |= SEG_A | SEG_C | SEG_D | SEG_F | SEG_G; break;
        case 6: GPIOA_PSOR |= SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G; break;
        case 7: GPIOA_PSOR |= SEG_A | SEG_B | SEG_C | SEG_F; break;
        case 8: GPIOA_PSOR |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G; break;
        case 9: GPIOA_PSOR |= SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G; break;
        default: break;
    }
}

// 특정 자릿수 선택
void select_digit_position(unsigned char position) {
    GPIOC_PCOR |= DIGIT_ALL;  // 모든 자리 OFF

    switch(position) {
        case 0: GPIOC_PSOR |= DIGIT_1; break;  // 1번째 자리 (H tens)
        case 1: GPIOC_PSOR |= DIGIT_2; break;  // 2번째 자리 (H ones)
        case 2: GPIOC_PSOR |= DIGIT_3; break;  // 3번째 자리 (M tens)
        case 3: GPIOC_PSOR |= DIGIT_4; break;  // 4번째 자리 (M ones)
        case 4: GPIOC_PSOR |= DIGIT_5; break;  // 5번째 자리 (S tens)
        case 5: GPIOC_PSOR |= DIGIT_6; break;  // 6번째 자리 (S ones)
        default: break;
    }
}

// HH:MM:SS 를 6자리로 표시하는 동적 스캔
void display_time_dynamic(void) {
    unsigned char digits[6];

    unsigned int h = g_hour;
    unsigned int m = g_minute;
    unsigned int s = g_second;

    digits[0] = (h / 10u) % 10u;
    digits[1] = h % 10u;
    digits[2] = (m / 10u) % 10u;
    digits[3] = m % 10u;
    digits[4] = (s / 10u) % 10u;
    digits[5] = s % 10u;

    select_digit_position(current_digit_display);
    set_digit_pattern(digits[current_digit_display]);

    current_digit_display = (unsigned char)((current_digit_display + 1u) % 6u);
}

// ---------- COLON 제어 ----------
void colon_on(void) {
    GPIOA_PSOR |= SEG_COLON;   // COLON HIGH
    colon_state = 1;
}

void colon_off(void) {
    GPIOA_PCOR |= SEG_COLON;   // COLON LOW
    colon_state = 0;
}

// ---------- 시간 관련 ----------
void increment_time_one_second(void) {
    if (!clock_running) return;

    g_second++;
    if (g_second >= 60) {
        g_second = 0;
        g_minute++;
        if (g_minute >= 60) {
            g_minute = 0;
            g_hour++;
            if (g_hour >= 24) {
                g_hour = 0;
            }
        }
    }
}

// ---------- GPIO 초기화 ----------
void init_gpio_hardware(void) {
    // 클럭 활성화
    PCC_PORTA |= (1<<CGC_BIT);
    PCC_PORTC |= (1<<CGC_BIT);

    // PORT A (Segments + COLON) MUX 설정
    PORTA_PCR(SEG_A_PIN)    = (PORTA_PCR(SEG_A_PIN)    & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTA_PCR(SEG_B_PIN)    = (PORTA_PCR(SEG_B_PIN)    & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTA_PCR(SEG_C_PIN)    = (PORTA_PCR(SEG_C_PIN)    & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTA_PCR(SEG_D_PIN)    = (PORTA_PCR(SEG_D_PIN)    & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTA_PCR(SEG_E_PIN)    = (PORTA_PCR(SEG_E_PIN)    & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTA_PCR(SEG_F_PIN)    = (PORTA_PCR(SEG_F_PIN)    & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTA_PCR(SEG_G_PIN)    = (PORTA_PCR(SEG_G_PIN)    & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTA_PCR(SEG_COLON_PIN)= (PORTA_PCR(SEG_COLON_PIN)& ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);

    // PORT A 출력 설정
    GPIOA_PDDR |= SEG_ALL | SEG_COLON;

    // PORT C (Digits) MUX 설정
    PORTC_PCR(DIGIT_1_PIN)  = (PORTC_PCR(DIGIT_1_PIN)  & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTC_PCR(DIGIT_2_PIN)  = (PORTC_PCR(DIGIT_2_PIN)  & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTC_PCR(DIGIT_3_PIN)  = (PORTC_PCR(DIGIT_3_PIN)  & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTC_PCR(DIGIT_4_PIN)  = (PORTC_PCR(DIGIT_4_PIN)  & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTC_PCR(DIGIT_5_PIN)  = (PORTC_PCR(DIGIT_5_PIN)  & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    PORTC_PCR(DIGIT_6_PIN)  = (PORTC_PCR(DIGIT_6_PIN)  & ~(0x7u<<MUX_BITS)) | (1u<<MUX_BITS);
    GPIOC_PDDR |= DIGIT_ALL;
}

// 스위치 처리 함수 (현재는 비어있음, 필요시 구현)
void handle_switch_polling(void) {
    // 스위치 처리 로직이 필요하면 여기에 구현
}

