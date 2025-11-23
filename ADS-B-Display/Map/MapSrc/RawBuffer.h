//---------------------------------------------------------------------------

#ifndef RawBufferH
#define RawBufferH
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <stdio.h>

#include "Exceptions.h"

/**
 * Simple in-memory buffer.
 */
class RawBuffer {
public:
	/**
	 * Construct RawBuffer object from existing data in memory.
	 *
	 * Allocates memory for a buffer and copies data from given
	 * memory location.
	 *
	 * @param data pointed to beginning of data
	 * @param size size of data in bytes
	 */
	RawBuffer(void *data, size_t size);

	/**
	 * Construct RawBuffer reading data from file.
	 *
	 * Allocates memory for a buffer and fills it with data
	 * read from file. Whole file is read.
	 *
	 * @param f source file descriptor
	 */
	RawBuffer(int f);

	/**
	 * Destructor.
	 *
	 * Frees memory used by buffer.
	 */
	virtual ~RawBuffer();

	/**
	 * Returns data size.
	 */
	size_t Size();

	/**
	 * Returns pointer to data.
	 */
	void *Data();

protected:
	unsigned char	*m_Data;	///< Pointer to data
	size_t		m_Size;		///< Size of data
};

//---------------------------------------------------------------------------
#endif
