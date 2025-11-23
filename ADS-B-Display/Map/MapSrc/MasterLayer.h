//---------------------------------------------------------------------------

#ifndef MasterLayerH
#define MasterLayerH
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <vector>

#include "Region.h"
#include "Layer.h"
#include "SlaveLayer.h"
#include "VMath.h"

/**
 * Base class for core geographical data handlers
 *
 * Master layers hangle base earth data, such as imaginery and
 * topography. There is only one master layer, but multiple slave
 * layers may be bound to it to display additional information,
 * such as grid or GPS position.
 */
class MasterLayer: public Layer {
public:
	/**
	 * Constructor
	 */
	MasterLayer();

	/**
	 * Destructor
	 */
	virtual ~MasterLayer();

	/**
	 * Render one specific region of earth surface
	 *
	 * @param rgn region to render
	 */
	virtual void RenderRegion(Region *rgn) = 0;

	/**
	 * Bind slave layer to this master layer
	 *
	 * @param layer pointer to SlaveLayer to bind.
	 */
	void BindSlaveLayer(SlaveLayer *layer);

	/**
	 * Unbind all slave layers
	 */
	void ClearSlaveLayers();

protected:
	/**
	 * Pointers to all slave layers currently bound
	 */
	std::vector<SlaveLayer*>	m_SlaveLayers;
};

//---------------------------------------------------------------------------
#endif
