#include "common.h"
#include "lcd.h"

/* Data lines */
#define LCD_D0_PIN      0u

#define LCD_D1_PIN      1u

#define LCD_D2_PIN      2u

#define LCD_D3_PIN      3u

#define LCD_D4_PIN      0u

#define LCD_D5_PIN      1u

#define LCD_D6_PIN      2u

#define LCD_D7_PIN      3u

/* Control lines (PTC) */
#define LCD_RS_PIN      0u     // PTC0
#define LCD_RW_PIN      1u     // PTC1
#define LCD_EN_PIN      2u     // PTC2


void lcd_delay(volatile uint32_t d)
{
    while (d--) __asm("NOP");
}


void lcd_port_init(void)
{
    /* 1. PORTA/B/C 클럭 ON */
    PCC_PORTA |= (1u << CGC_BIT);
    PCC_PORTB |= (1u << CGC_BIT);
    PCC_PORTC |= (1u << CGC_BIT);

    /* 2. 각 핀을 GPIO로 설정 */
    PORTA_PCR(LCD_D0_PIN) &= ~((0b111) << MUX_BITS);
    PORTA_PCR(LCD_D0_PIN) |=  ((0b001) << MUX_BITS);
    PORTA_PCR(LCD_D1_PIN) &= ~((0b111) << MUX_BITS);
    PORTA_PCR(LCD_D1_PIN) |=  ((0b001) << MUX_BITS);
    PORTA_PCR(LCD_D2_PIN) &= ~((0b111) << MUX_BITS);
    PORTA_PCR(LCD_D2_PIN) |=  ((0b001) << MUX_BITS);
    PORTA_PCR(LCD_D3_PIN) &= ~((0b111) << MUX_BITS);
    PORTA_PCR(LCD_D3_PIN) |=  ((0b001) << MUX_BITS);

    PORTB_PCR(LCD_D4_PIN) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(LCD_D4_PIN) |=  ((0b001) << MUX_BITS);
    PORTB_PCR(LCD_D5_PIN) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(LCD_D5_PIN) |=  ((0b001) << MUX_BITS);
    PORTB_PCR(LCD_D6_PIN) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(LCD_D6_PIN) |=  ((0b001) << MUX_BITS);
    PORTB_PCR(LCD_D7_PIN) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(LCD_D7_PIN) |=  ((0b001) << MUX_BITS);

    PORTC_PCR(LCD_RS_PIN) &= ~((0b111) << MUX_BITS);
    PORTC_PCR(LCD_RS_PIN) |=  ((0b001) << MUX_BITS);
    PORTC_PCR(LCD_RW_PIN) &= ~((0b111) << MUX_BITS);
    PORTC_PCR(LCD_RW_PIN) |=  ((0b001) << MUX_BITS);
    PORTC_PCR(LCD_EN_PIN) &= ~((0b111) << MUX_BITS);
    PORTC_PCR(LCD_EN_PIN) |=  ((0b001) << MUX_BITS);

    /* 3. 방향: 전부 출력 */
    GPIOA_PDDR |= (1u << LCD_D0_PIN) |
                  (1u << LCD_D1_PIN) |
                  (1u << LCD_D2_PIN) |
                  (1u << LCD_D3_PIN);

    GPIOB_PDDR |= (1u << LCD_D4_PIN) |
                  (1u << LCD_D5_PIN) |
                  (1u << LCD_D6_PIN) |
                  (1u << LCD_D7_PIN);

    GPIOC_PDDR |= (1u << LCD_RS_PIN) |
                  (1u << LCD_RW_PIN) |
                  (1u << LCD_EN_PIN);

    lcd_delay(200000);          // 전원 안정 대기
    lcd_cmd(0x38);          // Function set: 8bit, 2line, 5x8
    lcd_cmd(0x0C);          // Display ON, Cursor OFF
    lcd_cmd(0x01);          // Clear display
    lcd_delay(200000);          // Clear는 오래 기다림
    lcd_cmd(0x06);          // Entry mode set: 자동 인크리먼트
}

void lcd_write_bus(uint8_t data)
{
    /* 먼저 모든 데이터 핀 0으로 클리어 */
    GPIOA_PCOR = (1u << LCD_D0_PIN) |
                 (1u << LCD_D1_PIN) |
                 (1u << LCD_D2_PIN) |
                 (1u << LCD_D3_PIN);

    GPIOB_PCOR = (1u << LCD_D4_PIN) |
                 (1u << LCD_D5_PIN) |
                 (1u << LCD_D6_PIN) |
                 (1u << LCD_D7_PIN);

    /* data 각 비트에 따라 다시 세팅 */
    if (data & 0x01) GPIOA_PSOR = (1u << LCD_D0_PIN);
    if (data & 0x02) GPIOA_PSOR = (1u << LCD_D1_PIN);
    if (data & 0x04) GPIOA_PSOR = (1u << LCD_D2_PIN);
    if (data & 0x08) GPIOA_PSOR = (1u << LCD_D3_PIN);

    if (data & 0x10) GPIOB_PSOR = (1u << LCD_D4_PIN);
    if (data & 0x20) GPIOB_PSOR = (1u << LCD_D5_PIN);
    if (data & 0x40) GPIOB_PSOR = (1u << LCD_D6_PIN);
    if (data & 0x80) GPIOB_PSOR = (1u << LCD_D7_PIN);
}

void lcd_enable_pulse(void)
{
    GPIOC_PSOR = (1u << LCD_EN_PIN);   // EN = 1
    lcd_delay(5000);                       // 몇 µs 정도
    GPIOC_PCOR = (1u << LCD_EN_PIN);   // EN = 0
    lcd_delay(5000);
}

void lcd_cmd(uint8_t cmd)
{
    /* RS=0 (명령), RW=0 (쓰기) */
    GPIOC_PCOR = (1u << LCD_RS_PIN);   // RS = 0
    GPIOC_PCOR = (1u << LCD_RW_PIN);   // RW = 0

    lcd_write_bus(cmd);
    lcd_enable_pulse();
    lcd_delay(20000);  // 명령 처리 시간 여유
}

void lcd_data(uint8_t data)
{
    /* RS=1 (데이터), RW=0 (쓰기) */
    GPIOC_PSOR = (1u << LCD_RS_PIN);   // RS = 1
    GPIOC_PCOR = (1u << LCD_RW_PIN);   // RW = 0

    lcd_write_bus(data);
    lcd_enable_pulse();
    lcd_delay(20000);
}

void lcd_clear(void)
{
    lcd_cmd(0x01);
    lcd_delay(200000);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? 0x80 : 0xC0;  // 첫 줄: 0x80, 둘째 줄: 0xC0
    addr += col;
    lcd_cmd(addr);
}

void lcd_print_string(const char* str)
{
    while (*str)
    {
        lcd_data(*str++);
    }
}
void lcd_debug_delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    while (ms--)
    {
        count = 80000;  // 80MHz 기준 1ms
        while (count--) __asm("nop");
    }
}
