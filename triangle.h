//======================================================================================
/** \file  triangle.h
 *  This file contains methods and constructor for a class that triangulates position.
 *
 *  Revisions:
 *    \li  6-01-08  JBC&MR  created file base off of control.h
 *    \li  6-01-08  BC&MR  tested and finished methods
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _TRIANGLE_	// To prevent *.h file from being included in a source file more than once
#define _TRIANGLE_ 

#include <stdlib.h>     // Include standard library header files
#include <avr/io.h>
#include "rs232.h"      // Include header for serial port class

//-------------------------------------------------------------------------------------
/** This class converts local coords and angle to global x-y-coords and the other way with the help of lookuptables.
 *  Just pass global_to_angle the x-y- coords and receive an angle in degrees. Or:
 *  Pass angle_to_global a boolean variable (true for receiving the x-coord and false for receiving the y-coord) and 
 *  an angle in degrees and a distance (e.g. in tiles)
 */

class triangle
    {
    protected:
        // The triangulation class needs a pointer to the serial port used to say hello
        base_text_serial* ptr_to_serial;
	int cam_pos_x;			// x-value of global position of camera standard
	int cam_pos_y;			// y-value of global position of camera standard
	int cam_init_angle;		// the angle the camera is facing at initialization

    public:
        // This constructor sets up the triangulation. The constructor is passed the serial port
        triangle (base_text_serial*);
	void set_position(int, int, int);
        // This method allows a program to compute a angle from a global coordinate
        int global_to_angle (signed int, signed int);
	// This method allows a program to compute a global x or y from an angle and distance
	// If the first variable is true, then you'll get the x-value, else the y-value
	int angle_to_global (bool, signed int, signed int);
    };

#endif

