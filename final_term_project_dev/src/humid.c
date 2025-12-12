#include "common.h"
#include "humid.h"


// DHT11 Data pin (PTD12 사용)
#define HUMID_PIN PTD12

/**
 * @brief DHT11 초기화
 */
void humid_init(void)
{
    /* PORTD clock enable */
    PCC_PORTD |= (1u << CGC_BIT);

    /* PTD12을 GPIO로 설정 */
    PORTD_PCR(HUMID_PIN) &= ~((0b111) << MUX_BITS);
    PORTD_PCR(HUMID_PIN) |= (1u << MUX_BITS);

    /* 출력 모드로 설정하고 HIGH로 초기화 */
    GPIOD_PDDR |= (1u << HUMID_PIN);
    GPIOD_PSOR |= (1u << HUMID_PIN);
}

/**
 * @brief MCU가 센서에 데이터 요청 신호
 */
void humid_start_signal(void)
{
    /* PTD12을 output으로 설정 */
    GPIOD_PDDR |= (1u << HUMID_PIN);

    /* LOW를 18ms 이상 유지 */
    GPIOD_PCOR = (1u << HUMID_PIN);
    volatile uint32_t t = 228000u;
    while (t--);

    /* HIGH로 설정 뒤 바로 INPUT으로 설정 */
    GPIOD_PSOR = (1u << HUMID_PIN);
    GPIOD_PDDR &= ~(1u << HUMID_PIN);
}

/**
 * @brief DHT11 응답 신호 확인
 * @return 1: 정상 응답
 */
uint8_t humid_check_response(void)
{
    /* LOW 대기 (센서가 80µs 동안 LOW 신호 전송) */
    while (!(GPIOD_PDIR & (1u << HUMID_PIN)));

    /* HIGH 대기 (센서가 80µs 동안 HIGH 신호 전송) */
    while ((GPIOD_PDIR & (1u << HUMID_PIN)));

    return 1; /* 정상적으로 응답 신호 확인 */
}

/**
 * @brief DHT11에서 8비트 데이터 읽기
 * @return 읽은 8비트 데이터
 */
uint32_t humid_read_data(void)
{
    uint32_t data = 0u;

    for (uint8_t i = 0u; i < 8u; i++) /* 8bit씩 끊어서 읽기 */
    {
        uint32_t cnt = 0u;

        /* LOW 대기 */
        while (!(GPIOD_PDIR & (1u << HUMID_PIN)));

        /* HIGH 시간 count */
        while ((GPIOD_PDIR & (1u << HUMID_PIN)))
        {
            cnt++;
        }

        /* cnt > 250이면 비트 1 */
        if (cnt > 250u)
        {
            data |= (1u << (7u - i)); /* 1비트 데이터 저장 */
        }
    }

    return data;
}

/**
 * @brief DHT11 데이터 읽기
 * @param humidity 습도 값을 저장할 포인터
 * @param temperature 온도 값을 저장할 포인터
 * @return 1: 성공, 0: 실패 (체크섬 오류)
 */
uint8_t humid_get_data(uint32_t *humidity, uint32_t *temperature)
{
    uint32_t humi_high, humi_low, temp_high, temp_low, checksum;

    humid_start_signal();    /* start */
    humid_check_response();   /* response */

    /* response를 한번 하니 읽어들인 data가 1bit씩 밀려서 response를 2번함 */
    while (!(GPIOD_PDIR & (1u << HUMID_PIN)));
    while ((GPIOD_PDIR & (1u << HUMID_PIN)));

    humi_high = humid_read_data();  /* 습도 정수 값 */
    humi_low = humid_read_data();   /* 습도 소수 값 */
    temp_high = humid_read_data();  /* 온도 정수 값 */
    temp_low = humid_read_data();   /* 온도 소수 값 */
    checksum = humid_read_data();   /* 모두 다 더한 checksum */

    if (checksum == (humi_high + humi_low + temp_high + temp_low))
    {
        *humidity = humi_high;      /* 습도 상위 바이트 */
        *temperature = temp_high;   /* 온도 상위 바이트 */
        return 1;  /* 성공 */
    }

    /* 센서 응답 실패 - 이전 값 유지 (0으로 덮어쓰지 않음) */
    return 0;  /* 실패 */
}

/**
 * @brief 지연 함수 (80MHz 기준)
 */
void delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    while (ms--)
    {
        count = 80000u;  /* 80MHz 기준 1ms */
        while (count--) __asm("nop");
    }
}
