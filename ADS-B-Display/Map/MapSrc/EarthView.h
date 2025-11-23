//---------------------------------------------------------------------------

#ifndef EarthViewH
#define EarthViewH
#include <Windows.h>
#include "Viewpoint.h"
#include "MasterLayer.h"

#define NAV_DRAG_PAN	0x01
#define NAV_DRAG_ZOOM	0x02

#define NAV_PAN_LEFT	0x01
#define NAV_PAN_RIGHT	0x02
#define NAV_PAN_UP	0x04
#define NAV_PAN_DOWN	0x08

#define NAV_ZOOM_IN	0x10
#define NAV_ZOOM_OUT	0x20

/**
 * Abstract class for earth view.
 */
class EarthView {
public:
	EarthView(MasterLayer *ml);
	virtual ~EarthView();

	virtual void Render(bool drawmap) = 0;
	virtual void Animate(void) = 0;

	void Resize(int width, int height);

	virtual int StartDrag(int x, int y, int flags);
	virtual int Drag(int fromx, int fromy, int x, int y, int flags);
	virtual int Click(int x, int y, int flags);

	virtual int StartMovement(int flags);
	virtual int StopMovement(int flags);
	virtual int SingleMovement(int flags);
	Eye             m_Eye;			///< Current viewer's position
protected:
	MasterLayer     *m_MasterLayer;		///< Master layer to use

	int		m_ViewportWidth;	///< Viewport width in pixels
	int		m_ViewportHeight;	///< Viewport height in pixels
};
//---------------------------------------------------------------------------
#endif
