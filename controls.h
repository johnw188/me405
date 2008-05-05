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

class controls : public motor_driver {
	protected:
		// Status of the encoder pins
		bool encoder_pin_A, encoder_pin_B;
		// Variable to hold the number of errors encountered by the encoder
		int error_count;
		int encoder_max_value;
		long encoder_gear_max_value;
		// Position of the motor (from 0-255)
		unsigned int motor_position;
		// Position of the ouput of the geartrain
		unsigned long gear_position;
		// Stores controls constants
		int kp, ki, kd;
		// Stores value to set motor to
		int motor_setting;
		// Stores gear ratio of a geared system
		int gear_ratio;
		// Places to store error
		int position_error, velocity_error;
		long gear_position_error, gear_velocity_error;
		int position_error_sum, velocity_error_sum;
		long gear_position_error_sum, gear_velocity_error_sum;
		// Storage for controls info
		long desired_gear_position, desired_position;

	public:
		// Constructor
		controls(base_text_serial*);
		// Use to edit constants
		void set_kp(int value){kp = value;}
		void set_ki(int value){ki = value;}
		void set_kd(int value){kd = value;}
		//Use to read constants from other classes
		int get_kp(void){return kp;}
		int get_ki(void){return ki;}
		int get_kd(void){return kd;}
		// Use to read position
		int get_motor_position(void){return motor_position;}
		int get_motor_gear_position(void){return gear_position;}
		// Use to check number of errors encountered
		int get_errors(void){return error_count;}
		// Sets new reference value
		void set_reference_position(void);
		// Updates class member variables from ISR
		void update_ISR_values(void);
		// Position control methods
		void start_position_control(int);
		void start_position_control(int, int, int);
		void update_position_control(void);
		// Geared position control methods
		void start_geared_position_control(int);
		void start_geared_position_control(int, int, int);
		void update_geared_position_control(void);
		// Velocity control methods
		void start_velocity_control(int);
		void start_velocity_control(int, int, int);
		void update_velocity_control(void);
};

base_text_serial& operator<< (base_text_serial&, controls&);
