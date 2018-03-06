/*
 * @author David Williams <davidwcorral@hotmail.com>
 * @date   July 25, 2017
 */


#include "Senscape.h"
#include "apogeeDetection.h"
#include "singleKalman.h"
#include "altitude_kf.h"

Resource mag_resource = Resource(ARBITER_USCIB_0);

Altitude_KF K(0.1, 0.1, 0.125);
//Kalman myF(0.1,1,1023,0);
Kalman myF(2,1,1023,0);

SensApogee AP(&K, &myF);

boolean_t detected;
boolean_t once;

void singleExecution();

void setup(void) {
	Debug.begin();

	// TOSO F("...")
	Debug.println("AAPOGEE_02 example - Test Double basic Detection");
	Debug.println();
	once = false;

}

void loop(void) {
	if (!once) {
		singleExecution();
		once = 1;
	}
}

void singleExecution(){
	if (AP.apogeeDetectionDoubleKF((int32_t) 100*100)) Debug.print("Apogee detected at a ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 200*100)) Debug.print("Apogee detected at b ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 300*100)) Debug.print("Apogee detected at c ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 400*100)) Debug.print("Apogee detected at d ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 500*100)) Debug.print("Apogee detected at e ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 550*100)) Debug.print("Apogee detected at f ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 600*100)) Debug.print("Apogee detected at g ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 550*100)) Debug.print("Apogee detected at h ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 500*100)) Debug.print("Apogee detected at j ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 400*100)) Debug.print("Apogee detected at k ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 300*100)) Debug.print("Apogee detected at l ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 200*100)) Debug.print("Apogee detected at m ").println(AP.getCorrectedAltitudeDoubleKF()/100);
	if (AP.apogeeDetectionDoubleKF((int32_t) 100*100)) Debug.print("Apogee detected at n ").println(AP.getCorrectedAltitudeDoubleKF()/100);

	if ( not AP.apogeeDetection()) Debug.println("No apogee detected");

}
