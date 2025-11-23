//---------------------------------------------------------------------------

#ifndef FileSystemStorageH
#define FileSystemStorageH
#include <string>
#include <list>
#include <sys/stat.h>
#include <fcntl.h>

#include "SimpleTileStorage.h"

/**
 * Filesystem cache to store tiles locally.
 *
 * @deprecated This implementation wastes tons of inodes, lots of space, is
 * very slow and ineffective. Will be replaced, see TODO file.
 *
 * @deprecated should be restructured and merged to GoogleLayer
 */
class FilesystemStorage: public SimpleTileStorage {
public:
	/**
	 * Constructor.
	 *
	 * @param root path to root directory of the storage
	 */
	FilesystemStorage(std::string root,bool UseGE);

	/**
	 * Destructor.
	 */
	virtual ~FilesystemStorage();

protected:
	/**
	 * Load/save tile
	 */
	void Process(TilePtr tile);

private:
	/**
	 * Convert tile type and coords into path
	 *
	 * @returns tile path relative to storage root
	 */
	std::string PathFromCoordsGE(int x, int y, int level, int type);
	std::string PathFromCoordsNASA(int x, int y, int level, int type);


private:
	std::string	m_StorageRoot;	///< Root directory of the storage
	bool m_UseGE;
};
//---------------------------------------------------------------------------
#endif
