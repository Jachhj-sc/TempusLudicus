/***************************************************
 *
 * \file    pit.c
 * \author  Maarten van Riel
 * \date    Dec 2023
 * \note -> Timer 1 is used for the LED (to check the code)
 ****************************************************/

#include "pit.h"

// volatile bool pit_interrupt_flag = false;

/*!
 * \brief Initialises the Periodic Interrupt Timer (PIT)
 */
void pit_init(void)
{
    // Enables the clock for the PIT module
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

    // Enables the PIT module, freezes timers in debug mode
    PIT->MCR &= ~PIT_MCR_MDIS_MASK; // disable MDIS
    PIT->MCR |= PIT_MCR_FRZ_MASK;   // enable "freeze" mode

    // Initializes PIT0 to generate an event every second
    PIT->CHANNEL[0].LDVAL =
        PIT_LDVAL_TSV(24e6 - 1); // Set the timer to generate an event every second (clock speed 24 MHz)

    // No chaining of timers
    PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_CHN_MASK;

    // Generates interrupts
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

    // Initialize PIT1 to generate an event every 1s
    // PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV((24e6/1)-1);

    // No Chaining
    // PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_CHN_MASK;

    // Generate interrupt
    // PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;

    // Sets the interrupt priority and clears any previous interrupts
    NVIC_SetPriority(PIT_IRQn, 128); // Possible values: 0, 64, 128, 192
    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);

    // Enables the counter
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
    // PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;

    unix_timestamp = 0; // Example value, specific Unix time should be entered here
}

void PIT_IRQHandler(void)
{
    // Clears any previous interrupts
    NVIC_ClearPendingIRQ(PIT_IRQn);

    // Checks which channel triggered the interrupt
    if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
        // Clears the status flag for timer channel 0
        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;

        // Processes the event for channel 0
        unix_timestamp++; // Here you increase the value of 'timestamp' with each interrupt
    }

    /* if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK)
       {
           // Clear status flag for timer channel 1
           PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;

           // Handle channel 1 event
           pit_interrupt_flag = true;
       }
    */
}
