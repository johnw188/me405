//======================================================================================
/** \file camera_controller.cc
 *  This file implements a rotating camera mount controlled by a microcontroller.
 *
 *  Revisions:
 *    \li  04-30-08  Created file
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

                                            // System headers included with < >
#include <stdlib.h>                         // Standard C library
                                         
// Controller uses motor_driver class to power a motor
#include "rs232.h"                        
#include "controls.h"

/** This is the baud rate divisor for the serial port. It should give 9600 baud for the
 *  CPU crystal speed in use, for example 26 works for a 4MHz crystal on an ATmega8 
 */
#define BAUD_DIV        52                  // For testing an ATmega128


	// Define current position as initial position
	
}

// Method which takes a new position in degrees and moves the mount to that position
camera_controller::new_position(int new_position){
	// Sets the motor to move to a new position (how to not tie up processor?)
}
