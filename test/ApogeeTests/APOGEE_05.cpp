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
Kalman myF(2,1,1023,0);

SensApogee AP(&K, &myF);

boolean_t detected;
boolean_t once;

int32_t alt[33];
float_t alt_SK[33];
float_t alt_DK[33];

void singleExecution();

void setup(void) {
	Debug.begin();

	// TOSO F("...")
	Debug.println("APOGEE_05 example - Test More Data with both Kalman");
	Debug.println();
	once = false;
	alt[0] = -2383.319; alt[1] =  53885.231; alt[2] = 101207.937;
	alt[3] = 140615.105; alt[4] = 192875.933; alt[5] = 215627.371;
	alt[6] = 255859.731; alt[7] = 286180.817; alt[8] = 308379.491;
	alt[9] = 330595.766; alt[10] = 361065.015; alt[11] = 378710.116;
	alt[12] = 384578.338; alt[13] = 400762.044; alt[14] = 418456.385;
	alt[15] = 414339.118; alt[16] = 419581.518; alt[17] = 422887.987;
	alt[18] = 419483.938; alt[19] = 409074.753; alt[20] = 398749.531;
	alt[21] = 389610.343; alt[22] = 374757.745; alt[23] = 347986.639;
	alt[24] = 331492.944; alt[25] = 296417.175; alt[26] =  263200.787;
	alt[27] = 235993.335; alt[28] = 196784.155; alt[29] = 150570.922;
	alt[30] = 125068.827; alt[31] = 79901.021; alt[32] = 18149.371;


}

void loop(void) {
	if (!once) {
		singleExecution();
		once = 1;
	}
}

void printAltitude(float_t* alt){
	for (int i = 0; i < 33; ++i){
		Debug.print(alt[i]*).println("m ");
	}
}

void singleExecution(){
	for (int i = 0; i < 33; ++i){
		boolean_t SK = AP.apogeeDetectionSingleKF(alt[i]);
		boolean_t DK = AP.apogeeDetectionDoubleKF(alt[i]);
		alt_SK[i] = (float_t) AP.getCorrectedAltitudeSingleKF()/100;
		alt_DK[i] = (float_t) AP.getCorrectedAltitudeDoubleKF()/100;
		if (SK) {
			Debug.print("Apogee detected by Single Kalman at altitude: ").print(alt_SK[i]).println("m ");
		}

		if (DK){
			Debug.print("Apogee detected by Double Kalman at altitude: ").print(alt_DK[i]).print("m ");
		}
		Debug.println("-----------------------------------------------------------------------------------");
		Debug.println("Single Kalman altitudes:");
		printAltitude(&alt_SK);
		Debug.println("-----------------------------------------------------------------------------------");
		Debug.println("Double Kalman altitudes:");
		printAltitude(&alt_DK);
	}


}
