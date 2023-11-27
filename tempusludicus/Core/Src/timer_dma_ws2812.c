#include "timer_dma_ws2812.h"
#include "common.h"
#include "sk6812_config.h"
#include <MKL25Z4.h>

typedef struct {
    uint32_t T0H;
    uint32_t T0L;
    uint32_t T1H;
    uint32_t T1L;
    uint32_t TTOT;
    uint32_t RES;
} sk6812_timing_t;

sk6812_timing_t sk6812_TPM0_timing;

/* 	rgb / rgbw data needs to be encoded like the following
    1st pixel:
    0x00GGRRBB encoded as 24 * uint8_t time vars each holding a CnV value
    which is max 33u for 48 mhz and ps 0 period 700us

    uint8_t arr[pixel][bit] = CnV value;

 */

#if defined(RGB)
uint8_t sk6812_dma_buffer[LEDPIXELCOUNT][24];
#else if defined(RGBW)
uint8_t sk6812_dma_buffer[LEDPIXELCOUNT][32];
#endif

// 24 20 15 — PTE24 TPM0_CH0
// 48 Mhz
void init_ws2812()
{
    // enable clocks for port, dma, timer
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    // set pin to TPM0_CH0
    PORTE->PCR[24] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[24] |= PORT_PCR_MUX(3);

    // init dma
    DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_TRIG_MASK | DMAMUX_CHCFG_SOURCE(24); // 24 TPM0 Channel 0
    

    DMA0->DMA[0].SAR = (uint32_t)sk6812_dma_buffer;
    DMA0->DMA[0].DAR = (uint32_t)&TPM0->CONTROLS[0].CnV;
    DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(24 * LEDPIXELCOUNT);

    DMA_DCR0 |= DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK | DMA_DCR_SSIZE(1) | DMA_DCR_DSIZE(1) |
                DMA_DCR_SINC_MASK;
				
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK;

    // set timer count to 0
    TPM0->CNT = 0;
    /*
        set timer psc and cnt
        Frequency = 48Mhz
        ps = 0
        bit len = 1.2 us
     */
    TPM0->SC |= TPM_SC_PS(0);
    TPM0->MOD = (F_CPU / 1000000 * sk6812_TTOT) - 1u;

    sk6812_TPM0_timing.T0H = (F_CPU * sk6812_T0H) / 1000000000u;
    sk6812_TPM0_timing.T0L = (F_CPU * sk6812_T0L) / 1000000000u;
    sk6812_TPM0_timing.T1H = (F_CPU * sk6812_T1H) / 1000000000u;
    sk6812_TPM0_timing.T1L = (F_CPU * sk6812_T1L) / 1000000000u;
    sk6812_TPM0_timing.TTOT = (F_CPU * sk6812_TTOT) / 1000000000u;
    sk6812_TPM0_timing.RES = (F_CPU * sk6812_RES) / 1000000000u;

    // mode Edge-aligned PWM (high low) CH0
    TPM0_C0SC |= TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK;

    // example for a transmitting 0 or 1
    //    TPM0_C0V = sk6812_TPM0_timing.T0H - 1;
    //    TPM0_C0V = sk6812_TPM0_timing.T1H - 1;

    TPM0->SC |= TPM_SC_CMOD_MASK; // enable timer clock

    // enable dma transfers
    TPM0->SC |= TPM_SC_DMA_MASK;
    TPM0_C0SC |= TPM_CnSC_DMA_MASK; // dma transfer request on tpm (mod = cnt)
}
