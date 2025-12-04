#ifndef ULTRASONIC_H
#define ULTRASONIC_H

void ultrasonic_port_init(void);
void ultrasonic_short_delay(volatile uint32_t cnt);
void ultrasonic_send_trig_pulse(void);
uint32_t ultrasonic_measure_distance_cm(void);


#endif
