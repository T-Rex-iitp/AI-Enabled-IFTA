//---------------------------------------------------------------------------
#pragma hdrstop

#include "CPA.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#include <stdio.h>
#include <math.h>

#include <stdio.h>
#include <math.h>

// Constants
#define EARTH_RADIUS 6371.0 // Earth's radius in kilometers
#define KNOTS_TO_KMPH 1.852 // Convert knots to km/h
#define KM_TO_NM 0.539957    // Convert km to nautical miles
#define FEET_TO_KM 0.0003048 // Convert feet to km
#define DEG_TO_RAD (M_PI / 180.0) // Degrees to radians

// Structure to store aircraft information
typedef struct {
    double lat;      // Latitude in degrees
    double lon;      // Longitude in degrees
    double speed;    // Speed in knots
    double heading;  // Heading in degrees
    double altitude; // Altitude in feet
} Aircraft;

// Convert lat, lon, and altitude to Earth-Centered Earth-Fixed (ECEF) coordinates
void latLonToECEF(double lat, double lon, double altitude, double *x, double *y, double *z) {
    lat *= DEG_TO_RAD;
    lon *= DEG_TO_RAD;
    altitude *= FEET_TO_KM; // Convert feet to km
    double R = EARTH_RADIUS + altitude;

    *x = R * cos(lat) * cos(lon);
    *y = R * cos(lat) * sin(lon);
    *z = R * sin(lat);
}

// Convert speed and heading to ECEF velocity components
void velocityVector(double lat, double lon, double speed, double heading, double *vx, double *vy, double *vz) {
    speed *= KNOTS_TO_KMPH / 3600.0; // Convert knots to km/s
    heading *= DEG_TO_RAD;
    lat *= DEG_TO_RAD;
    lon *= DEG_TO_RAD;

    // Compute velocity components in the local tangent plane
    double v_north = speed * cos(heading);
    double v_east = speed * sin(heading);

    // Convert velocity components to ECEF coordinates
    *vx = -v_north * sin(lat) * cos(lon) - v_east * sin(lon);
    *vy = -v_north * sin(lat) * sin(lon) + v_east * cos(lon);
    *vz = v_north * cos(lat);
}

// Compute CPA and TCPA
bool computeCPA(double lat1, double lon1,double altitude1, double speed1, double heading1,
				double lat2, double lon2,double altitude2, double speed2, double heading2,
				double &tcpa,double &cpa_distance_nm, double &vertical_cpa) {
    double x1, y1, z1, x2, y2, z2;
    double vx1, vy1, vz1, vx2, vy2, vz2;

    // Convert positions of both aircraft to ECEF coordinates
	latLonToECEF(lat1, lon1, altitude1, &x1, &y1, &z1);
	latLonToECEF(lat2, lon2, altitude2, &x2, &y2, &z2);

    // Compute velocity vectors for both aircraft in ECEF frame
	velocityVector(lat1, lon1, speed1, heading1, &vx1, &vy1, &vz1);
	velocityVector(lat1, lon2, speed2, heading2, &vx2, &vy2, &vz2);

    // Relative position and velocity
    double dx = x2 - x1, dy = y2 - y1, dz = z2 - z1;
    double dvx = vx2 - vx1, dvy = vy2 - vy1, dvz = vz2 - vz1;

    // TCPA Calculation using vector projection
	tcpa = -(dx * dvx + dy * dvy + dz * dvz) / (dvx * dvx + dvy * dvy + dvz * dvz);

    // If TCPA is negative, the aircraft are diverging, no CPA will occur
    if (tcpa < 0) {
        printf("Aircraft are diverging; no CPA will occur.\n");
        return(false);
    }

    // Calculate CPA positions for both aircraft at TCPA time
    double cpa_x1 = x1 + vx1 * tcpa;
    double cpa_y1 = y1 + vy1 * tcpa;
    double cpa_z1 = z1 + vz1 * tcpa;
    double cpa_x2 = x2 + vx2 * tcpa;
    double cpa_y2 = y2 + vy2 * tcpa;
    double cpa_z2 = z2 + vz2 * tcpa;

    // Calculate the horizontal CPA using the great-circle distance (Haversine formula)
	double tlat1 = atan2(cpa_z1, sqrt(cpa_x1 * cpa_x1 + cpa_y1 * cpa_y1));
	double tlon1 = atan2(cpa_y1, cpa_x1);
	double tlat2 = atan2(cpa_z2, sqrt(cpa_x2 * cpa_x2 + cpa_y2 * cpa_y2));
	double tlon2 = atan2(cpa_y2, cpa_x2);

	// Haversine formula for great-circle distance
	double dlat = tlat2 - tlat1;
	double dlon = tlon2 - tlon1;
	double a = sin(dlat / 2) * sin(dlat / 2) +
			   cos(tlat1) * cos(tlat2) * sin(dlon / 2) * sin(dlon / 2);
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double horizontal_cpa = EARTH_RADIUS * c; // CPA in km

    // Vertical separation is the absolute difference in altitude
	vertical_cpa = fabs(altitude1 - altitude2); // in feet

    // Convert CPA distance to nautical miles
	cpa_distance_nm = horizontal_cpa * KM_TO_NM;

    // Output the results
    printf("TCPA: %.2f seconds\n", tcpa);
    printf("CPA Distance: %.2f NM (horizontal), %.2f feet (vertical)\n", cpa_distance_nm, vertical_cpa);
	return(true);
}
 //---------------------------------------------------------------------------
