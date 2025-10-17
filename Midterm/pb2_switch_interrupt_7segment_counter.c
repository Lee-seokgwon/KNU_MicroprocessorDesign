#include <stdint.h>

//============================================================================
// PCC for Port Clock Enable
//============================================================================

// PCC 베이스 주소 (datasheet 623p)
#define PCC_BASE   (0x40065000U)

#define PCC_PORTA       (*(volatile unsigned*)(PCC_BASE + 0x124))
#define PCC_PORTB       (*(volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC       (*(volatile unsigned*)(PCC_BASE + 0x12C))
#define PCC_PORTD       (*(volatile unsigned*)(PCC_BASE + 0x130))
#define PCC_PORTE       (*(volatile unsigned*)(PCC_BASE + 0x134))

// PORTn clock enable 마스크, (datasheet 656)
#define PCC_CGC_MASK (1U << 30)

//============================================================================
// PCC for Port Clock Enable
//============================================================================




//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO) , PORTn_BASE = PORTn의 PCR Base 주소
//============================================================================

// PORT 베이스 주소 (LMS exel file)
#define PORTA_BASE (0x40049000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTD_BASE (0x4004C000U)
#define PORTE_BASE (0x4004D000U)

//PORTn PCR (n = Pin number, 핀 마다의 설정)
#define PORTA_PCR(n)    (*(volatile unsigned*)(PORTA_BASE + (n)*4))
#define PORTB_PCR(n)    (*(volatile unsigned*)(PORTB_BASE + (n)*4))
#define PORTC_PCR(n)    (*(volatile unsigned*)(PORTC_BASE + (n)*4))
#define PORTD_PCR(n)    (*(volatile unsigned*)(PORTD_BASE + (n)*4))
#define PORTE_PCR(n)    (*(volatile unsigned*)(PORTE_BASE + (n)*4))

// PORT PCR - MUX
#define MUX_BITS        8

// PORT PCR - PullUp / PullDown / PullEnable
#define PORT_PCR_PE_MASK  (1U << 1) //PULL enable
#define PORT_PCR_PS_MASK  (1U << 0) //PULL select

// PORT PCR - Interrupt
#define IRQC_BITS       16
#define ISF_BIT         24

//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO) , PORTn_BASE = PORTn의 PCR Base 주소
//============================================================================





//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================

//GPIO 베이스 주소 (datasheet 229p)
#define GPIOA_BASE (0x400FF000U)
#define GPIOB_BASE (0x400FF040U) 
#define GPIOC_BASE (0x400FF080U)
#define GPIOD_BASE (0x400FF0C0U)
#define GPIOE_BASE (0x400FF100U)

#define GPIOA_PDOR      (*(volatile unsigned*)(GPIOA_BASE + 0x0))
#define GPIOA_PSOR      (*(volatile unsigned*)(GPIOA_BASE + 0x4))
#define GPIOA_PCOR      (*(volatile unsigned*)(GPIOA_BASE + 0x8))
#define GPIOA_PTOR      (*(volatile unsigned*)(GPIOA_BASE + 0xC))
#define GPIOA_PDIR      (*(volatile unsigned*)(GPIOA_BASE + 0x10))
#define GPIOA_PDDR      (*(volatile unsigned*)(GPIOA_BASE + 0x14))
#define GPIOA_PIDR      (*(volatile unsigned*)(GPIOA_BASE + 0x18))

#define GPIOB_PDOR      (*(volatile unsigned*)(GPIOB_BASE + 0x0))
#define GPIOB_PSOR      (*(volatile unsigned*)(GPIOB_BASE + 0x4))
#define GPIOB_PCOR      (*(volatile unsigned*)(GPIOB_BASE + 0x8))
#define GPIOB_PTOR      (*(volatile unsigned*)(GPIOB_BASE + 0xC))
#define GPIOB_PDIR      (*(volatile unsigned*)(GPIOB_BASE + 0x10))
#define GPIOB_PDDR      (*(volatile unsigned*)(GPIOB_BASE + 0x14))
#define GPIOB_PIDR      (*(volatile unsigned*)(GPIOB_BASE + 0x18))

#define GPIOC_PDOR      (*(volatile unsigned*)(GPIOC_BASE + 0x0))
#define GPIOC_PSOR      (*(volatile unsigned*)(GPIOC_BASE + 0x4))
#define GPIOC_PCOR      (*(volatile unsigned*)(GPIOC_BASE + 0x8))
#define GPIOC_PTOR      (*(volatile unsigned*)(GPIOC_BASE + 0xC))
#define GPIOC_PDIR      (*(volatile unsigned*)(GPIOC_BASE + 0x10))
#define GPIOC_PDDR      (*(volatile unsigned*)(GPIOC_BASE + 0x14))
#define GPIOC_PIDR      (*(volatile unsigned*)(GPIOC_BASE + 0x18))

#define GPIOD_PDOR      (*(volatile unsigned*)(GPIOD_BASE + 0x0))
#define GPIOD_PSOR      (*(volatile unsigned*)(GPIOD_BASE + 0x4))
#define GPIOD_PCOR      (*(volatile unsigned*)(GPIOD_BASE + 0x8))
#define GPIOD_PTOR      (*(volatile unsigned*)(GPIOD_BASE + 0xC))
#define GPIOD_PDIR      (*(volatile unsigned*)(GPIOD_BASE + 0x10))
#define GPIOD_PDDR      (*(volatile unsigned*)(GPIOD_BASE + 0x14))
#define GPIOD_PIDR      (*(volatile unsigned*)(GPIOD_BASE + 0x18))

#define GPIOE_PDOR      (*(volatile unsigned*)(GPIOE_BASE + 0x0))
#define GPIOE_PSOR      (*(volatile unsigned*)(GPIOE_BASE + 0x4))
#define GPIOE_PCOR      (*(volatile unsigned*)(GPIOE_BASE + 0x8))
#define GPIOE_PTOR      (*(volatile unsigned*)(GPIOE_BASE + 0xC))
#define GPIOE_PDIR      (*(volatile unsigned*)(GPIOE_BASE + 0x10))
#define GPIOE_PDDR      (*(volatile unsigned*)(GPIOE_BASE + 0x14))
#define GPIOE_PIDR      (*(volatile unsigned*)(GPIOE_BASE + 0x18))

//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================





//============================================================================
// NVIC (for interrupt)
//============================================================================

#define NVIC_ISER_BASE   (0xE000E100U)   // Interrupt Set Enable
#define NVIC_ICER_BASE   (0xE000E180U)   // Interrupt Clear Enable
#define NVIC_ISPR_BASE   (0xE000E200U)   // Interrupt Set Pending
#define NVIC_ICPR_BASE   (0xE000E280U)   // Interrupt Clear Pending
#define NVIC_IABR_BASE   (0xE000E300U)   // Interrupt Active Bit
#define NVIC_IPR_BASE    (0xE000E400U)   // Interrupt Priority

#define NVIC_ISER0   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x00))
#define NVIC_ISER1   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x04))
#define NVIC_ISER2   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x08))
#define NVIC_ISER3   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x0C))

#define NVIC_ICER0   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x00))
#define NVIC_ICER1   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x04))
#define NVIC_ICER2   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x08))
#define NVIC_ICER3   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x0C))

#define NVIC_ISPR0   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x00))
#define NVIC_ISPR1   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x04))
#define NVIC_ISPR2   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x08))
#define NVIC_ISPR3   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x0C))

#define NVIC_ICPR0   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x00))
#define NVIC_ICPR1   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x04))
#define NVIC_ICPR2   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x08))
#define NVIC_ICPR3   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x0C))

#define NVIC_IABR0   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x00))
#define NVIC_IABR1   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x04))
#define NVIC_IABR2   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x08))
#define NVIC_IABR3   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x0C))

#define NVIC_IPR59  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3B)) // PORTA
#define NVIC_IPR60  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3C)) // PORTB
#define NVIC_IPR61  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3D)) // PORTC
#define NVIC_IPR62  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3E)) // PORTD
#define NVIC_IPR63  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3F)) // PORTE

//============================================================================
// NVIC
//============================================================================


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

// 함수 선언
void PORT_init(void);
void NVIC_init_IRQS(void);
void SEG7_init(void);
void SEG7_display_scan(uint32_t num);
void set_segment(uint8_t pattern);
void set_digit(uint8_t idx);
void delay(volatile uint32_t d);

int main(void)
{
    PORT_init();
    SEG7_init();
    NVIC_init_IRQS();

    while (1)
    {
        SEG7_display_scan(count);
    }
}

void PORT_init(void)
{
    // PORT  A B C D E , Clock En
    PCC_PORTA |= PCC_CGC_MASK;
    PCC_PORTB |= PCC_CGC_MASK;
    PCC_PORTC |= PCC_CGC_MASK;
    PCC_PORTD |= PCC_CGC_MASK;
    PCC_PORTE |= PCC_CGC_MASK;

    // SWitch (PTA 9) 
    PORTA_PCR(9) &= ~(0b111 << MUX_BITS);
    PORTA_PCR(9) |= (1 << MUX_BITS);
    GPIOA_PDDR &= ~(1 << 9); //input
    
    // Switch (PTA 9) Interrupt
    PORTA_PCR(9) &= ~(0b1111 << IRQC_BITS);
    PORTA_PCR(9) |= (0b1001 << IRQC_BITS); // Rising-edge interrupt
}

void NVIC_init_IRQS(void)
{
    // Switch(PTA 9) interrupt. PORTA -> IRQ 59
    // 59/32 = 1
    NVIC_ICPR1 |= (1 << (59 % 32));
    NVIC_ISER1 |= (1 << (59 % 32));
    NVIC_IPR59 = 10;
}

void SEG7_init(void)
{
    // PTD5 , PTD10~PTD14  =  7seg a,b,c,d,e,f
    int D_pins[] = {5,10,11,12,13,14};
    for (int i = 0; i < 6; i++)
    {
        PORTD_PCR(D_pins[i]) &= ~(0b111 << MUX_BITS);
        PORTD_PCR(D_pins[i]) |= (1 << MUX_BITS);
        GPIOD_PDDR |= (1 << D_pins[i]);
    }

    // PTE15, PTE16  =  7seg g,dp
    int E_pins[] = {15,16};
    for (int i = 0; i < 2; i++)
    {
        PORTE_PCR(E_pins[i]) &= ~(0b111 << MUX_BITS);
        PORTE_PCR(E_pins[i]) |= (1 << MUX_BITS);
        GPIOE_PDDR |= (1 << E_pins[i]);
    }

    // PTB8~PTB13  =  Digit 선택
    for (int i = 8; i <= 13; i++)
    {
        PORTB_PCR(i) &= ~(0b111 << MUX_BITS);
        PORTB_PCR(i) |= (1 << MUX_BITS);
        GPIOB_PDDR |= (1 << i);
    }
}

//세그먼트 숫자 출력
void set_segment(uint8_t pattern)
{
    // 7seg 초기화 (PTD10~PTD14, PTD5, PTE15, PTE16)
    GPIOD_PDOR &= ~((1<<10)|(1<<11)|(1<<12)|(1<<13)|(1<<14)|(1<<5));
    GPIOE_PDOR &= ~(1<<15);

    // 입력된 7SEG 패턴에 따라 7SEG output 설정
    if (pattern & (1<<0)) GPIOD_PDOR |= (1<<10); // a (SEGa - D10)
    if (pattern & (1<<1)) GPIOD_PDOR |= (1<<11); // b (SEGb - D11)
    if (pattern & (1<<2)) GPIOD_PDOR |= (1<<12); // c (SEGc - D12)
    if (pattern & (1<<3)) GPIOD_PDOR |= (1<<13); // d (SEGd - D13)
    if (pattern & (1<<4)) GPIOD_PDOR |= (1<<14); // e (SEGe - D14)
    if (pattern & (1<<5)) GPIOD_PDOR |= (1<<5);  // f (SEGf - D5)
    if (pattern & (1<<6)) GPIOE_PDOR |= (1<<15); // g (SEGg - E15)
}

// 자리 선택 함수
void set_digit(uint8_t idx)
{
    //PTB8 ~ PTB13  = Digit 선택
    //입력받은 idx는 몇번째 7SEG를 킬건지
    GPIOB_PDOR = (1 << (8 + idx));
}

//6자리 스캔 출력함수
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
        delay(4000);
        GPIOB_PDOR = 0; // all digit off (현재 PTB8~PTB13 가 digit pin)
    }
}

// 스위치 인터럽트 핸들러 (PTA9)
void PORTA_IRQHandler(void)
{
    delay(40000); // debouncing
    count ++;
    if (count > 999999) count = 0;
    PORTA_PCR(9) |= (1 << ISF_BIT); // ISF Clear
}

void delay(volatile uint32_t d) {
    while(d--) __asm("NOP");
}

