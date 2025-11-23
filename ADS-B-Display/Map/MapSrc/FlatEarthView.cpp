//---------------------------------------------------------------------------


#pragma hdrstop

#include "FlatEarthView.h"
#include "DisplayGUI.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

FlatEarthView::FlatEarthView(MasterLayer *ml): EarthView(ml) {
	m_CurrentMovementFlags=0;
}

FlatEarthView::~FlatEarthView() {
}

void FlatEarthView::Render(bool drawmap) {
	/* x and y span of viewable size in global coords */
	double yspan = m_Eye.yspan((double)m_ViewportWidth/(double)m_ViewportHeight);
	double xspan = m_Eye.xspan((double)m_ViewportWidth/(double)m_ViewportHeight);

	/* setup projection */
	glMatrixMode(GL_PROJECTION);
	glutil_projection2d(0, 0, m_ViewportWidth, m_ViewportHeight);

	/* calculate virtual coordinates for sides of world rectangle */
	double world_left_virtual = ((-0.5 - m_Eye.x) * (double)m_ViewportWidth/xspan) + double(m_ViewportWidth)/2.0;
	double world_right_virtual = ((0.5 - m_Eye.x) * (double)m_ViewportWidth/xspan) + double(m_ViewportWidth)/2.0;
	double world_top_virtual = ((0.5 - m_Eye.y) * (double)m_ViewportHeight/yspan) + double(m_ViewportHeight)/2.0;
	double world_bottom_virtual = ((-0.5 - m_Eye.y) * (double)m_ViewportHeight/yspan) + double(m_ViewportHeight)/2.0;

	Region rgn(Vector3d(0, 0, 0),
		Vector3d((double)m_ViewportWidth, 0, 0),
		Vector3d((double)m_ViewportWidth, (double)m_ViewportHeight, 0),
		Vector3d(0, (double)m_ViewportHeight, 0),

		Vector2d(m_Eye.x - xspan/2.0, m_Eye.y - yspan/2.0),
		Vector2d(m_Eye.x + xspan/2.0, m_Eye.y + yspan/2.0),

		Vector3d(0, 0, 0),
		Vector3d((double)m_ViewportWidth, 0, 0),
		Vector3d((double)m_ViewportWidth, (double)m_ViewportHeight, 0),
		Vector3d(0, (double)m_ViewportHeight, 0));

	/* tune coords for the cases where earth bounds appear on screen */
	if (world_left_virtual > 0.0) {
		rgn.v[0].x = rgn.v[3].x = world_left_virtual;
		rgn.p[0].x = rgn.p[3].x = world_left_virtual;
		rgn.w[0].x = -0.5;
	}

	if (world_right_virtual < (double)m_ViewportWidth) {
		rgn.v[1].x = rgn.v[2].x = world_right_virtual;
		rgn.p[1].x = rgn.p[2].x = world_right_virtual;
		rgn.w[1].x = 0.5;
	}

	if (world_bottom_virtual > 0.0) {
		rgn.v[0].y = rgn.v[1].y = world_bottom_virtual;
		rgn.p[0].y = rgn.p[1].y = world_bottom_virtual;
		rgn.w[0].y = -0.5;
	}

	if (world_top_virtual < (double)m_ViewportHeight) {
		rgn.v[2].y = rgn.v[3].y = world_top_virtual;
		rgn.p[2].y = rgn.p[3].y = world_top_virtual;
		rgn.w[1].y = 0.5;
	}

   Form1->Map_v[0]= rgn.v[0];
   Form1->Map_v[1]= rgn.v[1];
   Form1->Map_v[2]= rgn.v[2];
   Form1->Map_v[3]= rgn.v[3];

   Form1->Map_w[0]= rgn.w[0];
   Form1->Map_w[1]= rgn.w[1];

   Form1->Map_p[0]= rgn.p[0];
   Form1->Map_p[1]= rgn.p[1];
   Form1->Map_p[2]= rgn.p[2];
   Form1->Map_p[3]= rgn.p[3];


	/* call master layer */
 if (drawmap)	m_MasterLayer->RenderRegion(&rgn);
}

void FlatEarthView::Animate(void) {

	NormalizeEye();
}

/* mouse navigation */
int FlatEarthView::StartDrag(int x, int y, int flags) {
	if (flags & NAV_DRAG_PAN)
		m_SavedPanEye = m_Eye;
	if (flags & NAV_DRAG_ZOOM)
		m_SavedZoomEye = m_Eye;

	return 1;
}

int FlatEarthView::Drag(int fromx, int fromy, int x, int y, int flags) {
	double yspan = m_Eye.yspan((double)m_ViewportWidth/(double)m_ViewportHeight);
	double xspan = m_Eye.xspan((double)m_ViewportWidth/(double)m_ViewportHeight);

	if (flags & NAV_DRAG_PAN) {
		m_Eye.y = m_SavedPanEye.y + double(y - fromy)/double(m_ViewportHeight)*yspan;
		m_Eye.x = m_SavedPanEye.x - double(x - fromx)/double(m_ViewportWidth)*xspan;	/* not exactly correct, but usable */
	}
	if (flags & NAV_DRAG_ZOOM) {
		if (y - fromy < 0)
			m_Eye.h = m_SavedZoomEye.h * (1.0 + double(y - fromy)/double(m_ViewportHeight));
		else 
			m_Eye.h = m_SavedZoomEye.h / (1.0 - double(y - fromy)/double(m_ViewportHeight));
	}

	return 1;
}

/* keyboard navigation */
int FlatEarthView::StartMovement(int flags) {
	m_CurrentMovementFlags |= flags;
	return 1;
}

int FlatEarthView::StopMovement(int flags) {
	m_CurrentMovementFlags &= ~flags;
	return 1;
}

int FlatEarthView::SingleMovement(int flags) {
	if (flags & NAV_ZOOM_IN)
		m_Eye.h /= 1.3;
	if (flags & NAV_ZOOM_OUT)
		m_Eye.h *= 1.3;
	NormalizeEye();

	return 1;
}

/* private flat-specific functions */
void FlatEarthView::NormalizeEye() {
	if (m_Eye.x < -0.5)	m_Eye.x = -0.5;
	if (m_Eye.x > 0.5)	m_Eye.x = 0.5;
	if (m_Eye.y < -0.5)	m_Eye.y = -0.5;
	if (m_Eye.y > 0.5)	m_Eye.y = 0.5;

#define MIN_HEIGHT 10.0/40000000.0
#define MAX_HEIGHT 1.0
	if (m_Eye.h < MIN_HEIGHT)	m_Eye.h = MIN_HEIGHT;
	if (m_Eye.h > MAX_HEIGHT)	m_Eye.h = MAX_HEIGHT;
}

