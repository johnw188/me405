class solenoid
	{
	protected:
		bool solenoid_on = false;
		//Constant for time in milliseconds to take a pic
		const int time_for_pic = 500;
		//Constant for time in milliseconds to hit the focus of the camera (that the camera not turns off after 5min)
		const int time_for_focus = 100;
 	public:
		// for debugging use a serial output
		solenoid (base_text_serial*);
		//taking a picture activating the solenoid for a certain amount of millisec
		void take_pic (void);
		//hit the focus of the camera for a certain amount of millisec
		void hit_focus (void);
}