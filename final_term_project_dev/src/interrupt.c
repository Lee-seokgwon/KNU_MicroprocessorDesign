#include "common.h"
#include "interrupt.h"
#include "sequential_turn_light.h" //extern 변수 참조하기 위함

void NVIC_init_IRQs(void)
{
    NVIC_ICPR1 |= (1 << (61 % 32));
    NVIC_ISER1 |= (1 << (61 % 32));
    NVIC_IPR61 = 5;

    //piezo engine timer용 인터럽트 (lpit 채널0 인터럽트)
    NVIC_ICPR1 |= (1 << (48 - 32));
    NVIC_ISER1 |= (1 << (48 - 32));
    NVIC_IPR48 = 15;   // 우선순위 스위치보다 낮게
}

void PORTC_IRQHandler(void)
{
    if (PORTC_PCR(PTC12) & (1 << ISF_BIT))
    {
        PORTC_PCR(PTC12) |= (1 << ISF_BIT);

        if ((GPIOC_PDIR & (1 << PTC12)) == 0)
        {
            g_left_request = true;
        }
    }

    if (PORTC_PCR(PTC13) & (1 << ISF_BIT))
    {
        PORTC_PCR(PTC13) |= (1 << ISF_BIT);

        if ((GPIOC_PDIR & (1 << PTC13)) == 0)
        {
            g_right_request = true;
        }
    }
}
