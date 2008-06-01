//======================================================================================
/** \file  sharp_sensor_driver.h
 *  Header for sharp sensor driver class
 *
 *  Revisions:
 *    \li  05-21-08  Created files
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _SHARP_SENSOR_DRIVER_H_
#define _SHARP_SENSOR_DRIVER_H_

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>

#include "rs232.h"                          // Include header for serial port class
#include "adc_driver.h"


//-------------------------------------------------------------------------------------
/** Sensor driver implements getReading and getDistance methods
 */

class sharp_sensor_driver : public adc_driver {
	protected:
		int sensor_value;		// Returns raw sensor data
		int sensor_distance;		// Returns distance in centimeters
		int sensor_distances[36];	// Initial Values for each 10 degrees
	public:
		sharp_sensor_driver(base_text_serial*);
		int get_reading(void);			// Get analog reading
		int get_distance(void);			// Converts analog reading into Distance with the help of lookup table
		void init_sensor_values(int);		//Fills the array with initial values, gets angle in degrees
		bool sth_changed(int);			// Compare reading to initialization reading, takes an angle in degrees to compare
};

#endif
