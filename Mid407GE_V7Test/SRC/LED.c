/*----------------------------------------------------------------------------
 * Name:    LED.c
 * Purpose: Low level LED functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "stm32f4xx.h"                  /* STM32F2xx Definitions              */
#include "Rtl.h"
#include "LED.h"
//-----------------------------------------------------------------------------
// 16-0205 指示灯程序调整，统一到LD2~LD9, LD1为电源指示，不可控
//-----------------------------------------------------------------------------
#define     NUM_LEDS        8           /* Number of LEDs                     */

const uint32_t led_mask[NUM_LEDS] = {  
                                        1UL <<  3,  // Port C
                                        1UL <<  8,                                        
                                        1UL <<  6,  // Port G
                                        1UL <<  7,
                                        1UL <<  8,
                                        1UL << 11,
                                        1UL << 13,
                                        1UL << 14,
                                    };
/*-----------------------------------------------------------------------------
 *      LED_Init:  Initialize LEDs
 *
 * Parameters: (none)
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Init (void) 
{
    U32 i;

    /* Enable clock for GPIOG   */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
    /* Configure Port G pins PG6, PG7, PG8,PG11,PG13,PG14  */
    /* Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  */
    GPIOG->MODER   &= ~((3UL << 2*14)|(3UL << 2*13)|(3UL << 2*11)|(3UL << 2*6)|(3UL << 2*7)|(3UL << 2*8));
    GPIOG->MODER   |=  ((1UL << 2*14)|(1UL << 2*13)|(1UL << 2*11)|(1UL << 2*6)|(1UL << 2*7)|(1UL << 2*8));
    GPIOG->OTYPER  &= ~((1UL <<   14)|(1UL <<   13)|(1UL <<   11)|(1UL <<   6)|(1UL <<   7)|(1UL <<   8));
    GPIOG->OSPEEDR &= ~((3UL << 2*14)|(3UL << 2*13)|(3UL << 2*11)|(3UL << 2*6)|(3UL << 2*7)|(3UL << 2*8));
    GPIOG->OSPEEDR |=  ((2UL << 2*14)|(2UL << 2*13)|(2UL << 2*11)|(2UL << 2*6)|(2UL << 2*7)|(2UL << 2*8));
    GPIOG->PUPDR   &= ~((3UL << 2*14)|(3UL << 2*13)|(3UL << 2*11)|(3UL << 2*6)|(3UL << 2*7)|(3UL << 2*8));
    GPIOG->PUPDR   |=  ((1UL << 2*14)|(1UL << 2*13)|(1UL << 2*11)|(1UL << 2*6)|(1UL << 2*7)|(1UL << 2*8));

    /* Enable clock for GPIOC   */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    /* Configure Port C pins PC3, PC8    */
    /* Pins in Push-pull output mode, 50 MHz Fast Speed with Pull-up resistors  */
    GPIOC->MODER   &= ~((3UL << 2*3)|(3UL << 2*8));
    GPIOC->MODER   |=  ((1UL << 2*3)|(1UL << 2*8));
    GPIOC->OTYPER  &= ~((1UL <<   3)|(1UL <<   8));
    GPIOC->OSPEEDR &= ~((3UL << 2*3)|(3UL << 2*8));
    GPIOC->OSPEEDR |=  ((2UL << 2*3)|(2UL << 2*8));
    GPIOC->PUPDR   &= ~((3UL << 2*3)|(3UL << 2*8));
    GPIOC->PUPDR   |=  ((1UL << 2*3)|(1UL << 2*8));
    // 关灯
    for (i=0; i<NUM_LEDS; i++) {
        LED_Off(i);
    }
}

/*-----------------------------------------------------------------------------
 *      LED_On: Turns on requested LED
 *
 * Parameters:  num - LED number
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_On (U32 num) 
{
    if (num < 2) {                      // 0~1 -> LD2~3 
        GPIOC->BSRRH = led_mask[num];   // 寄存器高半字复位reset，点亮
    } else if (num < NUM_LEDS) {        // 2~7 -> LD4~9
        GPIOG->BSRRH = led_mask[num];   // 寄存器高半字复位reset，点亮
    }
}

/*-----------------------------------------------------------------------------
 *       LED_Off: Turns off requested LED
 *
 * Parameters:  num - LED number
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Off (U32 num) 
{
    if (num < 2) {                      // 0~1 -> LD2~3 
        GPIOC->BSRRL = led_mask[num];   // 寄存器低半字置位set，熄灭
    } else if (num < NUM_LEDS) {        // 2~7 -> LD4~9
        GPIOG->BSRRL = led_mask[num];   // 寄存器低半字置位set，熄灭
    }
}

//----------------------------------------------------------------------------------------------------
//  LED-0~7 对应PCB丝印 LD-2~9
//----------------------------------------------------------------------------------------------------
void LED_Flash (U32 led_no) 
{
    static U32 stat[8] ={0,0,0,0,0,0,0,0};

    if(led_no < NUM_LEDS) {
        if (stat[led_no]) {
            LED_On (led_no);
            stat[led_no] = 0;
        } else {
            LED_Off(led_no);
            stat[led_no] = 1;
        }
    }
}
//-----------------------------------------------------------------------------
void LED_AllOn (void) 
{
    U32 i;
    
    for (i=0; i<NUM_LEDS; i++) {
        LED_On(i);
    }
}
/*-----------------------------------------------------------------------------
 * End of file
 *----------------------------------------------------------------------------*/
