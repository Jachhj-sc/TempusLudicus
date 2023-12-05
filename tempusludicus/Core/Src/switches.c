/*! ***************************************************************************
 *
 * \brief     Low level driver for the switches
 * \file      switches.c
 * \author    Hugo Arends
 * \date      June 2021
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
#include "switches.h"

#include "tpm1.h"
#include <stdio.h>



#define MASK(x)       (1UL << (x))

 
extern volatile uint8_t choice;

/*!
 * \brief Initialises the switches on the shield
 *
 * This functions initializes the switches on the shield.
 */
void sw_init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

	// interrupt handlers config
	
	// switch 1
	PORTD->PCR[1] = PORT_PCR_IRQC(11) |
									PORT_PCR_MUX(1) | 
                  PORT_PCR_PS(1) | 
									PORT_PCR_PE(1);
									
	// switch 2 								
	PORTD->PCR[3] = PORT_PCR_IRQC(11) |
									PORT_PCR_MUX(1) | 
                  PORT_PCR_PS(1) | 
									PORT_PCR_PE(1);
	
	// pulse from ultrasoon sensor
	PORTD->PCR[5] = PORT_PCR_IRQC(11) |
									PORT_PCR_MUX(1) | 
                  PORT_PCR_PS(1) | 
									PORT_PCR_PE(1);
	
	
	NVIC_SetPriority(PORTD_IRQn, 192);
	NVIC_ClearPendingIRQ(PORTD_IRQn); 
	NVIC_EnableIRQ(PORTD_IRQn);
}



void PORTD_IRQHandler(void)
{  
	
	// Clear pending interrupts
	NVIC_ClearPendingIRQ(PORTD_IRQn);

    // switch 1 
    if((PORTD->ISFR & (1<<1)))
    {
			if (PTD->PDIR & MASK(1))
				{
					choice = 1;
				}
        
        // Clear the flag
        PORTD->ISFR = (1<<1);
    }
		
		// switch 2 
		if((PORTD->ISFR & (1<<3)))
    {
			if (PTD->PDIR & MASK(3))
				{
					choice = 2;
				}
        
        // Clear the flag
        PORTD->ISFR = (1<<3);
    }
		
		// pulse from ultrasoon sensor interrupt
		if((PORTD->ISFR & (1<<5)))
    {
			if (PTD->PDIR & MASK(5))
				{
					SysTick->VAL = 0;
				}
				if (PTD->PDIR & ~MASK(5))
				{
				calculate();
				}

        // Clear the flag
        PORTD->ISFR = (1<<5);
    }
}
