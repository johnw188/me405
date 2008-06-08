//======================================================================================
/** \file  sharp_sensor_driver.cc
 *  This class interfaces with a sharp GP2Y0A700K IR rangefinder to return both the
 *  distance to the object being pointed at as well as a straight, unprocessed number.
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

#define SENSORPORT 0 //!< Pin that the sensor is connected to


#define BV(bit) (1<<(bit)) //!< Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) //!< Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  //!< Sets the corresponding bit in register reg

//		int lookupt_cm[14][2];		// this is the lookuptable for getting a distance from an analog value IN CM
//		int lookupt_tile[14][2];	// this is the lookuptable for getting a distance from an analog value IN Tiles

/** Lookup table for converting sensor readings into distances, with the distances
 *  listed in centimeters
 */
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

int initial_distances[36]; //!< Array to hold the initialization sensor readings

//--------------------------------------------------------------------------------------
/** \brief Constructor
 * 
 *  Stores the serial port pointer in class member data
 *  @param p_serial_port Pointer to a serial port object
*/

sharp_sensor_driver::sharp_sensor_driver(base_text_serial* p_serial_port) : adc_driver(p_serial_port){
	ptr_to_serial = p_serial_port;
	*ptr_to_serial << "Setting up sharp sensor controller" << endl;
}

//--------------------------------------------------------------------------------------
/** \brief Gets a raw reading from the sensor
*/

int sharp_sensor_driver::get_reading(void){
	return read_once(SENSORPORT);
}

//--------------------------------------------------------------------------------------
/** \brief Uses a lookup table to convert reading to distance
 *
 *  Searches through the lookup table to find the closest match
 *  \return Distance to whatever the sensor is pointing at, in centimeters
*/

int sharp_sensor_driver::get_distance(void){

	int analog_value;					// Value from get reading
	int min_diff = 1000;				// Value initialized very high
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
//	*ptr_to_serial << "distance" << distance << endl;
	return(distance);

}

/** \brief Takes an initialization reading
 *
 *  Takes a reading, then places it into the initial_distances array in the correct
 *  position.
 *  @param angle The current angle of the turntable
 */

void sharp_sensor_driver::init_sensor_values(int angle){
	initial_distances[angle/10] = get_distance();
}

/** \brief Checks for a change
 *
 *  Checks a sensor reading, passed to the function as an angle and a distance
 *  value, against the initial_distances array.
 *  @param angle The angle the turntable was set to when the reading was taken
 *  @param reading The reading to be checked
 */
bool sharp_sensor_driver::something_changed(int angle, int reading){

	if (initial_distances[angle/10] == reading)
		return(false);
	else 
		return(true);
}
