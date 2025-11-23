//---------------------------------------------------------------------------
#pragma hdrstop

#include "GlUtil.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void glutil_projection2d(double x1, double y1, double x2, double y2) {
	double m[16] = {
		2.0/(x2-x1),        0,                  0,   0,
		0,                  2.0/(y2-y1),        0,   0,
		0,                  0,                  1.0, 0,
		1.0+2.0*x2/(x1-x2), 1.0+2.0*y2/(y1-y2), 0,   1.0,
	};

	glLoadMatrixd(m);
}

void glutil_projection3d(double x, double y, double z, double fovy, double aspect) {
	glLoadIdentity();
	gluPerspective(fovy/M_PI*180.0, aspect, 0.001, 10);
	glTranslated(0, 0, -z);
	glRotated(-90.0 + y*360.0, 1, 0, 0);
	glRotated(-x*360.0, 0, 0, 1);
}

#define FONT_TEXTURE_SIZE	256
#define FONT_CHARACTER_WIDTH	8
#define FONT_CHARACTER_HEIGHT	14

void glutil_renderstring(int x, int y, char *string) {
	unsigned int i;
	unsigned int c;

	if (strlen(string) == 0)
		return;

	int cx = x;
	int cy = y;

	glBegin(GL_QUADS);
	for (i = 0; i < strlen(string); i++) {
		c = string[i];

		if (c == '\n') {
			cx = x;
			cy += FONT_CHARACTER_HEIGHT;
		} else {
			glTexCoord2f((float)(c%16)*(float)FONT_CHARACTER_WIDTH/FONT_TEXTURE_SIZE, (float)(c/16)*(float)FONT_CHARACTER_HEIGHT/FONT_TEXTURE_SIZE);                    
			glVertex3f((float)cx, (float)cy, 0.0);

			glTexCoord2f((float)(c%16+1)*(float)FONT_CHARACTER_WIDTH/FONT_TEXTURE_SIZE, (float)(c/16)*(float)FONT_CHARACTER_HEIGHT/FONT_TEXTURE_SIZE);          
			glVertex3f((float)(cx + FONT_CHARACTER_WIDTH), (float)cy, 0.0);

			glTexCoord2f((float)(c%16+1)*(float)FONT_CHARACTER_WIDTH/FONT_TEXTURE_SIZE, (float)(c/16+1)*(float)FONT_CHARACTER_HEIGHT/FONT_TEXTURE_SIZE);
			glVertex3f((float)(cx + FONT_CHARACTER_WIDTH), (float)(cy+FONT_CHARACTER_HEIGHT), 0.0);

			glTexCoord2f((float)(c%16)*(float)FONT_CHARACTER_WIDTH/FONT_TEXTURE_SIZE, (float)(c/16+1)*(float)FONT_CHARACTER_HEIGHT/FONT_TEXTURE_SIZE);
			glVertex3f((float)cx, (float)(cy+FONT_CHARACTER_HEIGHT), 0.0);

			cx += FONT_CHARACTER_WIDTH;
		}
	}
	glEnd();
}

