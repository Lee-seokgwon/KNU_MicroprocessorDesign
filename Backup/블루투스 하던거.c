
#define PTB0 0
#define PTB1 1
#define PTB2 2
#define PTB3 3
#define PTB4 3
#define PTB5 3
#define PTB6 3
#define PTB7 3
이거 위처럼 되어있어서 아래처럼 수정함. 
#define PTB0 0
#define PTB1 1
#define PTB2 2
#define PTB3 3
#define PTB4 4
#define PTB5 5
#define PTB6 6
#define PTB7 7



추가한 내용.
final_term_project_dev\src\piezo_buzzer.c
여기에 delay로 소리 내주는 코드 추가함. 









#include "S32K144.h"

// PCC Registers
#define PCC_BASE       (0x40065000)

#define PCS_BITS       24
#define CGC_BIT        30
#define MUX_BITS       8

//============================================================================
// LPUART1 (블루투스 HC-06용) - PTC6 (RX), PTC7 (TX)
//============================================================================

#define PCC_LPUART1   *((volatile unsigned*)(PCC_BASE + 0x1AC))  // PCC[107]

#define LPUART1_BASE  (0x4006B000)
#define LPUART1_BAUD  *((volatile unsigned*)(LPUART1_BASE + 0x10))
#define LPUART1_CTRL  *((volatile unsigned*)(LPUART1_BASE + 0x18))
#define LPUART1_DATA  *((volatile unsigned*)(LPUART1_BASE + 0x1C))
#define LPUART1_STAT  *((volatile unsigned*)(LPUART1_BASE + 0x14))
#define LPUART1_GLOBAL *((volatile unsigned*)(LPUART1_BASE + 0x08))

// PORT C 레지스터
#define PORTC_BASE    (0x4004B000)
#define PORTC_PCR6    *((volatile unsigned*)(PORTC_BASE + 0x18))  // PTC6 (RX)
#define PORTC_PCR7    *((volatile unsigned*)(PORTC_BASE + 0x1C))  // PTC7 (TX)
#define PCC_PORTC     *((volatile unsigned*)(PCC_BASE + 0x12C))   // PCC[75]

// LPUART 상태 비트
#define TDRE_BIT      23
#define RDRF_BIT      21

void LPUART1_init(void)
{
    // PORT C 클럭 활성화
    PCC_PORTC |= (1 << CGC_BIT);

    // PTC7 (TX) 설정: MUX = 010 (ALT2 = LPUART1_TX)
    PORTC_PCR7 &= ~((0b111) << MUX_BITS);
    PORTC_PCR7 |=  ((0b010) << MUX_BITS);

    // PTC6 (RX) 설정: MUX = 010 (ALT2 = LPUART1_RX)
    PORTC_PCR6 &= ~((0b111) << MUX_BITS);
    PORTC_PCR6 |=  ((0b010) << MUX_BITS);

    // LPUART1 클럭 설정
    PCC_LPUART1 &= ~(1 << CGC_BIT);
    PCC_LPUART1 &= ~((0b111) << PCS_BITS);
    PCC_LPUART1 |=  ((0b001) << PCS_BITS);  // SIRCDIV1_CLK
    PCC_LPUART1 |=  (1 << CGC_BIT);

    // LPUART1 리셋
    LPUART1_GLOBAL |= (1 << 1);
    while (LPUART1_GLOBAL & (1 << 1));

    // 보드레이트 설정 (9600 bps)
    LPUART1_BAUD = (15 << 24) | (25 << 0);

    // TX, RX 활성화
    LPUART1_CTRL = (1 << 19) | (1 << 18);
}

void LPUART1_send_char(char c)
{
    while ((LPUART1_STAT & (1 << TDRE_BIT)) == 0);
    LPUART1_DATA = (unsigned char)c;
}

void LPUART1_send_string(const char *str)
{
    while (*str) {
        LPUART1_send_char(*str++);
    }
}

char LPUART1_receive_char(void)
{
    while ((LPUART1_STAT & (1 << RDRF_BIT)) == 0);
    return (char)(LPUART1_DATA & 0xFF);
}

uint8_t LPUART1_is_data_ready(void)
{
    return (LPUART1_STAT & (1 << RDRF_BIT)) != 0;
}

//============================================================================
// 클럭 초기화
//============================================================================

void SOSC_init_8MHz(void)
{
   SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) | SCG_SOSCDIV_SOSCDIV2(1);
   SCG->SOSCCFG = SCG_SOSCCFG_RANGE(2) | SCG_SOSCCFG_EREFS_MASK;
   while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK);
   SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;
   while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));
}

void SPLL_init_160MHz(void)
{
   while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);
   SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
   SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(2) | SCG_SPLLDIV_SPLLDIV2(3);
   SCG->SPLLCFG = SCG_SPLLCFG_MULT(24);
   while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);
   SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
   while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK));
}

void NormalRUNmod_80MHz(void)
{
   SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(1) | SCG_SIRCDIV_SIRCDIV2(1);
   SCG->RCCR = SCG_RCCR_SCS(6) | SCG_RCCR_DIVCORE(0b01) |
               SCG_RCCR_DIVBUS(0b01) | SCG_RCCR_DIVSLOW(0b10);
   while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT ) != 6) {}
}

static void delay(volatile unsigned long t)
{
    while (t--) {
        __asm("nop");
    }
}

//============================================================================
// 메인 함수 - 블루투스 연결 확인용
//============================================================================

int main(void)
{
    char received_char;
    uint32_t counter = 0;

    // 클럭 초기화
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmod_80MHz();

    // 블루투스 초기화
    LPUART1_init();

    // 초기화 완료 메시지
    LPUART1_send_string("\r\n=== HC-06 Bluetooth Test ===\r\n");
    LPUART1_send_string("Connection OK!\r\n");
    LPUART1_send_string("Send any character to echo back\r\n");
    LPUART1_send_string("--------------------------------\r\n");

    for(;;)
    {
        // 데이터 수신 확인
        if (LPUART1_is_data_ready()) {
            received_char = LPUART1_receive_char();

            // 에코 백
            LPUART1_send_string("Echo: ");
            LPUART1_send_char(received_char);
            LPUART1_send_string("\r\n");
        }

        // 주기적으로 상태 메시지 전송 (선택사항)
        counter++;
        if (counter == 1000000) {  // 약간의 시간 후
            LPUART1_send_string("Bluetooth is working! Counter: ");
            // 간단한 카운터 표시는 생략 (복잡하므로)
            counter = 0;
        }

        delay(10000);
    }
}
