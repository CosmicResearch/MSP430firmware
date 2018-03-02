/**
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <robin.lilja@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. - Robin Lilja
 *
 * @file altitude_kf.h
 * @author Robin Lilja
 * @date 23 Jul 2015
 */

#include "Senscape.h"

#ifndef _ALTITUDE_KF_H_
#define _ALTITUDE_KF_H_

/**
 * A linear Kalman filter estimator of altitude and vertical velocity.
 */
class Altitude_KF {

public:

	/**
	 * Constructor.
	 * @param Q_accel covariance of acceleration input signal (σ^2).
	 * @param R_altitude covariance of the altitude measurement (σ^2).
	 */
	Altitude_KF(float_t Q_accel, float_t R_altitude, float_t Process_noise) {

		this->Q_accel = Q_accel;
		this->R_altitude = R_altitude;
		this->q = Process_noise;

		h = 0.0f;
		v = 0.0f;

		P[0][0] = 0.1f;
		P[0][1] = 0.0f;
		P[1][0] = 0.0f;
		P[1][1] = 0.1f;
	}

	/**
	 * Propagate the state.
	 * @param acceleration vertical acceleration in Earth frame (positive in the zenith direction) [m/s^2].
	 * @param dt update/sampling period [s].
	 */
	void propagate(float_t acceleration, const float_t dt);

	/**
	 * State correction update. Use this method if you use multiple sensors measuring the altitude.
	 * @param altitude measurement of altitude in Earth frame (positive in the zenith direction) [m].
	 * @param R_altitude covariance of the altitude measurement (σ^2).
	 */
	void update(float_t altitude, float_t R_altitude);

	/**
	 * State correction update.
	 * @param altitude measurement of altitude in Earth frame (positive in the zenith direction) [m].
	 */
	void update(float_t altitude) { update(altitude, this->R_altitude); };

	/**
	 * Estimated vertical height or altitude in Earth frame (positive in the zenith direction) [m].
	 */
	float_t h;

	/**
	 * Estimated vertical velocity (positive in the zenith direction) [m/s].
	 */
	float_t v;

	float_t q; //process noise

	/**
	 * Accelerometer covariance.
	 */
	float_t Q_accel;

	/**
	 * Altitude measurement covariance.
	 */
	float_t R_altitude;

private:

	/**
	 * Predicted covariance matrix 'P'.
	 */
	float_t P[2][2];


};

#endif /* _ALTITUDE_KF_H_ */
