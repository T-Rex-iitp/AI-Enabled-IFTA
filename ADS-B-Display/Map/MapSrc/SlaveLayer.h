//---------------------------------------------------------------------------

#ifndef SlaveLayerH
#define SlaveLayerH
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "Layer.h"
#include "Math.h"
#include "Region.h"

/* layer draws over earth surface */
#define SLAVELAYERCAP_OVERDRAW		0x01

/* layer uses multitexturing to draw data over earth surface */
/* #define SLAVELAYERCAP_OVERTEXTURE	0x02 */

/* render non-region-bound data once every frame */
/* #define SLAVELAYERCAP_DRAWONCE	0x04 */

/**
 * Base class for handling extra geo-bound data
 *
 *
 */
class SlaveLayer: public Layer {
public:
	/**
	 * Constructor
	 */
	SlaveLayer();

	/**
	 * Destructor
	 */
	virtual ~SlaveLayer();

	/**
	 * Check whether capability is supported bu this layer
	 */
	int GetCap(int cap);

	/* virtual int BindOvertexture(Vector3d &worldPoint1, Vector3d &worldPoint2, Vector3d &worldPoint3); */

	/**
	 * Render layer data as region
	 *
	 * @param rgn region to render
	 *
	 * This function is similar to MasterLayer::RenderRegion,
	 * and is called when slave layer has OVERDRAW capability set.
	 */
	virtual void Overdraw(Region *rgn);

protected:
	/**
	 * Layer capabilities
	 *
	 * Bitfield indicationg in which ways layer's data may be
	 * rendered. Most common is OVERDRAW, which works similar
	 * to MasterLayer::RenderRegion
	 */
	int	m_Caps;
};

//---------------------------------------------------------------------------
#endif
