#include "common.h"
#include "piezo_buzzer.h"
#include "pwm.h"

//PTD16 PWM
void piezo_port_init(void)
{
    PCC_PORTD |= PCC_CGC_MASK;
    PORTD_PCR(16) &= ~((0b111)<<MUX_BITS);
    PORTD_PCR(16) |= ((0b010)<<MUX_BITS);
    FTM0_CH1_PWM();
}

void piezo_playTone(uint32_t freq)
{
    uint32_t mod = (8000000U / freq) - 1;   // 8MHz / freq
    FTM0_MOD = mod;
    FTM0_C1V = mod / 2;   // 50% duty
}

void piezo_noTone(void)
{
    FTM0_C1V = 0;
}

void piezo_delay_ms(uint32_t ms)
{
    // 코어 클럭 80MHz → 1ms = 80000 사이클
    volatile uint32_t count;
    while (ms--)
    {
        count = 80000;
        while (count--) __asm("nop");
    }
}

void piezo_playNote(uint32_t freq, uint32_t duration_ms)
{
    piezo_playTone(freq);
    piezo_delay_ms(duration_ms);
    piezo_noTone();          // 소리 끔
    piezo_delay_ms(50);      // 음 사이 50ms 쉬기
}


// 인자를 문자열로 받아서, 해당 문자열에 맞는 노래 출력
// (내부 음계는 노래 정해지면 추후 수정)
void piezo_playMelody(const char* song)
{
    uint32_t d = 100;	// 기본음 길이

    if(strcmp(song, "AA") == 0)
    {
        piezo_playNote(523, d);
        piezo_playNote(659, d);
        piezo_playNote(784, d);
        piezo_playNote(1046, d);
    }
    else if(strcmp(song, "BB") == 0)
    {
        piezo_playNote(880, 80);
        piezo_playNote(1318, 80);
    }
    else if(strcmp(song, "CC") == 0)
    {
        piezo_playNote(659, 150);
        piezo_playNote(523, 200);
        piezo_playNote(392, 300);
    }
    else
    {
        // 잘못된 문자열 → 아무 소리도 안 냄
        piezo_noTone();
        return;
    }

    piezo_noTone();
}

// 떳다떳다비행기 노래 재생
void piezo_playAirplane(void)
{
    uint32_t d = 10;   // 음 길이 20ms

    // 미 레 도 레
    piezo_playNote(659, d);  // 미
    piezo_playNote(587, d);  // 레
    piezo_playNote(523, d);  // 도
    piezo_playNote(587, d);  // 레

    // 미 미 미
    piezo_playNote(659, d);
    piezo_playNote(659, d);
    piezo_playNote(659, d);

    // 레 레 레
    piezo_playNote(587, d);
    piezo_playNote(587, d);
    piezo_playNote(587, d);

    // 미 미 미
    piezo_playNote(659, d);
    piezo_playNote(659, d);
    piezo_playNote(659, d);

    piezo_noTone();
}

void piezo_playElije(void)
{
    uint32_t d = 10;   // 음 길이(ms) — 필요하면 조절

    // 엘리제를 위하여 (첫 구절)
    piezo_playNote(659, d);  // E5
    piezo_playNote(622, d);  // D#5
    piezo_playNote(659, d);  // E5
    piezo_playNote(622, d);  // D#5
    piezo_playNote(659, d);  // E5
    piezo_playNote(494, d);  // B4
    piezo_playNote(587, d);  // D5
    piezo_playNote(523, d);  // C5
    piezo_playNote(440, d); // A4 (조금 길게)

    piezo_noTone();

}
