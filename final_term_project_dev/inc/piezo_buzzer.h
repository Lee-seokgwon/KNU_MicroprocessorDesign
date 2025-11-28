#ifndef PIEZOBUZZER_H
#define PIEZOBUZEER_H
void piezo_port_init(void);
void piezo_playTone(uint32_t freq);
void piezo_noTone(void);
void piezo_playNote(uint32_t freq, uint32_t duration_ms);
void piezo_playMelody(void);
void piezo_delay_ms(uint32_t ms);
#endif