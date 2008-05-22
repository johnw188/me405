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


//-------------------------------------------------------------------------------------
/** Comments
 */

class motor_driver
    {
    protected:

    public:
        sharp_sensor_driver(base_text_serial*);
    };


//--------------------------------------------------------------------------------------
/// This operator allows status information about the sensor to be written to serial
/// ports easily

base_text_serial& operator<< (base_text_serial&);


#endif
