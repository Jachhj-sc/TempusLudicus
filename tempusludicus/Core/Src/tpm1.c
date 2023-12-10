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
#include "tpm1.h"

/*!
 * \brief Initialises Timer/PWM Module 1 (TPM1)
 */
 
 #define mask(x) (1<<x)


static uint32_t teller = 0;
 
void tpm1_init(void)
{
	SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK; // enable clock	
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	

	PORTB->PCR[1] = PORT_PCR_MUX(3); // pwm option
	GPIOB->PDDR |= mask(1); // output portb 0
	TPM1->SC |= mask (1) | mask (0) | mask(2);// prescaler 128x
	TPM1->MOD = 37499; 
	TPM1->CONTROLS[1].CnSC = TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1);
	TPM1->CONTROLS[1].CnV = 4;
	TPM1->SC |= TPM_SC_CMOD(1); // pwm disabled 
	

}

int read_distance(void)
{
return teller;
}

void calculate(void)
{
				// calculate the distance from the ultra soon sensor in cm
				teller = 0;
				teller = 0x00FFFFFF - SysTick->VAL;
				teller = (teller / 48);
				teller = (teller / 58);

}
