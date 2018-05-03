#include <ApogeeDetection.h>
#include <SingleKalman.h>
#include "Senscape.h"
#include "altitude_kf.h"

boolean_t SensApogee::_apogee_pending = true;
uint8_t SensApogee::_altitude_descending_countDF = 3;
uint8_t SensApogee::_altitude_descending_countSF = 3;
float_t SensApogee::_last_altitude = 0;
uint8_t SensApogee::_apogee_altitude_delta = 10;
uint8_t SensApogee::_apogee_delta_count = 5;
uint8_t SensApogee::_apogee_velocity_counter = 5;


SensApogee::SensApogee(Altitude_KF* K, Kalman* myF){
	kalmanDouble = K;
	kalmanSingle = myF;
}

boolean_t SensApogee::apogeeDetectionDoubleKF(int32_t altitude, int16_t accel_y, float_t df){
	kalmanDouble->propagate(accel_y - earth_gravity, df);
	kalmanDouble->update(altitude / 100);

	float_t new_altitude = kalmanDouble->h;
	//*************** VERSION 1 ***************
	if (new_altitude < _last_altitude) {
		--_altitude_descending_countDF;
		if (_apogee_pending & (_altitude_descending_countDF == 0)){
			_apogee_pending = 0;
			_last_altitude = new_altitude;
			return 1;
		}
	}
	else {
		_altitude_descending_countDF = 3;
		_last_altitude = new_altitude;
	}
	return 0;
}

boolean_t SensApogee::apogeeDetectionDoubleKF(int32_t altitude){
	kalmanDouble->update(altitude / 100);
	float_t new_altitude = kalmanDouble->h;
	//*************** VERSION 1 ***************
	if (new_altitude < _last_altitude) {
		--_altitude_descending_countDF;
		if (_apogee_pending & (_altitude_descending_countDF == 0)){
			_apogee_pending = 0;
			_last_altitude = new_altitude;
			return 1;
		}
	}
	else {
		_altitude_descending_countDF = 3;
		_last_altitude = new_altitude;
	}
	return 0;
}

boolean_t SensApogee::apogeeDetectionSingleKF(int32_t altitude){
	float_t newAlt = kalmanSingle->getFilteredValue(altitude / 100);
	Debug.println(newAlt);
	if (newAlt < _last_altitude) {
		--_altitude_descending_countSF;
		if (_apogee_pending and (_altitude_descending_countSF == 0)){
			_apogee_pending = 0;
			_last_altitude = newAlt;
			return 1;
		}
	}
	else {
		_altitude_descending_countSF = 3;
		_last_altitude = newAlt;
	}
	return 0;
}

int32_t SensApogee::getCorrectedAltitudeDoubleKF(){
	return (int32_t) (kalmanDouble->h*100);
}

int32_t SensApogee::getCorrectedAltitudeSingleKF(){
	return (int32_t) (kalmanSingle->getValue()*100);
}

float_t SensApogee::getVelocityDoubleKF(){
	return kalmanDouble->v;
}

boolean_t SensApogee::apogeeDetection(){
	return (not this->_apogee_pending);
}

/*boolean_t SensApogee::apogeeDetection(int32 accel_y){
	kalman.propagate(accel_y - earth_gravity, dt);
	if (kalman.v < velocityhreshold) {
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
}*/

