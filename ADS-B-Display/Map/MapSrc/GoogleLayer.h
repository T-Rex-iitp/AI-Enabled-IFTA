//---------------------------------------------------------------------------

#ifndef GoogleLayerH
#define GoogleLayerH

#include "MasterLayer.h"
#include "TileManager.h"

#define MIN_TEXTURE_DISTANCE 192.0

class GoogleLayer: public MasterLayer {
public:
	GoogleLayer(TileManager *tm);
	virtual ~GoogleLayer();

	void RenderRegion(Region *rgn);

	int GetSplitLevel(double wlen, double plen);

protected:
	TileManager	*m_TileManager;
};

//---------------------------------------------------------------------------
#endif
