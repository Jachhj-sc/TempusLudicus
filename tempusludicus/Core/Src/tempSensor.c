/*! ***************************************************************************
 *
 * \brief     Low level driver for the lm35 temparature sensor
 * \file      te pSensor.h
 * \author    Maarten van Riel
 * \date      Decemeber 2023
 *
 *****************************************************************************/
#include <math.h>
#include "tempSensor.h"
#include "common.h"
#include "MKL25Z4.h"  // Ensure this is correct for your microcontroller

#define TEMPERATURE_BUFFER_SIZE 1000
#define MIN_TEMPERATURE 2.0f
#define MAX_TEMPERATURE 120.0f

// Buffer to store temperature readings
static float temperatureBuffer[TEMPERATURE_BUFFER_SIZE] = {0};
// Index for the current position in the temperature buffer
static int temperatureBufferIndex = 0;

// Prototype for ADC calibration function
static int adc_cal(void);

// Function to initialize the ADC for LM35 temperature sensor
void init_adc_lm35(void)
{
    int cal_res;

    // Enable clock to Port B (PTB3_SE13)
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    // // Configure PTB3 as ADC0_SE13
    // PORTB->PCR[3] = PORT_PCR_MUX(0);

  /// potential bug
  #warning "double check this"
    // Configure PTD1 as ADC0_SE5b
    PORTD->PCR[1] = PORT_PCR_MUX(0);

    // Enable clock to ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Start ADC calibration
    cal_res = adc_cal();
    if (cal_res == -1) {
        // Handle the error: calibration failed
        // For example, set an error flag or indicator
    }

    // Configure ADC with specific settings:
    // - ADLPC = 1: Low-power configuration
    // - ADIV[1:0] = 00: Divide ratio is 1, clock rate is input clock
    // - ADLSMP = 1: Long sample time
    // - MODE[1:0] = 11: Single-ended 16-bit conversion
    // - ADICLK[1:0] = 01: Bus clock divided by 2
    ADC0->CFG1 = 0x9D;
}

// Function to read ADC value from LM35 sensor
uint32_t read_adc_lm35(void)
{
    // Start conversion by writing to the SC1 register
    ADC0->SC1[0] = ADC_SC1_ADCH(8);


    // Wait for the conversion to complete
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    // Read and return the conversion result
    return ADC0->R[0];
}

// ADC Calibration function
static int adc_cal(void)
{
    uint16_t calib;

    ADC0->CFG1 |= (ADC_CFG1_MODE(3)  |  // 16 bits mode
                   ADC_CFG1_ADICLK(1)|  // Input Bus Clock divided by 2 (48Mhz / 2)
                   ADC_CFG1_ADIV(3));   // Clock divide by 8 (3 MHz)

    ADC0->SC3  |= (ADC_SC3_AVGE_MASK |    // Enable HW average
                   ADC_SC3_AVGS(3)   |  // Set HW average of 32 samples
                   ADC_SC3_CAL_MASK);   // Start calibration process

    while (ADC0->SC3 & ADC_SC3_CAL_MASK); // Wait for calibration to end

    if (ADC0->SC3 & ADC_SC3_CALF_MASK)   // Check for successful calibration
        return -1;

    calib = 0;
    calib += ADC0->CLPS + ADC0->CLP4 + ADC0->CLP3 +
             ADC0->CLP2 + ADC0->CLP1 + ADC0->CLP0;
    calib /= 2;
    calib |= 0x8000;    // Set MSB
    ADC0->PG = calib;

    calib = 0;
    calib += ADC0->CLMS + ADC0->CLM4 + ADC0->CLM3 +
             ADC0->CLM2 + ADC0->CLM1 + ADC0->CLM0;
    calib /= 2;
    calib |= 0x8000;    // Set MSB
    ADC0->MG = calib;

    return 0;
}

// Function to calculate temperature from ADC result
float calculate_temperature_from_lm35(uint16_t adc_result) {
    // Convert ADC result to voltage (assuming ADC reference voltage is 3.01V)
    float measured_voltage = (3.01f / 65536) * adc_result;
    // LM35 outputs 10 mV/C, divide the voltage by 0.01 to get temperature
    float temperature = measured_voltage / 0.01f;

    // Check if temperature is within the specified range
    if (temperature < MIN_TEMPERATURE || temperature > MAX_TEMPERATURE) {
        return -1.0f; // Return an error value if temperature is out of range
    } else {
        return temperature;
    }
}

// Function to add a temperature reading to the buffer
void addTemperatureToBuffer(float temperature) {
    // Only add valid temperatures to the buffer
    if (temperature >= MIN_TEMPERATURE && temperature <= MAX_TEMPERATURE) {
        temperatureBuffer[temperatureBufferIndex] = temperature;
        temperatureBufferIndex = (temperatureBufferIndex + 1) % TEMPERATURE_BUFFER_SIZE;
    }
}

// Function to calculate the average temperature from the buffer
float calculateAverageTemperature(void) {
    float sum = 0.0f;
    float validCount = 0;
    for (int i = 0; i < TEMPERATURE_BUFFER_SIZE; i++) {
        if (temperatureBuffer[i] >= MIN_TEMPERATURE && temperatureBuffer[i] <= MAX_TEMPERATURE) {
            sum += temperatureBuffer[i];
            validCount++;
        }
    }
    // Calculate and return average temperature if valid readings exist
    if (validCount > 0) {
        float averageTemperature = sum / validCount;
        // Round to the nearest half degree
        return roundf(averageTemperature * 2) / 2;
    } else {
        return -1.0f; // Return an error value if no valid readings exist
    }
}
