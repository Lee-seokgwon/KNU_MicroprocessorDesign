#include <stdint.h>
#include "S32K144.h" // 클럭/SCG 구조체 정의 헤더

//============================================================================
// PCC for Port Clock Enable
//============================================================================

#define PCC_BASE   (0x40065000U)

#define PCC_PORTA       (*(volatile unsigned*)(PCC_BASE + 0x124))
#define PCC_PORTB       (*(volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC       (*(volatile unsigned*)(PCC_BASE + 0x12C))
#define PCC_PORTD       (*(volatile unsigned*)(PCC_BASE + 0x130))
#define PCC_PORTE       (*(volatile unsigned*)(PCC_BASE + 0x134))

// PORTn clock enable 마스크
#define PCC_CGC_MASK (1U << 30)

//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO / ADC)
//============================================================================

#define PORTA_BASE (0x40049000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTD_BASE (0x4004C000U)
#define PORTE_BASE (0x4004D000U)

#define PORTA_PCR(n)    (*(volatile unsigned*)(PORTA_BASE + (n)*4))
#define PORTB_PCR(n)    (*(volatile unsigned*)(PORTB_BASE + (n)*4))
#define PORTC_PCR(n)    (*(volatile unsigned*)(PORTC_BASE + (n)*4))
#define PORTD_PCR(n)    (*(volatile unsigned*)(PORTD_BASE + (n)*4))
#define PORTE_PCR(n)    (*(volatile unsigned*)(PORTE_BASE + (n)*4))

// PORT PCR - MUX
#define MUX_BITS        8

// PORT PCR - PullUp / PullDown / PullEnable
#define PORT_PCR_PE_MASK  (1U << 1) //PULL enable
#define PORT_PCR_PS_MASK  (1U << 0) //PULL select

// PORT PCR - Interrupt
#define IRQC_BITS       16
#define ISF_BIT         24

// 편의용 매크로 (LED 핀)
#define PTD0  0
#define PTD15 15
#define PTD16 16

//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================

#define GPIOA_BASE (0x400FF000U)
#define GPIOB_BASE (0x400FF040U)
#define GPIOC_BASE (0x400FF080U)
#define GPIOD_BASE (0x400FF0C0U)
#define GPIOE_BASE (0x400FF100U)

#define GPIOA_PDOR      (*(volatile unsigned*)(GPIOA_BASE + 0x0))
#define GPIOA_PSOR      (*(volatile unsigned*)(GPIOA_BASE + 0x4))
#define GPIOA_PCOR      (*(volatile unsigned*)(GPIOA_BASE + 0x8))
#define GPIOA_PTOR      (*(volatile unsigned*)(GPIOA_BASE + 0xC))
#define GPIOA_PDIR      (*(volatile unsigned*)(GPIOA_BASE + 0x10))
#define GPIOA_PDDR      (*(volatile unsigned*)(GPIOA_BASE + 0x14))
#define GPIOA_PIDR      (*(volatile unsigned*)(GPIOA_BASE + 0x18))

#define GPIOB_PDOR      (*(volatile unsigned*)(GPIOB_BASE + 0x0))
#define GPIOB_PSOR      (*(volatile unsigned*)(GPIOB_BASE + 0x4))
#define GPIOB_PCOR      (*(volatile unsigned*)(GPIOB_BASE + 0x8))
#define GPIOB_PTOR      (*(volatile unsigned*)(GPIOB_BASE + 0xC))
#define GPIOB_PDIR      (*(volatile unsigned*)(GPIOB_BASE + 0x10))
#define GPIOB_PDDR      (*(volatile unsigned*)(GPIOB_BASE + 0x14))
#define GPIOB_PIDR      (*(volatile unsigned*)(GPIOB_BASE + 0x18))

#define GPIOC_PDOR      (*(volatile unsigned*)(GPIOC_BASE + 0x0))
#define GPIOC_PSOR      (*(volatile unsigned*)(GPIOC_BASE + 0x4))
#define GPIOC_PCOR      (*(volatile unsigned*)(GPIOC_BASE + 0x8))
#define GPIOC_PTOR      (*(volatile unsigned*)(GPIOC_BASE + 0xC))
#define GPIOC_PDIR      (*(volatile unsigned*)(GPIOC_BASE + 0x10))
#define GPIOC_PDDR      (*(volatile unsigned*)(GPIOC_BASE + 0x14))
#define GPIOC_PIDR      (*(volatile unsigned*)(GPIOC_BASE + 0x18))

#define GPIOD_PDOR      (*(volatile unsigned*)(GPIOD_BASE + 0x0))
#define GPIOD_PSOR      (*(volatile unsigned*)(GPIOD_BASE + 0x4))
#define GPIOD_PCOR      (*(volatile unsigned*)(GPIOD_BASE + 0x8))
#define GPIOD_PTOR      (*(volatile unsigned*)(GPIOD_BASE + 0xC))
#define GPIOD_PDIR      (*(volatile unsigned*)(GPIOD_BASE + 0x10))
#define GPIOD_PDDR      (*(volatile unsigned*)(GPIOD_BASE + 0x14))
#define GPIOD_PIDR      (*(volatile unsigned*)(GPIOD_BASE + 0x18))

#define GPIOE_PDOR      (*(volatile unsigned*)(GPIOE_BASE + 0x0))
#define GPIOE_PSOR      (*(volatile unsigned*)(GPIOE_BASE + 0x4))
#define GPIOE_PCOR      (*(volatile unsigned*)(GPIOE_BASE + 0x8))
#define GPIOE_PTOR      (*(volatile unsigned*)(GPIOE_BASE + 0xC))
#define GPIOE_PDIR      (*(volatile unsigned*)(GPIOE_BASE + 0x10))
#define GPIOE_PDDR      (*(volatile unsigned*)(GPIOE_BASE + 0x14))
#define GPIOE_PIDR      (*(volatile unsigned*)(GPIOE_BASE + 0x18))

//============================================================================
// NVIC (for interrupt) - (현재 예제에서는 사용 안 함, 그대로 둠)
//============================================================================

#define NVIC_ISER_BASE   (0xE000E100U)
#define NVIC_ICER_BASE   (0xE000E180U)
#define NVIC_ISPR_BASE   (0xE000E200U)
#define NVIC_ICPR_BASE   (0xE000E280U)
#define NVIC_IABR_BASE   (0xE000E300U)
#define NVIC_IPR_BASE    (0xE000E400U)

#define NVIC_ISER0   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x00))
#define NVIC_ISER1   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x04))
#define NVIC_ISER2   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x08))
#define NVIC_ISER3   (*(volatile unsigned*)(NVIC_ISER_BASE + 0x0C))

#define NVIC_ICER0   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x00))
#define NVIC_ICER1   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x04))
#define NVIC_ICER2   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x08))
#define NVIC_ICER3   (*(volatile unsigned*)(NVIC_ICER_BASE + 0x0C))

#define NVIC_ISPR0   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x00))
#define NVIC_ISPR1   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x04))
#define NVIC_ISPR2   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x08))
#define NVIC_ISPR3   (*(volatile unsigned*)(NVIC_ISPR_BASE + 0x0C))

#define NVIC_ICPR0   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x00))
#define NVIC_ICPR1   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x04))
#define NVIC_ICPR2   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x08))
#define NVIC_ICPR3   (*(volatile unsigned*)(NVIC_ICPR_BASE + 0x0C))

#define NVIC_IABR0   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x00))
#define NVIC_IABR1   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x04))
#define NVIC_IABR2   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x08))
#define NVIC_IABR3   (*(volatile unsigned*)(NVIC_IABR_BASE + 0x0C))

#define NVIC_IPR48  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x30)) // LPIT0
#define NVIC_IPR59  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3B)) // PORTA
#define NVIC_IPR60  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3C)) // PORTB
#define NVIC_IPR61  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3D)) // PORTC
#define NVIC_IPR62  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3E)) // PORTD
#define NVIC_IPR63  (*(volatile unsigned char*)(NVIC_IPR_BASE + 0x3F)) // PORTE

//============================================================================
// LPIT Timer (현재 예제에서는 사용 안 함, 그대로 둠)
//============================================================================

#define PCC_LPIT        (*(volatile unsigned*)(PCC_BASE + 0xDC))
#define PCS_BITS 24

#define LPIT_BASE (0x40037000)
#define LPIT_MCR (*(volatile unsigned*)(LPIT_BASE + 0x8))
#define LPIT_MSR (*(volatile unsigned*)(LPIT_BASE + 0xC))
#define LPIT_MIER (*(volatile unsigned*)(LPIT_BASE + 0x10))
#define LPIT_TVAL0 (*(volatile unsigned*)(LPIT_BASE + 0x20))
#define LPIT_TCTRL0 (*(volatile unsigned*)(LPIT_BASE + 0x28))

#define M_CEN_BIT      0
#define TIF0_BIT       0
#define TIE0_BIT       0
#define LPIT_MODE_BITS 2   // LPIT용 MODE 비트 (ADC와 이름 안 겹치게)
#define T_EN_BIT       0

//============================================================================
// ADC
//============================================================================

#define PCC_ADC0 (*(volatile unsigned*)(PCC_BASE + 0xEC))
#define PCC_ADC1 (*(volatile unsigned*)(PCC_BASE + 0x9C))

#define ADC0_BASE (0x4003B000)
#define ADC1_BASE (0x40027000)

#define ADC0_SC1A (*(volatile unsigned*)(ADC0_BASE + 0x0))
#define ADC0_CFG1 (*(volatile unsigned*)(ADC0_BASE + 0x40))
#define ADC0_CFG2 (*(volatile unsigned*)(ADC0_BASE + 0x44))
#define ADC0_RA   (*(volatile unsigned*)(ADC0_BASE + 0x48))
#define ADC0_SC2  (*(volatile unsigned*)(ADC0_BASE + 0x90))
#define ADC0_SC3  (*(volatile unsigned*)(ADC0_BASE + 0x94))

#define ADC1_SC1A (*(volatile unsigned*)(ADC1_BASE + 0x0))
#define ADC1_CFG1 (*(volatile unsigned*)(ADC1_BASE + 0x40))
#define ADC1_CFG2 (*(volatile unsigned*)(ADC1_BASE + 0x44))
#define ADC1_RA   (*(volatile unsigned*)(ADC1_BASE + 0x48))
#define ADC1_SC2  (*(volatile unsigned*)(ADC1_BASE + 0x90))
#define ADC1_SC3  (*(volatile unsigned*)(ADC1_BASE + 0x94))

#define ADCH_BITS        0
#define COCO_BIT         7

// ADC 전용 비트 위치 (LPIT와 이름 겹치지 않게 별도 정의)
#define ADC_MODE_BITS    2   // CFG1.MODE[1:0]
#define ADC_ADIV_BITS    5   // CFG1.ADIV[1:0]
#define ADC_SMPLTS_BITS  0   // CFG2.SMPLTS[7:0]
#define ADC_ADTRG_BIT    6   // SC2.ADTRG

// *** 채널 번호 정의 ***
#define ADC_CH_POT   12U   // PTC14, 온보드 가변저항 (ADC0_SE12)
#define ADC_CH_CDS   4U    // PTB0, CdS 센서 (ADC0_SE4)

//============================================================================
// 함수 프로토타입
//============================================================================
void PORT_init(void);
void ADC0_init(void);
static uint16_t ADC0_ReadChannel(uint8_t ch);

void SOSC_init_8MHz(void);
void SPLL_init_160MHz(void);
void NormalRUNmode_80MHz(void);

//============================================================================
// main
//============================================================================
int main(void)
{
    uint32_t cdsResult = 0;
    // uint32_t potResult = 0; // 필요하면 사용

    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    PORT_init();
    ADC0_init();

    for (;;)
    {
        // CdS가 연결된 채널(PTB0, ADC0_SE4) 읽기
        cdsResult = ADC0_ReadChannel(ADC_CH_CDS);

        // 온보드 가변저항 값도 보고 싶으면 같이 읽기
        // potResult = ADC0_ReadChannel(ADC_CH_POT);

        // CdS 결과값에 따라 LED 색 변경
        if (cdsResult > 3072)
        {
            // 빨간색 ON (PTD15 low), 나머지 OFF
            GPIOD_PSOR |= ((1U<<PTD0) | (1U<<PTD16)); // 파랑, 초록 OFF
            GPIOD_PCOR |= (1U<<PTD15);                // 빨강 ON
        }
        else if (cdsResult > 2048)
        {
            // 초록색 ON
            GPIOD_PSOR |= ((1U<<PTD0) | (1U<<PTD15)); // 파랑, 빨강 OFF
            GPIOD_PCOR |= (1U<<PTD16);                // 초록 ON
        }
        else if (cdsResult > 1024)
        {
            // 파란색 ON
            GPIOD_PSOR |= ((1U<<PTD15) | (1U<<PTD16)); // 빨강, 초록 OFF
            GPIOD_PCOR |= (1U<<PTD0);                  // 파랑 ON
        }
        else
        {
            // 모두 OFF
            GPIOD_PSOR |= ((1U<<PTD0) | (1U<<PTD15) | (1U<<PTD16));
        }
    }
}

//============================================================================
// PORT 초기화
//   - PTD0, PTD15, PTD16 : RGB LED (GPIO 출력)
//   - PTB0 : CdS용 아날로그 입력 (MUX=0, pull 비활성)
//   - PTC14 : 온보드 POT용 아날로그 입력 (MUX=0, pull 비활성)
//============================================================================
void PORT_init(void)
{
    // RGB LED용 Port D
    PCC_PORTD |= PCC_CGC_MASK;

    PORTD_PCR(0)  &= ~((0b111U<<MUX_BITS));
    PORTD_PCR(0)  |=  (1U<<MUX_BITS);   // MUX=1 → GPIO
    PORTD_PCR(15) &= ~((0b111U<<MUX_BITS));
    PORTD_PCR(15) |=  (1U<<MUX_BITS);
    PORTD_PCR(16) &= ~((0b111U<<MUX_BITS));
    PORTD_PCR(16) |=  (1U<<MUX_BITS);

    GPIOD_PDDR |= (1U<<PTD0) | (1U<<PTD15) | (1U<<PTD16);

    // CdS용 PTB0 (ADC0_SE4) : 아날로그 입력 → MUX=0, Pull 비활성
    PCC_PORTB |= PCC_CGC_MASK;
    PORTB_PCR(0) &= ~((0b111U<<MUX_BITS) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);

    // 온보드 POT용 PTC14 (ADC0_SE12) : 아날로그 입력 → MUX=0, Pull 비활성
    PCC_PORTC |= PCC_CGC_MASK;
    PORTC_PCR(14) &= ~((0b111U<<MUX_BITS) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
}

//============================================================================
// ADC0 초기화
//   - 12비트, SW trigger, single-ended, interrupt 사용 안 함
//============================================================================
void ADC0_init(void)
{
    // 1. ADC0 클럭 선택 & Enable
    PCC_ADC0 &= ~(PCC_CGC_MASK);
    PCC_ADC0 &= ~((0b111U)<<PCS_BITS);
    PCC_ADC0 |=  ((0b001U)<<PCS_BITS);   // PCS = SOSCDIV2 (예: 8MHz/2 등)
    PCC_ADC0 |=  PCC_CGC_MASK;

    // 2. SC1A 초기값: 채널 disable (ADCH=31, 0x1F)
    ADC0_SC1A = 0x1FU;

    // 3. CFG1: 분주=1(기본), 12비트 모드
    ADC0_CFG1 &= ~((0b11U)<<ADC_ADIV_BITS);
    ADC0_CFG1 &= ~((0b11U)<<ADC_MODE_BITS);
    ADC0_CFG1 |=  ((0b01U)<<ADC_MODE_BITS);  // MODE=01 → 12bit

    // 4. CFG2: 샘플 타임 설정
    ADC0_CFG2 &= ~(0xFFU<<ADC_SMPLTS_BITS);
    ADC0_CFG2 |=  (12U<<ADC_SMPLTS_BITS);    // 12 ADC clock 사이 샘플

    // 5. SC2: SW trigger 사용 (ADTRG=0)
    ADC0_SC2 &= ~(1U<<ADC_ADTRG_BIT);

    // SC3는 기본값(연속 변환, HW avg 사용 안 함) 사용
    ADC0_SC3 = 0x0U;
}

//============================================================================
// 지정 채널 1회 변환 후 결과 리턴
//   - ch: 0~31 (ADC0_SEn 채널 번호)
//============================================================================
static uint16_t ADC0_ReadChannel(uint8_t ch)
{
    // 채널 번호(ADCH[4:0])만 변경, 나머지 비트(AIEN, DIFF 등)는 0 유지
    ADC0_SC1A = (ADC0_SC1A & ~0x1FU) | (ch & 0x1FU);

    // 변환 완료(COCO=1)될 때까지 대기
    while ((ADC0_SC1A & (1U << COCO_BIT)) == 0U) {
        // wait
    }

    return (uint16_t)ADC0_RA;   // 0 ~ 4095 (12bit)
}

//============================================================================
// Clock 초기화 (네가 쓰던 그대로)
//============================================================================

void SOSC_init_8MHz(void)
{
    /*!
     * SOSC Initialization (8 MHz):
     * ===================================================
     */
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) |
                   SCG_SOSCDIV_SOSCDIV2(1);    /* Divide by 1 */

    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(2) |       /* Medium frequency range (8MHz~40MHz) */
                   SCG_SOSCCFG_EREFS_MASK;      /* External crystal */

    while (SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK); /* Wait until unlocked */

    SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;     /* Enable system oscillator */

    while (!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)); /* Wait until SOSC is valid */
}

void SPLL_init_160MHz(void)
{
    /*!
     * SPLL Initialization (160 MHz):
     * ===================================================
     */
    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);     /* Ensure SPLL control register is unlocked */
    SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;       /* Disable SPLL before configuration */

    SCG->SPLLDIV |= SCG_SPLLDIV_SPLLDIV1(2) |       /* Divide by 2 */
                    SCG_SPLLDIV_SPLLDIV2(3);        /* Divide by 3 */

    SCG->SPLLCFG = SCG_SPLLCFG_MULT(24);            /* Multiply factor = 24 (160 MHz output) */

    while (SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);     /* Wait until register is unlocked */
    SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;        /* Enable SPLL */

    while (!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); /* Wait until SPLL is valid */
}

void NormalRUNmode_80MHz(void)
{
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1) |
                   SCG_SIRCDIV_SIRCDIV2(1);

    SCG->RCCR = SCG_RCCR_SCS(6) |            /* Select PLL as clock source */
                SCG_RCCR_DIVCORE(0b01) |     /* DIVCORE=1 → divide by 2 → Core clock = 80 MHz */
                SCG_RCCR_DIVBUS(0b01)  |     /* DIVBUS=1 → divide by 2 → Bus clock = 40 MHz */
                SCG_RCCR_DIVSLOW(0b10);      /* DIVSLOW=2 → divide by 3 → SCG slow, flash clock = 26.6 MHz */

    while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) != 6)
    {
        /* Wait until PLL is selected as clock source */
    }
}
