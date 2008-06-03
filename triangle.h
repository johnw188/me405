//======================================================================================
/** \file  triangle.h
 *  This file contains methods and constructor for a class that triangulates position.
 *
 *  Revisions:
 *    \li  6-01-08  Jb  created file base off of control.h
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _TRIANGLE_                         // To prevent *.h file from being included
#define _TRIANGLE_                         // in a source file more than once


#include "rs232.h"      // Include header for serial port class

//-------------------------------------------------------------------------------------
/** This class runs a motor power chip from an AVR processor. Included in the protected
 *  portion is the serial adress for output to screen, the address of the motor to be
 *  controled, the adress of the adc converter,a signed integer to hold the desired
 *  position of the pan mechanism, a signed integer to hold the value of the error,
 *  and a signed integer to hold the current position of the camera. The public portion 
 *  allows a user to reset the desired position with absolute or relative input angles,
 *  and to run the controler.
 */

class triangle
    {
    protected:
        // The ADC class needs a pointer to the serial port used to say hello
        base_text_serial* ptr_to_serial;
	int cam_pos_x;				// x-value of global position of camera standard
	int cam_pos_y;				// y-value of global position of camera standard
	int cam_init_angle;			// the angle the camera is facing at initialization
	int obj_pos_x;				// detected object by the camera, to be calculated by angle_to_global
	int obj_pos_y;

    public:
        // This constructor sets up the triangulation. The constructor is passed the serial port
        triangle (base_text_serial*);
	// Sets camera position, takes global coords x-y-coords and angle the camera is facing
	void set_position(int, int, int);
        // Computes a angle from a global coordinate; takes global x,-y-coords
        int global_to_angle (signed int, signed int);
        // Computes global coords from an angle from the camera, writes the values into memberdata
        void angle_to_global (signed int);
    };


#endif
