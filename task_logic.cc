//======================================================================================/** \file  task_logic.cc *  Task which controls the overall logic of the camera project * *  Revisions: *    \li  05-31-08  Created file * *  License: *    This file released under the Lesser GNU Public License. The program is intended *    for educational use only, but its use is not restricted thereto.  *///======================================================================================char task_logic::run(char state){	switch(state){		case(INIT):			if(init_done)				return(WAITING);		case(WAITING):			if(change_detected){				send_coordinate_flag = true;				return(TAKE_PICTURE);			}			else if(position_stablized_flag = true){			}			else if(reading_taken){				desired_position = ((desired_position + 5) % 360);				new_motor_position_flag = true;			}