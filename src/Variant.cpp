/*
    Bondar avionics
    Copyright (C) 2017  Associació Cosmic Research
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Variant.h>

Variant::Variant(gps_data_t data) {
    this->data.gps_data = data;
    this->type = GPS_DATA;
}

Variant::Variant(accel_data_t data) {
    this->data.accel_data = data;
    this->type = ACCEL_DATA;
}

Variant::Variant(mag_data_t data) {
    this->data.mag_data = data;
    this->type = MAG_DATA;
}

Variant::Variant(gyro_data_t data) {
    this->data.gyro_data = data;
    this->type = GYRO_DATA;
}

Variant::Variant(bar_data_t data) {
    this->data.bar_data = data;
    this->type = BAR_DATA;
}

Variant::Variant(kalman_data_t data) {
    this->data.kalman_data = data;
    this->type = KALMAN_DATA;
}

Variant::Variant(sensfusion_data_t data) {
    this->data.sensfusion_data = data;
    this->type = SENSFUSION_DATA;
}

Variant::Variant(uint8_t data) {
    this->data.uint8 = data;
    this->type = UINT8;
}

Variant::Variant(void* data) {
    this->data.unknown_data = data;
    this->type = UNKNOWN;
}

 gps_data_t Variant::toGPSData(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == GPS_DATA);
    if (this->type == GPS_DATA) {
        return this->data.gps_data;
    }
    return gps_data_t();
}

 accel_data_t Variant::toAccelData(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == ACCEL_DATA);
    if (this->type == ACCEL_DATA) {
        return this->data.accel_data;
    }
    return accel_data_t();
}

 mag_data_t Variant::toMagData(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == MAG_DATA);
    if (this->type == MAG_DATA) {
        return this->data.mag_data;
    }
    return mag_data_t();
}

 gyro_data_t Variant::toGyroData(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == GYRO_DATA);
    if (this->type == GYRO_DATA) {
        return this->data.gyro_data;
    }
    return gyro_data_t();
}

 bar_data_t Variant::toBarData(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == BAR_DATA);
    if (this->type == BAR_DATA) {
        return this->data.bar_data;
    }
    return bar_data_t();
}

 kalman_data_t Variant::toKalmanData(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == KALMAN_DATA);
    if (this->type == KALMAN_DATA) {
        return this->data.kalman_data;
    }
    return kalman_data_t();
}

 sensfusion_data_t Variant::toSensFusionData(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == SENSFUSION_DATA);
    if (this->type == SENSFUSION_DATA) {
        return this->data.sensfusion_data;
    }
    return sensfusion_data_t();
}

 uint8_t Variant::toUInt8(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == UINT8);
    if (this->type == UINT8) {
        return this->data.uint8;
    }
    return 0;
}

void* Variant::toVoidPointer(bool* ok) {
    if (ok != NULL)
        *ok = (this->type == UNKNOWN);
    if (this->type == UNKNOWN) {
        return this->data.unknown_data;
    }
    return NULL;
}

data_base_t Variant::toDataBase(bool* ok) {
    if (ok != NULL) {
        *ok = (this->type != UNKNOWN && this->type != UINT8 && this->type != NONE);
    }
    switch (this->type) {

    case GPS_DATA:
        return this->toGPSData(ok);
        break;
    case ACCEL_DATA:
        return this->toAccelData(ok);
        break;
    case GYRO_DATA:
        return this->toGyroData(ok);
        break;
    case MAG_DATA:
        return this->toMagData(ok);
        break;
    case BAR_DATA:
        return this->toBarData(ok);
        break;
    case KALMAN_DATA:
        return this->toKalmanData(ok);
        break;
    case SENSFUSION_DATA:
        return this->toSensFusionData(ok);
        break;
    default:
        return data_base_t();

    }
}
