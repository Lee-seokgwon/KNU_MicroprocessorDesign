// ADC로 가변저항 값 읽어와서 LED dimming하는 기능

#include <stdint.h>
#include "S32K144.h" //클럭 생성용 임시 헤더

//============================================================================
// PCC for Port Clock Enable
//============================================================================

// PCC 베이스 주소 (datasheet 623p)
#define PCC_BASE   (0x40065000U)

#define PCC_PORTA       (*(volatile unsigned*)(PCC_BASE + 0x124))
#define PCC_PORTB       (*(volatile unsigned*)(PCC_BASE + 0x128))
#define PCC_PORTC       (*(volatile unsigned*)(PCC_BASE + 0x12C))
#define PCC_PORTD       (*(volatile unsigned*)(PCC_BASE + 0x130))
#define PCC_PORTE       (*(volatile unsigned*)(PCC_BASE + 0x134))

// PORTn clock enable 마스크, (datasheet 656)
#define PCC_CGC_MASK (1U << 30)

//============================================================================
// PCC for Port Clock Enable
//============================================================================




//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO) , PORTn_BASE = PORTn의 PCR Base 주소
//============================================================================

// PORT 베이스 주소 (LMS exel file)
#define PORTA_BASE (0x40049000U)
#define PORTB_BASE (0x4004A000U)
#define PORTC_BASE (0x4004B000U)
#define PORTD_BASE (0x4004C000U)
#define PORTE_BASE (0x4004D000U)

//PORTn PCR (n = Pin number, 핀 마다의 설정)
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

#define PTD0 0
#define PTD15 15
#define PTD16 16

//============================================================================
// PORTn_PCRx MUX setting (Set PORTn Pin x operate as GPIO) , PORTn_BASE = PORTn의 PCR Base 주소
//============================================================================





//============================================================================
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================

//GPIO 베이스 주소 (datasheet 229p)
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
// GPIO BASE (PDDR, PDIR, PDOR, PSOR, PCOR)
//============================================================================





//============================================================================
// NVIC (for interrupt)
//============================================================================

#define NVIC_ISER_BASE   (0xE000E100U)   // Interrupt Set Enable
#define NVIC_ICER_BASE   (0xE000E180U)   // Interrupt Clear Enable
#define NVIC_ISPR_BASE   (0xE000E200U)   // Interrupt Set Pending
#define NVIC_ICPR_BASE   (0xE000E280U)   // Interrupt Clear Pending
#define NVIC_IABR_BASE   (0xE000E300U)   // Interrupt Active Bit
#define NVIC_IPR_BASE    (0xE000E400U)   // Interrupt Priority

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
// NVIC
//============================================================================


//============================================================================
// LPIT Timer
//============================================================================

#define PCC_LPIT        (*(volatile unsigned*)(PCC_BASE + 0xDC))
#define PCS_BITS 24

#define LPIT_BASE (0x40037000)
#define LPIT_MCR (*(volatile unsigned*)(LPIT_BASE + 0x8))
#define LPIT_MSR (*(volatile unsigned*)(LPIT_BASE + 0xC))
#define LPIT_MIER (*(volatile unsigned*)(LPIT_BASE + 0x10))
#define LPIT_TVAL0 (*(volatile unsigned*)(LPIT_BASE + 0x20))
#define LPIT_TCTRL0 (*(volatile unsigned*)(LPIT_BASE + 0x28))

#define M_CEN_BIT 0
#define TIF0_BIT 0
#define TIE0_BIT 0
#define MODE_BITS 2
#define T_EN_BIT 0

//============================================================================
// LPIT Timer
//============================================================================


//============================================================================
// ADC
//============================================================================

#define PCC_ADC0 *((volatile unsigned*)(PCC_BASE + 0xEC))
#define PCC_ADC1 *((volatile unsigned*)(PCC_BASE + 0x9C))

#define ADC0_BASE (0x4003B000)
#define ADC1_BASE (0x40027000)

#define ADC0_SC1A *((volatile unsigned*)(ADC0_BASE + 0x0)) //SC1B SC1C 는 4바이트 간격
#define ADC0_CFG1 *((volatile unsigned*)(ADC0_BASE + 0x40))
#define ADC0_CFG2 *((volatile unsigned*)(ADC0_BASE + 0x44))
#define ADC0_RA *((volatile unsigned*)(ADC0_BASE + 0x48)) //RB RC .. 는 4바이트 간격
#define ADC0_SC2 *((volatile unsigned*)(ADC0_BASE + 0x90))
#define ADC0_SC3 *((volatile unsigned*)(ADC0_BASE + 0x94))

#define ADC1_SC1A *((volatile unsigned*)(ADC1_BASE + 0x0)) //SC1B SC1C .. 는 4바이트 간격
#define ADC1_CFG1 *((volatile unsigned*)(ADC1_BASE + 0x40))
#define ADC1_CFG2 *((volatile unsigned*)(ADC1_BASE + 0x44))
#define ADC1_RA *((volatile unsigned*)(ADC1_BASE + 0x48)) //RB RC .. 는 4바이트 간격
#define ADC1_SC2 *((volatile unsigned*)(ADC1_BASE + 0x90))
#define ADC1_SC3 *((volatile unsigned*)(ADC1_BASE + 0x94))

#define ADCH_BITS 0
#define COCO_BIT 7
#define MODE_BITS 2
#define ADIV_BITS 5
#define SMPLTS_BITS 0
#define ADTRG_BIT 6

#define ADC0_SE12 12

//============================================================================
// ADC
//============================================================================


//============================================================================
// PWM (FTM + PWM) , PWM생성시에는 Flexible Timer 사용해야함
//============================================================================

#define PCC_FTM0 *((volatile unsigned*)(PCC_BASE + 0xE0))

#define FTM0_BASE (0x40038000)
#define FTM0_SC *((volatile unsigned*)(FTM0_BASE + 0x0))
#define FTM0_MOD *((volatile unsigned*)(FTM0_BASE + 0x8))
#define FTM0_C1SC *((volatile unsigned*)(FTM0_BASE + 0x14))
#define FTM0_C1V *((volatile unsigned*)(FTM0_BASE + 0x18))
#define FTM0_CNTIN *((volatile unsigned*)(FTM0_BASE + 0x4C))

#define PWMEN1_BIT 17
#define CLKS_BITS 3
#define PS_BITS 0
#define MSB_BIT 5
#define MSA_BIT 4
#define ELSB_BIT 3
#define ELSA_BIT 2

//============================================================================
// PWM (FTM + PWM) , PWM생성시에는 Flexible Timer 사용해야함
//============================================================================

void SOSC_init_8MHz(void);
void SPLL_init_160MHz(void);
void NormalRUNmode_80MHz(void);
void PORT_init(void);
void ADC0_init(void);
void adc_start(void);
uint32_t read_adc_chx(void);
void FTM0_CH1_PWM(void);

int main(void)
{
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    PORT_init();
    FTM0_CH1_PWM();
    ADC0_init();
    uint32_t adcResult = 0;
    for(;;)
    {
        adc_start();
        adcResult = read_adc_chx();
        FTM0_C1V = (adcResult * 16000) / 4095;
        // PWM 해상도에 맞게 읽어온 현재 위치 값 = (adcresult/전체adc)*PWM전체범위
    }
}

void PORT_init(void)
{
    PCC_PORTD |= PCC_CGC_MASK;
    PORTD_PCR(16) &= ~((0b111)<<MUX_BITS);
    PORTD_PCR(16) |= ((0b010)<<MUX_BITS);
}

void ADC0_init(void)
{
    PCC_ADC0 &= ~(PCC_CGC_MASK);
    PCC_ADC0 &= ~((0b111)<<PCS_BITS);
    PCC_ADC0 |= ((0b001)<<PCS_BITS);
    PCC_ADC0 |= PCC_CGC_MASK;

    ADC0_SC1A |= ((0b111111)<<ADCH_BITS);
    //0번 ADC모듈에 슬롯 추가하고 싶으면 여기서 ADC0_SC1X (X = A~P) 추가.
    //이후 ADC_start에서 슬롯에 채널 할당
    //이후 read adc 함수내부에서 슬롯 마다 결과 읽어서 리턴
    ADC0_CFG1 &= ~((0b11)<<ADIV_BITS);
    ADC0_CFG1 &= ~((0b11)<<MODE_BITS);
    ADC0_CFG1 |= ((0b01)<<MODE_BITS);

    ADC0_CFG2 &= ~(255<<SMPLTS_BITS);
    ADC0_CFG2 |= (12<<SMPLTS_BITS);

    ADC0_SC2 &= ~(1<<ADTRG_BIT);
}

void adc_start(void)
{
    //// 슬롯 A: SE12 (가변저항)
    ADC0_SC1A &= ~((0b1111111)<<ADCH_BITS);
    ADC0_SC1A |= (ADC0_SE12<<ADCH_BITS);
}

uint32_t read_adc_chx(void)
{
    while ((ADC0_SC1A & (1<<COCO_BIT))==0){}

    return ADC0_RA;
}


void FTM0_CH1_PWM(void)
{
    PCC_FTM0 &= PCC_CGC_MASK;
    PCC_FTM0 &= ~((0b111)<<PCS_BITS);
    PCC_FTM0 |= ((0b010)<<PCS_BITS);
    PCC_FTM0 |= PCC_CGC_MASK;

    FTM0_SC |= (1<<PWMEN1_BIT);
    FTM0_SC &= ~((0b111)<<PS_BITS);
    FTM0_SC |= ((0b001)<<PS_BITS);

    FTM0_MOD = 16000 - 1;
    FTM0_CNTIN = 0;

    FTM0_C1SC |= (1<<MSB_BIT);
    FTM0_C1SC |= (1<<ELSA_BIT);

    FTM0_SC |= ((0b11)<<CLKS_BITS);
}

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
