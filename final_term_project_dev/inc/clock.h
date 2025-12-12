#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

//============================================================================
// Clock Functions
//============================================================================

/**
 * @brief 7-Segment 디스플레이 GPIO 초기화
 */
void clock_gpio_init(void);

/**
 * @brief SysTick 타이머 초기화 (1ms 주기)
 * @param core_clock_hz 코어 클럭 주파수 (Hz)
 */
void clock_systick_init(uint32_t core_clock_hz);

/**
 * @brief 현재 시간 가져오기
 * @param hour 시 (0~23)
 * @param minute 분 (0~59)
 * @param second 초 (0~59)
 */
void clock_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second);

/**
 * @brief 시간 설정
 * @param hour 시 (0~23)
 * @param minute 분 (0~59)
 * @param second 초 (0~59)
 */
void clock_set_time(uint8_t hour, uint8_t minute, uint8_t second);

/**
 * @brief 시간 1초 증가
 */
void clock_increment_one_second(void);

/**
 * @brief 디스플레이 동적 스캔 (매번 호출 시 다음 자릿수 표시)
 */
void clock_display_update(void);

/**
 * @brief 현재 ms 틱 값 가져오기
 * @return ms 틱 값
 */
uint32_t clock_get_ms_ticks(void);

/**
 * @brief SysTick 인터럽트 핸들러 (외부에서 호출)
 */
void SysTick_Handler(void);

#endif

