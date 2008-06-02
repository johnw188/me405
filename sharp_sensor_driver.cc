//======================================================================================
/** \file  motor_driver.cc
 *  This class interfaces with a sharp GP2Y0A700K IR rangefinder to return both the
 *  distance to the object being pointed at as well as a straight, unprocessed number.
 *  The rangefindercan 
 *
 *
 *  Revisions:
 *    \li  05-21-08  Created files
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================


#include "sharp_sensor_driver.h"

#define SENSORPORT 0


/** These defines make it easier for us to manipulate the bits of our registers, by
 * creating two new commands - cbi for clear bit i and sbi for set bit i
 */

#define BV(bit) (1<<(bit)) // Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) // Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  // Sets the corresponding bit in register reg

//		int lookupt_cm[14][2];		// this is the lookuptable for getting a distance from an analog value IN CM
//		int lookupt_tile[14][2];	// this is the lookuptable for getting a distance from an analog value IN Tiles

int lookupt_cm[14][2] ={{75,632},
			{100,545},
			{125,444},
			{150,368},
			{175,312},
			{200,271},
			{225,236},
			{250,207},
			{275,192},
			{300,180},
			{325,178},
			{350,172},
			{375,160},
			{400,142}};

int sensor_distances[36]; 		// Initial Values for each 10 degrees

//--------------------------------------------------------------------------------------
/** Constructor
 */

sharp_sensor_driver::sharp_sensor_driver(base_text_serial* p_serial_port) : adc_driver(p_serial_port){

ptr_to_serial = p_serial_port;

*ptr_to_serial << "Setting up sharp sensor controller" << endl;
}

//--------------------------------------------------------------------------------------
/** Gets a raw reading from the sensor
 */

int sharp_sensor_driver::get_reading(void){
	return read_once(SENSORPORT);
}

//--------------------------------------------------------------------------------------
/** Uses a lookup table to convert reading to distance
 */

int sharp_sensor_driver::get_distance(void){

int analog_value;					// Value from get reading
int min_diff = 1000;					// Value initialized very high
int current_diff;					// value between comparison
int distance;						// looked up value

analog_value = get_reading();

for (int i = 0; i < 14; i++){

	current_diff = lookupt_cm[i][1]-analog_value;

	if (current_diff < 0)
		current_diff = current_diff - 2 * current_diff;

	if (current_diff < min_diff){
		min_diff = current_diff;
		distance = lookupt_cm[i][0];
	}
}
	*ptr_to_serial << "distance" << distance << endl;
return(distance);

}

void sharp_sensor_driver::init_sensor_values(int angle){

sensor_distances[angle/10] = get_distance();

}

bool sharp_sensor_driver::something_changed(int angle){

if (sensor_distances[angle/10] == get_distance())
	return(false);
else 
	return(true);
}
