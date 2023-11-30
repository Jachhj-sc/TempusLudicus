#ifndef	_TIMER_DMA_WS2812_H
#define	_TIMER_DMA_WS2812_H

#include "common.h"
#include "ledcontrol.h"
#include "sk6812_config.h"

void init_ws2812();
void loadStrip_pixel(uint32_t pixel, uint32_t rgb);
void strip_sendContinuous();
void strip_write();
void stopStripDataTransfer();
void ws2812_setleds(struct cRGBW *ledarray, const uint16_t num_leds);

#endif