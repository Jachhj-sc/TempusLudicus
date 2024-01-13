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

/// \name T_SENS
/// \{
#define PIN_T_SENS_PORT  PORTD
#define PIN_T_SENS_PT    PTD
#define PIN_T_SENS_SHIFT (1)
#define PIN_T_SENS       (1 << PIN_T_SENS_SHIFT)
/// \}

void init_adc_lm35(void);
uint32_t read_adc_lm35(void);

float calculate_temperature_from_lm35(uint16_t adc_result);
void addTemperatureToBuffer(float temperature);

float calculateAverageTemperature(void);

#endif // IR_H
