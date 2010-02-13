/*
 *  B21 Driver - By David Lu!! 2/2010
 *  Modified from Player code
 *
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2000
 *     Brian Gerkey, Kasper Stoy, Richard Vaughan, & Andrew Howard
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <rflex/b21_driver.h>
#include <rflex/b21_config.h>
#include <math.h>

B21::B21() {
    // empty constructor
}

B21::~B21() {
    // empty destructor
}

float B21::getDistance() const {
    return distance / (float) ODO_DISTANCE_CONVERSION;
}

float B21::getBearing() const {
    return (bearing - HOME_BEARING) / (float) ODO_ANGLE_CONVERSION;
}

float B21::getTranslationalVelocity() const {
    return transVelocity / (float) ODO_DISTANCE_CONVERSION;
}

float B21::getRotationalVelocity() const {
    return rotVelocity / (float) ODO_ANGLE_CONVERSION;
}

float B21::getVoltage() const {
    if (voltage==0.0)
        return 0.0;
    else
        return voltage/100.0 + POWER_OFFSET;
}

bool B21::isPluggedIn() const {
    float v = getVoltage();
    if (v>PLUGGED_THRESHOLD)
        return true;
    else
        return false;
}

int B21::getNumBodySonars() const {
    return SONARS_PER_RING[BODY_INDEX];
}

int B21::getNumBaseSonars() const {
    return SONARS_PER_RING[BASE_INDEX];
}

void B21::getBodySonarReadings(float* readings) const {
    getSonarReadings(BODY_INDEX, readings);
}

void B21::getBaseSonarReadings(float* readings) const {
    getSonarReadings(BASE_INDEX, readings);
}

void B21::getBodySonarPoints(sensor_msgs::PointCloud* cloud) const {
    getSonarPoints(BODY_INDEX, cloud);
}
void B21::getBaseSonarPoints(sensor_msgs::PointCloud* cloud) const {
    getSonarPoints(BASE_INDEX, cloud);
}

void B21::setSonarPower(bool on) {
    unsigned long echo, ping, set, val;
    if (on) {
        echo = SONAR_ECHO_DELAY;
        ping = SONAR_PING_DELAY;
        set = SONAR_SET_DELAY;
        val = 2;
    } else {
        echo = ping = set = val = 0;
    }
    configureSonar(echo, ping, set, val);
}


void B21::setMovement( float tvel, float rvel,
                       float acceleration ) {
    setVelocity(tvel * ODO_DISTANCE_CONVERSION,
                rvel * ODO_ANGLE_CONVERSION,
                acceleration * ODO_DISTANCE_CONVERSION);
}


void B21::getSonarReadings(const int ringi, float* adjusted_ranges) const {
    int i = 0;
    for (int x = SONAR_RING_BANK_BOUND[ringi];x<SONAR_RING_BANK_BOUND[ringi+1];x++) {
        for (int y=0;y<SONARS_PER_BANK[x];y++) {
            int range = sonar_ranges[x*SONAR_MAX_PER_BANK+y];
            if (range > SONAR_MAX_RANGE)
                range = SONAR_MAX_RANGE;
            float fRange = range / (float) RANGE_CONVERSION;
            adjusted_ranges[i] = fRange;
            i++;
        }
    }
}

void B21::getSonarPoints(const int ringi, sensor_msgs::PointCloud* cloud) const {
    int numSonar = SONARS_PER_RING[ringi];
    float* readings = new float[numSonar];
    getSonarReadings(ringi, readings);
    cloud->set_points_size(numSonar);
    int c = 0;
    for (int i = 0; i < numSonar; ++i) {
        if (readings[i] < SONAR_MAX_RANGE/ (float) RANGE_CONVERSION) {
            double angle =  SONAR_RING_START_ANGLE[ringi] + SONAR_RING_ANGLE_INC[ringi]*i;
            angle *= M_PI / 180.0;

            double d = SONAR_RING_DIAMETER[ringi] + readings[i];
            cloud->points[c].x = cos(angle)*d;
            cloud->points[c].y = sin(angle)*d;
            cloud->points[c].z = SONAR_RING_HEIGHT[ringi];
            c++;
        }
    }
}


