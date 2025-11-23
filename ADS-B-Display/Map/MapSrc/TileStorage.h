#ifndef __TILESTORAGE_H__
#define __TILESTORAGE_H__

#include "Tile.h"

/**
 * Abstract tile storage.
 *
 * Tile storages deal with loading and saving of all tiles.
 *
 * @deprecated should be restructured and merged to GoogleLayer
 */
class TileStorage {
public:
	/**
	 * Constructor.
	 */
	TileStorage() {}

	/**
	 * Destructor.
	 */
	virtual ~TileStorage() {}

	/**
	 * Start procesing of tile, whatever that means.
	 */
	virtual void Enqueue(TilePtr tile) = 0;
};

#endif
