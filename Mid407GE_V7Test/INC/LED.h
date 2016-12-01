/*----------------------------------------------------------------------------
 * Name:    LED.h
 * Purpose: Low level LED definitions
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

#ifndef __LED_H
    #define __LED_H
#include "rtl.h"
//----------------------------------------------------------------------------------------------------
//#define       LD1     // 电源指示灯
enum {LD2 = 0, LD3, LD4, LD5, LD6, LD7, LD8, LD9};    // PC口,PG口
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    extern "C" 
    {
#endif
//----------------------------------------------------------------------------------------------------

extern  void    LED_Init        (void);
//extern  void    LED_UnInit      (void);
extern  void    LED_On          (U32);
extern  void    LED_Off         (U32);
//extern  void    LED_Val         (U32);
extern  U32     LED_Num         (void);
extern  void    LED_out         (U32);
extern  void    LED_Flash       (U32);

//extern  void    Test_LED_On     (U32);
//extern  void    Test_LED_Off    (U32);
//extern  void    Test_LED_Flash  (U32);
extern  void    LED_AllOn       (void) ;
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------

#endif /* __LED_H */
