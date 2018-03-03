#include "SensorFusion.h"

/*
Populates the .pitch/.roll fields in the sensors_vec_t struct
with the right angular data (in degree)
*/
boolean_t SensFusion::accelGetdata(adxl377_data_t *accel, sensfusion_data_t *data){
	if (accel == NULL) return false;
	if (data == NULL) return false;

	float_t  t_pitch, t_roll, t_heading;
    float_t  signOfZ = accel->_chanz >= 0 ? 1.0F : -1.0F;

    t_roll = accel->_chanx * accel->_chanx + accel->_chanz * accel->_chanz;
  	data->roll = (float_t )atan2(accel->_chany, sqrt(t_roll)) * 180 / PI;

	t_pitch = accel->_chany * accel->_chany + accel->_chanz * accel->_chanz;
	data->pitch = (float_t )atan2(accel->_chanx, signOfZ * sqrt(t_pitch)) * 180 / PI;

	return true;
}

/*
Utilize the sensor data from an accelerometer to compensate
the magnetic sensor measurements when the sensor is tilted
(the pitch and roll angles are not equal 0°)
*/
boolean_t SensFusion::magTiltCompensation(sensaxis_t axis, lsm9ds0_data_t* mag, adxl377_data_t* accel){
	if (mag == NULL) return false;
  	if (accel == NULL) return false;

	float_t  accel_X, accel_Y, accel_Z;
	float_t  *mag_X, *mag_Y, *mag_Z;

	switch (axis){

		case SENSOR_AXIS_X:
		  /* The X-axis is parallel to the gravity */
		  accel_X = accel->chany;
		  accel_Y = accel->chanz;
		  accel_Z = accel->chanx;
		  mag_X = &(mag->y);
		  mag_Y = &(mag->z);
		  mag_Z = &(mag->x);
		  break;

		case SENSOR_AXIS_Y:
		  /* The Y-axis is parallel to the gravity */
		  accel_X = accel->chany;
		  accel_Y = accel->chanx;
		  accel_Z = accel->chany;
		  mag_X = &(mag->z);
		  mag_Y = &(mag->x);
		  mag_Z = &(mag->y);
		  break;

		case SENSOR_AXIS_Z:
		  /* The Z-axis is parallel to the gravity */
		  accel_X = accel->chanx;
		  accel_Y = accel->chany;
		  accel_Z = accel->chany;
		  mag_X = &(mag->x);
		  mag_Y = &(mag->y);
		  mag_Z = &(mag->z);
		  break;

		default:
		  return false;
	}

	float_t t_roll = accel_X * accel_X + accel_Z * accel_Z;
	float_t rollRadians = (float_t )atan2(accel_Y, sqrt(t_roll));

	float_t t_pitch = accel_Y * accel_Y + accel_Z * accel_Z;
	float_t pitchRadians = (float_t )atan2(accel_X, sqrt(t_pitch));

	float_t cosRoll = (float_t )cos(rollRadians);
	float_t sinRoll = (float_t )sin(rollRadians);
	float_t cosPitch = (float_t )cos(-1*pitchRadians);
	float_t sinPitch = (float_t )sin(-1*pitchRadians);

	/* The tilt compensation algorithm                            */
	/* Xh = X.cosPitch + Z.sinPitch                               */
	/* Yh = X.sinRoll.sinPitch + Y.cosRoll - Z.sinRoll.cosPitch   */
	float_t raw_mag_X = *mag_X;
	float_t raw_mag_Y = *mag_Y;
	float_t raw_mag_Z = *mag_Z;
	*mag_X = (raw_mag_X) * cosPitch + (raw_mag_Z) * sinPitch;
	*mag_Y = (raw_mag_X) * sinRoll * sinPitch + (raw_mag_Y) * cosRoll - (raw_mag_Z) * sinRoll * cosPitch;

	return true;
}

/*
Populates the .heading fields in the sensors_vec_t
struct with the right angular data (0-359°)
*/
boolean_t magGetOrientation(sensaxis_t axis, lsm9ds0_data_t* mag, sensfusion_data_t* data){
	/* Make sure the input is valid, not null, etc. */
	if (mag == NULL) return false;
	if (data == NULL) return false;

	switch (axis){
		case SENSOR_AXIS_X:
		  /* Sensor rotates around X-axis                                                                 */
		  /* "heading" is the angle between the 'Y axis' and magnetic north on the horizontal plane (Oyz) */
		  /* heading = atan(Mz / My)                                                                      */
		  data->heading = (float_t tan2(mag->z, mag->y) * 180 / PI;
		  break;

		case SENSOR_AXIS_Y:
		  /* Sensor rotates around Y-axis                                                                 */
		  /* "heading" is the angle between the 'Z axis' and magnetic north on the horizontal plane (Ozx) */
		  /* heading = atan(Mx / Mz)                                                                      */
		  data->heading = (float_t tan2(mag->x, mag->z) * 180 / PI;
		  break;

		case SENSOR_AXIS_Z:
		  /* Sensor rotates around Z-axis                                                                 */
		  /* "heading" is the angle between the 'X axis' and magnetic north on the horizontal plane (Oxy) */
		  /* heading = atan(My / Mx)                                                                      */
		  data->heading = (float_t tan2(mag->y, mag->x) * 180 / PI;
		  break;

		default:
		  return false;
	}

	/* Normalize to 0-359° */
	if (data->heading < 0){
		data->heading = 360 + data->heading;
	}

	return true;
}

/*
Populates the .roll/.pitch/.heading fields in the sensfusion_data_t
struct with the right angular data (in degree).
The starting position is set by placing the object flat and
pointing northwards (Z-axis pointing upward and X-axis pointing
northwards).
The orientation of the object can be modeled as resulting from
3 consecutive rotations in turn: heading (Z-axis), pitch (Y-axis),
and roll (X-axis) applied to the starting position.
*/
boolean_t fusionGetOrientation(adxl377_data_t* accel, lsm9ds0_data_t* mag, sensfusion_data_t* data){
	/* Make sure the input is valid, not null, etc. */
	if ( accel  == NULL) return false;
	if ( mag    == NULL) return false;
	if ( data  == NULL) return false;


	/* roll: Rotation around the X-axis. -180 <= roll <= 180                                          */
	/* a positive roll angle is defined to be a clockwise rotation about the positive X-axis          */
	/*                                                                                                */
	/*                    y                                                                           */
	/*      roll = atan2(---)                                                                         */
	/*                    z                                                                           */
	/*                                                                                                */
	/* where:  y, z are returned value from accelerometer sensor                                      */
	data->roll = (float_t tan2(accel->_chany, accel->_chanz);

	/* pitch: Rotation around the Y-axis. -180 <= roll <= 180                                         */
	/* a positive pitch angle is defined to be a clockwise rotation about the positive Y-axis         */
	/*                                                                                                */
	/*                                 -x                                                             */
	/*      pitch = atan(-------------------------------)                                             */
	/*                    y * sin(roll) + z * cos(roll)                                               */
	/*                                                                                                */
	/* where:  x, y, z are returned value from accelerometer sensor                                   */
	if (accel->_chany * sin(orientation->roll) + accel->_chanz * cos(data->roll) == 0)
		data->pitch = accel->_chanx > 0 ? (PI / 2) : (-PI / 2);
	else
		data->pitch = (float_t tan(-accel->_chanx / (accel->_chany * sin(data->roll) + \
	                                                                 accel->_chan.z * cos(data->roll)));

	/* heading: Rotation around the Z-axis. -180 <= roll <= 180                                       */
	/* a positive heading angle is defined to be a clockwise rotation about the positive Z-axis       */
	/*                                                                                                */
	/*                                       z * sin(roll) - y * cos(roll)                            */
	/*   heading = atan2(--------------------------------------------------------------------------)  */
	/*                    x * cos(pitch) + y * sin(pitch) * sin(roll) + z * sin(pitch) * cos(roll))   */
	/*                                                                                                */
	/* where:  x, y, z are returned value from magnetometer sensor                                    */
	data->heading = (float_t tan2(mag->z * sin(data->roll) - mag->y * cos(data->roll), \
	                                  mag->x * cos(data->pitch) + \
	                                  mag->y * sin(data->pitch) * sin(data->roll) + \
	                                  mag->z * sin(data->pitch) * cos(data->roll));


	/* Convert angular data to degree */
	data->roll = data->roll * 180 / PI_F;
	data->pitch = data->pitch * 180 / PI_F;
	data->heading = data->heading * 180 / PI_F;

	return true;
}


