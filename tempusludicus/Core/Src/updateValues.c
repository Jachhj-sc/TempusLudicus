#include <ctype.h>
#include "queue.h"
#include "uart0.h"
#include "pit.h"
#include "updateValues.h"
/*
receivedTimestamp * 10: This part of the expression multiplies the existing value of receivedTimestamp by 10.
It effectively shifts the existing digits to the left by one position, making room for the new digit.


(nextChar - '0'): Here, nextChar is assumed to be a character representing a digit ('0' to '9'). 
The expression (nextChar - '0') converts the character to its corresponding integer value. In ASCII encoding.
The characters '0' to '9' are represented consecutively, so subtracting the ASCII value of '0' from the ASCII value of nextChar results in the integer value of the digit.

For example:

If nextChar is '3', then (nextChar - '0') evaluates to 3.
If nextChar is '9', then (nextChar - '0') evaluates to 9.

*/
					void updateValue (void)	
					{
						char receivedChar = uart0_get_char();

            if (receivedChar == 'U') 
							{
                // Handle the Unix timestamp reception
                uint32_t receivedTimestamp = 0;

                // Assuming that the Unix timestamp is sent as a sequence of digits
                do {
                    char nextChar = uart0_get_char();
									
                if (isdigit((unsigned char)nextChar)) 
									{
                        receivedTimestamp = receivedTimestamp * 10 + (nextChar - '0');
                  } else 								
									
									{
                        // Break the loop if a non-digit character is encountered
                        break;
                  }
                } while (q_empty(&RxQ) == true);

                // Now 'receivedTimestamp' contains the Unix timestamp received via UART
                // Update unix_timestamp with the received value
                unix_timestamp = receivedTimestamp;
            }
							
						 if (receivedChar == 'M') 
						 {
							char receivedMoodsetting = 0;						 
							char nextChar = uart0_get_char();
							 
								receivedMoodsetting = nextChar; 
								moodSetting = receivedMoodsetting;
							}
						
        }
