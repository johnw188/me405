solenoid::solenoid(base_text_serial* p_serial_port)
{
	ptr_to_serial = p_serial_port;
	*ptr_to_serial << "Setting up solenid controller";
	//Sets up the data direction register to open the relevant bit of Port A
	DDRA = 0b00000001;
	//Sets the output to zero at the beginning
	PORTA = 0b00000000;
}

solenoid::take_pic()
{
   volatile unsigned long dummy;           // Used as a not-smart delay loop counter
	if(dummy++ < time_for_pic && solenoid_on){
		dummy = 0;	
		PORTA = 0b00000001
	}
};

solenoid::hit_focus()
{
	volatile unsigned long dummy;           // Used as a not-smart delay loop counter
	if(dummy++ < time_for_focus && solenoid_on){
		dummy = 0;	
		PORTA = 0b00000001
	}
};












Hi John, 
I started writing a class for the solenoid. I'd have some more questions about things here and then we can test it. 
I'll work on this stupid presentation little bit more, and then come back. could you write me an email, until when you plan to stay here for today? linearization is one more topic...
		richter.markus@arcor.de
				
cu later.