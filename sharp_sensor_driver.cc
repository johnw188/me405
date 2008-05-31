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

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>

#include "rs232.h"                          // Include header for serial port class
#include "adc_driver.h"
#include "sharp_sensor_driver.h"

#define SENSORPORT 0


/** These defines make it easier for us to manipulate the bits of our registers, by
 * creating two new commands - cbi for clear bit i and sbi for set bit i
 */

#define BV(bit) (1<<(bit)) // Byte Value => sets bit'th bit to 1
#define cbi(reg, bit) reg &= ~(BV(bit)) // Clears the corresponding bit in register reg
#define sbi(reg, bit) reg |= (BV(bit))  // Sets the corresponding bit in register reg

//--------------------------------------------------------------------------------------
/** Constructor
 */

sharp_sensor_driver::sharp_sensor_driver(base_text_serial* p_serial_port) : adc_driver(p_serial_port){
}


//--------------------------------------------------------------------------------------
/** Gets a raw reading from the sensor
 */

int sharp_sensor_driver::getReading(void){
	return read_once(SENSORPORT);
}

//--------------------------------------------------------------------------------------
/** Uses a lookup table to convert reading to distance
 */

int sharp_sensor_driver::getDistance(void){

}
