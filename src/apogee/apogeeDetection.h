#include "singleKalman.h"
#include "altitude_kf.h"

#define DELTA_COUNT    (5)

const float_t earth_gravity = 9.80665;
const uint8_t velocity_threshold = 5;

class SensApogee {
	private:
		static boolean_t _apogee_pending; // 1 no apogee dtected, 0 apogee detected
		static uint8_t _altitude_descending_count;
		static int32_t _last_altitude;
		static uint8_t _apogee_altitude_delta; // change in altitude
		static uint8_t _apogee_delta_count;
		static uint8_t _apogee_velocity_counter;
		Altitude_KF* kalmanDouble;
		Kalman* kalmanSingle;

	public:
		SensApogee(float Q_accel, float R_altitude); // Q_accel is the covariance of your accelerometer measurement
														 // R_altitude is your altitude measurement
		boolean_t apogeeDetectionDoubleKF(int32_t altitude, int16_t accel_y, float_t df); //altitude, vertical acceleration, sampling interval.
		boolean_t apogeeDetectionSingleKF(int32_t altitude);
		boolean_t apogeeDetection(int16_t accel_y);
		int32_t getCorrectedAltitudeDoubleKF();
		int32_t getCorrectedAltitudeSingleKF();

};
