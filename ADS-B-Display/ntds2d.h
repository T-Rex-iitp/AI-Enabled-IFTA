//---------------------------------------------------------------------------

#ifndef NTDS2DH
#define NTDS2DH
int MakeAirplaneImages(void);
void MakeAirTrackFriend(void);
void MakeAirTrackHostile(void);
void MakeAirTrackUnknown(void);
void MakePoint(void);
void MakeTrackHook(void);
void DrawAirTrackFriend(float x,float y);
void DrawAirTrackHostile(float x,float y);
void DrawAirTrackUnknown(float x,float y);
void DrawPoint(float x,float y);
void DrawAirplaneImage(float x, float y,float scale,float heading,int imageNum);
void DrawTrackHook(float x, float y);
void DrawRadarCoverage(float xc, float yc, float major, float minor);
void DrawLeader(float x1, float y1, float x2, float y2);
void ComputeTimeToGoPosition(float  TimeToGo,
							 float  xs, float  ys,
							 float  xv, float  yv,
							 float &xe, float &ye);
 void DrawLines(DWORD resolution, double xpts[],double ypts[]);				   

//---------------------------------------------------------------------------
#endif

