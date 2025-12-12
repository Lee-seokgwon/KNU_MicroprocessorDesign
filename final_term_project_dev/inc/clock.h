#ifndef CLOCK_H
#define CLOCK_H

#include "common.h"

// 전역 변수 (clock.c에서 정의)
extern unsigned char current_digit_display;
extern unsigned int g_hour;
extern unsigned int g_minute;
extern unsigned int g_second;
extern unsigned char clock_running;
extern unsigned char colon_state;
extern unsigned char switch_d10_pressed_flag;
extern unsigned char switch_d11_pressed_flag;
extern unsigned char switch_e10_pressed_flag;
extern unsigned char switch_e11_pressed_flag;

// 세그먼트 / 자릿수 제어
void clear_all_segments(void);
void set_digit_pattern(unsigned char digit);
void select_digit_position(unsigned char position);
void display_time_dynamic(void);

// 제어/상태
void handle_switch_polling(void);
void init_gpio_hardware(void);
void colon_on(void);
void colon_off(void);
void increment_time_one_second(void);

#endif // CLOCK_H


