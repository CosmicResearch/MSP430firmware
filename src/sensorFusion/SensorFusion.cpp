#include "SensorFusion.h"
#include "math.h"


float SensFusion::q[4];    // vector to hold quaternion
float SensFusion::deltat;
uint32_t SensFusion::lastUpdate;    // used to calculate integration interval
uint32_t SensFusion::Now;           // used to calculate integration interval
boolean_t SensFusion::madgDone;

SensFusion::SensFusion(){
    q[0] = 1.0f; q[1] = 0.0f; q[2] = 0.0f; q[3] = 0.0f;
    deltat = 0.0f;
    lastUpdate = 0;
    Now = 0;
    madgDone = 0;
};



/*
Populates the .pitch/.roll fields in the sensors_vec_t struct
with the right angular data (in degree)
*/
boolean_t SensFusion::accelGetOrientation(adxl377_data_t *accel, sensfusion_data_t *data){
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
	int16_t  *mag_X, *mag_Y, *mag_Z;

	switch (axis){

		case SENSOR_AXIS_X:
		  /* The X-axis is parallel to the gravity */
		  accel_X = accel->_chany;
		  accel_Y = accel->_chanz;
		  accel_Z = accel->_chanx;
		  mag_X = &mag->y;
		  mag_Y = &mag->z;
		  mag_Z = &mag->x;
		  break;

		case SENSOR_AXIS_Y:
		  /* The Y-axis is parallel to the gravity */
		  accel_X = accel->_chany;
		  accel_Y = accel->_chanx;
		  accel_Z = accel->_chany;
		  mag_X = &(mag->z);
		  mag_Y = &(mag->x);
		  mag_Z = &(mag->y);
		  break;

		case SENSOR_AXIS_Z:
		  /* The Z-axis is parallel to the gravity */
		  accel_X = accel->_chanx;
		  accel_Y = accel->_chany;
		  accel_Z = accel->_chany;
		  mag_X = &(mag->x);
		  mag_Y = &(mag->y);
		  mag_Z = &(mag->z);
		  break;

		default:
		  return false;
	}

	float_t t_roll = accel_X * accel_X + accel_Z * accel_Z;
	float_t rollRadians = (float_t) atan2(accel_Y, sqrt(t_roll));

	float_t t_pitch = accel_Y * accel_Y + accel_Z * accel_Z;
	float_t pitchRadians = (float_t) atan2(accel_X, sqrt(t_pitch));

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
boolean_t SensFusion::magGetOrientation(sensaxis_t axis, lsm9ds0_data_t* mag, sensfusion_data_t* data){
	/* Make sure the input is valid, not null, etc. */
	if (mag == NULL) return false;
	if (data == NULL) return false;

	switch (axis){
		case SENSOR_AXIS_X:
		  /* Sensor rotates around X-axis                                                                 */
		  /* "heading" is the angle between the 'Y axis' and magnetic north on the horizontal plane (Oyz) */
		  /* heading = atan(Mz / My)                                                                      */
		  data->heading = (float_t) atan2(mag->z, mag->y) * 180 / PI;
		  break;

		case SENSOR_AXIS_Y:
		  /* Sensor rotates around Y-axis                                                                 */
		  /* "heading" is the angle between the 'Z axis' and magnetic north on the horizontal plane (Ozx) */
		  /* heading = atan(Mx / Mz)                                                                      */
		  data->heading = (float_t) atan2(mag->x, mag->z) * 180 / PI;
		  break;

		case SENSOR_AXIS_Z:
		  /* Sensor rotates around Z-axis                                                                 */
		  /* "heading" is the angle between the 'X axis' and magnetic north on the horizontal plane (Oxy) */
		  /* heading = atan(My / Mx)                                                                      */
		  data->heading = (float_t) atan2(mag->y, mag->x) * 180 / PI;
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
boolean_t SensFusion::fusionGetOrientation(adxl377_data_t* accel, lsm9ds0_data_t* mag, sensfusion_data_t* data){
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
	data->roll = (float_t) atan2(accel->_chany, accel->_chanz);

	/* pitch: Rotation around the Y-axis. -180 <= roll <= 180                                         */
	/* a positive pitch angle is defined to be a clockwise rotation about the positive Y-axis         */
	/*                                                                                                */
	/*                                 -x                                                             */
	/*      pitch = atan(-------------------------------)                                             */
	/*                    y * sin(roll) + z * cos(roll)                                               */
	/*                                                                                                */
	/* where:  x, y, z are returned value from accelerometer sensor                                   */
	if (accel->_chany * sin(data->roll) + accel->_chanz * cos(data->roll) == 0)
		data->pitch = accel->_chanx > 0 ? (PI / 2) : (-PI / 2);
	else
		data->pitch = (float_t) atan(-accel->_chanx / (accel->_chany * sin(data->roll) + accel->_chanz * cos(data->roll)));


	/* heading: Rotation around the Z-axis. -180 <= roll <= 180                                       */
	/* a positive heading angle is defined to be a clockwise rotation about the positive Z-axis       */
	/*                                                                                                */
	/*                                       z * sin(roll) - y * cos(roll)                            */
	/*   heading = atan2(--------------------------------------------------------------------------)  */
	/*                    x * cos(pitch) + y * sin(pitch) * sin(roll) + z * sin(pitch) * cos(roll))   */
	/*                                                                                                */
	/* where:  x, y, z are returned value from magnetometer sensor                                    */
	data->heading = (float_t) atan2(mag->z * sin(data->roll) - mag->y * cos(data->roll), \
	                                  mag->x * cos(data->pitch) + \
	                                  mag->y * sin(data->pitch) * sin(data->roll) + \
	                                  mag->z * sin(data->pitch) * cos(data->roll));


	/* Convert angular data to degree */
	data->roll = data->roll * 180 / PI;
	data->pitch = data->pitch * 180 / PI;
	data->heading = data->heading * 180 / PI;

	return true;
}

// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
void SensFusion::MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
    norm = 1.0f/norm;

    //Debug.print("norm:").println(norm);

    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    //Debug.print("s1").println(s1);
    //Debug.print("s2").println(s2);
    //Debug.print("s3").println(s3);
    //Debug.print("s4").println(s4);

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;


    //Debug.print("qDot1").println(qDot1);
    //Debug.print("qDot2").println(qDot2);
    //Debug.print("qDot3").println(qDot3);
    //Debug.print("qDot4").println(qDot4);


    Now = localTimeMillis()*1000;
    deltat = ((Now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
    lastUpdate = Now;
    ///Debug.print("Milis: ").println( localTimeMillis());
    //Debug.print("Micros: ").println( localTimeMicros());

    //Debug.print("deltat: ").println(deltat);

    // Integrate to yield quaternion
    q1 += qDot1 * deltat;
    q2 += qDot2 * deltat;
    q3 += qDot3 * deltat;
    q4 += qDot4 * deltat;
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;
    //Debug.print("q1 ").println(q1);
    //Debug.print("q2 ").println(q2);
    //Debug.print("q3 ").println(q3);
    //Debug.print("q4 ").println(q4);
    //Debug.print("norm: ").println(norm);
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;
    //Debug.print("q[0] ").println(q[0]);
    //Debug.print("q[1] ").println(q[1]);
    //Debug.print("q[2] ").println(q[2]);
    //Debug.print("q[3] ").println(q[3]);

    madgDone = 1;
}

error_t SensFusion::getMadgwickOrientation(sensfusion_data_t* data){
    if (madgDone) {
        //Debug.print("qDot1 ").println(q[0]);
        //Debug.print("qDot2 ").println(q[1]);
        //Debug.print("qDot3 ").println(q[2]);
        //Debug.print("qDot4 ").println(q[3]);
        float aux1 = 2.0f * (q[1] * q[2] + q[0] * q[3]);
        float aux2 = q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3];
        //Debug.print("aux1: ").print(aux1).print("aux2: ").print(aux2);
        data->heading   = atan2(aux1, aux2);
        data->pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
        data->roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
        data->pitch *= 180.0f / PI;
        data->heading   *= 180.0f / PI;
        //data->heading   -= 13.8; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
        //2.01667
        data->roll  *= 180.0f / PI;
        return SUCCESS;
    }
    return ERROR;
}




