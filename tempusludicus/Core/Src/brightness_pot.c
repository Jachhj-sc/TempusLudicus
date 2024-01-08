#include "brightness_pot.h"
#include "math.h"

uint16_t map_ui(uint16_t value, uint16_t fromLow, uint16_t fromHigh, uint16_t toLow, uint16_t toHigh);

void init_brightness_pot(void)
{
    // configure adc

    // Enable clock to Port D (D6 ADC0_SE7b)
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    // Configure PTD1 as ADC0_SE7b
    PORTD->PCR[6] = PORT_PCR_MUX(0);

    // Enable clock to ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Configure ADC with specific settings:
    // - ADLPC = 1: Low-power configuration
    // - ADIV[1:0] = 00: Divide ratio is 1, clock rate is input clock
    // - ADLSMP = 1: Long sample time
    // - MODE[1:0] = 11: Single-ended 16-bit conversion
    // - ADICLK[1:0] = 01: Bus clock divided by 2
    ADC0->CFG1 = 0x9D;

    // select b channels, selected ADC0_SE7b
    ADC0->CFG2 |= ADC_CFG2_MUXSEL(1);
}

uint8_t get_brightness_pot_value(void)
{
    uint32_t brightness = 0;
    // Start conversion by writing to the SC1 register

    for (int i = 0; i < 20; i++) {
        ADC0->SC1[0] = ADC_SC1_ADCH(7);
        // Wait for the conversion to complete
        while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
            ;

        // Read and return the conversion result
        brightness += ADC0->R[0];
    }
    brightness /= 20;
    brightness = map_ui(brightness, 0, 65535, 0, MAX_BRIGHTNESS);

    return (uint8_t)brightness;
}

uint16_t map_ui(uint16_t value, uint16_t fromLow, uint16_t fromHigh, uint16_t toLow, uint16_t toHigh)
{
    // this function maps a value with a certain range to another range perspectively
    volatile uint32_t result = 0;
    volatile double ratio;

    // make sure the boundaries are not crossed.
    if (value > fromHigh) {
        value = fromHigh;
    }
    if (value < fromLow) {
        value = fromLow;
    }

    // zero values
    value = value - fromLow;
    fromHigh = fromHigh - fromLow;

    toHigh = toHigh - fromLow;

    // safeguard for division by 0 error
    if (fromHigh == 0) {
        fromHigh = 1;
    }

    // calculate ratio
    ratio = (double)((double)value / (double)fromHigh);

    result = (uint16_t)(ratio * toHigh);
    result = result + toLow;

    return (uint16_t)result;
}
