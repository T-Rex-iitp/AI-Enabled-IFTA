//---------------------------------------------------------------------------

#ifndef TileH
#define TileH
//---------------------------------------------------------------------------
#include "Timer.h"
#include "SmartPtr.h"
#include "RawBuffer.h"
#include "global.h"

enum {
	TILETYPE_NONE,
	TILETYPE_TEXTURE
};

class Tile;
typedef SmartPtr<Tile> TilePtr;

/**
 * Base class for all earth data split into tiles.
 *
 * @todo tiles should be more abstract (so we won't need to change
 * EarthView class to support new tiles). Think how to make it
 * possible to introduce new tile types without changing View code,
 * for example wi-fi hotspot data.
 * Also dynamic tiles may be introduced (like weather data).
 * We have endless possiblities.
 */
class Tile {
	friend class	SmartPtr<Tile>;

public:
	/**
	 * Constructor. Creates empty tile.
	 *
	 * @param x x coordinate of tile
	 * @param y y coordinate of tile
	 * @param level level of tile
	 */
	Tile(int x, int y, int level);

	/**
	 * Destructor.
	 */
	virtual ~Tile();

	/**
	 * Sets tile last access time to `now'.
	 */
	void Touch();

	/**
	 * Returns time (in frames) passed from last tile usage.
	 */
	ticks_t GetAge();

	/**
	 * Test whether this tile os old.
	 *
	 * In practice this means that this tile was not used
	 * int last frame renderer.
	 */
	int IsOld();

	/**
	 * Get X-coordinate of a tile
	 */
	int GetX();

	/**
	 * Get Y-coordinate of a tile
	 */
	int GetY();

	/**
	 * Get level of a tile
	 */
	int GetLevel();

	/**
	 * Get type of a tile.
	 *
	 * Should be redefined in derived classes.
	 */
	virtual int GetType();

	/**
	 * Load actual data into tile.
	 *
	 * @param data buffer containing raw (compressed) tile data
	 * @param keep whether to store raw data (for later saving)
	 *
	 * For image tiles, for example, \a data contains compressed
	 * JPEG data, and `loading' means uncompressing it and preparing
	 * for usage as texture in OpenGL. If \a keep is set, \a data isr
	 * preserved insided for later saving. \a data should not be
	 * used after it was passed to Tile.
	 *
	 * @see ReleaseRawData
	 */
	virtual void Load(RawBuffer *data, int keep);

	/**
	 * Give back the raw data.
	 * RawBuffer returned by this function should be delete'd by caller.
	 *
	 * @return pointer RawBuffer containing raw tile data.
	 *
	 * @see Load
	 */
	virtual RawBuffer *ReleaseRawData();

	/**
	 * Test whether there's data to save
	 *
	 * @see Load, ReleaseRawData
	 */
	virtual int IsSaveable();

	/**
	 * Test whether this tile is loaded with data.
	 *
	 * @see Load
	 */
	virtual int IsLoaded();

	/**
	 * Test whether this tile may be actually used.
	 */
	virtual int IsReady();

	/**
	 * Inits tile as dummy object.
	 *
	 * Null tiles are `known-not-to-exist-anywhere'.
	 * No attempts will be made to load this tile from anywhere or
	 * to use it (for example, in rendering).
	 */
	virtual void Null();

	/**
	 * Test whether this is null tile.
	 */
	virtual int IsNull();

protected:
	int     m_X;            ///< X coordinate. Range is [0..(2^level)-1]
	int     m_Y;            ///< Y coordinate. Range is [0..(2^level)-1]
	int     m_Level;        ///< Level. Range is [0..] (actually [0..18])

	ticks_t m_LastUsed;	///< Last time the tile was used

	int	m_IsNull;	///< Indicates whether this tile is know to not exist 
	RawBuffer	*m_RawData;	///< Raw (compressed) data stored to be saved later

protected:
    int IncRef();               ///< Return resulting value after increment
    int DecRef();               ///< Return resulting value after decrement
    HANDLE  m_rcMutex;          ///< Mutex to protect refcount

	unsigned int	refcount;	///< Reference counter, see SmartPtr
};


#endif
