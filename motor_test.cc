//======================================================================================
/** \file adc_test.cc
 *      This file contains a test program which checks that the A/D converter on the
 *      AVR processor is working correctly. 
 *
 *  Revisions:
 *    \li  00-00-00  The Earth cooled, and then there was breakfast
 *    \li  01-01-00  Confusion
 *    \li  04-10-08  Man writes adc_test.cc
 *    \li  04-14-08  Man completes code/comments. There is much rejoycing
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

                                            // System headers included with < >
#include <stdlib.h>                         // Standard C library
                                            // User written headers included with " "
#include "rs232.h"                          // Include header for serial port class
#include "motor_driver.h"                        // Include header for the A/D class
#include "adc_driver.h"

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal on an ATmega8 
 */
#define BAUD_DIV        52                  // For testing an ATmega128


//--------------------------------------------------------------------------------------
/** The main function is the "entry point" of every C program, the one which runs first
 *  (after standard setup code has finished). For mechatronics programs, main() runs an
 *  infinite loop and never exits. 
 */

int main ()
    {
    volatile unsigned long dummy;           // Used as a not-smart delay loop counter

    // Create an RS232 serial port object. Diagnostic information can be printed out 
    // using this port
    rs232 the_serial_port (BAUD_DIV, 1);

    motor_driver motor(&the_serial_port);

    adc_driver duty_cycle_input(&the_serial_port);

    // Say hello
    the_serial_port << "\r\nMotor Driver Test Application\r\n";

    unsigned char power;
    char input_char;

    // Run the main scheduling loop, in which the action to run is done repeatedly.
    // In the future, we'll run tasks here; for now, just do things in a simple loop
    while (true)
    {
	    if (the_serial_port.check_for_char ())
	    {
		    input_char = the_serial_port.getchar ();
		    if (input_char == ' ')
		    {
			    power = (unsigned char)duty_cycle_input.read_once(0);
			    the_serial_port << endl << "New power value: " << power << endl;
			    motor.set_power(power);
		    }
	    }
    }

    return (0);
    }
