//======================================================================================
/** \file controls_test.cc
 *      This file contains a test program which checks that the motor controls on the
 *      ME405 board is working correctly. 
 *
 *  Revisions:
 *    \li  04-22-08  Wrote test class
 *    \li  04-24-08  Cleaned up, commented, implamented functionality
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

                                            // System headers included with < >
#include <stdlib.h>                         // Standard C library
#include <avr/io.h>
                                            // User written headers included with " "
#include "rs232.h"                          // Include header for serial port class
#include "motor_driver.h"                        // Include header for the A/D class
#include "adc_driver.h"
#include "controls.h"

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal on an ATmega8 
 */
#define BAUD_DIV        52                  // For testing an ATmega128


//--------------------------------------------------------------------------------------
/** The main loop checks for user input and continuously updates the values being
 *  sent to the motor
 */

int main ()
    {
    volatile unsigned long dummy;           // Used as a not-smart delay loop counter

    // Create an RS232 serial port object. Diagnostic information can be printed out 
    // using this port
    rs232 the_serial_port (BAUD_DIV, 1);

    // Creates a motor object and an adc object
    controls controller(&the_serial_port);

    // Say hello
    the_serial_port << "\r\nControls Test App\r\n";

    controller.set_kp(10);
    controller.set_ki(10);
    controller.start_geared_position_control(10);


    // Run the main scheduling loop, in which the action to run is done repeatedly.
    while (true)
    {
	    // Continuously check the value of the adc and set the motor accordingly.
	    if(dummy++ > 10000L){
		    dummy = 0;
		    if(PINE & 0b00010000){
			    the_serial_port << "pin4 high" << endl;
		    }
		    else{
			    the_serial_port << "pin4 low" << endl;
		    }
		    if(PINE & 0b00100000){
			    the_serial_port << "pin5 high" << endl;
		    }
		    else{
			    the_serial_port << "pin5 low" << endl;
		    }
		    controller.update_ISR_values();
		    the_serial_port << controller;
		    controller.update_geared_position_control();
	    }
    }

    return (0);
    }
