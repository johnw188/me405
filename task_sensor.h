//======================================================================================
/** \file  task_sensor.h
 *  Task that deals with sensor readings
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
#include "task_motor.h"

//-------------------------------------------------------------------------------------
/** \brief Task dealing with sensor readings
 *  
 *  This task is in charge of taking sensor readings and checking them against the
 *  initial readings to determine if someone moved in the sensor's field of view
 */

class task_sensor : public stl_task
    {
    protected:
        sharp_sensor_driver* ptr_sharp_sensor_driver;    //!< Pointer to a sharp_sensor_driver object
		task_motor* ptr_task_motor; //!< Pointer to a task_motor object
        base_text_serial* ptr_serial; //!< Pointer to a serial port for messages
		bool take_reading_flag; //!< Flag set when a reading is required
		bool reading_taken_flag; //!< Flag set when that reading is taken
		bool take_initial_reading_flag; //!< Flag set when an initialization reading is required
		bool change_detected_flag; //!< Flag set if a change was detected
		int latest_reading;     //!< Variable to hold the latest value recorded from the sensor
    public:
        // The constructor creates a sharp_sensor_driver task object
        task_sensor(time_stamp*, sharp_sensor_driver*, task_motor*, base_text_serial*);

        // The run method is where the task actually performs its function
        char run(char);
	bool change_detected(void);
	void take_reading(void);
	bool check_reading_taken(void);
	bool reading_taken(void);
	void init_sensor_values(void);
    };

#endif
