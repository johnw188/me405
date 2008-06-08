//======================================================================================
/** \file  controls.h
 *  Class which implements basic controls algorithms for the ME405 board
 *
 *  Revisions:
 *    \li  05-01-08  Created files
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _CONTROLS_H_
#define _CONTROLS_H_

// Includes libraries
#include <stdlib.h>  
#include <avr/io.h>
#include <avr/interrupt.h> 

// Including header files
#include "rs232.h"      
#include "motor_driver.h"

/** \brief Implements PID control
 *
 *  This class implements PID positional control for a DC motor, with motor
 *  feedback coming from an encoder attached to the motor shaft. Currently
 *  the class is incomplete, as I intend to flesh it out for more general
 *  use in the future.
 */
class controls : public motor_driver {
	protected:
		bool encoder_pin_A; //!< Logical state of first encoder output
		bool encoder_pin_B; //!< Logical state of second encoder output
		
		int error_count; //!< Holds the number of errors encountered by the encoder
		int encoder_max_value; //!< Number of encoder pulses per revolution of motor shaft
		
		long encoder_gear_max_value; //!<	Number of encoder pulses per revolution of the output shaft of a geartrain driven by the motor shaft
		

		unsigned int motor_position; //!< Current motor position, in units of encoder pulses
		unsigned long gear_position; //!< Position of the ouput of the geartrain, in encoder pulses
		int motor_position_degrees; //!< Current motor position, in degrees
		int gear_position_degrees; //!< Current geartrain output position, in degrees 
		
		int kp; //!< Proportional gain
		int ki; //!< Integral gain
		int kd; //!< Derivative gain
		
		int motor_setting; //!< Current motor power setting (-255 to 255)
		
		int gear_ratio; //!< Gear ratio of geartrain
		
		int position_error; //!< Current positional error
		int velocity_error; //!< Current velocity error
		long gear_position_error; //!< Current positional error of geartrain
		long gear_velocity_error; //!< Current velocity error of geartrain
		int position_error_sum; //!< Holds numerical integration of position error of motor shaft
		int velocity_error_sum; //!< Holds numerical integration of velocity error of motor shaft
		long gear_position_error_sum; //!< Holds numerical integration of position error of geartrain shaft
		long gear_velocity_error_sum; //!< Holds numerical integration of velocity error of geartrain shaft

		long desired_gear_position; //!< Target position of geartrain output for positional control
		long desired_position; //!< Target position of motor output for positional control

	public:
		controls(base_text_serial*);
		/** \brief Sets proportional gain kp
 		*  \param value New gain kp
 		*/
		void set_kp(int value){kp = value;}
		/** \brief Sets integral gain ki
 		*  \param value New gain ki
 		*/
		void set_ki(int value){ki = value;}
		/** \brief Sets derivative gain kd
 		*  \param value New gain kd
 		*/		
		void set_kd(int value){kd = value;}
		/** \brief Returns proportional gain kp
 		*  \return Value of kp
 		*/
		int get_kp(void){return kp;}
		/** \brief Returns integral gain ki
 		*  \return Value of ki
 		*/
		int get_ki(void){return ki;}
		/** \brief Returns derivative gain kd
 		*  \return Value of kd
 		*/
		int get_kd(void){return kd;}
		/** \brief Returns current motor position
 		*  \return Current motor position
 		*/
		int get_motor_position(void){return motor_position;}
		/** \brief Returns current geartrain position
 		*  \return Current geartrain position
 		*/
		int get_gear_position(void){return gear_position};
		/** \brief Returns current motor position in degrees
 		*  \return Current motor position in degrees
 		*/
		int get_motor_position_degrees(void){return motor_position_degrees;}
		/** \brief Returns current geartrain position in degrees
 		*  \return Current geartrain position in degrees
 		*/
		int get_gear_position_degrees(void){return gear_position_degrees;}
		/** \brief Returns number of errors encountered by the encoder reader
 		*  \return Errors detected
 		*/
		int get_errors(void){return error_count;}
		// Sets new reference value
		void set_reference_position(void);
		// Updates class member variables from ISR
		void update_ISR_values(void);
		// Position control methods
		void start_position_control(int); //!< Starts motor shaft positional control
		void start_position_control(int, int, int); //!< Starts motor shaft positional control
		void update_position_control(void); //!< Updates motor power value for positional control of motor shaft
		// Geared position control methods
		void start_geared_position_control(int);
		void start_geared_position_control(int, int, int);
		void update_geared_position_control(void);
		void change_gear_position(int);
		// Velocity control methods
		void start_velocity_control(int); //!< Starts motor shaft velocity control
		void start_velocity_control(int, int, int); //!< Starts motor shaft velocity control
		void update_velocity_control(void); //!< Updates motor power value for velocity control of mototor shaft
};

base_text_serial& operator<< (base_text_serial&, controls&);

#endif
