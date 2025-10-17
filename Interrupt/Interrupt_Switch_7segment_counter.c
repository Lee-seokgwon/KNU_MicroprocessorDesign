// 스위치를 인터럽트로 읽어서, 입력될시마다 7세그먼트 표시 숫자가 하나씩 올라감

#include <stdint.h>

/* =================== PCC =================== */
#define PCC_BASE        (0x40065000)
#define PCC_PORTB       (*(volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC       (*(volatile unsigned*)(PCC_BASE + 0x12C))
#define PCC_PORTD       (*(volatile unsigned*)(PCC_BASE + 0x130))
#define PCC_PORTE       (*(volatile unsigned*)(PCC_BASE + 0x134))
#define CGC_BIT         30

/* =================== PORT =================== */
#define PORTB_BASE      (0x4004A000)
#define PORTC_BASE      (0x4004B000)
#define PORTD_BASE      (0x4004C000)
#define PORTE_BASE      (0x4004D000)

#define PORTB_PCR(n)    (*(volatile unsigned*)(PORTB_BASE + (n)*4))
#define PORTC_PCR(n)    (*(volatile unsigned*)(PORTC_BASE + (n)*4))
#define PORTD_PCR(n)    (*(volatile unsigned*)(PORTD_BASE + (n)*4))
#define PORTE_PCR(n)    (*(volatile unsigned*)(PORTE_BASE + (n)*4))

#define MUX_BITS        8
#define IRQC_BITS       16
#define ISF_BIT         24

/* =================== GPIO =================== */
#define GPIOB_BASE      (0x400FF040)
#define GPIOC_BASE      (0x400FF080)
#define GPIOD_BASE      (0x400FF0C0)
#define GPIOE_BASE      (0x400FF100)

#define GPIOB_PDOR      (*(volatile unsigned*)(GPIOB_BASE + 0x0))
#define GPIOB_PDDR      (*(volatile unsigned*)(GPIOB_BASE + 0x14))

#define GPIOC_PDDR      (*(volatile unsigned*)(GPIOC_BASE + 0x14))

#define GPIOD_PDOR      (*(volatile unsigned*)(GPIOD_BASE + 0x0))
#define GPIOD_PDDR      (*(volatile unsigned*)(GPIOD_BASE + 0x14))

#define GPIOE_PDOR      (*(volatile unsigned*)(GPIOE_BASE + 0x0))
#define GPIOE_PDDR      (*(volatile unsigned*)(GPIOE_BASE + 0x14))

#define PTC12           12

/* =================== NVIC =================== */
#define NVIC_ISER_BASE  (0xE000E100)
#define NVIC_ISER1      (*(volatile unsigned*)(NVIC_ISER_BASE + 0x4))

#define NVIC_ICPR_BASE  (0xE000E280)
#define NVIC_ICPR1      (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x4))

#define NVIC_IPR_BASE   (0xE000E400)
#define NVIC_IPR61      (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3D))

/* =================== 전역 변수 =================== */
volatile uint32_t count = 0;

// Common Cathode 세그먼트 패턴 (a~g 순)
const unsigned char SEG_PATTERN[10] = {
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

/* =================== 함수 선언 =================== */
void PORT_init(void);
void NVIC_init_IRQs(void);
void SEG7_init(void);
void SEG7_display_scan(uint32_t num);
void set_segment(uint8_t pattern);
void set_digit(uint8_t idx);
void delay(volatile uint32_t d);

/* =================== main =================== */
int main(void)
{
    PORT_init();
    SEG7_init();
    NVIC_init_IRQs();

    while (1)
    {
        SEG7_display_scan(count);
    }
}

/* =================== PORT 초기화 =================== */
void PORT_init(void)
{
    PCC_PORTB |= (1 << CGC_BIT);
    PCC_PORTC |= (1 << CGC_BIT);
    PCC_PORTD |= (1 << CGC_BIT);
    PCC_PORTE |= (1 << CGC_BIT);

    /* --- SW2 (PTC12) --- */
    PORTC_PCR(12) &= ~(0b111 << MUX_BITS);
    PORTC_PCR(12) |= (1 << MUX_BITS);            // GPIO
    GPIOC_PDDR &= ~(1 << PTC12);                 // 입력
    PORTC_PCR(12) &= ~(0b1111 << IRQC_BITS);
    PORTC_PCR(12) |=  (0b1001 << IRQC_BITS);     // Rising-edge interrupt
}

/* =================== NVIC 초기화 =================== */
void NVIC_init_IRQs(void)
{
    NVIC_ICPR1 |= (1 << (61 % 32));
    NVIC_ISER1 |= (1 << (61 % 32));
    NVIC_IPR61 = 10;
}

/* =================== 7세그먼트 초기화 =================== */
void SEG7_init(void)
{
    // PORTD: a,b,c,d,e,f (D10~14, D5)
    int D_pins[] = {5,10,11,12,13,14};
    for (int i = 0; i < 6; i++)
    {
        PORTD_PCR(D_pins[i]) &= ~(0b111 << MUX_BITS);
        PORTD_PCR(D_pins[i]) |= (1 << MUX_BITS);
        GPIOD_PDDR |= (1 << D_pins[i]);
    }

    // PORTE: g (E15), DP(E16)
    int E_pins[] = {15,16};
    for (int i = 0; i < 2; i++)
    {
        PORTE_PCR(E_pins[i]) &= ~(0b111 << MUX_BITS);
        PORTE_PCR(E_pins[i]) |= (1 << MUX_BITS);
        GPIOE_PDDR |= (1 << E_pins[i]);
    }

    // PORTB: Digit 선택 (B8~13)
    for (int i = 8; i <= 13; i++)
    {
        PORTB_PCR(i) &= ~(0b111 << MUX_BITS);
        PORTB_PCR(i) |= (1 << MUX_BITS);
        GPIOB_PDDR |= (1 << i);
    }
}

/* =================== 세그먼트 출력 설정 =================== */
void set_segment(uint8_t pattern)
{
    // pattern: bit0~bit6 = a~g
    // PTD10=a, PTD11=b, PTD12=c, PTD13=d, PTD14=e, PTD5=f, PTE15=g

    // PORTD 초기화
    GPIOD_PDOR &= ~((1<<10)|(1<<11)|(1<<12)|(1<<13)|(1<<14)|(1<<5));
    GPIOE_PDOR &= ~(1<<15);

    if (pattern & (1<<0)) GPIOD_PDOR |= (1<<10); // a
    if (pattern & (1<<1)) GPIOD_PDOR |= (1<<11); // b
    if (pattern & (1<<2)) GPIOD_PDOR |= (1<<12); // c
    if (pattern & (1<<3)) GPIOD_PDOR |= (1<<13); // d
    if (pattern & (1<<4)) GPIOD_PDOR |= (1<<14); // e
    if (pattern & (1<<5)) GPIOD_PDOR |= (1<<5);  // f
    if (pattern & (1<<6)) GPIOE_PDOR |= (1<<15); // g
}

/* =================== 자리 선택 함수 =================== */
void set_digit(uint8_t idx)
{
    // PORTB8~13 = Digit1~6
    GPIOB_PDOR = (1 << (8 + idx));
}

/* =================== 6자리 스캔 출력 =================== */
void SEG7_display_scan(uint32_t num)
{
    uint8_t digit[6];

    for (int i = 0; i < 6; i++)
    {
        digit[i] = num % 10;
        num /= 10;
    }

    for (int i = 0; i < 6; i++)
    {
        set_segment(SEG_PATTERN[digit[i]]);
        set_digit(i);
        delay(4000);        // 약 2ms
        GPIOB_PDOR = 0;        // Digit OFF
    }
}

/* =================== 딜레이 =================== */

void delay(volatile uint32_t d) {
    while(d--) __asm("NOP");
}

/* =================== PORTC 인터럽트 핸들러 =================== */
void PORTC_IRQHandler(void)
{
    count++;
    if (count > 999999) count = 0;
    PORTC_PCR(12) |= (1 << ISF_BIT);  // ISF clear
}

