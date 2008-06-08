//======================================================================================
/** \file  task_logic.h
 *  Task which controls the high level flow of our application
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
#include "task_solenoid.h"
#include "task_sensor.h"
#include "task_motor.h"
#include "nRF24L01_text.h"			// Nordic nRF24L01 radio module header
#include "task_rad.h"
#include "triangle.h"

//-------------------------------------------------------------------------------------
/** This class contains a task which moves a motorized lever back and forth. 
 */

class task_logic : public stl_task
    {
    protected:
        task_solenoid* ptr_task_solenoid;                 // Pointer to solenoid object
	task_sensor* ptr_task_sensor;
	task_motor* ptr_task_motor;
	triangle* ptr_triangle;
	task_rad* ptr_task_radio;
        base_text_serial* ptr_serial;         	// Pointer to a serial port for messages
	bool take_picture_flag;

    public:
        // The constructor creates a new task object
        task_logic(time_stamp*, task_solenoid*, task_sensor*, task_motor*, task_rad*, triangle*, base_text_serial*);

        // The run method is where the task actually performs its function
        char run(char);
    };
#endif
