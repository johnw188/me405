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

#include "rs232.h"		// Include header for serial port class
#include "solenoid.h"     	// User written headers included with " "



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
    volatile unsigned long dummy, controldummy;           // Used as a not-smart delay loop counter
    char input_char;

    // Create an RS232 serial port object. Diagnostic information can be printed out 
    // using this port
    rs232 the_serial_port (BAUD_DIV, 1);

    // Creates a solenoid object
    solenoid hit_cam_button (&the_serial_port);

    // Say hello
    the_serial_port << "\r\nControls Test App\r\n";

    hit_cam_button.set_pic_time(2000);
    hit_cam_button.set_focus_time(1000);

    // Run the main scheduling loop, in which the action to run is done repeatedly.
    while (true)
    {
	    if (the_serial_port.check_for_char ())
	    {
		    input_char = the_serial_port.getchar();
		    if(input_char == '1')
		    {
			the_serial_port << "turning solenoid on" << endl;
			hit_cam_button.turn_on ();
		    }
		    else if(input_char == '0'){
			the_serial_port << "turning solenoid off" << endl;
			hit_cam_button.turn_off ();
		    }
		    else if(input_char == '2'){
			the_serial_port << "turning solenoid on and off for focussing" << endl;
			hit_cam_button.turn_on ();
	    		while(dummy++ < 10000L){
			}
			dummy = 0;
			hit_cam_button.turn_off ();
		    }
	    }
    }
    return (0);
    }
