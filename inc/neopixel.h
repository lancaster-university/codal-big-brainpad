//
// Created by Riven on 2018/10/31.
//

#ifndef CODAL_NEOPIXEL_H
#define CODAL_NEOPIXEL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32.h"
#include "ZPin.h"

#define GPIO_PORT() ((GPIO_TypeDef *)(GPIOA_BASE + 0x400 * ((int)name >> 4)))
#define GPIO_PIN() (1 << ((uint32_t)name & 0xf))

#define TICK1 60
#define TICK0 20

#define NEOPIXEL_SEND_ONE GPIOx->BSRR = PIN; \
        startTime = DWT->CYCCNT; \
        while((DWT->CYCCNT - startTime) < TICK1); \
		GPIOx->BSRR = (uint32_t)PIN << 16U; \
		startTime = DWT->CYCCNT; \
		while((DWT->CYCCNT - startTime) < TICK0); \

#define NEOPIXEL_SEND_ZERO GPIOx->BSRR = PIN; \
        startTime = DWT->CYCCNT; \
        while((DWT->CYCCNT - startTime) < TICK0); \
		GPIOx->BSRR = (uint32_t)PIN << 16U;  \
        startTime = DWT->CYCCNT; \
		while((DWT->CYCCNT - startTime) < TICK1); \

static inline void neopixel_send_buffer(ZPin &pin, const uint8_t *ptr, int numBytes){
    GPIO_TypeDef* GPIOx;
    uint32_t PIN;
    uint32_t name = pin.name;
    GPIOx = GPIO_PORT();
    PIN = GPIO_PIN();
    uint32_t startTime;

    if (!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }

    __disable_irq();
    for (int i = 0; i < numBytes; i++)
    {
        if ((ptr[i] & 128) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}

        if ((ptr[i] & 64) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}

        if ((ptr[i] & 32) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}

        if ((ptr[i] & 16) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}

        if ((ptr[i] & 8) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}

        if ((ptr[i] & 4) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}

        if ((ptr[i] & 2) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}

        if ((ptr[i] & 1) > 0)	{NEOPIXEL_SEND_ONE}
        else	{NEOPIXEL_SEND_ZERO}
    }
    __enable_irq();
}


#endif //CODAL_NEOPIXEL_H
