// OpenStreetGoogleConversion.cpp : Defines the entry point for the console application.
//

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

static void OSM_LatLon2TileNumbers(double LatDeg, double LonDeg, int Zoom, int& XTile, int& YTile);
static void OSM_TileNumbers2LatLon(int XTile, int YTile, int Zoom, double& LatDeg, double& LonDeg);
static void OSM_TileNumbers2GoogleOSM_TileNumbers(int OSM_XTile, int OSM_YTile, int OSM_Zoom,
	int& Google_XTile, int& Google_YTile, int& Google_Zoom);
static void Google_LatLon2TileNumbers(double LatDeg, double LonDeg, int Zoom, int& XTile, int& YTile);

int main()
{
	int OSM_XTile, OSM_YTile, OSM_Zoom;
	int Google_XTile, Google_YTile, Google_Zoom;
	double LatDeg, LonDeg;

	OSM_Zoom = 10;
	Google_Zoom = OSM_Zoom;

	LatDeg = 35.71548879;
	LonDeg = -120.764015;

	//OSM_TileNumbers2LatLon(4,2,3,LatDeg,LonDeg);
	printf("Lat %f Lon %f\n", LatDeg, LonDeg);
	OSM_LatLon2TileNumbers(LatDeg, LonDeg, OSM_Zoom, OSM_XTile, OSM_YTile);
	Google_LatLon2TileNumbers(LatDeg, LonDeg, Google_Zoom, Google_XTile, Google_YTile);
	//OSM_TileNumbers2GoogleOSM_TileNumbers(OSM_XTile,OSM_YTile,OSM_Zoom,Google_XTile,Google_YTile,Google_Zoom);
	printf("http://b.tile.openstreetmap.org/%d/%d/%d.png\n", OSM_Zoom, OSM_XTile, OSM_YTile);
	printf("http://mt2.google.com//vt/lyrs=s@128&hl=en&x=%d&y=%d&z=%d\n", Google_XTile, Google_YTile, Google_Zoom);
	printf("http://www.runwayfinder.com/media/%s/?x=%d&y=%d&z=%d\n", "charts", Google_XTile / 3, Google_YTile / 3, Google_Zoom);

	return 0;
}



static void OSM_LatLon2TileNumbers(double LatDeg, double LonDeg, int Zoom, int& XTile, int& YTile)
{
	double n;
	n = pow(2.0, Zoom);
	XTile = (int)(floor((LonDeg + 180.0) / 360.0 * n));
	YTile = (int)(floor((1.0 - log(tan(LatDeg * M_PI / 180.0) + 1.0 / cos(LatDeg * M_PI / 180.0)) / M_PI) / 2.0 * n));
}

static void OSM_TileNumbers2LatLon(int XTile, int YTile, int Zoom, double& LatDeg, double& LonDeg)
{
	double n, m;
	n = pow(2.0, Zoom);
	m = M_PI - 2.0 * M_PI * YTile / n;
	LatDeg = 180.0 / M_PI * atan(0.5 * (exp(m) - exp(-m)));
	LonDeg = XTile / n * 360.0 - 180.0;
}

static void Google_LatLon2TileNumbers(double LatDeg, double LonDeg, int Zoom, int& XTile, int& YTile)
{

	if (LatDeg > 85.0511287798066) LatDeg = 85.0511287798066;

	double sin_phi = sin(LatDeg * M_PI / 180.0);
	double norm_x = LonDeg / 180;
	double norm_y = (0.5 * log((1 + sin_phi) / (1 - sin_phi))) / M_PI;
	YTile = (int)(pow(2.0, Zoom) * ((1.0 - norm_y) / 2.0));
	XTile = (int)(pow(2.0, Zoom) * ((norm_x + 1.0) / 2.0));
}

static void OSM_TileNumbers2GoogleOSM_TileNumbers(int OSM_XTile, int OSM_YTile, int OSM_Zoom,
	int& Google_XTile, int& Google_YTile, int& Google_Zoom)
{
	Google_XTile = OSM_XTile;
	Google_YTile = OSM_YTile;
	Google_Zoom = 24 - OSM_Zoom;
}
