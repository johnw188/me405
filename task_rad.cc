//*************************************************************************************
/** \file task_rad.cc
 *      This file contains a task class for operating the radio on the AVR boards. It
 *      allows transmission and reception of data from other boards. It allows the
 *      boards to communicate with each other. Since receiving and transmiting data
 *      cannot be done at the same time, and is not mission critical, they are seperate
 *      states of the same radio task.
 *
 *  @author Jose Angulo
 *
 *  Revisions:
 *      \li 06-05-08	Initial Release
 *
 *  License:
 *    This file released under the Lesser GNU Public License, version 2. This program
 *    is intended for educational use only, but it is not limited thereto.
 */
//*************************************************************************************


#include <stdlib.h>
#include <avr/io.h>
#include "stl_debug.h"
#include "rs232.h"
#include "stl_us_timer.h"
#include "stl_task.h"
#include "task_rad.h"
#include "nRF24L01_text.h"              // Nordic nRF24L01 radio module header
#include "base_text_serial.h"
#include "triangle.h"    // Include header for this class
#include "sharp_sensor_driver.h"
#include "task_motor.h"

#define IDLE 0
#define SEND 1
#define RECEIVE 2

//-------------------------------------------------------------------------------------
/** This constructor creates a radio task class. The radio needs pointers to the base
 *  class and the serial port for debugging.
 *  @param cameraID 	the ID of the camera
 *  @param packetType	the type of packet to be sent
 *  @param t_stamp 	A timestamp which contains the time between runs of this task
 *  @param p_rad   	A pointer to a radio controller object
 *  @param p_ser   	A pointer to a serial port for sending messages if required
 *  @param p_tria  	A pointer to a triangulator object
 */

task_rad::task_rad (unsigned char cameraID, unsigned char packetType, 
			time_stamp* t_stamp, nRF24L01_text* p_rad, rs232* p_ser,
			task_motor* p_task_motor, triangle* p_triangle, sharp_sensor_driver* p_sharp_sensor_driver)
			: stl_task (*t_stamp, p_ser)
	{
	 // Save pointers to other objects
	p_serial = p_ser;
	p_radio = p_rad;
	ptr_sharp_sensor_driver = p_sharp_sensor_driver;                        // Save pointers to other objects
	ptr_task_motor = p_task_motor;
	ptr_triangle = p_triangle;
	ID = cameraID;
	packet_type = packetType;
	x = 0;
	y = 0;
	a_i = 0;
	a_j = 0;
	end_of_packet = 0xFF;
	checksum = 0;

	// Say hello
	p_serial->puts ("Radio task constructor\r\n");
}


//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It allows
 *  the radio to be in three states: reset, receive, and transmit. Reset is used when
 *  starting up and won't run again, but can if needed. Receive mode polls for characters
 *  on the queue and loads them 8 at a time into the receive buffer. Transmit checks to
 *  see if the transmit flag is set and sends the data.
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char task_rad::run (char state)
    {
    int count;
    switch (state)
        {
        // In State 0, reset the radio
        case (IDLE):
		{
		if (send)
			{
			return (SEND);
			}
		else
			{
			return (RECEIVE);
			}
	
		}

	case (SEND):
		{
		char sendbuffer[4];

		sendbuffer[0] = x;
		sendbuffer[1] = y;
		sendbuffer[2] = checksum;
		sendbuffer[3] = end_of_packet;

		*p_serial << endl << "Sending...";
	
		for (int n = 0; n < 4; n++)
			{
			*p_serial << endl << "S: " << sendbuffer[n];
			}

		*p_radio << sendbuffer;

		send = false;
		return (IDLE);
		}

	case (RECEIVE):
		{
		
		if (p_radio->check_for_char())
			{
			sth_received = true;
			//*p_serial << endl << "Receiving...";
			char temp = 0;
			char buffer[4];
			int i = 0;
			int j = 0;
			while (temp != 0xFF)
				{
				temp = p_radio->getchar();
                                j++;
				if (temp == 0) 
					{
					if (j > 12) 
                                           return (IDLE);
					
					}
				else
					{
					//*p_serial << endl << temp;
					buffer[i] = temp;
					i++;
					if (i== 4) break;
					}
				}
			char remote_Checksum = 0;
			for (int i = 0; i < 2; i++)
				{
				remote_Checksum += (int)buffer[i];
				}
			if (remote_Checksum != buffer[2])
				{
				return (IDLE);
				}
			x = buffer[0];
			y = buffer[1];
			//p_triangulate->setFoundExact(x, y);
			
			//*p_serial << endl << "X: " << x;
			//*p_serial << endl << "Y: " << y;
			sth_received = true;
			return (IDLE);	
			}
		}	
   	}
   }

void task_rad::setCoords (void)
	{
    	x = ptr_triangle->angle_to_global (1, ptr_task_motor->get_current_position(), ptr_sharp_sensor_driver->get_distance());
	y = ptr_triangle->angle_to_global (0, ptr_task_motor->get_current_position(), ptr_sharp_sensor_driver->get_distance());
*p_serial << "X: " << x;
*p_serial << "Y: " << y;
	checkSum();
	send = true;
	}

void task_rad::setAngles (char new_i, char new_j)
	{
    	a_i = new_i;
	a_j = new_j;
	checkSum();
	send = true;
	}

void task_rad::checkSum()
	{
	checksum = (int)x + (int)y;
	}

bool task_rad::check(void)
{
if (sth_received)
	return(true);
else
	return(false);
}

int task_rad::get_coords(bool vector){

if (vector)
	return(x);
else
	return(y);

}
