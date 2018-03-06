#include "singleKalman.h"
#include "altitude_kf.h"

#define DELTA_COUNT    (5)

const float_t earth_gravity = 9.80665;
const uint8_t velocity_threshold = 5;

class SensApogee {
	private:
		static boolean_t _apogee_pending; // 1 no apogee dtected, 0 apogee detected
		static uint8_t _altitude_descending_countDF;
		static uint8_t _altitude_descending_countSF;
		static float_t _last_altitude;
		static uint8_t _apogee_altitude_delta; // change in altitude
		static uint8_t _apogee_delta_count;
		static uint8_t _apogee_velocity_counter;
		Altitude_KF* kalmanDouble;
		Kalman* kalmanSingle;

	public:
		//Create intance of classes of both Altitude_KF and Kalman in your code
		//Values used in test are: Altitude_KF K(0.1, 0.1, 0.125);
		//						   Kalman myF(0.1,1,1023,0);
		SensApogee(Altitude_KF* K, Kalman* myF);
		boolean_t apogeeDetectionDoubleKF(int32_t altitude, int16_t accel_y, float_t df); //altitude, vertical acceleration, sampling interval.
		boolean_t apogeeDetectionDoubleKF(int32_t altitude);
		boolean_t apogeeDetectionSingleKF(int32_t altitude);
		boolean_t apogeeDetection(int16_t accel_y);
		int32_t getCorrectedAltitudeDoubleKF();
		int32_t getCorrectedAltitudeSingleKF();
		float_t getVelocityDoubleKF();
		boolean_t apogeeDetection();

};
