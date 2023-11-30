#include "timer_dma_ws2812.h"
#include <MKL25Z4.h>

#if defined(RGB)
static uint8_t STRIP_dma_buffer[LEDPIXELCOUNT][24];
#define bitsAmount 24
#elif defined(RGBW)
static uint8_t STRIP_dma_buffer[LEDPIXELCOUNT][32];
#define bitsAmount 32
#endif

typedef struct {
    uint8_t T0H;
    uint8_t T0L;
    uint8_t T1H;
    uint8_t T1L;
    uint8_t TTOT;
    uint32_t RES;
} STRIP_timing_t;

static STRIP_timing_t STRIP_TPM0_timing;

/* 	rgb / rgbw data needs to be encoded like the following
    1st pixel:
    0x00GGRRBB encoded as 24 * uint8_t time vars each holding a CnV value
    which is max 33u for 48 mhz and ps 0 period 700us

    uint8_t arr[pixel][bit] = CnV value;

 */
static const uint8_t zero = 0;
static const uint8_t one = 1;

static uint8_t enableTransfer = 1;

// 24 20 15 — PTE24 TPM0_CH0
// 48 Mhz
void init_ws2812()
{
    // enable clocks for port, dma, timer
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    // set pin to TPM0_CH0
    PORTE->PCR[30] |= PORT_PCR_MUX(3); // PTE30 TPM0_CH3

    // init dma
    DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_TRIG_MASK | DMAMUX_CHCFG_SOURCE(27); // 27 = TPM0 Channel 3
    /*
       enable interrupt on complete | enable peripheral request | cycle steal (single transfer mode) |
       source size 1 byte | destination size 1 byte | source address increments
    */
    DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK | DMA_DCR_SSIZE(1) | DMA_DCR_DSIZE(1) |
                       DMA_DCR_SINC_MASK;
    DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;

    // set timer count to 0
    TPM0->CNT = 0;
    /*
        set timer psc and cnt
        Frequency = 48Mhz
        ps = 0
        bit len = 1.2 us
     */
    TPM0->SC |= TPM_SC_PS(0);
    TPM0->MOD = (F_CPU / 1000000 * STRIP_TTOT) - 1u;

    STRIP_TPM0_timing.T0H = ((F_CPU * STRIP_T0H) / 1000000000u) - 1;
    STRIP_TPM0_timing.T0L = ((F_CPU * STRIP_T0L) / 1000000000u) - 1;
    STRIP_TPM0_timing.T1H = ((F_CPU * STRIP_T1H) / 1000000000u) - 1;
    STRIP_TPM0_timing.T1L = ((F_CPU * STRIP_T1L) / 1000000000u) - 1;
    STRIP_TPM0_timing.TTOT = ((F_CPU * STRIP_TTOT) / 1000000000u) - 1;
    STRIP_TPM0_timing.RES = ((F_CPU * STRIP_RES) / 1000000000u) - 1;

    // mode Edge-aligned PWM (high low) CH0
    TPM0->CONTROLS[3].CnSC |= TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK;

    // example for a transmitting 0 or 1
    //    TPM0->CONTROLS[0].CnV = STRIP_TPM0_timing.T0H - 1;
    //    TPM0->CONTROLS[0].CnV = STRIP_TPM0_timing.T1H - 1;

    // TPM0->SC |= TPM_SC_CMOD_MASK; // enable timer clock
    TPM0->SC &= ~TPM_SC_CMOD_MASK; // disable timer clock

    // enable dma transfers
    TPM0->SC |= TPM_SC_DMA_MASK;
    TPM0->CONTROLS[3].CnSC |= TPM_CnSC_DMA_MASK; // dma transfer request on tov (mod = cnt)
}

void loadStrip_pixel(uint32_t pixel, uint32_t rgb)
{
    if (pixel >= LEDPIXELCOUNT)
        return;

    // load rgb value into dma buffer
    for (uint8_t bit = 0; bit < bitsAmount; bit++) {
        if (rgb & (1 << ((bitsAmount-1) - bit))) {
            STRIP_dma_buffer[pixel][bit] = STRIP_TPM0_timing.T1H;
        } else {
            STRIP_dma_buffer[pixel][bit] = STRIP_TPM0_timing.T0H;
        }
    }
}

void strip_sendContinuous()
{
    if (!(TPM0->SC & TPM_SC_CMOD_MASK)) {
        DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;
        DMA0->DMA[0].SAR = (uint32_t)STRIP_dma_buffer;
        DMA0->DMA[0].DAR = (uint32_t)&TPM0->CONTROLS[3].CnV;
        DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(bitsAmount * LEDPIXELCOUNT);
        TPM0->SC |= TPM_SC_CMOD_MASK; // enable timer clock
    }
    enableTransfer = 1;
}

void strip_write()
{
    if (!(TPM0->SC & TPM_SC_CMOD_MASK)) {
        DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;
        DMA0->DMA[0].SAR = (uint32_t)STRIP_dma_buffer;
        DMA0->DMA[0].DAR = (uint32_t)&TPM0->CONTROLS[3].CnV;
        DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(bitsAmount * LEDPIXELCOUNT);

        // Start first dma transfer to load first CnV value
        DMA0->DMA[0].DCR |= DMA_DCR_START_MASK;
    }
    TPM0->SC |= TPM_SC_CMOD_MASK; // enable timer clock
    enableTransfer = 0;
}

void stopStripDataTransfer()
{
    // set stop transfer Bit
    enableTransfer = 0;
}

void ws2812_setleds(struct cRGBW *ledarray, const uint16_t num_leds)
{
#ifdef RGB
    for (uint16_t i = 0; i < num_leds && i < LEDPIXELCOUNT; i++) {
        loadStrip_pixel(i,
                        (uint32_t)(((uint32_t)ledarray[i].r << 8 * 2) | ((uint32_t)ledarray[i].g << 8 * 1) |
                                   ((uint32_t)ledarray[i].b << 8 * 0)));
    }
    strip_write();
#else
    for (uint16_t i = 0; i < num_leds && i < LEDPIXELCOUNT; i++) {
        loadStrip_pixel(i,
                        (uint32_t)(((uint32_t)ledarray[i].r << 8 * 3) | ((uint32_t)ledarray[i].g << 8 * 2) |
                                   ((uint32_t)ledarray[i].b << 8 * 1) | ((uint32_t)ledarray[i].w << 8 * 0)));
    }
    strip_write();
#endif
}

/*
    interrupt on dma transfer complete.
    set dma to write next x cycles 0. for the reset pulse
    on interrupt check reset cycle done, and setup dma for next transfer
    make this option! (Stop pwm timer until software turns it back on.)
 */
void DMA0_IRQHandler()
{
    static uint8_t DMACh_ResetPulseDone = 0;

    if (DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK) { // DMA data transfer done?
        DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;

        if (enableTransfer == 0) {
            // Stop the transfer of data
#if (STRIP_COUNT == 1)
            TPM0->SC &= ~TPM_SC_CMOD_MASK; // disable timer clock
#else
            // code for multiple strip support on 1 TPM
#endif
        } else if (DMACh_ResetPulseDone == 0) {
            // no reset pulse done yet
            // initiate reset pulse
            DMA0->DMA[0].SAR = (uint32_t)&zero;
            DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(STRIP_RES / STRIP_TTOT);
            DMA0->DMA[0].DCR &= ~DMA_DCR_SINC_MASK; // disable source address increment
            DMACh_ResetPulseDone = 1;
        } else {
            // reset pulse done
            // initiate the next data transfer
            DMA0->DMA[0].SAR = STRIP_dma_buffer;
            DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(bitsAmount * LEDPIXELCOUNT);
            DMA0->DMA[0].DCR |= DMA_DCR_SINC_MASK; // enable source address increment
            DMACh_ResetPulseDone = 0;
        }
    }
}
