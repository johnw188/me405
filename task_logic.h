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

#ifndef _TASK_LOGIC_
#define _TASK_LOGIC_

#include <stdlib.h>
#include <avr/io.h>
#include "base_text_serial.h"
#include "stl_us_timer.h"
#include "stl_debug.h"
#include "rs232.h"
#include "stl_task.h"
#include "solenoid.h"

//-------------------------------------------------------------------------------------
/** This class contains a task which moves a motorized lever back and forth. 
 */

class task_logic : public stl_task
    {
    protected:
        solenoid* ptr_solenoid;                 // Pointer to solenoid object
        base_text_serial* ptr_serial;         	// Pointer to a serial port for messages
	bool take_picture_flag;

    public:
        // The constructor creates a new task object
        task_logic(time_stamp*, solenoid*, sharp_sensor_driver*, base_text_serial*);

        // The run method is where the task actually performs its function
        char run(char);
    };
#endif