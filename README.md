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