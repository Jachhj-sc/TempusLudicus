#include "timer_dma_ws2812.h"
#include <MKL25Z4.h>

// +1 is to define one pixel extra that will always hold zero. this is needed for the reset pulse
#if defined(RGB)
static volatile uint8_t STRIP_dma_buffer[LEDPIXELCOUNT + 1][24];
#define bitsAmount 24
#elif defined(RGBW)
static volatile uint8_t STRIP_dma_buffer[LEDPIXELCOUNT + 1][32];
#define bitsAmount 32
#endif

static volatile uint16_t currentBitTimingBuffer = 0u;

typedef struct {
    uint32_t T0H;
    uint32_t T0L;
    uint32_t T1H;
    uint32_t T1L;
    uint32_t TTOT;
    uint32_t RES;
} STRIP_timing_t;

static STRIP_timing_t STRIP_timing_chart;

/* 	rgb / rgbw data needs to be encoded like the following
    1st pixel:
    0x00GGRRBB encoded as 24 * uint8_t time vars each holding a CnV value
    which is max 33u for 48 mhz and ps 0 period 700us

    uint8_t arr[pixel][bit] = CnV value;

 */
static volatile uint8_t DMA_Frame_Update_Done = 1;
static volatile uint8_t StripUpdateContinuously = 0;

void conf_dma(void)
{
    // Configure DMA0 channels
    DMA0->DMA[0].SAR = (uint32_t)(STRIP_dma_buffer); // Source address
    DMA0->DMA[0].DAR = (uint32_t)(&currentBitTimingBuffer);
    DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR((LEDPIXELCOUNT * bitsAmount + 1) * sizeof(uint8_t)); // 1 byte transfer
    DMA0->DMA[0].DCR = DMA_DCR_D_REQ_MASK | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK | DMA_DCR_SSIZE(1) | DMA_DCR_DSIZE(1) |
                       DMA_DCR_SINC_MASK | DMA_DCR_EINT_MASK | DMA_DCR_LINKCC(2) | DMA_DCR_LCH1(1);

    DMA0->DMA[1].SAR = (uint32_t)(&currentBitTimingBuffer); // Source address
    DMA0->DMA[1].DAR = (uint32_t)(&TPM2->CONTROLS[0].CnV);
    DMA0->DMA[1].DSR_BCR = DMA_DSR_BCR_BCR((LEDPIXELCOUNT * bitsAmount + 1) * sizeof(uint16_t)); // 4 bytes transfer
    DMA0->DMA[1].DCR = DMA_DCR_D_REQ_MASK | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK | DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2);

    TPM2->CONTROLS[0].CnV = 0;
}

void init_ws2812()
{
    // enable clocks for port, dma, timer
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // Use MCGFLLCLK as TPM clock source

    // set pin to TPM2_CH0
    PORTB->PCR[2] |= PORT_PCR_MUX(3); // PTB2 TPM2_CH0

    DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(56); // TPM2 OVF

    conf_dma();
    NVIC_EnableIRQ(DMA0_IRQn);

    /*
        set timer psc and cnt
        Frequency = 48Mhz
        ps = 0
        bit len = 1.2 us
     */
    TPM2->SC &= ~TPM_SC_CMOD_MASK;                                 // disable timer clock
    TPM2->SC |= TPM_SC_PS(0) | TPM_SC_DMA_MASK | TPM_SC_TOIE_MASK; // enable dma transfers and toie on overflow flag
    TPM2->MOD = (uint16_t)(((F_CPU / 1000000u) * STRIP_TTOT) / 1000) - 1u;

    // mode Edge-aligned PWM (high low) CH0 and enable dma transfers
    TPM2->CONTROLS[0].CnSC |= TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_DMA_MASK;
    TPM2->CONTROLS[0].CnV = STRIP_timing_chart.T0H;

    NVIC_EnableIRQ(TPM2_IRQn);

    STRIP_timing_chart.T0H = (uint32_t)(((float)F_CPU / 1000000.0f * (float)STRIP_T0H) / 1000.0f);
    STRIP_timing_chart.T0L = (uint32_t)(((float)F_CPU / 1000000.0f * (float)STRIP_T0L) / 1000.0f);
    STRIP_timing_chart.T1H = (uint32_t)(((float)F_CPU / 1000000.0f * (float)STRIP_T1H) / 1000.0f);
    STRIP_timing_chart.T1L = (uint32_t)(((float)F_CPU / 1000000.0f * (float)STRIP_T1L) / 1000.0f);
    STRIP_timing_chart.TTOT = (uint32_t)(((float)F_CPU / 1000000.0f * (float)STRIP_TTOT) / 1000.0f);
    STRIP_timing_chart.RES = (uint32_t)(((float)F_CPU / 1000000.0f * (float)STRIP_RES) / 1000.0f);

    NVIC_SetPriority(DMA0_IRQn, 0);
    NVIC_SetPriority(TPM2_IRQn, 0);

    for (uint32_t i = 0; i <= LEDPIXELCOUNT; i++) {
        for (uint32_t j = 0; j < bitsAmount; j++) {
            STRIP_dma_buffer[i][j] = 0;
        }
    }
}

void loadStrip_pixel(uint32_t pixel, uint32_t rgb)
{
    if (pixel >= LEDPIXELCOUNT)
        return;

    // load rgb value into dma buffer
    for (uint8_t bit = 0; bit < bitsAmount; bit++) {
        if (rgb & (1 << ((bitsAmount - 1) - bit))) {
            STRIP_dma_buffer[pixel][bit] = (uint8_t)STRIP_timing_chart.T1H;
        } else {
            STRIP_dma_buffer[pixel][bit] = (uint8_t)STRIP_timing_chart.T0H;
        }
    }
}

void strip_sendContinuous()
{
    if (!StripUpdateContinuously) {
        strip_write();
    }
    StripUpdateContinuously = 1;
}

void strip_write()
{
    // wait for an ongoing write to finish
    while (!DMA_Frame_Update_Done)
        ;

    DMA_Frame_Update_Done = 0;
    conf_dma();
    // start timer
    TPM2->SC |= TPM_SC_CMOD(1);
}

void stopStripDataTransfer()
{
    // stop timer
    TPM2->SC &= ~TPM_SC_CMOD_MASK;
    StripUpdateContinuously = 0;
}

void ws2812_setleds(struct cRGBW *ledarray, const uint16_t num_leds)
{
    if (DMA_Frame_Update_Done) {
#ifdef RGB
        for (uint16_t i = 0; i < num_leds && i < LEDPIXELCOUNT; i++) {
            loadStrip_pixel(i,
                            (uint32_t)(((uint32_t)ledarray[i].r << 8 * 1) | ((uint32_t)ledarray[i].g << 8 * 2) |
                                       ((uint32_t)ledarray[i].b << 8 * 0)));
        }
#else
        for (uint16_t i = 0; i < num_leds && i < LEDPIXELCOUNT; i++) {
            loadStrip_pixel(i,
                            (uint32_t)(((uint32_t)ledarray[i].r << 8 * 2) | ((uint32_t)ledarray[i].g << 8 * 3) |
                                       ((uint32_t)ledarray[i].b << 8 * 1) | ((uint32_t)ledarray[i].w << 8 * 0)));
        }
#endif
        //strip_write();
    	strip_sendContinuous();
    }
}

void DMA0_IRQHandler()
{
    if (DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK) {
        // setup timer for reset pulse
        // the last dma transfer tranferred a 0. so cnv is always 0 after a dma transfer done.
        TPM2->MOD = (uint32_t)(((uint32_t)(F_CPU / 1000000u) * (uint32_t)STRIP_RES) / 1000u) - 1u;

        // clear interrupt flag
        DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;

        // disable dma requests
        TPM2->SC &= ~TPM_SC_DMA_MASK;

        NVIC_ClearPendingIRQ(TPM2_IRQn);
        NVIC_EnableIRQ(TPM2_IRQn);
        NVIC_SetPriority(TPM2_IRQn, 0);
    }
}

void TPM2_IRQHandler()
{
    NVIC_DisableIRQ(TPM2_IRQn);

    TPM2->MOD = (uint32_t)(((F_CPU / 1000000ul) * STRIP_TTOT) / 1000ul) - 1u;

    DMA_Frame_Update_Done = 1u;

    // enable dma req again to be ready for the next transfer
    TPM2->SC |= TPM_SC_DMA_MASK;

    if (StripUpdateContinuously) {
        conf_dma();
    } else {
        TPM2->SC &= ~TPM_SC_CMOD_MASK; // stop timer
    }
}
