2025 경북대 전자공학부 마이크로프로세서설계실험
(nxp s32k144 EVB) (URE_1040 Cortex-M4F/M0+ EDU KIT)

2025 경북대 전자공학부 마이크로프로세서설계실험
(nxp s32k144 EVB) (URE_1040 Cortex-M4F/M0+ EDU KIT)

브랜치 변경
git checkout son

자주쓰는 명령어 // -b는 생성까지 해줌줌
git checkout -b son


-b로 새 브랜치 생성해준다

git push origin son


좌깜/우깜
하드웨어 핀 
예전: PTB0~PTB3 사용
GPIOB_PDOR = (GPIOB_PDOR & ~0x0F) | (seq & 0x0F);
현재: PTB4~PTB7 사용

PTC12,13  좌깜우깜용 버튼


// (내부 음계는 노래 정해지면 추후 수정)
void piezo_playMelody(const char* song)

여기서 노래 AA, BB , CC 다 짜줌


내가 한것
좌깜우깜코드 main에 구현



CDS 센서 연결 포트:
포트: PTC17


피에조 부저 연결 포트:
포트: PTD16 (Port D



처음에 합친이후로 CDS가 검증이 되지 않았다. 이걸 직접 검증하였다. 


cds 채널이 잘못되어있어서 다시 ADC 채널15인데 12로 잘못해놨었다. 수정완료
common.h에


온습도 센서 
DHT11 디지털 온습도 




건드리면 안되는핀
PTA 0~3,
PTB 0~3
PTC 0~2, 12, 13
PTD 0~7, 10, 11, 16

수정해야될것
초음파 PTD10 -> PTD9
서보모터 PTD10 -> PTD8
스탭모터 PTB0~3 -> PTB4~7
CDS센서    PTB0 -> PTC17



온습도 DHT11은 PTD12사용예정

1211 진행상황
인터넷에 도는 dht11.cpp / dht11.h 아두이노 헤더 , 
https://m.blog.naver.com/icfactory/221441750133
이거참고함

코드 들고와서
humid.h 
humid.c
생성성


10:46 : LCD에


LCD핀
데이터 핀:
- D0~D3: PTA0, PTA1, PTA2, PTA3
- D4~D7: PTB0, PTB1, PTB2, PTB3
제어 핀:
- RS, RW, EN: PTC0, PTC1, PTC2


J4에 PTA6 / PTE11 / PTA2 를 처음에 써서 J1의 PTA2로 옮겼더니 잘된다

https://github.com/adafruit/DHT-sensor-library
데이터 센서 라이브러리


DTH11 스펙

Ultra low cost
3 to 5V power and I/O
2.5mA max current use during conversion (while requesting data)
Good for 20-80% humidity readings with 5% accuracy
Good for 0-50°C temperature readings ±2°C accuracy
No more than 1 Hz sampling rate (once every second)
Body size 15.5mm x 12mm x 5.5mm
4 pins with 0.1" spacing


데이터시트는 5kΩ 정도 외부 풀업 + 100 nF 디커플링 캡 추천


common.h에 #define PTD12 12 추가해줘


기존의 숫자 올리는 코드들이나 이런걸 pruning가지치기해서 7segment가 포함된, clock.c  clock.h만들어줌


시계, 습도센서 무조건 합치기, 초음파 


오늘 시계 습도 같이 도는거 무조거 ㄴ되겧가ㅣ 