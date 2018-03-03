#include "Accelerometer.h"
#include "Magnetometer.h"
#include "Senscape.h"

typedef enum { 
	SENSOR_AXIS_X = (1),
	SENSOR_AXIS_Y = (2),
	SENSOR_AXIS_Z = (3)
} sensaxis_t;

struct sensfusion_data_t{
	float_t pitch, roll, heading;
};

const float_t PI = 3.141592;

class SensFusion{
	public:
		boolean_t accelGetOrientation(adxl377_data_t* accel, sensfusion_data_t* data); //int16
		boolean_t magTiltCompensation(sensaxis_t axis, lsm9ds0_data_t* mag, adxl377_data_t* accel);
		boolean_t magGetOrientation(sensaxis_t axis, lsm9ds0_data_t* mag, sensfusion_data_t* data);
		boolean_t fusionGetOrientation(adxl377_data_t* accel, lsm9ds0_data_t* mag, sensfusion_data_t* data);
}