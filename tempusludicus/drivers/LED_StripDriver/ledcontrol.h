/*
 * ledcontrol.h
 *
 * Created: 13/07/2021 17:12:57
 *  Author: william
 */

#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#include <stdint.h>

struct cRGBW {
    uint8_t g;
    uint8_t r;
    uint8_t b;
    uint8_t w;
};

//                         RRGGBBWW
#define COLOR_BLACK      0x00000000
#define COLOR_RED        0xFF000000
#define COLOR_GREEN      0x00FF0000
#define COLOR_BLUE       0x0000FF00
#define COLOR_WHITE      0xFFFFFF00
#define COLOR_CYAN       0x00FFFF00
#define COLOR_MAGENTA    0xFF00FF00
#define COLOR_YELLOW     0xFFFF0000
#define COLOR_CHARTREUSE 0x7FFF0000
#define COLOR_ORANGE     0xFF600000
#define COLOR_AQUAMARINE 0x7FFFD400
#define COLOR_PINK       0xFF5F5F00
#define COLOR_TURQUOISE  0x3FE0C000
#define COLOR_REALWHITE  0xC8FFFFFF
#define COLOR_INDIGO     0x3F007F00
#define COLOR_VIOLET     0xBF7FBF00
#define COLOR_MAROON     0x32001000
#define COLOR_BROWN      0x0E060000
#define COLOR_CRIMSON    0xDC283C00
#define COLOR_PURPLE     0x8C00FF00

void init_strip(void);
void setStrip_ExBounds(uint16_t low, uint16_t high);
void setStrip_Brightness(uint8_t bright);

void setStrip_all(uint32_t color);
void setStrip_part(uint16_t start, uint16_t end, uint32_t color);
void setStrip_pixel(uint16_t pixel, uint32_t color);
void setStrip_clear(void);

void applyBrightness(uint32_t *color, uint8_t _brightness);

void Strip_send(void);

uint32_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val, uint8_t whiteness);
uint32_t color32(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
uint8_t gamma8(uint8_t x);
uint32_t gamma32(uint32_t x);
uint8_t sine8(uint8_t x);

void effect_snake_nb(int length, uint32_t color);

void effect_snakeBounce_b(int length, uint32_t color);
void effect_snakeBounce_nb(int length, uint32_t color);

void effect_chase_b(uint32_t color);

void effect_snakeGrowHue_b(int growSpd, int hueSpd);
void effect_snakeGrowHue_nb(int growSpd, int hueSpd);

void effect_snakeGrow_b(int growSpd, uint32_t color);
void effect_snakeGrow_nb(int growSpd, uint32_t color);

void effect_pulse_b(uint8_t maxBrightness, uint32_t color);

#endif /* LEDCONTROL_H_ */
