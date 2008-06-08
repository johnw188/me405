//======================================================================================
/** \file triangulate_test.cc
 *      This file contains a test program which checks that the A/D converter on the
 *      AVR processor is working correctly.
 *
 *  Revisions:
 *    \li  4-14-08  JB, BC  added keyboard interface to print a reading whenever
 *                          spacebar is pressed.
 *    \li  4-16-08  BMC     added ability to print only one port at a time
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
//#include "avr_adc.h"                        // Include header for the A/D class
//#include "motor_drv.h"                      // Include header for the motor class
#include "triangle.h"				// trianglulation class

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal on an ATmega8 
 */
#define BAUD_DIV        52                  // For testing an ATmega128


//--------------------------------------------------------------------------------------
/** The main function is the "entry point" of every C program, the one which runs first
 *  (after standard setup code has finished). For mechatronics programs, main() runs an
 *  infinite loop and never exits. 
 */

char input_char;

int main ()
    {
	int obj_x = 2;
	int obj_y = 2;
	signed int obj_ang = 45;
	signed int obj_dis = 3;
    // Create an RS232 serial port object. Diagnostic information can be printed out 
    // using this port
    rs232 the_serial_port (BAUD_DIV, 1);

    // Create an ADC (Analog to Digital Converter) object. This object must be given a
    // pointer to the serial port object so that it can print debugging information
//    avr_adc my_adc (&the_serial_port);
//    motor_drv my_motor (&the_serial_port);
    triangle my_triangle (&the_serial_port);
 
    // Say hello
    the_serial_port << "triangulation test program" << endl;
    the_serial_port << "press spacebar for reading" << endl;

	my_triangle.set_position(4,1,0);

    // Run the main scheduling loop, in which the action to run is done repeatedly.
    // In the future, we'll run tasks here; for now, just do things in a simple loop
    while (true)
        {
        // The dummy counter is used to slow down the rate at which stuff is printed
        // on the terminal
        if (the_serial_port.check_for_char ())
            {
            input_char = the_serial_port.getchar ();
            if  (input_char == ' ')
                {
                // read ports 0-3 and print
                the_serial_port << "camera angle is: " << (my_triangle.global_to_angle(obj_x,obj_y)) << endl << endl;

                the_serial_port << "From angle 45 and distance 3: x_global: " << (my_triangle.angle_to_global(1,obj_ang,obj_dis)) << 
		" y_global: " << (my_triangle.angle_to_global(0,obj_ang,obj_dis)) << endl << endl;
                }
            else  //if any other key is pressed, print error and do nothing
                {
                the_serial_port << "invalid key\r";
                }
            }
        }

    return (0);
    }

