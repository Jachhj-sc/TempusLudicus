/*! ***************************************************************************
 *
 * \brief     Low level driver for the lm35 temparature sensor
 * \file      tempSensor.h
 * \author    Maarten van Riel
 * \date      Decemeber 2023
 *
 *****************************************************************************/
#include "tempSensor.h"
#include "MKL25Z4.h"
#include "common.h"
#include "main.h"
#include <math.h>

// Buffer to store temperature readings
static float temperatureBuffer[TEMPERATURE_BUFFER_SIZE] = {0};
// Index for the current position in the temperature buffer
static int temperatureBufferIndex = 0;

// Prototype for ADC calibration function
static int adc_cal(void);

float get_average_temperature(void)
{
    for (uint32_t i = 0; i < (TEMPERATURE_BUFFER_SIZE / 5); i++) {
        uint16_t adc_result = (uint16_t)read_adc_lm35();
        float temperature = calculate_temperature_from_lm35(adc_result);
        addTemperatureToBuffer(temperature);
    }
    return calculateAverageTemperature();
}

// Function to initialize the ADC for LM35 temperature sensor
void init_adc_lm35(void)
{
    int cal_res;

    // Enable clock to Port E 20 (ADC0_SE0)
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // Configure pin
    PIN_T_SENS_PORT->PCR[PIN_T_SENS_SHIFT] = PORT_PCR_MUX(0);

    // Enable clock to ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Start ADC calibration
    cal_res = adc_cal();
    if (cal_res == -1) {
        // Handle the error: calibration failed
        HardFault_Handler();
    }

    // Configure ADC with specific settings:
    // - ADLPC = 1: Low-power configuration
    // - ADIV[1:0] = 00: Divide ratio is 1, clock rate is input clock
    // - ADLSMP = 1: Long sample time
    // - MODE[1:0] = 11: Single-ended 16-bit conversion
    // - ADICLK[1:0] = 01: Bus clock divided by 2
    ADC0->CFG1 = 0x9D;

    // select VDDA as reference voltage
    ADC0->SC2 |= ADC_SC2_REFSEL(1);

    // select a channels, selected PTD1_SE0
    // ADC0->CFG2 &= ~ADC_CFG2_MUXSEL(1);
}

// Function to read ADC value from LM35 sensor
uint32_t read_adc_lm35(void)
{
    // if no conversion is in progress
    if (!(ADC0->SC2 & ADC_SC2_ADACT_MASK)) {
        // select a channels
        ADC0->CFG2 &= ~ADC_CFG2_MUXSEL(1);
        // Start conversion by writing to the SC1 register
        ADC0->SC1[0] = ADC_SC1_ADCH(PIN_T_SENS_ADCH);

        // Wait for the conversion to complete
        while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
            ;

        // Read and return the conversion result
        return ADC0->R[0];
    }
    return 0;
}

// ADC Calibration function
static int adc_cal(void)
{
    uint16_t calib;

    ADC0->CFG1 |= (ADC_CFG1_MODE(3) |   // 16 bits mode
                   ADC_CFG1_ADICLK(1) | // Input Bus Clock divided by 2 (48Mhz / 2)
                   ADC_CFG1_ADIV(3));   // Clock divide by 8 (3 MHz)

    ADC0->SC3 |= (ADC_SC3_AVGE_MASK | // Enable HW average
                  ADC_SC3_AVGS(3) |   // Set HW average of 32 samples
                  ADC_SC3_CAL_MASK);  // Start calibration process

    while (ADC0->SC3 & ADC_SC3_CAL_MASK)
        ; // Wait for calibration to end

    if (ADC0->SC3 & ADC_SC3_CALF_MASK) // Check for successful calibration
        return -1;

    calib = 0;
    calib += ADC0->CLPS + ADC0->CLP4 + ADC0->CLP3 + ADC0->CLP2 + ADC0->CLP1 + ADC0->CLP0;
    calib /= 2;
    calib |= 0x8000; // Set MSB
    ADC0->PG = calib;

    calib = 0;
    calib += ADC0->CLMS + ADC0->CLM4 + ADC0->CLM3 + ADC0->CLM2 + ADC0->CLM1 + ADC0->CLM0;
    calib /= 2;
    calib |= 0x8000; // Set MSB
    ADC0->MG = calib;

    return 0;
}

// Function to calculate temperature from ADC result
float calculate_temperature_from_lm35(uint16_t adc_result)
{
    // Convert ADC result to voltage (assuming ADC reference voltage is 3.28V = 3280 mv)
    // LM35 outputs 10 mV/C, multiply the voltage by 100 to get temperature
    float temperature = ((3.28f * 100.0f * (float)adc_result) / 65536.0f);

    // Check if temperature is within the specified range
    if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATURE) {
        return -1.0f; // Return an error value if temperature is out of range
    } else {
        return temperature;
    }
}

// Function to add a temperature reading to the buffer
void addTemperatureToBuffer(float temperature)
{
    // Only add valid temperatures to the buffer
    if (temperature >= MIN_TEMPERATURE && temperature <= MAX_TEMPERATURE) {
        temperatureBuffer[temperatureBufferIndex] = temperature;
        temperatureBufferIndex = (temperatureBufferIndex + 1) % TEMPERATURE_BUFFER_SIZE;
    }
}

// Function to calculate the average temperature from the buffer
float calculateAverageTemperature(void)
{
    double sum = 0.0f;
    uint32_t validCount = 0;
    static float prev_temparature = 0.0f;

    for (int i = 0; i < TEMPERATURE_BUFFER_SIZE; i++) {
        if (temperatureBuffer[i] >= MIN_TEMPERATURE && temperatureBuffer[i] <= MAX_TEMPERATURE) {
            sum += temperatureBuffer[i];
            validCount++;
        }
    }

    // Calculate and return average temperature if valid readings exist
    if (validCount > 0) {
        float averageTemperature = (float)(sum / (double)validCount);

        // round to the half a degree precision with hysterisis
        if (averageTemperature > prev_temparature + 0.4f) {
            averageTemperature = ceilf(averageTemperature * 2.0f) / 2.0f;
            prev_temparature = averageTemperature;
        } else if (averageTemperature < prev_temparature - 0.4f) {
            averageTemperature = floorf(averageTemperature * 2.0f) / 2.0f;
            prev_temparature = averageTemperature;
        }else{
            averageTemperature = prev_temparature;
        }

        return averageTemperature;
    } else {
        return -1.0f; // Return an error value if no valid readings exist
    }
}
