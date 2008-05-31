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

#include "adc_driver.h"


//-------------------------------------------------------------------------------------
/** Sensor driver implements getReading and getDistance methods
 */

class sharp_sensor_driver : public adc_driver {
	protected:
		int sensor_value; //Returns raw sensor data
		int sensor_distance; //Returns distance in centimeters
	public:
		sharp_sensor_driver(base_text_serial*);
		int getReading(void);
		int getDistance(void);
};

#endif
