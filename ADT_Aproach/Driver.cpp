#include "Communicaitons_Manager.h"
#include "Vision_Module.h"

const uint8_t CAMERA_NUMBER = 0;

int main(){
	// Create a communications manager so that we can talk with the main controller,
	// and thus the pixhawk and ground station
	Communicaitons_Manager coms;

	// Create a Vision_Module in order to process video feed from the camera, 
	// and find the target
	Vision_Module vision( 
		,		// FOCAL_LENGTH
		,		// SENSOR_HEIGHT
		,		// SENSOR_WIDTH
		,		// AREA_THRESHOLD
		,		// ALLOWABLE_GET_FRAME_ATTEMPTS
		&coms	// coms_address
	)

	// We need to check that the initialization is all G, otherwise the feed hasn't
	// opened and we won't be able to see anything
	if( !vision.initialize( CAMERA_NUMBER ) )
		return coms.send_error( "Cannot initialize vision module!" );

	// Let's create an instance of a target structure that contains all the visual 
	// HSV properties of the farmers jeans
	Target target = {
		75,		// MIN_HUE
		130,	// MAX_HUE
		160,	// MIN_SATURATION
		255,	// MAX_SATURATION
		60,		// MIN_VALUE
		250		// MAX_VALUE
	};

	// The program will sit in this loop until the main controller asks it to do
	// something
	while( true ){
		// We need to determine the message type we are currently recieving, so 
		// then we can handle the data appropriately 
		switch( coms.get_recieve_type() ){
			case VISION_COMMAND:{
				// Now we look at the actual data. We only have one type atm, but this switch
				// structure is nice if we want to add more
				switch( coms.get_message() ){
					case FIND_TARGET:{
						// If the target is in the current frame we have captured, 
						if( vision.find_target( target ) ){
							// Send the target status as true.. i.e. found 
							coms.set_send_type( TARGET_STATUS );
							coms.send( true );

							// Get the distance from the UAV
							Coordinate_Vector coordinates = vision.get_target_position();
							
							// Send the coordinates 
							coms.set_send_type( TARGET_COORDINATE_X );
							coms.send( coordinates.x );
							coms.set_send_type( TARGET_COORDINATE_Y );
							coms.send( coordinates.y );
						} else {
							// We couldn't find anything so tell the main controller 
							coms.set_send_type( TARGET_STATUS );
							coms.send( false );
						} 
						break;
					}
				}
				break;
			}
		}
	}
}