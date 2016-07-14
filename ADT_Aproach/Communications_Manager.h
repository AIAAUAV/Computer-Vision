#ifndef COMMUNICATIONS_MANAGER_H
#define COMMUNICATIONS_MANAGER_H

#include <string>

// Symbols for the type of data we might recieve 
enum Recieve_Type {
	LASER_ALTITUDE,
	VISION_COMMAND
};

// Symbols for the type of data the we might want to send 
enum Send_Type {
	TARGET_STATUS,
	TARGET_COORDINATE_X,
	TARGET_COORDINATE_Y
};

class Communications_Manager {
public:
	Communications_Manager();
	~Communications_Manager();
	
	// Used to send error message to the main controller 
	short int send_error( std::string message );

	// Get the recieve type of the current message being sent from the controller 
	Recieve_Type get_recieve_type();

	// Get the actuall data that is being sent 
	int get_message(); 

	// Set the type of data that will be sending with the send method
	void set_send_type( Send_Type type );

	// Send the data of type we have specified with the set_send_type() method 
	void send( int data );

	/*
	.... send and recieve methods
	*/
private:

};

#endif // COMMUNICATIONS_MANAGER_H