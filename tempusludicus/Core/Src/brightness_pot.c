#include "brightness_pot.h"
#include "math.h"

#define ADC_AVERGAGING 5

uint16_t map_ui(uint16_t value, uint16_t fromLow, uint16_t fromHigh, uint16_t toLow, uint16_t toHigh);
float mapf(float value, float fromLow, float fromHigh, float toLow, float toHigh);

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

    // select VDDA as reference voltage
    ADC0->SC2 |= ADC_SC2_REFSEL(1);

    // select b channels, selected ADC0_SE7b
    ADC0->CFG2 |= ADC_CFG2_MUXSEL(1);
}

uint8_t get_brightness_pot_value(void)
{
    uint32_t brightness = 0;
    static float prev_brightness = 0;

    for (int i = 0; i < ADC_AVERGAGING; i++) {
        // select b channels
        ADC0->CFG2 |= ADC_CFG2_MUXSEL(1);
        // Start conversion by writing to the SC1 register
        ADC0->SC1[0] = ADC_SC1_ADCH(7);

        // Wait for the conversion to complete
        while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
            ;

        // Read and return the conversion result
        brightness += ADC0->R[0];
    }

    float averagebrightness = (float)(brightness / (float)ADC_AVERGAGING);
    brightness = mapf(averagebrightness, 100.0f, 65535.0f, (float)MIN_BRIGHTNESS, (float)MAX_BRIGHTNESS);

    // round to the half a degree precision with hysterisis
    if (averagebrightness > prev_brightness + 0.9f) {
        averagebrightness = ceilf(averagebrightness);
        prev_brightness = averagebrightness;
    } else if (averagebrightness < prev_brightness - 0.9f) {
        averagebrightness = floorf(averagebrightness);
        prev_brightness = averagebrightness;
    } else {
        averagebrightness = prev_brightness;
    }


    return (uint8_t)brightness;
}

float mapf(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
    // this function maps a value with a certain range to another range perspectively
    volatile float result = 0;
    volatile float ratio;

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
    ratio = (value / fromHigh);
    result = (ratio * toHigh);
    result = result + toLow;

    return (float)result;
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
