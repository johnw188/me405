//======================================================================================
/** \file  task_solenoid.h
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

#ifndef _TASK_SOLENOID_
#define _TASK_SOLENOID_

#include <stdlib.h>
#include <avr/io.h>
#include "base_text_serial.h"
#include "stl_us_timer.h"
#include "stl_debug.h"
#include "rs232.h"
#include "stl_task.h"
#include "solenoid.h"

//-------------------------------------------------------------------------------------
/** \brief Task which %controls the position of a %solenoid
 *
 *  This task allows a %solenoid to be used to take pictures, by pressing the
 *  shutter button of a digital camera
 */

class task_solenoid : public stl_task
    {
    protected:
        solenoid* ptr_solenoid;                 //!< Pointer to solenoid object
        base_text_serial* ptr_serial;         	//!< Pointer to a serial port for messages
		bool take_picture_flag; //!< Flag set when a picture is to be taken
		bool picture_done_flag; //!< Flag set when the picture is finished

    public:
        // The constructor creates a new task object
        task_solenoid(time_stamp*, solenoid*, base_text_serial*);

        // The run method is where the task actually performs its function
        char run(char);
	void take_picture(void);
	bool picture_done(void);
    };
#endif
