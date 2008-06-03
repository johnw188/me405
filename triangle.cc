//======================================================================================
/** \file  triangle.cc
 *  This file contains constructor and methods for the triangulation class. This class
 *  provides a way to get an angle from global position
 *
 *  Revisions:
 *    \li  6-01-08  JB&BC  created constructor and methods
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include <stdlib.h>     // Include standard library header files
#include <avr/io.h>

#include "triangle.h"    // Include header for this class

// First Column: angle, Second column: invers tangent
int traing_tbl[45][2]  ={{88,2863},
			 {86,1430},
			 {84,951},
			 {82,711},
			 {80,567},
			 {78,470},
			 {76,401},
			 {74,348},
			 {72,307},
			 {70,274},
			 {68,247},
			 {66,224},
			 {64,205},
			 {62,188},
			 {60,173},
			 {58,160},
			 {56,148},
			 {54,137},
			 {52,127},
			 {50,119},
			 {48,111},
			 {46,103},
			 {44,96},
			 {42,90},
			 {40,83},
			 {38,78},
			 {36,72},
			 {34,67},
			 {32,62},
			 {30,57},
			 {28,53},
			 {26,48},
			 {24,44},
			 {22,40},
			 {20,36},
			 {18,32},
			 {16,28},
			 {14,24},
			 {12,21},
			 {10,17},
			 {8,14},
			 {6,10},
			 {4,6},
			 {2,3},
			 {0,0}};

//-------------------------------------------------------------------------------------
/** This constructor sets up the triangulation. The constructor is passed the serial port 
 *  to the serial for debugging purposes the camera position and the zero angle which is
 *  assumed to be measured 0 through 360 in degrees
 *  
 *  @param p_serial_port A pointer to the serial port which writes debugging info.
 *  @param camera_pos_x
 *  @param camera_pos_y
 *  @param init_angle value (in dac bits) of the camera at power up.
 */

triangle::triangle (base_text_serial* p_serial_port){

    ptr_to_serial = p_serial_port;          // Store the serial port pointer locally
    *ptr_to_serial << "Setting up triangulation" << endl;

    }

void triangle::set_position(int pos_x, int pos_y, int init_a){

cam_pos_x = pos_x;
cam_pos_y = pos_y;
cam_init_angle = init_a;

}

//-------------------------------------------------------------------------------------
/** This method takes a global position and converts the information to an angle from
 *  the camera's zero angle.
 *  \param point_angle desired angle in degree
 */

int triangle::global_to_angle (signed int x_global, signed int y_global)
    {
	int quad;
	int inv_tan;
	int curr_dif;
	int min_dif = 5000;
	int angle;
       x_global= x_global - cam_pos_x;
       y_global= y_global - cam_pos_y;    

     if (x_global <= 0)
	{
	if (y_global > 0)
	     {
	     quad=90;
	     x_global=x_global - 2 * x_global;
	     }
	else
	    {
	    quad=180;
	    x_global=x_global - 2 * x_global;
	    y_global=y_global - 2 * y_global;
	    }
	}
        else
	{
	if (y_global > 0)
	    quad=0;
	else
	    {
	    quad=270;
	    y_global=y_global - 2 * y_global;
	    }
	}
    inv_tan = x_global * 100/y_global;
    for (int n = 0; n < 45; n++)
	{
	curr_dif= traing_tbl[n][1]-inv_tan;

	if (curr_dif < 0)
	    curr_dif = curr_dif - 2 * curr_dif;

	if (curr_dif < min_dif)
	     {
	     min_dif = curr_dif;
	     angle = traing_tbl[n][0] + quad - cam_init_angle;
	     }
	}
	return (angle);
}

//-------------------------------------------------------------------------------------

