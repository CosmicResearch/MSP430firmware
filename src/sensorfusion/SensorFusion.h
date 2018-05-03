
#ifndef BONDAR_SENSORFUSION
#define BONDAR_SENSORFUSION

#include "SensADXL377.h"
#include "SensMag.h"
#include "Senscape.h"
#include "math.h"
#include "sensor_types.h"

//const float_t MPI = 3.141592;
#define GyroMeasError PI * (40.0f / 180.0f)       // gyroscope measurement error in rads/s (shown as 3 deg/s)
#define GyroMeasDrift PI * (0.0f / 180.0f)
#define beta sqrt(3.0f / 4.0f) * GyroMeasError   // compute beta
#define zeta sqrt(3.0f / 4.0f) * GyroMeasDrift

typedef enum { 
	SENSOR_AXIS_X = (1),
	SENSOR_AXIS_Y = (2),
	SENSOR_AXIS_Z = (3)
} sensaxis_t;


class SensFusion{
    private:
        static float q[4];    // vector to hold quaternion
        static float deltat;
        static boolean_t madgDone;
        static uint32_t lastUpdate;    // used to calculate integration interval
        static uint32_t Now;           // used to calculate integration interval

	public:
		SensFusion();
		boolean_t accelGetOrientation(accel_data_t* accel, sensfusion_data_t* data); //int16
		boolean_t magTiltCompensation(sensaxis_t axis, mag_data_t* mag, accel_data_t* accel);
		boolean_t magGetOrientation(sensaxis_t axis, mag_data_t* mag, sensfusion_data_t* data);
		boolean_t fusionGetOrientation(accel_data_t* accel, mag_data_t* mag, sensfusion_data_t* data);
		void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);
		error_t getMadgwickOrientation(sensfusion_data_t* data);

};

#endif
