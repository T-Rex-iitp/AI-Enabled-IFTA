//---------------------------------------------------------------------------
#include "LatLonConv.h"

//---------------------------------------------------------------------------

#define EPS 0.00000000000005          // a small number
//---------------------------------------------------------------------------
TCoordConvStatus VInverse(double Latitude1, double Longitude1,
						  double Latitude2, double Longitude2,
						  double *Distance,   double *Azimuth12,
						  double *Azimuth21)
{

  int  icount,MAXCT=100;   //max loops
  double lat1, long1, lat2, long2, A, a0, B, b0, flat, C, r, s, u2, x,
		 sinu1, sinu2, cosu1, cosu2, tanu1, tanu2, sigma ,lambda,
		 testlambda, L, s2s, ss,c2sm, cs, sinalpha, cosalpha, dsigma, 
		 alpha12, alpha21;

  lat1 = DEGTORAD(Latitude1);
  lat2 = DEGTORAD(Latitude2);
  long1 = DEGTORAD(Longitude1);
  long2 = DEGTORAD(Longitude2);
  icount = 0;

  //check for antipodal points
  if (IsAntipodal(Latitude1, Latitude2,
				 Longitude1,Longitude2))  return(ANTIPODAL);

  //same point detection
	if ((fabs(Latitude1 - Latitude2) < 0.000001) &&
		(fabs(Longitude1 - Longitude2) < 0.000001))
	  {
		*Azimuth12 = 0.0;
		*Azimuth21 = 0.0;
		*Distance = 0.0;
		return(SAMEPT);
	 }

	a0 = EllipseMajor;
	b0 = EllipseMinor;
	flat = (a0 - b0) / a0;
	r = 1.0 - flat;

	tanu1 = r * tan(lat1);
	tanu2 = r * tan(lat2);

	x = atan(tanu1);
	cosu1 = cos(x);
	sinu1 = sin(x);

	x = atan(tanu2);
	cosu2 = cos(x);
	sinu2 = sin(x);

	L = long2 - long1;
	lambda = L;

   do
	 {
	  icount++;
	  testlambda = lambda;

	  //equation 14
	  s2s = sqr(cosu2 * sin(lambda)) +
			  sqr(cosu1 * sinu2 - sinu1 * cosu2 * cos(lambda));
	  ss = fabs(sqrt(s2s));  //sin(sigma)
	  //equation 15
	  cs = sinu1 * sinu2 + cosu1 * cosu2 * cos(lambda);  //cos(sigma)

	  sigma = atan2(ss,cs);    //ArcCos(cs);
	  //no need for equation 16
	  //equation 17
	  sinalpha = cosu1 * cosu2 * sin(lambda) / ss;
	  x = asin(sinalpha);
	  cosalpha = cos(x);
	  //equation 18
	  c2sm = cs - (2.0 * sinu1 * sinu2 / sqr(cosalpha));
	  //equation 10
	  C = flat / 16.0 *
		   sqr(cosalpha) *
		   (4.0 + flat *
		   (4.0 - 3.0 * sqr(cosalpha)));

	  //equation 11
	  lambda = L +
				(1.0 - C) *
				flat * sinalpha *
				(sigma + C * ss *
				(c2sm + C *
				cs * (-1.0 + 2.0 * sqr(c2sm))));
	}
	while ((fabs(testlambda - lambda) > EPS) && (icount <= MAXCT));

	//test failure to converge
	if (icount > MAXCT) return(NOCONVERGE);

	u2 = sqr(cosalpha) *
		  (sqr(a0) - sqr(b0)) / sqr(b0);

	A =  1.0 +
		  (u2 / 16384.0) *
		  (4096.0 + u2 *
		  (-768.0 + u2 *
		  (320.0 - 175.0 * u2)));

	B = (u2 / 1024.0) *
		 (256.0 + u2 *
		 (-128.0 + u2 *
		 (74.0 - 47.0 * u2)));

	dsigma = B * ss *
			  (c2sm + (B / 4.0) *
			  (cs * (-1.0 + 2.0 * sqr(c2sm)) -
			  (B / 6.0) * c2sm *
			  (-3.0 + 4.0 * sqr(ss)) *
			  (-3.0 + 4.0 * sqr(c2sm))));

	s = b0 * A * (sigma - dsigma);

	alpha12 = atan2(cosu2 * sin(lambda),
	  (cosu1 * sinu2 - sinu1 * cosu2 * cos(lambda)));

	alpha21 = atan2(cosu1 * sin(lambda),
		(-sinu1 * cosu2 + cosu1 * sinu2 * cos(lambda))) - M_PI;

	alpha12 = ModAzimuth(alpha12);      //constrain value
	alpha21 = ModAzimuth(alpha21);      //ditto
	*Azimuth12 = RADTODEG(alpha12);   //convert to decimal degrees
	*Azimuth21 = RADTODEG(alpha21);   //ditto
	*Distance = s;                    //distance in meters
	 return OKNOERROR;
}
//---------------------------------------------------------------------------
TCoordConvStatus VDirect(double Latitude1,  double Longitude1,
						  double Azimuth12,  double Distance,
						  double *Latitude2, double *Longitude2,
						  double *Azimuth21)
{
   double lat1, lat2, long1, long2, az12, az21, s, a0, b0, flat, r, u1,
		  tanu1, sinu1, cosu1, tansigma1, sinalpha, cosalpha, a, b, c, usqr,
		  sigma, sigma1, deltasigma, twosigmam, lastsigma, term1,
		  term2, term3, lambda, omega, ss, cs, c2sm;

	lat1 = DEGTORAD(Latitude1);
	long1 = DEGTORAD(Longitude1);
	az12 = DEGTORAD(Azimuth12);
	s = Distance;

	//handle zero distance
	if (s==0.0)
	  {
		*Latitude2 = Latitude1;
		*Longitude2 = Longitude1;
		*Azimuth21 = 0.0;
		return(ZERODIST);
	  }

	a0 = EllipseMajor;
	b0 = EllipseMinor;
	flat = (a0 - b0) / a0;
	r = 1.0 - flat;
	tanu1 = r * tan(lat1);

	tansigma1 = tanu1 / cos(az12);          //eq 1

	u1 = atan(tanu1);
	sinu1 = sin(u1);
	cosu1 = cos(u1);

	sinalpha = cosu1 * sin(az12);           //eq 2
	cosalpha = sqrt(1.0 - sqr(sinalpha));

	usqr = sqr(cosalpha) * (sqr(a0) - sqr(b0)) / sqr(b0);

	term1 = usqr / 16384.0;
	term2 = 4096.0 + usqr * (-768.0 + usqr * (320.0 - 175.0 * usqr));
	a = 1.0 + term1 * term2;
	b = usqr / 1024.0 * (256.0 + usqr * (-128.0 + usqr *
		  (74.0 - 47.0 * usqr)));            //eq 4

	sigma = s / (b0 * a);
	sigma1 =atan(tansigma1);

	do
	 {
	  lastsigma = sigma;
	  twosigmam = 2.0 * sigma1 + sigma;     //eq 5
	  ss = sin(sigma);                      //sin(sigma)
	  cs = cos(sigma);                      //cos(sigma)
	  c2sm = cos(twosigmam);                //cos(twosigmam)

	  deltasigma = b * ss * (c2sm + b / 4.0 * (cs * (-1.0 + 2.0 * sqr(c2sm)) -
		  b / 6.0 * c2sm * (-3.0 + 4.0 * sqr(ss)) *
		  (-3.0 + 4.0 * sqr(c2sm))));        //eq 6

	  sigma = s / (b0 * a) + deltasigma;    //eq 7
	 }
	while (fabs(sigma - lastsigma) > EPS);

	twosigmam = 2.0 * sigma1 + sigma;       //eq 5
	ss = sin(sigma);                        //sin(sigma)
	cs = cos(sigma);                        //cos(sigma)
	c2sm = cos(twosigmam);                  //cos(twosigmam)
	term1 = sinu1 * cs + cosu1 * ss * cos(az12);
	term2 = sqr(sinalpha) + sqr(sinu1 * ss - cosu1 * cs * cos(az12));
	term3 = r * sqrt(term2);
	lat2 = atan2(term1, term3);

	term1 = ss * sin(az12);
	term2 = cosu1 * cs - sinu1 * ss * cos(az12);
	lambda = atan2(term1, term2);

	c = flat / 16.0 * sqr(cosalpha) * (4.0 + flat *
		  (4.0 - 3.0 * sqr(cosalpha)));

	omega = lambda - (1.0 - c) * flat * sinalpha *
		  (sigma + c * ss * (c2sm + c * cs * (-1.0 + 2.0 * sqr(c2sm))));

	long2 = long1 + omega;

	term1 = -sinu1 * ss + cosu1 * cs * cos(az12);
	az21 = atan2(sinalpha, term1);
	az21 = ModAzimuth(az21) - M_PI;


	lat2 = ModLatitude(lat2);
	long2 = ModLongitude(long2);


	*Latitude2 = RADTODEG(lat2);
	*Longitude2 = RADTODEG(long2);
	*Azimuth21 = RADTODEG(az21);
	if (*Azimuth21 > 360.0)
	  *Azimuth21 = *Azimuth21 - 360.0;
	if (*Azimuth21 < 0.0)
	  *Azimuth21 = *Azimuth21 + 360.0;


  return(OKNOERROR);
}
//---------------------------------------------------------------------------
 static bool IsAntipodal(double Latitude1, double Latitude2,
						 double Longitude1,double Longitude2)
 {
   double la,lo;
   int eflag;

   eflag = Antipod(Latitude1, Longitude1, &la, &lo);  //compute antipodal values
  if (eflag != OKNOERROR)  return(false);

  if ((Latitude2 == la) && (Longitude2 == lo)) return(true);
  else return(false);
 }
//---------------------------------------------------------------------------
 static TCoordConvStatus Antipod(double latin,   double lonin,
								 double *latout, double *lonout)
{
  //this function returns the antipod of latin, lonin in latout, lonout
  //NOTE: angles are assumed to be in decimal degrees

  //function returns:
  //   OkNOERROR if successful and latout and lonouut contain the
  //       antipodal values otherwise they are indeterminate. I set them
  //       both to 9999.0 degrees.
  //   LONGERR if abs(lonin) is > 180 degrees
  //   LATERR if abs(latin) is > 90 degrees


  if (fabs(lonin) > 180.0)
	{
	  *latout = 9999.0;
	  *lonout = 9999.0;
	  return(LONGERR);
	}

  if (fabs(latin) > 90.0)
	{
	  *latout = 9999.0;
	  *lonout = 9999.0;
	  return(LATERR);

	}

  *latout = -latin;
  *lonout= modulus(lonin + 180.0, 180.0);
  return(OKNOERROR);

}
//---------------------------------------------------------------------------
#if 0
static double modulus (double const X, double Y)
{
 double Z,Result;

  Result = X / Y;
  Z = (int)Result;
  if (Frac(Result) < 0.0) Z = Z - 1.0;
  Result = X - Y * Z;
  return(Result);
}
#endif
//---------------------------------------------------------------------------
static double modulus(double Num1, double Num2)
{
  return(Num1 - Num2 * floor(Num1 / Num2));
}
//---------------------------------------------------------------------------
static double Frac(double Num1)
{
 return(Num1-((int)Num1));
}
//---------------------------------------------------------------------------
static double sqr(double X)
{
 return(X*X);
}
//---------------------------------------------------------------------------
static double ModAzimuth(double az)
{
 return(modulus(az, 2.0 * M_PI));
}
//---------------------------------------------------------------------------
static double ModLatitude(double lat)
{
 return(modulus(lat + M_PI / 2.0, M_PI) - M_PI / 2.0);
}
//---------------------------------------------------------------------------
static double ModLongitude(double lon)
{
 return(modulus(lon + M_PI, 2.0 * M_PI) - M_PI);
}
//---------------------------------------------------------------------------

