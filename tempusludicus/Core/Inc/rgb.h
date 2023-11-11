/**
    @file rgb.h
    @author William Hak (william.hak57@gmail.com)
    @brief Implementation of rgb led on the mlk25z
    @version 0.1
    @date 2023-09-28

    @copyright Copyright (c) 2023

 */
#ifndef RGB_H
#define RGB_H

#include <MKL25Z4.h>

#define LED_ON  0
#define LED_OFF 1

#define RED_LED       GPIOB
#define RED_LED_PORT  PORTB
#define RED_LED_SHIFT 18
#define RED_LED_MASK  (1 << RED_LED_SHIFT)

#define GREEN_LED       GPIOB
#define GREEN_LED_PORT  PORTB
#define GREEN_LED_SHIFT 19
#define GREEN_LED_MASK  (1 << GREEN_LED_SHIFT)

#define BLUE_LED       GPIOD
#define BLUE_LED_PORT  PORTD
#define BLUE_LED_SHIFT 1
#define BLUE_LED_MASK  (1 << BLUE_LED_SHIFT)

void init_rgb(void);
void set_rgb(int r, int g, int b);

#endif
