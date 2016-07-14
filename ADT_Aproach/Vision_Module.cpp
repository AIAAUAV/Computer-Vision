#include "Vision_Manager.h"

Vision_Module::Vision_Module( 
	unsigned float FOCAL_LENGTH,
	unsigned float SENSOR_HEIGHT,
	unsigned float SENSOR_WIDTH,
	unsigned float AREA_THRESHOLD,
	uint8_t ALLOWABLE_GET_FRAME_ATTEMPTS,
	Communications_Manager* coms_address
): // Initialise everything 
	FOCAL_LENGTH( FOCAL_LENGTH ),
	SENSOR_HEIGHT( SENSOR_HEIGHT ),
	SENSOR_WIDTH( SENSOR_WIDTH ),
	AREA_THRESHOLD( AREA_THRESHOLD ),
	ALLOWABLE_GET_FRAME_ATTEMPTS( ALLOWABLE_GET_*frame_ATTEMPTS ),
	coms( coms_address )
{}

bool Vision_Module::initialize( unsigned int camera_number ){
	// Assign a new video capture object 
	feed = new VideoCapture( camera_number );

	// Perform a check to see if the feed is opened, otherwise we can't fly 
	if( !feed->isOpened() )
		return false;

	return true;
}

bool Vision_Module::find_target( Target* target ){
	// Now let's threshold based on the colours of the jeans
	inRange( *frame, Scalar( target->LOW_HUE, target->LOW_SATURATION, target->LOW_VALUE ), Scalar( target->HIGH_HUE, target->HIGH_SATURATION, target->HIGH_VALUE ), *frame );

	// Calculate the moments... I don't really understand how this works yet
	Moments moments = moments( *frame );

	float target_area = moments.m00;

	// The area we have needs to be bigger than the threshold, otherwise
	// we haven't actually found anything
	if( target_area > TARGET_AREA_THRESHOLD )
		return true;

	return false;
}

bool Vision_Module::get_frame(){
	frame = new Mat;

	// Try and read a *frame
	for( int i = 1; i <= ALLOWABLE_READS && !feed->read( *frame ); i++ )
		if( i == ALLOWABLE_READS ) // This means we're on our last read number, and the read failed
			return false;

	// Now convert the *frame from BGR to HSV, because it's easier to threshold and process a single
	// colour this way  
	cvtColor( *frame, *frame, COLOR_BGR2HSV );

	// We need to take out noise, and small objects we don't care about
	// Let's do some filtering through erosion and dilation
	erode( *frame, *frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );
	dilate( *frame, *frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );
	dilate( *frame, *frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );
	erode( *frame, *frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );

	return true;
}

Coordinate_Vector Vision_Module::get_target_position(){
	// Calculate the moments... I don't really understand how this works yet
	Moments moments = moments( *frame );

	float target_area = moments.m00;

	// create a vector that will hold the position of the target from the uav
	Coordinate_Vector coordinates;
	coordinates.x = 0;
	coordinates.y = 0;

	// we have to calculate the pixel position from the moments 
	int target_x = moments.m10 / target_area;
	int target_y = moments.m01 / target_area; 

	// Now perform a translation so that we have a center origin instead 
	// of one in the top left corner of the view port 
	centre_origin( target_x, target_y );

	// To perform the calculations to find the actual distance fomr the uav, we need to get
	// the vertical distance from the plane on which the target it on 
	unsigned float UAV_height = coms.recieve( LASER_ALTITUDE );

	// Using equations for horizontal distances 
	coordinates.x = UAV_height * ( ( ( SENSOR_WIDTH / IMAGE_WIDTH ) * target_x ) / FOCAL_LENGTH );
	coordinates.y = UAV_height * ( ( ( SENSOR_HEIGHT / IMAGE_HEIGHT ) * target_y ) / FOCAL_LENGTH ); 

	return coordinates;
}
	
Coordinate_Vector Vision_Module::get_target_position( Target* target ){
	// Now let's threshold based on the colours of the jeans
	inRange( *frame, Scalar( target->LOW_HUE, target->LOW_SATURATION, target->LOW_VALUE ), Scalar( target->HIGH_HUE, target->HIGH_SATURATION, target->HIGH_VALUE ), *frame );

	// Calculate the moments... I don't really understand how this works yet
	Moments moments = moments( *frame );

	float target_area = moments.m00;

	// create a vector that will hold the position of the target from the uav
	Coordinate_Vector coordinates;
	coordinates.x = 0;
	coordinates.y = 0;

	if( target_area > TARGET_AREA_THRESHOLD )
		// we have to calculate the pixel position from the moments 
	int target_x = moments.m10 / target_area;
	int target_y = moments.m01 / target_area; 

	// Now perform a translation so that we have a center origin instead 
	// of one in the top left corner of the view port 
	centre_origin( target_x, target_y );

	// To perform the calculations to find the actual distance fomr the uav, we need to get
	// the vertical distance from the plane on which the target it on 
	unsigned float UAV_height = coms.recieve( LASER_ALTITUDE );

	// Using equations for horizontal distances 
	coordinates.x = UAV_height * ( ( ( SENSOR_WIDTH / IMAGE_WIDTH ) * target_x ) / FOCAL_LENGTH );
	coordinates.y = UAV_height * ( ( ( SENSOR_HEIGHT / IMAGE_HEIGHT ) * target_y ) / FOCAL_LENGTH ); 
	}
}

