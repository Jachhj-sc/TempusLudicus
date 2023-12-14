/*! ***************************************************************************
 *
 * \file      pit.h
 * \author    Maarten van Riel
 * \date      Dec 2023
 *
 *****************************************************************************/
#ifndef PIT_H  // Check if PIT_H is not already defined
#define PIT_H  // Define PIT_H to prevent multiple inclusions of this header file

#include <MKL25Z4.h>  // Include the header for the KL25Z microcontroller definitions
#include <stdbool.h>  // Include the standard boolean definitions header

extern volatile uint32_t unix_timestamp;  // Declare an external global variable for Unix timestamp
                                          // 'extern' keyword indicates that the unix_timestamp variable
                                          // is defined in another file, making it accessible to files
                                          // that include this header.

void pit_init(void);  // Function prototype for pit_init
                      // This function is responsible for initializing the Periodic Interrupt Timer (PIT)

#endif // PIT_H  // End of the include guard

