//---------------------------------------------------------------------------

#ifndef CPAH
#define CPAH
//---------------------------------------------------------------------------
bool computeCPA(double lat1, double lon1,double altitude1, double speed1, double heading1,
				double lat2, double lon2,double altitude2, double speed2, double heading2,
				double &tcpa,double &cpa_distance_nm, double &vertical_cpa);
#endif
