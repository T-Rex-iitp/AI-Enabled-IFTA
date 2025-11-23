//---------------------------------------------------------------------------

#ifndef FlatEarthViewH
#define FlatEarthViewH
#include <Windows.h>
#include "glutil.h"
#include "EarthView.h"

/**
 * Flat representation of Earth.
 *
 * Simplest possible earth representation. Orthogonal projection, pitch
 * cannot be changed and north is always 'up'.
 */
class FlatEarthView: public EarthView {
public:
	/**
	 * Constructor.
	 */
	FlatEarthView(MasterLayer *ml);

	/**
	 * Destructor.
	 */
	virtual ~FlatEarthView();

	/**
	 * Render one frame.
	 */
	void Render(bool drawmap);

	/**
	 * Animate view.
	 * 
	 * @param delta time since last frame
	 */
	void Animate(void);

	int StartDrag(int x, int y, int flags);
	int Drag(int fromx, int fromy, int x, int y, int flags);

	int StartMovement(int flags);
	int StopMovement(int flags);
	int SingleMovement(int flags);

private:
	/**
	 * Fix eye coordinates after movements
	 */
	void NormalizeEye();

protected:
	int		m_CurrentMovementFlags;
	Eye		m_SavedZoomEye;
	Eye		m_SavedPanEye;
};


//---------------------------------------------------------------------------
#endif
