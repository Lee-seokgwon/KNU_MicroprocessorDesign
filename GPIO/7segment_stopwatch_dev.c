// 7세그먼트 계속 카운트 올라가고, 스위치 누르면 초기값 바뀜
//정상동작 확인. 포트 A~E 다 사용함.

#include "lsg_s32k144_hal.h"

#define DELAY_TIME  1000  // 대략적인 delay (us 단위)

// ---------------- 세그먼트 매핑 ---------------- //
// PTD10~14 + PTD5 + PTE15,16
#define SEG_A   (1U << 10)  // PTD10
#define SEG_B   (1U << 11)  // PTD11
#define SEG_C   (1U << 12)  // PTD12
#define SEG_D   (1U << 5)   // PTD5
#define SEG_E   (1U << 13)  // PTD13
#define SEG_F   (1U << 14)  // PTD14
#define SEG_G   (1U << 15)  // PTE15
#define SEG_DP  (1U << 16)  // PTE16

// ---------------- Digit(COM) 선택 ---------------- //
// PTB8~13
#define DIGIT1   (1U << 8)   // COM1
#define DIGIT2   (1U << 9)   // COM2
#define DIGIT3   (1U << 10)  // COM3
#define DIGIT4   (1U << 11)  // COM4
#define DIGIT5   (1U << 12)  // COM5
#define DIGIT6   (1U << 13)  // COM6

// ---------------- Switch ---------------- //
// 스위치 4개 (PA6~PA8), PTC8
#define SW1   (1U << 6)
#define SW2   (1U << 7)
#define SW3   (1U << 8)
#define SW4   (1U << 8)

// ---------------- 세그먼트 숫자 패턴 ---------------- //
// gfedcba (active high 기준, 공통 캐소드)
const uint8_t seg_pattern[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

// ---------------- 전역 변수 ---------------- //
float number = 1995.34;  // 초기값
float presets[4] = {1111.11, 2222.22, 3333.33, 4444.44};

// ---------------- 함수 선언 ---------------- //
void delay(volatile uint32_t d);
void init_ports(void);
void display_number(float num);
void display_digit(int digit, int value, int dp);
void check_switch(void);

// ---------------- 메인 ---------------- //
int main(void) {
    init_ports();

    while (1) {
        check_switch();   // 스위치 확인
        display_number(number);
        number += 0.01;   // 값 증가
        delay(50000);     // 대략 1/100 초 (정밀X)
    }
}

// ---------------- 함수 구현 ---------------- //
void delay(volatile uint32_t d) {
    while(d--) __asm("NOP");
}

void init_ports(void) {
    // GPIO 클럭 인에이블
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC_MASK; // 스위치용
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK; // 스위치용

    // ---------------- 세그먼트 핀 출력 ---------------- //
    PORTD->PCR[10] = PORT_PCR_SET_GPIO_MUX; // A
    PORTD->PCR[11] = PORT_PCR_SET_GPIO_MUX; // B
    PORTD->PCR[12] = PORT_PCR_SET_GPIO_MUX; // C
    PORTD->PCR[5]  = PORT_PCR_SET_GPIO_MUX; // D
    PORTD->PCR[13] = PORT_PCR_SET_GPIO_MUX; // E
    PORTD->PCR[14] = PORT_PCR_SET_GPIO_MUX; // F
    PORTE->PCR[15] = PORT_PCR_SET_GPIO_MUX; // G
    PORTE->PCR[16] = PORT_PCR_SET_GPIO_MUX; // DP

    GPIOD->PDDR |= (1U<<5)|(1U<<10)|(1U<<11)|(1U<<12)|(1U<<13)|(1U<<14);
    GPIOE->PDDR |= (1U<<15)|(1U<<16);

    // ---------------- Digit(COM) 핀 출력 ---------------- //
    for (int i=8; i<=13; i++) PORTB->PCR[i] = PORT_PCR_SET_GPIO_MUX;
    GPIOB->PDDR |= (0x3F << 8); // PB8~13

    // ---------------- 스위치 입력 ---------------- //
    for (int i=6; i<9; i++)
        PORTA->PCR[i] = PORT_PCR_SET_GPIO_MUX | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    GPIOA->PDDR &= ~(0b111<<6); // PA6~8 입력

    PORTC->PCR[8] = PORT_PCR_SET_GPIO_MUX | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    GPIOC->PDDR &= ~(0b1<<8); //PTC8 입력
}

void display_number(float num) {
    int int_part = (int)(num * 100); // 소수점 둘째자리까지 포함
    int digits[6];
    for (int i=5; i>=0; i--) {
        digits[i] = int_part % 10;
        int_part /= 10;
    }

    // 6자리 순차 스캔
    display_digit(1, digits[0], 0);
    display_digit(2, digits[1], 0);
    display_digit(3, digits[2], 1); // 소수점 표시
    display_digit(4, digits[3], 0);
    display_digit(5, digits[4], 0);
    display_digit(6, digits[5], 0);
}

void display_digit(int digit, int value, int dp) {
    // 세그먼트 모두 끄기
    GPIOD->PCOR = (1U<<5)|(1U<<10)|(1U<<11)|(1U<<12)|(1U<<13)|(1U<<14);
    GPIOE->PCOR = (1U<<15)|(1U<<16);

    // 숫자 패턴 출력
    uint8_t p = seg_pattern[value];
    if (p & 0x01) GPIOD->PSOR = SEG_A; else GPIOD->PCOR = SEG_A;
    if (p & 0x02) GPIOD->PSOR = SEG_B; else GPIOD->PCOR = SEG_B;
    if (p & 0x04) GPIOD->PSOR = SEG_C; else GPIOD->PCOR = SEG_C;
    if (p & 0x08) GPIOD->PSOR = SEG_D; else GPIOD->PCOR = SEG_D;
    if (p & 0x10) GPIOD->PSOR = SEG_E; else GPIOD->PCOR = SEG_E;
    if (p & 0x20) GPIOD->PSOR = SEG_F; else GPIOD->PCOR = SEG_F;
    if (p & 0x40) GPIOE->PSOR = SEG_G; else GPIOE->PCOR = SEG_G;

    if (dp) GPIOE->PSOR = SEG_DP; else GPIOE->PCOR = SEG_DP;

    // 자리(COM) 모두 끄기
    GPIOB->PCOR = (0x3F << 8);

    // 해당 자리만 켜기
    switch(digit) {
        case 1: GPIOB->PSOR = DIGIT1; break;
        case 2: GPIOB->PSOR = DIGIT2; break;
        case 3: GPIOB->PSOR = DIGIT3; break;
        case 4: GPIOB->PSOR = DIGIT4; break;
        case 5: GPIOB->PSOR = DIGIT5; break;
        case 6: GPIOB->PSOR = DIGIT6; break;
    }

    delay(4000); // 짧은 점등 유지
    GPIOB->PCOR = (0x3F << 8); // 자리 off
}


void check_switch(void) {
    if (!(GPIOA->PDIR & SW1)) number = presets[0];
    else if (!(GPIOA->PDIR & SW2)) number = presets[1];
    else if (!(GPIOA->PDIR & SW3)) number = presets[2];
    else if (!(GPIOC->PDIR & SW4)) number = presets[3];
}

/*
void check_switch(void) {
    static uint8_t prev_sw_state = 0x0F;  // 이 줄 추가

    uint8_t curr_sw_state = GPIOC->PDIR & 0x0F;  // 기존 코드 수정
    uint8_t pressed = prev_sw_state & (~curr_sw_state);  // 기존 코드 수정

    if (pressed & SW1) number = presets[0];      // 기존과 동일
    else if (pressed & SW2) number = presets[1];
    else if (pressed & SW3) number = presets[2];
    else if (pressed & SW4) number = presets[3];

    prev_sw_state = curr_sw_state;  // 이 줄 추가
}
*/
