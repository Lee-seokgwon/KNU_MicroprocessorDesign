// main.c (S32K144, 버튼으로 카운터 제어)
// 회로: 4-Digit 7-Segment, Common Cathode + 버튼
// Segments: a,b,c,d,e,f = PB12..PB17, g = PE8, dp = PE9
// Digits:   D1..D4 = PE13..PE16
// Button:   BTN0 = PTC12 (풀업저항 있음, 누르면 LOW)

#include <stdint.h>

//============================================================================
// 주변장치 베이스 주소
//============================================================================
#define WDOG_BASE  (0x40052000U)
#define PCC_BASE   (0x40065000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTE_BASE (0x4004D000U)
#define GPIOB_BASE (0x400FF040U)
#define GPIOC_BASE (0x400FF080U)
#define GPIOE_BASE (0x400FF100U)

// GPIO 구조체
typedef struct {
    volatile uint32_t PDOR;
    volatile uint32_t PSOR;
    volatile uint32_t PCOR;
    volatile uint32_t PTOR;
    volatile uint32_t PDIR;
    volatile uint32_t PDDR;
} GPIO_Type;

// PORT 구조체
typedef struct {
    volatile uint32_t PCR[32];
} PORT_Type;

// WDOG 구조체
typedef struct {
    volatile uint32_t CS;
    volatile uint32_t CNT;
    volatile uint32_t TOVAL;
    volatile uint32_t WIN;
} WDOG_Type;

// PCC 구조체
typedef struct {
    volatile uint32_t PCCn[128];
} PCC_Type;

// 주변장치 포인터
#define WDOG  ((WDOG_Type*)WDOG_BASE)
#define PCC   ((PCC_Type*)PCC_BASE)
#define PORTB ((PORT_Type*)PORTB_BASE)
#define PORTC ((PORT_Type*)PORTC_BASE)
#define PORTE ((PORT_Type*)PORTE_BASE)
#define PTB   ((GPIO_Type*)GPIOB_BASE)
#define PTC   ((GPIO_Type*)GPIOC_BASE)
#define PTE   ((GPIO_Type*)GPIOE_BASE)

// PCC
#define PCC_PORTB_INDEX (74)
#define PCC_PORTC_INDEX (75)
#define PCC_PORTE_INDEX (77)
#define PCC_PCCn_CGC_MASK (1U << 30)

// PORT PCR
#define PORT_PCR_MUX_GPIO (1U << 8)
#define PORT_PCR_PE_MASK  (1U << 1)
#define PORT_PCR_PS_MASK  (1U << 0)
#define PORT_PCR_SET_GPIO_MUX (PORT_PCR_MUX_GPIO)

//============================================================================
// 메인 코드
//============================================================================
#define REFRESH_DELAY_CYCLES  4500
#define FRAME_REPEAT          50
#define DEBOUNCE_DELAY        200000

// bit순서: a b c d e f g dp
static const uint8_t segTable[10] = {
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

static inline void short_delay(volatile uint32_t n) {
    while (n--) __asm("nop");
}

static void disable_wdog(void) {
    WDOG->CNT = 0xD928C520U;
    WDOG->TOVAL = 0x0000FFFFU;
    WDOG->CS = 0x00002100U;
}

static void gpio_init(void) {
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

    for (int pin = 12; pin <= 17; pin++) {
        PORTB->PCR[pin] = PORT_PCR_SET_GPIO_MUX;
        PTB->PDDR |= (1U << pin);
        PTB->PCOR  = (1U << pin);
    }

    PORTE->PCR[8]  = PORT_PCR_SET_GPIO_MUX; PTE->PDDR |= (1U<<8);  PTE->PCOR = (1U<<8);
    PORTE->PCR[9]  = PORT_PCR_SET_GPIO_MUX; PTE->PDDR |= (1U<<9);  PTE->PCOR = (1U<<9);

    for (int pin = 13; pin <= 16; pin++) {
        PORTE->PCR[pin] = PORT_PCR_SET_GPIO_MUX;
        PTE->PDDR |= (1U << pin);
        PTE->PCOR  = (1U << pin);
    }

    PORTC->PCR[12] = PORT_PCR_SET_GPIO_MUX | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PTC->PDDR &= ~(1U << 12);
}

static inline void digits_all_off(void) {
    PTE->PCOR = (0xFU << 13);
}

static inline void segments_output(uint8_t seg) {
    for (int i = 0; i <= 5; i++) {
        if (seg & (1U << i)) PTB->PSOR = (1U << (12 + i));
        else                 PTB->PCOR = (1U << (12 + i));
    }
    if (seg & (1U << 6)) PTE->PSOR = (1U << 8);
    else                 PTE->PCOR = (1U << 8);

    if (seg & (1U << 7)) PTE->PSOR = (1U << 9);
    else                 PTE->PCOR = (1U << 9);
}

static inline void show_one_digit(uint8_t digit, uint8_t value) {
    digits_all_off();
    segments_output(segTable[value % 10]);
    PTE->PSOR = (1U << (13 + digit));
    short_delay(REFRESH_DELAY_CYCLES);
    digits_all_off();
}

static void display_number(uint16_t num) {
    uint8_t d[4];
    d[0] = (num / 1000) % 10;
    d[1] = (num / 100)  % 10;
    d[2] = (num / 10)   % 10;
    d[3] = (num / 1)    % 10;

    for (int rep = 0; rep < FRAME_REPEAT; rep++) {
        show_one_digit(0, d[0]);
        show_one_digit(1, d[1]);
        show_one_digit(2, d[2]);
        show_one_digit(3, d[3]);
    }
}

static uint8_t read_button(void) {
    return !(PTC->PDIR & (1U << 12));
}

static uint8_t button_pressed(void) {
    static uint8_t prev_state = 0;
    uint8_t current_state = read_button();

    if (!prev_state && current_state) {
        prev_state = current_state;
        short_delay(DEBOUNCE_DELAY);
        return 1;
    }
    prev_state = current_state;
    return 0;
}

int main(void) {
    disable_wdog();
    gpio_init();

    uint16_t counter = 0;
    while (1) {
        display_number(counter);
        if (button_pressed()) {
            counter = (counter + 1) % 10000;
        }
    }
}
