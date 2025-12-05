#ifndef PIEZOBUZZER_H
#define PIEZOBUZEER_H
void piezo_port_init(void);
void piezo_playTone(uint32_t freq);
void piezo_noTone(void);
void piezo_playNote(uint32_t freq, uint32_t duration_ms);
void piezo_playMelody(const char* song);
void piezo_playAirplane(void);  // 떳다떳다비행기 노래 재생
void piezo_playElije(void); //엘리제를 위하여 재생
void piezo_delay_ms(uint32_t ms);
void piezo_stop(void);
#endif
