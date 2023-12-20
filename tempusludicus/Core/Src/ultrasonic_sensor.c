/*! ***************************************************************************
 *
 * \brief     Low level driver for TPM1
 * \file      tpm1.c
 * \author    Hugo Arends
 * \date      July 2021
 *
 * \copyright 2021 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include "ultrasonic_sensor.h"
#include "delay.h"
#include "sysTick.h"

/*!
 * \brief Initialises Timer/PWM Module 1 (TPM1)
 */

#define MINIMAL_PULSE_INTERVAL 60u

static uint32_t distance_cm = 0;
static uint16_t tpm1_psc = 0;
static uint32_t prevTriggerPulseTime = 0;

void ultraS_sensor_process(void)
{
    if (get_millis() > prevTriggerPulseTime + MINIMAL_PULSE_INTERVAL) {
        // send pulse
        GPIOB->PSOR = MASK(1);
        delay_us(10);
        GPIOB->PCOR = MASK(1);

        prevTriggerPulseTime = get_millis();
    }
}

void ultraS_sensor_init(void)
{
    SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK; // enable clock
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

    PORTB->PCR[1] = PORT_PCR_MUX(1); // gpio
    GPIOB->PDDR |= MASK(1);          // output portb 1

    TPM1->SC |= TPM_SC_PS(5); // prescaler 32x
    tpm1_psc = 32;            // add prescaler value to variable for easy calculations

    TPM1->MOD = 0xFFFF; // set max value

    // pulse from ultrasoon sensor falling and rising edge interrupt
    PORTD->PCR[5] = PORT_PCR_IRQC(11) | PORT_PCR_MUX(1) /* | PORT_PCR_PS(1) | PORT_PCR_PE(1)*/;

    NVIC_SetPriority(PORTD_IRQn, 0);
    NVIC_ClearPendingIRQ(PORTD_IRQn);
    NVIC_EnableIRQ(PORTD_IRQn);
}

void tpm1_start(void)
{
    TPM1->CNT = 0;
    TPM1->SC |= TPM_SC_CMOD(1);
}

uint32_t tpm1_stop(void)
{
    uint32_t stopVal = TPM1->CNT;
    // stop timer and return timer value
    TPM1->SC &= ~TPM_SC_CMOD_MASK;
    return stopVal;
}

void tpm1_reset(void)
{
    TPM1->CNT = 0;
}

uint16_t ultraS_get_distance_cm(void)
{
    return (uint16_t)distance_cm;
}

// calculate the distance from the ultra soon sensor in cm
void ultraS_updateDistance(uint32_t tpm_cnt)
{
    uint32_t PulseDuration_uS =
        (uint32_t)((float)tpm_cnt * (float)((float)1000000UL / (float)((float)F_CPU / (float)tpm1_psc)));

    distance_cm = PulseDuration_uS / 58;
}
