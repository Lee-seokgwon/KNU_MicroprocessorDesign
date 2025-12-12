#include "common.h"
#include "pwm.h"
#include "dcmotor.h"

// DC 모터 핀 정의
// PTB10: 전진용 PWM (FTM2_CH0)
// PTB11: 후진용 PWM (FTM2_CH1)
#define MOTOR_FORWARD_PIN 10  // PTB10
#define MOTOR_REVERSE_PIN 11  // PTB11

void DcMotor_init(void)
{
    // PORTB 클럭 활성화
    PCC_PORTB |= (1u << CGC_BIT);

    // PTB10을 FTM2_CH0로 설정 (전진용 PWM)
    PORTB_PCR(MOTOR_FORWARD_PIN) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(MOTOR_FORWARD_PIN) |=  ((0b010) << MUX_BITS);  // ALT2 = FTM2_CH0

    // PTB11을 FTM2_CH1로 설정 (후진용 PWM)
    PORTB_PCR(MOTOR_REVERSE_PIN) &= ~((0b111) << MUX_BITS);
    PORTB_PCR(MOTOR_REVERSE_PIN) |=  ((0b010) << MUX_BITS);  // ALT2 = FTM2_CH1

    // PWM 초기화 (FTM2_CH0와 FTM2_CH1 모두 초기화)
    FTM2_CH0_PWM();
    FTM2_CH1_PWM();
}

void DcMotor_run_forward(uint32_t accel)
{
    // 전진: FTM2_CH0에 PWM 출력, 후진 채널은 0
    FTM2_C0V = accel;  // 전진 PWM
    FTM2_C1V = 0;      // 후진 채널 비활성화
}

void DcMotor_run_reverse(uint32_t accel)
{
    // 후진: FTM2_CH1에 PWM 출력, 전진 채널은 0
    FTM2_C0V = 0;      // 전진 채널 비활성화
    FTM2_C1V = accel;  // 후진 PWM
}

void DcMotor_stop(void)
{
    // 두 채널 모두 0으로 설정
    FTM2_C0V = 0;  // 전진 채널 정지
    FTM2_C1V = 0;  // 후진 채널 정지
}
