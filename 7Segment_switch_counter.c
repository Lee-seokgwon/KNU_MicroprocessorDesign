// main.c (S32K144, 버튼으로 카운터 제어)
// 회로: 4-Digit 7-Segment, Common Cathode + 버튼
// Segments: a,b,c,d,e,f = PB12..PB17, g = PE8, dp = PE9
// Digits:   D1..D4 = PE13..PE16
// Button:   BTN0 = PTC12 (풀업저항 있음, 누르면 LOW)

#include <stdint.h>

//============================================================================
//============================================================================

// PCC 베이스 주소 (datasheet 623p)
#define PCC_BASE   (0x40065000U)

// PORT 베이스 주소 (LMS exel file)
#define PORTA_BASE (0x40049000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTD_BASE (0x4004C000U)
#define PORTE_BASE (0x4004D000U)

//GPIO 베이스 주소 (datasheet 229p)
#define GPIOA_BASE (0x400FF000U)
#define GPIOB_BASE (0x400FF040U) 
#define GPIOC_BASE (0x400FF080U)
#define GPIOD_BASE (0x400FF0C0U)
#define GPIOE_BASE (0x400FF100U)

//============================================================================
//============================================================================



//============================================================================
//============================================================================

// PCC 내부 포트들 위치 (PCCn[PCC_PORTA_INDEX] -> PORTA의 PCC 영역으로 접근)
// PCCn 원소 한개 4바이트. 배열인덱스 = datasheet Offset / 4 
#define PCC_PORTA_INDEX (73)
#define PCC_PORTB_INDEX (74)
#define PCC_PORTC_INDEX (75)
#define PCC_PORTD_INDEX (76)
#define PCC_PORTE_INDEX (77)

// PORTn clock enable 마스크, (datasheet 656)
#define PCC_PCCn_CGC_MASK (1U << 30)

// PORT PCR
#define PORT_PCR_MUX_GPIO (1U << 8) //MUX set to GPIO
#define PORT_PCR_PE_MASK  (1U << 1) //PULL enable
#define PORT_PCR_PS_MASK  (1U << 0) //PULL select
#define PORT_PCR_SET_GPIO_MUX (PORT_PCR_MUX_GPIO)

//============================================================================
//============================================================================



//============================================================================
//============================================================================

// PCC 구조체 (포트별 Clock Enable) (datasheet 623p)
typedef struct {
    volatile uint32_t PCCn[128];
} PCC_Type;

// PORT 구조체 (포트 내부 개별 핀 설정) (datasheet 211p) 
// 추후 핀 추가시 배열 원소 개수 늘려주기.
// 1 원소당 32비트 이므로, 인덱스 = offset/4 
typedef struct {
    volatile uint32_t PCR[32];
} PORT_Type;

// GPIO 구조체
typedef struct {
    volatile uint32_t PDOR;
    volatile uint32_t PSOR;
    volatile uint32_t PCOR;
    volatile uint32_t PTOR;
    volatile uint32_t PDIR;
    volatile uint32_t PDDR;
    volatile uint32_t PIDR;
} GPIO_Type;

//============================================================================
//============================================================================



//============================================================================
//============================================================================

#define PCC   ((PCC_Type*)PCC_BASE)

#define PORTB ((PORT_Type*)PORTA_BASE)
#define PORTB ((PORT_Type*)PORTB_BASE)
#define PORTC ((PORT_Type*)PORTC_BASE)
#define PORTC ((PORT_Type*)PORTD_BASE)
#define PORTE ((PORT_Type*)PORTE_BASE)

#define GPIOA   ((GPIO_Type*)GPIOA_BASE)
#define GPIOB   ((GPIO_Type*)GPIOB_BASE)
#define GPIOC   ((GPIO_Type*)GPIOC_BASE)
#define GPIOD   ((GPIO_Type*)GPIOD_BASE)
#define GPIOE   ((GPIO_Type*)GPIOE_BASE)

//============================================================================
//============================================================================



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



static void gpio_init(void) {
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTE_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
    
    // 7 segment (1)
    for (int pin = 12; pin <= 17; pin++) {
        //PORTB->PCR[pin] &= ~(0b111<<8);
        //PORTB->PCR[pin] |= PORT_PCR_SET_GPIO_MUX
        PORTB->PCR[pin] = PORT_PCR_SET_GPIO_MUX;
        GPIOB->PDDR |= (1U << pin);
        GPIOB->PCOR  = (1U << pin);
    }

    // 7 segment (2)
    PORTE->PCR[8]  = PORT_PCR_SET_GPIO_MUX; GPIOE->PDDR |= (1U<<8);  GPIOE->PCOR = (1U<<8);
    PORTE->PCR[9]  = PORT_PCR_SET_GPIO_MUX; GPIOE->PDDR |= (1U<<9);  GPIOE->PCOR = (1U<<9);

    // 7 segment enable BJT
    for (int pin = 13; pin <= 16; pin++) {
        PORTE->PCR[pin] = PORT_PCR_SET_GPIO_MUX;
        GPIOE->PDDR |= (1U << pin);
        GPIOE->PCOR  = (1U << pin);
    }

    // Switch input
    PORTC->PCR[12] = PORT_PCR_SET_GPIO_MUX | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    GPIOC->PDDR &= ~(1U << 12);
}



static inline void digits_all_off(void) {
    // 7 segment disable BJT
    GPIOE->PCOR = (0xFU << 13);
}



static inline void segments_output(uint8_t seg) {
    //실제 7segment 숫자 부분 동작시키는 곳
    for (int i = 0; i <= 5; i++) {
        if (seg & (1U << i)) GPIOB->PSOR = (1U << (12 + i));
        else                 GPIOB->PCOR = (1U << (12 + i));
    }
    if (seg & (1U << 6)) GPIOE->PSOR = (1U << 8);
    else                 GPIOE->PCOR = (1U << 8);

    if (seg & (1U << 7)) GPIOE->PSOR = (1U << 9);
    else                 GPIOE->PCOR = (1U << 9);
}




static inline void show_one_digit(uint8_t digit, uint8_t value) {
    //digit -> 몇번째 7 segment인지
    digits_all_off();
    segments_output(segTable[value % 10]);
    GPIOE->PSOR = (1U << (13 + digit));
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
    // 내부 풀업 (active low) 사용시, 누르면 0이니까 ! 사용
    return !(GPIOC->PDIR & (1U << 12));
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
    gpio_init();

    uint16_t counter = 0;
    while (1) {
        display_number(counter);
        if (button_pressed()) {
            counter = (counter + 1) % 10000;
        }
    }
}
