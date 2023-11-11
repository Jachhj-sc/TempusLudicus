/**
    @file time.h
    @author William Hak (william.hak57@gmail.com)
    @brief Implemenation for keeping track of time
    @version 0.1
    @date 2023-11-09

    @copyright Copyright (c) 2023

 */
#ifndef TIME_H
#define TIME_H

#include <stdint.h>

int init_sysTick(void);

uint32_t get_millis(void);

#endif
