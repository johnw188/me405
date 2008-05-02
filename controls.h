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
		bool encoder_pin_A, encoder_pin_B;
		int encoder_per_rev;
		long encoder_per_gear_rev;
		// Position of the motor (from 0-255)
		unsigned int motor_position;
		// Position of the ouput of the geartrain
		unsigned long gear_position;
		// Stores controls constants
		int kp, ki, kd;
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
		void get_kp(void){return kp;}
		void get_ki(void){return ki;}
		void get_kd(void){return kd;}
		// Use to read position
		int get_motor_position(void){return motor_position;}
		// Sets new reference value
		bool set_reference_position(void);
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
