//----------------------------------------------------------------------------------------------------
// Header:      ��ʱ�� 
// File Name:   Timer.c
// Author:      David
// Date:        2013-05-24
//----------------------------------------------------------------------------------------------------
#define     IN_TIMER
    #include    "stm32f4xx.h"
    #include    "RTL.h"
    #include    "sys_config.h"
    #include    "main.h"
    #include    "led.h"
    #include    "timer.h"
//----------------------------------------------------------------------------------------------------
#define     T1PSC_VAL       (100000)        // Ԥ��Ƶ��Ƶ�ʣ�100K��

#define     FASTER_SEC      ( 12 )          // ָ��ʱ���� �����������ʵ�ʿ�Ϊ��+������Ϊ��-����
#define     TIME            (24*3600)       // ָ��ʱ�䣨�룩3600��һСʱ 

#if     FASTER_SEC < 0      // ���봦��
    #define     ROUND       (-(TIME / 2))
#else
    #define     ROUND       (TIME / 2)
#endif

#define     ARR_BASE_VAL        (CTRL_PERIOD * (T1PSC_VAL/1000))                // �������Զ���װ�ؼ���ֵ(��ʱ���� --> �������� ��λ:mS)

#define     COMP_VAL            ((FASTER_SEC * ARR_BASE_VAL + ROUND) / TIME)    // ��ʱ��������ֵ

#define     TIM1_ARR_REAL_VAL   (ARR_BASE_VAL + COMP_VAL)                       // ʵ�ʵ��Զ���װ�ؼ���ֵ

#if         TIM1_ARR_REAL_VAL > 65535
    #error      "Timer1 Auto-reload Register value overflowed!!!"
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  ��ʱ������   (Timer)
//----------------------------------------------------------------------------------------------------
/*----------------------------------------------------------------------------
  initialize TIM1
 *----------------------------------------------------------------------------*/
void TIM1_Init (void) 
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;             /* enable clock for TIM1    */

    TIM1->CR1   = TIM_CR1_ARPE;                     /* Auto-reload preload enable */
    TIM1->PSC   = (SystemCoreClock/T1PSC_VAL - 1);  /* set prescaler            */
    TIM1->ARR   = TIM1_ARR_REAL_VAL - 1;            /* set auto-reload = ���ƽ��� ms */
    TIM1->RCR   = 0;                                /* set repetition counter   */

    TIM1->DIER = TIM_DIER_UIE;                      /* Update Interrupt enable  */
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);             /* TIM1   Interrupt enable  */

    TIM1->CR1  |= TIM_CR1_CEN;                      /* timer enable             */
}
/*----------------------------------------------------------------------------
  Timer1 Update Interrupt Handler
 *----------------------------------------------------------------------------*/
void TIM1_UP_TIM10_IRQHandler (void) 
{
    if (TIM1->SR & (TIM_SR_UIF)) {                  /* UIF set?                 */
        isr_sem_send (CtrlTick_sem);                /* Send Semphore to CtrlLoop*/
        TIM1->SR &= ~(TIM_SR_UIF);                  /* clear UIF flag           */
    }
}

//----------------------------------------------------------------------------------------------------
//eof
