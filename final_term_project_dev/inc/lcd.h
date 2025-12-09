#ifndef LCD_H
#define LCD_H

#include <stdint.h>

void lcd_port_init(void);
void lcd_delay(volatile uint32_t d);
void lcd_write_bus(uint8_t data);
void lcd_enable_pulse(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print_string(const char* str);

#endif
