#include "timer_dma_ws2812.h"
#include <MKL25Z4.h>

#if defined(RGB)
static volatile uint32_t STRIP_dma_buffer[LEDPIXELCOUNT][24];
#define bitsAmount 24
#elif defined(RGBW)
static volatile uint16_t STRIP_dma_buffer[LEDPIXELCOUNT][32];
#define bitsAmount 32
#endif

typedef struct {
    uint32_t T0H;
    uint32_t T0L;
    uint32_t T1H;
    uint32_t T1L;
    uint32_t TTOT;
    uint32_t RES;
} STRIP_timing_t;

static STRIP_timing_t STRIP_TPM0_timing;

/* 	rgb / rgbw data needs to be encoded like the following
    1st pixel:
    0x00GGRRBB encoded as 24 * uint8_t time vars each holding a CnV value
    which is max 33u for 48 mhz and ps 0 period 700us

    uint8_t arr[pixel][bit] = CnV value;

 */
static volatile uint8_t DMA_Frame_Update_Done = 1;
static volatile uint8_t resetPulseFlag = 0;

void conf_dma(void)
{
    // Configure DMA channel
    DMA0->DMA[0].SAR = (uint32_t)(STRIP_dma_buffer); // Source address
    DMA0->DMA[0].DAR = (uint32_t)(&TPM0->CONTROLS[3].CnV);
    DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR((LEDPIXELCOUNT * bitsAmount) * sizeof(uint16_t)); // 4 bytes transfer
    DMA0->DMA[0].DCR = DMA_DCR_D_REQ_MASK | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK | DMA_DCR_SSIZE(1) | DMA_DCR_DSIZE(2) |
                       DMA_DCR_SINC_MASK | DMA_DCR_EINT_MASK;
}

// 48 Mhz
void init_ws2812()
{
    // enable clocks for port, dma, timer
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // Use MCGFLLCLK as TPM clock source

    // set pin to TPM0_CH0
    PORTE->PCR[30] |= PORT_PCR_MUX(3); // PTE30 TPM0_CH3

    DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(54);

    conf_dma();
    NVIC_EnableIRQ(DMA0_IRQn);

    /*
        set timer psc and cnt
        Frequency = 48Mhz
        ps = 0
        bit len = 1.2 us
     */
    TPM0->SC &= ~TPM_SC_CMOD_MASK;                                 // disable timer clock
    TPM0->SC |= TPM_SC_PS(0) | TPM_SC_DMA_MASK | TPM_SC_TOIE_MASK; // enable dma transfers and toie on overflow flag
    TPM0->MOD = (uint16_t)(((F_CPU / 1000000u) * STRIP_TTOT) / 1000) - 1u;

    // mode Edge-aligned PWM (high low) CH0 and enable dma transfers
    TPM0->CONTROLS[3].CnSC |= TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_DMA_MASK;
    TPM0->CONTROLS[3].CnV = STRIP_TPM0_timing.T0H;

    NVIC_EnableIRQ(TPM0_IRQn);

    STRIP_TPM0_timing.T0H = ((F_CPU / 1000000u * STRIP_T0H) / 1000u);
    STRIP_TPM0_timing.T0L = ((F_CPU / 1000000u * STRIP_T0L) / 1000u);
    STRIP_TPM0_timing.T1H = ((F_CPU / 1000000u * STRIP_T1H) / 1000u);
    STRIP_TPM0_timing.T1L = ((F_CPU / 1000000u * STRIP_T1L) / 1000u);
    STRIP_TPM0_timing.TTOT = ((F_CPU / 1000000u * STRIP_TTOT) / 1000u);
    STRIP_TPM0_timing.RES = ((F_CPU / 1000000u * STRIP_RES) / 1000u);
}

void loadStrip_pixel(uint32_t pixel, uint32_t rgb)
{
    if (pixel >= LEDPIXELCOUNT)
        return;

    // load rgb value into dma buffer
    for (uint8_t bit = 0; bit < bitsAmount; bit++) {
        if (rgb & (1 << ((bitsAmount - 1) - bit))) {
            STRIP_dma_buffer[pixel][bit] = (uint8_t)STRIP_TPM0_timing.T1H;
        } else {
            STRIP_dma_buffer[pixel][bit] = (uint8_t)STRIP_TPM0_timing.T0H;
        }
    }
}

void strip_sendContinuous() {}

void strip_write()
{
    // wait for an ongoing write to finish
    while (!DMA_Frame_Update_Done)
        ;

    DMA_Frame_Update_Done = 0;
    conf_dma();
    // start timer
    TPM0->SC |= TPM_SC_CMOD(1);
}

void stopStripDataTransfer()
{
    // stop timer
    TPM0->SC &= ~TPM_SC_CMOD_MASK;
}

void ws2812_setleds(struct cRGBW *ledarray, const uint16_t num_leds)
{
#ifdef RGB
    for (uint16_t i = 0; i < num_leds && i < LEDPIXELCOUNT; i++) {
        loadStrip_pixel(i,
                        (uint32_t)(((uint32_t)ledarray[i].r << 8 * 1) | ((uint32_t)ledarray[i].g << 8 * 2) |
                                   ((uint32_t)ledarray[i].b << 8 * 0)));
    }
    strip_write();
#else
    for (uint16_t i = 0; i < num_leds && i < LEDPIXELCOUNT; i++) {
        loadStrip_pixel(i,
                        (uint32_t)(((uint32_t)ledarray[i].r << 8 * 2) | ((uint32_t)ledarray[i].g << 8 * 3) |
                                   ((uint32_t)ledarray[i].b << 8 * 1) | ((uint32_t)ledarray[i].w << 8 * 0)));
    }
    strip_write();
#endif
}

void DMA0_IRQHandler()
{
    if (DMA0->DMA[0].DSR_BCR & DMA_DSR_BCR_DONE_MASK) {
        // clear interrupt flag
        DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;

        NVIC_ClearPendingIRQ(TPM0_IRQn);
        NVIC_EnableIRQ(TPM0_IRQn);

        NVIC_DisableIRQ(DMA0_IRQn);

        resetPulseFlag = 1;
    }
}

void TPM0_IRQHandler()
{
    if (resetPulseFlag) {
        resetPulseFlag = 0;
        TPM0->MOD = (uint16_t)(((uint32_t)(F_CPU / 1000000u) * (uint32_t)STRIP_RES) / 1000u) - 1u;
        TPM0->CONTROLS[3].CnV = 0;
        TPM0->CNT = 0;

    } else {
       	NVIC_DisableIRQ(TPM0_IRQn);
        TPM0->MOD = (uint16_t)(((F_CPU / 1000000u) * STRIP_TTOT) / 1000u) - 1u;
        TPM0->CONTROLS[3].CnV = 0;
        DMA_Frame_Update_Done = 1u;

        NVIC_ClearPendingIRQ(DMA0_IRQn);
        NVIC_EnableIRQ(DMA0_IRQn);
    }
}
