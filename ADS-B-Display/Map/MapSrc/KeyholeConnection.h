//---------------------------------------------------------------------------

#ifndef KeyholeConnectionH
#define KeyholeConnectionH
#include <gefetch.h>

#include "SimpleTileStorage.h"

#define GoogleMaps             0
#define SkyVector_VFR          1
#define SkyVector_IFR_Low      2
#define SkyVector_IFR_High     3
#define SkyVector              4

/**
 * Connection to Google server.
 *
 * Handles and encapsulates all network stuff, authentification and
 * downloading of tiles from Google servers. Of course, only handles
 * loading of tiles.
 *
 * @deprecated should be restructured and merged to GoogleLayer
 */
class KeyholeConnection: public SimpleTileStorage {
public:
	/**
	 * Constructor.
	 */
	KeyholeConnection(int Type);

	/**
	 * Destructor.
	 */
	virtual ~KeyholeConnection();

protected:
	/**
	 * Download tile from google.
	 */
	void Process(TilePtr tile);

private:
	gefetch_t	m_GEFetch;
	int         ServerType;
	const char  *Key;
	const char  *Chart;
	const char  *Edition;
};

//---------------------------------------------------------------------------
#endif
