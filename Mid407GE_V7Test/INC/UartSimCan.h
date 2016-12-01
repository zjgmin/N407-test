/*-----------------------------------------------------------------------------
 * Name:    UartSimCan.h
 * Purpose: Serial I/O definitions
 *-----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef __UARTSIMCAN_H
    #define __UARTSIMCAN_H
    
        extern void     SimCan_Init (unsigned int);
        extern void     Uart1SimCan_Init (void);
        extern void     Uart3SimCan_Init (void);

#endif  /* __UARTSIMCAN_H */
