//---------------------------------------------------------------------------

#ifndef GlUtilH
#define GlUtilH
//---------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#ifdef WIN32
#define  _USE_MATH_DEFINES
#endif
#include <math.h>

void glutil_projection2d(double x1, double y1, double x2, double y2);
void glutil_projection3d(double x, double y, double z, double fovx, double fovy);
void glutil_renderstring(int x, int y, char *string);

#endif
