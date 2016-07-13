#ifndef VISION_MODULE_H
#define VISION_MODULE_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

struct Coordinate_Vector {
	int x, y;
};

struct Target {
	uint8_t 
		MIN_HUE,
		MAX_HUE,
		MIN_SATURATION,
		MAX_SATURATION,
		MIN_VALUE,
		MAX_VAULE;
};

class Vision_Module {
public:
	Vision_Module( 
		unsigned float FOCAL_LENGTH,
		unsigned float SENSOR_HEIGHT,
		unsigned float SENSOR_WIDTH,
		unsigned float AREA_THRESHOLD,
		uint8_t ALLOWABLE_GET_FRAME_ATTEMPTS 
	);

	~Vision_Module();

	bool initialise( unsigned int camera_number ); 

	bool find_target( Target* target );

	Coordinate_Vector get_target_position();

	Coordinate_Vector get_target_position( Target* target );
	
private:
	const unsigned float FOCAL_LENGTH;
	const unsigned float SENSOR_HEIGHT;
	const unsigned float SENSOR_WIDTH;
	const unsigned float AREA_THRESHOLD;
	const uint8_t ALLOWABLE_GET_FRAME_ATTEMPTS;

	cv::VideoCapture* feed;
	Target* current_target;
	Mat* current_frame;
	Communications_Manager* coms;
};

#endif // VISION_MODULE_H