

#ifndef _SOLENOID_H_                         // To prevent *.h file from being included
#define _SOLENOID_H_                         // in a source file more than once


class solenoid
  {
  protected:
    // Pointer to serial port
    base_text_serial* ptr_to_serial;
    //still not needed variable
    bool solenoid_on;
    //Constant for time in milliseconds to take a pic
    int time_for_pic;
    //Constant for time in milliseconds to hit the focus of the camera (that the camera not turns off after 5min)
    int time_for_focus;
   public:
    // for debugging use a serial output
    solenoid (base_text_serial*);
    //setting time of taking picture
    bool set_pic_time (int);
    //setting time of hitting the focus
    bool set_focus_time (int);
    //taking a picture activating the solenoid for a certain amount of millisec
    void turn_on (void);
    //hit the focus of the camera for a certain amount of millisec
    void turn_off (void);
};


#endif // _SOLENOID_H_
