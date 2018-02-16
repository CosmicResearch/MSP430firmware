#include "Senscape.h"
#include "altitude_kf.h"
#include "SensApogee.h"
#include "Apogee.h"

boolean_t SensApogee::_apogee_pending = true;
uint8_t SensApogee::_altitude_descending_count = 5;
int32_t SensApogee::_last_atitude = 0;
int32_t SensApogee::_apogee_altitude_delta = 10;
uint8_t SensApogee::_apogee_delta_count = 5;
uint8_t SensApogee::_apogee_velocity_counter = 5;

SensApogee::SensApogee(float_t Q_accel, float_t R_altitude){
	Altitude_KF K(Q_accel, R_altitude);
	kalman = &K;
}

boolean_t SensApogee::apogeeDetection(int32_t altitude, int32_t accel_y, float_t dt){
	kalman.propagate(accel_y - earth_gravity, dt);
	kalman.update((float_t) altitude); // very not sure about all this

	int32_t new_altitude = (int32_t) kalman.h;
	//*************** VERSION 1 ***************
	if (new_altitude < _last_altitude) {
		--_altitude_descending_count;
		if (_apogee_pending & (_altitude_descending_count == 0)){
			_apogee_pending = 0;
			_last_altitude = new_altitude;
			return 1;
		}
	}
	else {
		_altitude_descending_count = 5;
		_last_altitude = kalman.h;
		return 0;
	}

	//*************** VERSION 2 *************** Carefull not to detect launch as apogee
	/*if (((uint32_t)(new_altitude - _last_altitude)) < _apogee_altitude_delta){
		if (_apogee_delta_count > 0) --_apogee_delta_count;

		if (_apogee_pending & (_apogee_delta_count == 0)){
			_apogee_pending = 0;
			_last_altitude = new_altitude;
			return 1;
		}
	}
	else {
		apogee_delta_count = 5;
		_last_altitude = kalman.h;
		return 0;
	}*/
}

boolean_t SensApogee::apogeeDetection(int32_t accel_y){
	kalman.propagate(accel_y - earth_gravity, dt);
	if (kalman.v < velocity_threshold) {
		--_apogee_velocity_counter;
		if (_apogee_pending & (_apogee_velocity_counter == 0)){
			_apogee_pending = 0;
			return 1;
		}
	}
	else {
		_apogee_velocity_counter = 5;
		return 0;
	}
}

