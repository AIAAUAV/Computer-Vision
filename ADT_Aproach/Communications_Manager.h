#ifndef COMMUNICATIONS_MANAGER_H
#define COMMUNICATIONS_MANAGER_H

#include <string>

enum Recieve_Type {
	LASER_ALTITUDE
};

enum Send_Type {
	
};

class Communications_Manager {
public:
	Communications_Manager();
	~Communications_Manager();
	short int send_error( std::string message );
	int recieve( Recieve_Type type );
	bool send( Send_Type type );
	/*
	.... send and recieve methods
	*/
private:

};

#endif // COMMUNICATIONS_MANAGER_H