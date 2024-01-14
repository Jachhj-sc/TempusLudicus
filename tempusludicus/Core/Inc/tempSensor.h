/*! ***************************************************************************
 *
 * \file      tempSensor.h
 * \author    Maarten van Riel
 * \date      Dec 2023
 *
 *****************************************************************************/
#ifndef IR_H
#define IR_H

#include <MKL25Z4.h>

/// \name T_SENS ADC0_SE0
/// \{
#define PIN_T_SENS_PORT  PORTE
#define PIN_T_SENS_PT    PTE
#define PIN_T_SENS_SHIFT (20)
#define PIN_T_SENS       (1 << PIN_T_SENS_SHIFT)
#define PIN_T_SENS_ADCH (0)
/// \}

#define TEMPERATURE_BUFFER_SIZE 500
#define MIN_TEMPERATURE         2.0f
#define MAX_TEMPERATURE         120.0f

void init_adc_lm35(void);
uint32_t read_adc_lm35(void);

float calculate_temperature_from_lm35(uint16_t adc_result);
void addTemperatureToBuffer(float temperature);

float calculateAverageTemperature(void);

float get_average_temperature(void);

#endif // IR_H
