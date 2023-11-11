/**
    @file time.c
    @author William Hak (william.hak57@gmail.com)
    @brief Implemenation for keeping track of time
    @version 0.1
    @date 2023-11-09

    @copyright Copyright (c) 2023

 */
#include "sysTick.h"
#include "main.h"

static uint32_t msTicks = 0;

int init_sysTick()
{
    return SysTick_Config(DEFAULT_SYSTEM_CLOCK / 1000);
}

void SysTick_Handler(void)
{              /* SysTick interrupt Handler. */
    msTicks++; /* See startup file startup_LPC17xx.s for SysTick vector */
}

uint32_t get_millis()
{
    return msTicks;
}
