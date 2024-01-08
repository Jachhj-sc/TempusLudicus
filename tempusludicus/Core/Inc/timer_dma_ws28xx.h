#ifndef _TIMER_DMA_WS2812_H
#define _TIMER_DMA_WS2812_H

#include "common.h"
#include "ledcontrol.h"
#include "ws2815b_config.h"

void init_ws2812(void);
void loadStrip_pixel(uint32_t pixel, uint32_t rgb);
void strip_sendContinuous(void);
void strip_write(void);
void stopStripDataTransfer(void);
void ws2812_setleds(struct cRGBW *ledarray, const uint16_t num_leds);

void DMA0_IRQHandler(void);
void TPM2_IRQHandler(void);

#endif
