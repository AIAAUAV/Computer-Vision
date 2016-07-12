#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

const unsigned float FOCAL_LENGTH = 100; // mm 
const unsigned float SENSOR_HEIGHT = 100; // mm
const unsigned float SENSOR_WIDTH = 100; // mm 
const unsigned float TARGET_AREA_THRESHOLD = 100; // pix^2	
const uint16_t IMAGE_WIDTH = 1000; // pix
const uint16_t IMAGE_HEIGHT = 1000; // pix
const uint8_t ALLOWABLE_READS = 3;


// These are the HSV ranges we need to tune in order to find the colour
// of the pants
const uint8_t LOW_HUE = 0;
const uint8_t HIGH_HUE = 179;
const uint8_t LOW_SATURATION = 0;
const uint8_t HIGH_SATURATION = 255;
const uint8_t LOW_VALUE = 0;
const uint8_t HIGH_VALUE = 255;

// Function used to send an error to the ground station
int send_error( string message ){
	// ... some method to send message to ground station
	return -1;
}

// Check to see if we need to take a picture
bool take_picture(){
	// ... some code to recieve signal that tells use if we need to take picture
	// for processing 
}

// Get the laser height from the uav
float get_height(){
	// ... som code to get height from the controller 
}

void send_target_coordinates( int x, int y ){
	// .. some code to send the target coordinates to the controller
}

// Used to transform the x-y coordinates such that the origin is in the centre of the image
// rather then top left
void centre_origin( int &x, int &y ){
	x -= IMAGE_WIDTH;
	y += IMAGE_HEIGHT; 
}


int main(){
	// Let's create a feed from the camera  
	VideoCapture feed( 0 );

	// Perform a check to see if the feed is opened, otherwise we can't fly 
	if( !feed.isOpened() )
		return send_error( "can't open feed!" );

	// Let's begin processing the feed 
	while( true ){
		// If we get a signal to take a picture
		if( get_frame() ){
			// Create a matrix that will hold the frame
			Mat frame;

			// Try and read a frame
			for( int i = 1; i <= ALLOWABLE_READS && !feed.read( frame ); i++ )
				if( i == ALLOWABLE_READS ) // This means we're on our last read number, and the read failed
					return send_error( "can't read a frame!" );

			// Now convert the frame from BGR to HSV, because it's easier to threshold and process a single
			// colour this way  
			cvtColor( frame, frame, COLOR_BGR2HSV );

			// Now let's threshold based on the colours of the jeans
			inRange( frame, Scalar( LOW_HUE, LOW_SATURATION, LOW_VALUE ), Scalar( HIGH_HUE, HIGH_SATURATION, HIGH_VALUE ), frame );

			// We need to take out noise, and small objects we don't care about
			// Let's do some filtering through erosion and dilation
			erode( frame, frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );
			dilate( frame, frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );
			dilate( frame, frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );
			erode( frame, frame, getStructuralElement( MORPH_ELLIPSE, Size( 5, 5 ) ) );

			
			// Calculate the moments... I don't really understand how this works yet
			Moments moments = moments( frame );

			float target_area = moments.m00;

			if( target_area > TARGET_AREA_THRESHOLD ){
				int target_x = moments.m10 / target_area;
				int target_y = moments.m01 / target_area; 

				centre_origin( target_x, target_y );

				unsigned float UAV_height = get_height();

				target_ground_distance_x = UAV_height * ( ( ( SENSOR_WIDTH / IMAGE_WIDTH ) * target_x ) / FOCAL_LENGTH );
				target_ground_distance_y = UAV_height * ( ( ( SENSOR_HEIGHT / IMAGE_HEIGHT ) * target_y ) / FOCAL_LENGTH ); 

				send_target_coordinates( target_ground_distance_x, target_ground_distance_y );
			}
		}
	}

	return 0;
}