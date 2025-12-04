#ifndef LCD_H
#define LCD_H

void lcd_port_init(void);
void lcd_delay(volatile uint32_t d);
void lcd_write_bus(uint8_t data);
void lcd_enable_pulse(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);

#endif