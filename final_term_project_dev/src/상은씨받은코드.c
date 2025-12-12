#include "S32K144.h"
#include "common.h"

void DHT11_init(void)
{
    PCC_PORTB |= (1<<CGC_BIT);
    PORTB_PCR2 &= ~((0b111)<<MUX_BITS);
    PORTB_PCR2 |= (1<<MUX_BITS);

    GPIOB_PDDR |= (1<<PTB2);
    GPIOB_PSOR |= (1<<PTB2);

    PCC_PORTD |= (1<<CGC_BIT);
      PORTD_PCR16 &= ~((0b111)<<MUX_BITS);
      PORTD_PCR16 |= 1<<MUX_BITS;

      GPIOD_PDDR |= (1<<PTD16);
}

// MCU가 센서에 데이터 요청 신호
void DHT11_StartSignal(void)
{
   GPIOB_PDDR |= (1<<PTB2);    // PB2을 output으로 설정

   GPIOB_PCOR = (1<<PTB2);    // LOW를 18ms 이상 유지
   volatile int t = 228000;
   while (t--);

   GPIOB_PSOR = (1<<PTB2);      // HIGH로 설정 뒤
   GPIOB_PDDR &= ~(1<<PTB2);    // 바로 INPUT으로 설정
}

uint8_t DHT11_CheckResponse(void)
{
   // LOW 대기 (센서가 80µs 동안 LOW 신호 전송)
   while (!(GPIOB_PDIR & (1<<PTB2)));

   // HIGH 대기 (센서가 80µs 동안 HIGH 신호 전송)
   while ((GPIOB_PDIR & (1<<PTB2)));

   return 1; // 정상적으로 응답 신호 확인
}

uint32_t DHT11_ReadData(void)
{
   uint32_t data = 0;

   for (int i = 0; i < 8; i++) {// 8bit씩 끊어서 읽기
      uint32_t cnt = 0;

      while (!(GPIOB_PDIR & (1<<PTB2))); // LOW 대기

      while ((GPIOB_PDIR & (1<<PTB2))) { //HIGH 시간 count
         cnt++;
      }

      if (cnt > 250) {
         data |= (1 << (7 - i)); // 1비트 데이터 저장
      }
   }

   return data;
}

void DHT11_GetData(uint32_t *humidity, uint32_t *temperature)
{
   uint32_t humi_high, humi_low, temp_high, temp_low, checksum;
   DHT11_StartSignal(); // start
   DHT11_CheckResponse(); // response

   //response를 한번 하니 읽어들인 data가 1bit씩 밀려서 response를 2번함
   while (!(GPIOB_PDIR & (1<<PTB2)));
   while ((GPIOB_PDIR & (1<<PTB2)));

   humi_high = DHT11_ReadData(); // 습도 정수 값
   humi_low = DHT11_ReadData(); // 습도 소수 값
   temp_high = DHT11_ReadData(); // 온도 정수 값
   temp_low = DHT11_ReadData(); // 온도 소수 값
   checksum = DHT11_ReadData(); // 모두 다 더한 checksum

   if (checksum == (humi_high + humi_low + temp_high + temp_low)) {
      *humidity = humi_high;    // 습도 상위 바이트
      *temperature = temp_high; // 온도 상위 바이트
      return;
   }

   *humidity = 0; // 센서 응답 실패 시 초기화
   *temperature = 0;
}


int main(void) {
   uint32_t hum = 0;
   uint32_t temp = 0;

   SOSC_init_8MHz();
   SPLL_init_160MHz();
   NormalRUNmode_80MHz();

   DHT11_init();

    for (;;) {
       DHT11_GetData(&hum, &temp);


       volatile uint32_t del = 0;
       while(del < 1000000) {
          del++;
       }
       //GPIOD_PTOR |= (1<<PTD16);


       if (temp > 26) {
          GPIOD_PSOR |= (1<<PTD16);
       }
       else {
          GPIOD_PCOR |= (1<<PTD16);
       }


    }
    return 0;
}
