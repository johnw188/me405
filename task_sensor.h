//======================================================================================
/** \file  task_solenoid.cc
 *  Task which controls the solenoid to keep the camera awake and take pictures when
 *  flagged
 *
 *  Revisions:
 *    \li  05-31-08  Created file
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _TASK_SENSOR_
#define _TASK_SENSOR_

#include <stdlib.h>
#include <avr/io.h>
#include "base_text_serial.h"
#include "stl_us_timer.h"
#include "stl_debug.h"
#include "rs232.h"
#include "stl_task.h"
#include "solenoid.h"
#include "sharp_sensor_driver.h"
#include "motor_driver.h"

//-------------------------------------------------------------------------------------
/** This class contains a task which moves a motorized lever back and forth. 
 */

class task_sensor : public stl_task
    {
    protected:
        sharp_sensor_driver* ptr_sharp_sensor_driver;                 // Pointer to solenoid object
        base_text_serial* ptr_serial;         	// Pointer to a serial port for messages
	bool take_reading_flag;
	bool reading_taken_flag;
	bool take_initial_reading_flag;
    public:
        // The constructor creates a sharp_sensor_driver task object
        task_sensor(time_stamp*, sharp_sensor_driver*, motor_driver*, base_text_serial*);

        // The run method is where the task actually performs its function
        char run(char);
	void take_reading(void);
	void init_sensor_values(void);
    };

#endif
