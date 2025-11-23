//---------------------------------------------------------------------------

#ifndef SimpleTileStorageH
#define SimpleTileStorageH
#include <queue>

#include "Exceptions.h"
#include "TileStorage.h"
#include "global.h"

/**
 * Base class for tile storages with queue and one working thread;
 *
 * @deprecated should be restructured and merged to GoogleLayer
 */
class SimpleTileStorage: public TileStorage {
public:
	/**
	 * Constructor.
	 */
	SimpleTileStorage();

	/**
	 * Destructor.
	 */
	virtual ~SimpleTileStorage();

	/**
	 * Insert tile into processing queue.
	 */
	void Enqueue(TilePtr tile);

	/**
	 * Set next storage for tile loading.
	 *
	 * If we cannot load given tile, it will be passed to
	 * NextLoadStorage.
	 */
	void SetNextLoadStorage(TileStorage *ts);

	/**
	 * Set storage for tile saving.
	 *
	 * After successfull loading, tile will be passed to
	 * SaveStorage to be saved.
	 */
	void SetSaveStorage(TileStorage *ts);

	/**
	 * Stop passing tiles to other storages.
	 *
	 * Need to be called before destruction of storage.
	 */
	void Detach();

protected:
	/**
	 * Do actual processing - defined in derived class.
	 */
	virtual void Process(TilePtr tile) = 0;

private:
	/**
	 * Entry point for worker thread.
	 */
	static DWORD WINAPI ThreadEntryPoint(LPVOID  pthis);

	/**
	 * Thread function.
	 *
	 * This function loops endlessly taking tiles from queue one
	 * by one and processig them with Process()
	 */
	void ThreadRun();

/* variables */
private:
	std::queue<TilePtr>	m_Queue;	///< Queue of tiles waiting to be saved/loaded with this storage

	HANDLE			m_QueueMutex;		///< Mutex to protect queue
	HANDLE          SemQueueCount;      ///<  Condition to wake up thread after sleep
	HANDLE          ThreadKillEvent;
	HANDLE          m_Thread;		///< Thread in which all actual loading/saving goes asynchronously

protected:
	TileStorage	*m_pNextLoadStorage;	///< Storage to pass tile to for loading, if we couldn't load it
	TileStorage	*m_pSaveStorage;	///< Storage to pass tile to for saving
};

//---------------------------------------------------------------------------
#endif
