//---------------------------------------------------------------------------

#ifndef LatLonConvH
#define LatLonConvH
#define _USE_MATH_DEFINES
#include <math.h>

#define DEGTORAD(degree) ((M_PI  / 180.0f) * (degree)) // converts from degrees to radians
#define RADTODEG(radian) ((180.0f / M_PI ) * (radian)) // converts from radians to degrees
#define METERS_PER_NAUICAL_MILE 1852.0
#define EllipseMajorInMeters  6378137.000 // wgs 84 in meters
#define EllipseMinorInMeters  6356752.314 // wgs 84 in meters
#define EllipseMajor   (EllipseMajorInMeters/METERS_PER_NAUICAL_MILE)  // wgs 84 in Data Miles
#define EllipseMinor   (EllipseMinorInMeters/METERS_PER_NAUICAL_MILE)  // wgs 84 in Data Miles

typedef enum
{ OKNOERROR  = 0,     // no error
  LONGERR    = 1,     // longitude maximum exceeded
  LATERR     = 2,     // latitude maximum exceeded
  ANTIPODAL  = 3,     // using antipodal points in VInverse
  SAMEPT     = 4,     // lat/longs are same point - VInverse
  ZERODIST   = 5,     // distance is zero - VDirect
  NOCONVERGE = 6      // loop did not converge
} TCoordConvStatus;

 TCoordConvStatus VInverse(double Latitude1, double Longitude1,
						   double Latitude2, double Longitude2,
						   double *Distance, double *Azimuth1,
						   double *Azimuth2);
 TCoordConvStatus VDirect(double Latitude1,  double Longitude1,
						  double Azimuth12,  double Distance,
						  double *Latitude2, double *Longitude2,
						  double *Azimuth21);

 static double Frac(double Num1);
 static double modulus(double Num1, double Num2);
 //static double  modulus (const double  X, double Y);
 static TCoordConvStatus Antipod(double latin,   double lonin,
						  double *latout, double *lonout);
 static bool IsAntipodal(double Latitude1, double Latitude2,
				  double Longitude1,double Longitude2);
 static double sqr(double X);
 static double ModAzimuth(double az);
 static double ModLatitude(double lat);
 static double ModLongitude(double lon);


//---------------------------------------------------------------------------
#endif
