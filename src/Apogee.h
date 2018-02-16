
#define DELTA_COUNT    (5)

const float_t earth_gravity = 9.80665;
const uint8_t velocity_threshold = 5;

class SensApogee {
	private:
		static boolean_t _apogee_pending; // 1 no apogee dtected, 0 apogee detected
		static uint8_t _altitude_descending_count;
		static int32_t _last_atitude;
		static int32_t _apogee_altitude_delta; // change in altitude
		static uint8_t _apogee_delta_count;
		static uint8_t _apogee_velocity_counter;
		Altitude_KF* kalman;

	public:
		SensApogee(float_t Q_accel, float_t R_altitude); // Q_accel is the covariance of your accelerometer measurement
														// R_altitude is your altitude measurement
		boolean_t apogeeDetection(int32_t altitude, int32_t accel_y, float_t df); //altitude, vertical acceleration, sampling interval.
		boolean_t apogeeDetection(int32_t accel_y);
};
