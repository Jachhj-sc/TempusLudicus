/*! ***************************************************************************
 *
 * \brief     Low level driver for the lm35 temparature sensor
 * \file      te pSensor.h
 * \author    Maarten van Riel
 * \date      Decemeber 2023
 *
 *****************************************************************************/
#include "tempSensor.h"
#include "common.h"
#define TEMPERATURE_BUFFER_SIZE 10

static float temperatureBuffer[TEMPERATURE_BUFFER_SIZE] = {0};
static int temperatureBufferIndex = 0;

void init_adc_lm35(void)
{
    // Enable clock to Port B (PTB3_SE13)
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    // // Configure PTB3 as ADC0_SE13
    // PORTB->PCR[3] = PORT_PCR_MUX(0);

    // Configure PTD1 as ADC0_SE5b
    PORTD->PCR[1] = PORT_PCR_MUX(0);

    // Enable clock to ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Configure ADC
    // - ADLPC = 1 : Low-power configuration. The power is reduced at the
    //               expense of maximum clock speed.
    // - ADIV[1:0] = 00 : The divide ratio is 1 and the clock rate is input
    //                    clock.
    // - ADLSMP = 1 : Long sample time.
    // - MODE[1:0] = 11 : Single-ended 16-bit conversion
    // - ADICLK[1:0] = 01 : (Bus clock)/2
    ADC0->CFG1 = 0x9D;
}

uint32_t read_adc_lm35(void)
{
    // // Start de conversie door het SC1-register te schrijven
    // ADC0->SC1[0] = ADC_SC1_ADCH(13);

    // Start de conversie door het SC1-register te schrijven
    ADC0->SC1[0] = ADC_SC1_ADCH(5);

    // Wacht tot de conversie voltooid is
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
        ;

    // Lees het conversieresultaat
    return ADC0->R[0];
}

float calculate_temperature_from_lm35(uint16_t adc_result)
{
    // Omzetten van ADC-resultaat naar spanning (aannemend dat ADC referentiespanning 5V is)
    float measured_voltage = (5.0f / 65536) * adc_result;

    // De LM35 geeft 10 mV/C, dus deel de spanning door 0.01
    float temperature = measured_voltage / 0.01f;

    temperature -= 15.6f;

    return temperature;
}

void addTemperatureToBuffer(float temperature) {
    
    temperatureBuffer[temperatureBufferIndex] = temperature;
    temperatureBufferIndex = (temperatureBufferIndex + 1) % TEMPERATURE_BUFFER_SIZE;
}

float calculateAverageTemperature(void) {
    float sum = 0.0f;
    for(int i = 0; i < TEMPERATURE_BUFFER_SIZE; i++) {
        sum += temperatureBuffer[i];
    }
    return sum / TEMPERATURE_BUFFER_SIZE;
}

