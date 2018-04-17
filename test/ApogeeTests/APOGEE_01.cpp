/*
 * @author David Williams <davidwcorral@hotmail.com>
 * @date   March 5, 2018
 */


#include "Senscape.h"
#include "apogeeDetection.h"
#include "singleKalman.h"
#include "altitude_kf.h"
#include "GPS.h"

Resource mag_resource = Resource(ARBITER_USCIB_0);

Altitude_KF K(0.1, 0.1, 0.125);
//Kalman myF(0.1,1,1023,0);
Kalman myF(2,1,1023,0);

SensApogee AP(&K, &myF);

boolean_t detected;
boolean_t once;

const uint32_t baudrate = 9600;
GPS gps(&Serial0, baudrate);

void singleExecution();

void startDone(error_t error) {
    if (error == SUCCESS) {
        Debug.println("start done");
        //gps.read();
    }
    else {
        Debug.println("error on start");
    }
}

void stopDone(error_t error) {

}

void readDone(sensor_data_t* data, error_t error) {
    if (error != SUCCESS) {
        return;
    }
    gps_data_t gps_data = *((gps_data_t*)data);
    if (strcmp(gps_data.type,"GGA") == 0){
            Debug.println("---------------------------------------");
            Debug.println("new gps data");
            //Debug.print("Latitude: ").print(gps_data.latitude/1000000.0f).println(gps_data.latitudeChar);
            //Debug.print("Longitude: ").print(gps_data.longitude/1000000.0f).println(gps_data.longitudeChar);
            Debug.print("Altitude gps: ").print(gps_data.altitude/100).println(" m");
            //Debug.print("Fix: ").println(gps_data.fix);
            //Debug.print("Time: ").print(gps_data.hour).print(":").print(gps_data.minute).print(":").println(gps_data.seconds);
            //Debug.print("Type: ").println(gps_data.type);
            if (gps_data.altitude/100 != 0 and AP.apogeeDetectionSingleKF(gps_data.altitude)) {
                Debug.print("Apogee Detected at: ").print(AP.getCorrectedAltitudeSingleKF()/100).println(" m");
            }
            else {
                Debug.print("Altitude SKF: ").print(AP.getCorrectedAltitudeSingleKF()/100).println(" m");
            }

            delay(100);
            delete (gps_data_t*)data;
    }

    //gps.read();
}

void setup(void) {
	Debug.begin();
	Debug.println("APOGEE_01 example - Test basic detection with Single Kalman");
	Debug.println();
	gps.attachStartDone(startDone);
    gps.attachReadDone(readDone);
    gps.attachStopDone(stopDone);

	gps.start();
	once = false;

}

void loop(void) {

}


