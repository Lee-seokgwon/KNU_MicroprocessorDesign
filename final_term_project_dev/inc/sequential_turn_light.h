#ifndef SEQUENTIALTURNLIGHT_H
#define SEQUENTIALTURNLIGHT_H

//interrupt.c 에 공유하는 변수
extern bool g_left_request;
extern bool g_right_request;
extern unsigned char SEQUENTIAL_LIGHT_PATTERN_LEFT[9];
extern unsigned char SEQUENTIAL_LIGHT_PATTERN_RIGHT[9];

void sequential_turn_light_led_init (void);
void sequential_turn_light_switch_init (void);
void sequential_turn_light_port_init(void);

#endif
