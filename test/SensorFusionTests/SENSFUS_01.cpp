/*
 * @author David Williams <davidwcorral@hotmail.com>
 * @date   MARCH 12, 2018
 */


#include "Senscape.h"
#include "SensorFusion.h"
#include "SensADC.h"
#include "Accelerometer.h"
#include "Magnetometer.h"

Resource mag_resource = Resource(ARBITER_USCIB_0);

SensADC _adcx =  SensADC(
				ADC_CHANNEL_1,
				REFERENCE_AVcc_AVss,
				REFVOLT_LEVEL_NONE,
				SHT_SOURCE_ACLK,
				SHT_CLOCK_DIV_1,
				SAMPLE_HOLD_4_CYCLES,
				SAMPCON_SOURCE_SMCLK,
				SAMPCON_CLOCK_DIV_1);

SensADC _adcy =  SensADC(
				ADC_CHANNEL_2,
				REFERENCE_AVcc_AVss,
				REFVOLT_LEVEL_NONE,
				SHT_SOURCE_ACLK,
				SHT_CLOCK_DIV_1,
				SAMPLE_HOLD_4_CYCLES,
				SAMPCON_SOURCE_SMCLK,
				SAMPCON_CLOCK_DIV_1);

SensADC _adcz =  SensADC(
				ADC_CHANNEL_3,
				REFERENCE_AVcc_AVss,
				REFVOLT_LEVEL_NONE,
				SHT_SOURCE_ACLK,
				SHT_CLOCK_DIV_1,
				SAMPLE_HOLD_4_CYCLES,
				SAMPCON_SOURCE_SMCLK,
				SAMPCON_CLOCK_DIV_1);

Accelerometer ACC (_adcx, _adcy, _adcz);
Magnetometer  MAG (&Spi0, &mag_resource, M_SCALE_2GS, M_ODR_125);

adxl377_data_t* data_acc;
lsm9ds0_data_t* data_mag;
sensfusion_data_t* data;
SensorFusion SF();

boolean_t acc_read, mag_read;

void onReadDoneAcc(sensor_data_t* data, error_t result);
void onStartDoneAcc(error_t result);
void onReadDoneMag(sensor_data_t* data, error_t result);
void onStartDoneMag(error_t result);

void setup(void) {
	Debug.begin();

	// TOSO F("...")
	Debug.println("SENSFUS_01 example - Test Sensor Fusion");
	Debug.println();

	data* = new sensfusion_data_t;

	acc_read = mag_read = 0;

	MAG.attachReadDone(onReadDoneMag);
	MAG.attachStartDone(onStartDoneMag);
	MAG.start();

	ACC.attachReadDone(onReadDoneAcc);
	ACC.attachStartDone(onStartDoneAcc);
	ACC.start();
}

void loop(void) {
	if (acc_read and mag_read){
		SF.accelGetOrientation(data_acc, data);

		SF.magTiltCompensation(SENSOR_AXIS_X, data_mag, data_acc);
		SF.magTiltCompensation(SENSOR_AXIS_Y, data_mag, data_acc);
		SF.magTiltCompensation(SENSOR_AXIS_Z, data_mag, data_acc);

		SF.magGetOrientation(SENSOR_AXIS_X, data_mag, sensfusion_data_t* data)
		//SF.magGetOrientation(SENSOR_AXIS_Y, data_mag, sensfusion_data_t* data)
		//SF.magGetOrientation(SENSOR_AXIS_Z, data_mag, sensfusion_data_t* data)

		Debug.print("Pitch: ").println(data->pitch);
		Debug.print("Roll: ").println(data->roll);
		Debug.print("Heading: ").println(data->heading);

		Debug.println("---------------------------------------------------------------------");
		Debug.delay(1000);

		acc_read = 0;
		mag_read = 0;
		MAG.read();
		ACC.read();
	}
}

void onStartDoneMag(error_t result){
	if (result == SUCCESS) {

		// 3. Read sensor.
		Debug.println();
		Debug.print("Reading Magnetometer ... ");
		MAG.read();
	}
	else {
		Debug.println("error");
	}
}

void onReadDoneMag(sensor_data_t* data_t, error_t result) {
    if (result == SUCCESS) {
    		Debug.println();
    		Debug.print("Magnetometer has been read... ");
    		data = (lsm9ds0_data_t*) data_t;
    		mag_read = 1;

	}
	else {
		Debug.println("error in Magnetometer, trying to read again ...");
		MAG.read();
	}
}

void onStartDoneAcc(error_t result){
	if (result == SUCCESS) {

		// 3. Read sensor.
		Debug.println();
		Debug.print("Reading Accelerometer ... ");
		ACC.read();
	}
	else {
		Debug.println("error");
	}
}

void onReadDoneAcc(sensor_data_t* data_t, error_t result) {
    if (result == SUCCESS) {
    		Debug.println();
    		Debug.print("Accelerometer has been read... ");
    		data = (adxl377_data_t*) data_t;
    		acc_read = 1;

	}
	else {
		Debug.println("error in Accelerometer, trying to read again ...");
		ACC.read();
	}
}

