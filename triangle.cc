//======================================================================================
/** \file  triangle.cc
 *  This file contains constructor and methods for the triangulation class. This class
 *  provides a way to get an angle from global position
 *
 *  Revisions:
 *    \li  6-01-08  BC&MR  created constructor and methods
 *    \li  6-01-08  BC&MR  tested and finished methods
 *    \li  6-04-08  BC     debuged methods
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

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

// First Column: angle, Second column: unit x distance scaled to 1000,  Third column unit y
// distance scaled to 1000.

int unit_tbl[49][3]   ={{0,1000,0},
			{1,999,17},
			{2,999,35},
			{3,999,52},
			{4,998,70},
			{5,996,87},
			{6,995,105},
			{7,993,122},
			{8,990,139},
			{10,985,174},
			{12,978,208},
			{14,970,242},
			{16,961,276},
			{18,951,309},
			{20,940,342},
			{22,927,375},
			{24,914,407},
			{26,899,438},
			{28,883,469},
			{30,866,500},
			{32,848,530},
			{34,829,559},
			{36,809,588},
			{38,788,616},
			{40,766,643},
			{42,743,669},
			{44,719,695},
			{46,695,719},
			{48,669,743},
			{50,643,766},
			{52,616,788},
			{54,588,809},
			{56,559,829},
			{58,530,848},
			{60,500,866},
			{62,469,883},
			{64,438,899},
			{66,407,914},
			{68,375,927},
			{70,342,940},
			{72,309,951},
			{74,276,961},
			{76,242,970},
			{78,208,978},
			{80,174,985},
			{82,139,990},
			{84,105,995},
			{86,70,998},
			{88,35,999}};

//-------------------------------------------------------------------------------------
/** This constructor sets up the triangulation. The constructor is passed the serial port 
 *  to the serial for debugging purposes the camera position
 *  
 *  @param p_serial_port A pointer to the serial port which writes debugging info.
 */

triangle::triangle (base_text_serial* p_serial_port){

    ptr_to_serial = p_serial_port;          // Store the serial port pointer locally
    *ptr_to_serial << "Setting up triangulation" << endl;

    }

/** This sets the position of the camera into member data
 */

void triangle::set_position(int pos_x, int pos_y, int init_a){

cam_pos_x = pos_x;
cam_pos_y = pos_y;
cam_init_angle = init_a;

}

/** With this you can get the local position of the cam entered in the set_position method
  * set the bool variable true to get the x-value, false for y
 */

int triangle::get_position(bool vector){

if (vector)
	return(cam_pos_x);
else
	return(cam_pos_y);

}

//-------------------------------------------------------------------------------------
/** This method takes a global position and converts the information to an angle from
 *  the camera's zero angle.
 *  @param x_global
 *  @param y_global
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
	     x_global= 0 - x_global;
	     }
	else
	    {
	    quad=180;
	    x_global= 0 - x_global;
	    y_global= 0 - y_global;
	    }
	}
    else
	{
	if (y_global > 0)
	   quad=0;
	else
	   {
	   quad=270;
	   y_global= 0 - y_global;
	   }
	}
    inv_tan = y_global * 100/x_global;
    for (int n = 0; n <= 45; n++)
	{
	curr_dif= traing_tbl[n][1]-inv_tan;

	if (curr_dif < 0)
	    curr_dif = 0 - curr_dif;

	if (curr_dif < min_dif)
	     {
	     min_dif = curr_dif;
	     angle = traing_tbl[n][0] + quad - cam_init_angle;
	     }
	}

     return (angle);
}

//-------------------------------------------------------------------------------------

 /**  @param point_angle desired angle in degree
 ** This method takes a local angle and distance and converts the information to a global
 *  x (if vector is true) and y (if vector is false)
 *  @param vector
 *  @param loc_angle
 *  @param distance
 */

int triangle::angle_to_global (bool vector, signed int loc_angle, signed int distance)
    {
	int quad;
	int curr_dif;
	int min_dif=400;
	signed int global;
	signed int local_angle;
	bool x_sign=false;
	bool y_sign=false;

	local_angle = loc_angle + cam_init_angle;
	*ptr_to_serial << "loc_angle " << loc_angle << " cam_init_angle " << cam_init_angle << " local_angle " << local_angle << endl;
	while (local_angle >= 360)
	local_angle = local_angle - 360;
	
	while (local_angle < 0)
	local_angle = local_angle + 360;

	if (local_angle > 90 && local_angle < 270)
	x_sign = true;

	if (local_angle > 180 && local_angle < 360)
	y_sign = true;

	if ( local_angle > 90 && local_angle <= 180 )
	local_angle = 180 - local_angle;

	if ( local_angle > 180 && local_angle <= 270 )
	local_angle = local_angle - 180;

	if (local_angle > 270)
	local_angle = 360 - local_angle;

	for (int i = 0; i < 49; i++)
	{
	curr_dif= unit_tbl[i][0]-local_angle;
	if (curr_dif < 0)
	    curr_dif = 0 - curr_dif;


	if (curr_dif < min_dif)
	     {
	     min_dif = curr_dif;
	     if ( vector == true )
		{
	        global= unit_tbl[i][1];
		if ( x_sign == true )
			global = 0 - global;
		}
	     if ( vector == false )
		{
	        global= unit_tbl[i][2];
		if ( y_sign == true )
		global = 0 - global;
		}
		}
	}

//*ptr_to_serial << "global: " << global << endl;

	if ( vector == true )
	   global = ((global)*distance)/1000 + cam_pos_x;

	if ( vector == false )
	   global = ((global)*distance)/1000 + cam_pos_y;

return (global);
}
