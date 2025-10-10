#ifndef LSG_S32K144_HAL_H_
#define LSG_S32K144_HAL_H_


#include <stdint.h>

//============================================================================
// PCC for Port Clock Enable
//============================================================================

// PCC 베이스 주소 (datasheet 623p)
#define PCC_BASE   (0x40065000U)

// PCC 내부 포트들 위치 (PCCn[PCC_PORTA_INDEX] -> PORTA의 PCC 영역으로 접근)
// PCCn 원소 한개 4바이트. 배열인덱스 = datasheet Offset / 4 
#define PCC_PORTA_INDEX (73)
#define PCC_PORTB_INDEX (74)
#define PCC_PORTC_INDEX (75)
#define PCC_PORTD_INDEX (76)
#define PCC_PORTE_INDEX (77)

// PORTn clock enable 마스크, (datasheet 656)
#define PCC_PCCn_CGC_MASK (1U << 30)

//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO) , PORTn_BASE = PORTn의 PCR Base 주소
//============================================================================

// PORT 베이스 주소 (LMS exel file)
#define PORTA_BASE (0x40049000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTD_BASE (0x4004C000U)
#define PORTE_BASE (0x4004D000U)

// PORT PCR
#define PORT_PCR_MUX_GPIO (1U << 8) //MUX set to GPIO
#define PORT_PCR_PE_MASK  (1U << 1) //PULL enable
#define PORT_PCR_PS_MASK  (1U << 0) //PULL select
#define PORT_PCR_SET_GPIO_MUX (PORT_PCR_MUX_GPIO)

//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================

//GPIO 베이스 주소 (datasheet 229p)
#define GPIOA_BASE (0x400FF000U)
#define GPIOB_BASE (0x400FF040U) 
#define GPIOC_BASE (0x400FF080U)
#define GPIOD_BASE (0x400FF0C0U)
#define GPIOE_BASE (0x400FF100U)

//============================================================================
// 구조체
//============================================================================

// PCC 구조체 (포트별 Clock Enable) (datasheet 623p)
typedef struct {
    volatile uint32_t PCCn[128];
} PCC_Type;

// PORT 구조체 (포트 내부 개별 핀 설정) (datasheet 211p) 
// 추후 핀 추가시 배열 원소 개수 늘려주기.
// 1 원소당 32비트 이므로, 인덱스 = offset/4 
typedef struct {
    volatile uint32_t PCR[32];
} PORT_Type;

// GPIO 구조체
typedef struct {
    volatile uint32_t PDOR;
    volatile uint32_t PSOR;
    volatile uint32_t PCOR;
    volatile uint32_t PTOR;
    volatile uint32_t PDIR;
    volatile uint32_t PDDR;
    volatile uint32_t PIDR;
} GPIO_Type;

//============================================================================
//============================================================================



//============================================================================
// 구조체 접근을 위한 구조체 포인터 선언
//============================================================================

#define PCC   ((PCC_Type*)PCC_BASE)

#define PORTA ((PORT_Type*)PORTA_BASE)
#define PORTB ((PORT_Type*)PORTB_BASE)
#define PORTC ((PORT_Type*)PORTC_BASE)
#define PORTD ((PORT_Type*)PORTD_BASE)
#define PORTE ((PORT_Type*)PORTE_BASE)

#define GPIOA   ((GPIO_Type*)GPIOA_BASE)
#define GPIOB   ((GPIO_Type*)GPIOB_BASE)
#define GPIOC   ((GPIO_Type*)GPIOC_BASE)
#define GPIOD   ((GPIO_Type*)GPIOD_BASE)
#define GPIOE   ((GPIO_Type*)GPIOE_BASE)

//============================================================================
//============================================================================

#endif