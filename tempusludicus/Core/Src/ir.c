/*! ***************************************************************************
 *
 * \brief     Low level driver for the infrared proximity sensor
 * \file      ir.h
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
#include "ir.h"

void init_adc_lm35() {
    // Enable clock to Port B (PTB3_SE13)
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;        

    // Configure PTB3 as ADC0_SE13
    PORTB->PCR[3] = PORT_PCR_MUX(0);

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

uint16_t read_adc_lm35() {
    // Start de conversie door het SC1-register te schrijven
    ADC0->SC1[0] = ADC_SC1_ADCH(8);

    // Wacht tot de conversie voltooid is
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    // Lees het conversieresultaat
    return ADC0->R[0];			
}

		float calculate_temperature_from_lm35(uint16_t adc_result) {
    // Omzetten van ADC-resultaat naar spanning (aannemend dat ADC referentiespanning 5V is)
    float measured_voltage = (5.0 / 65536) * adc_result;

    // De LM35 geeft 10 mV/C, dus deel de spanning door 0.01
    float temperature = measured_voltage / 0.01;
	
		temperature -= 15.6;

    return temperature;
}

 
