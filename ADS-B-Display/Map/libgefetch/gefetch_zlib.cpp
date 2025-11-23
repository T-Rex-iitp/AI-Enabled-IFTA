
#include "gefetch_internal.h"

/**
 * Internal function to decompress data received from Google
 *
 * Deccompresses file stored in handle->currentdata. Google uses
 * following format for compressed files:
 *
 * offs size desc
 *    0    4 ?, probably signature
 *    4    4 Size of uncompressed data
 *    8    * Zlib-compressed data
 */
gefetch_error gefetch_decompress_current_file(gefetch *handle) {
	/* sanity */
	if (handle->currentsize < 8)
		return GEFETCH_CORRUPT_DATA;

	/* size of decompressed data - from google's header */
	uLongf decomp_size = ((uLongf)handle->currentdata[4]) |
		((uLongf)handle->currentdata[5] << 8) |
		((uLongf)handle->currentdata[6] << 16) |
		((uLongf)handle->currentdata[7] << 24);

	/* real size of decompressed data - may be changed by zlib */
	uLongf real_decomp_size = decomp_size;

	/* allocate decompressed data */
	unsigned char *decompressed;
	if ((decompressed = (unsigned char*)malloc(decomp_size)) == 0)
		return GEFETCH_NOMEM;

	/* decompress */
	if (uncompress(decompressed, &real_decomp_size, &handle->currentdata[8], (uLong)handle->currentsize-8) != Z_OK || real_decomp_size != decomp_size) {
		free(decompressed);
		return GEFETCH_CORRUPT_DATA;
	}

	/* free unneeded compressed data */
	free(handle->currentdata);
	handle->currentdata = decompressed;
	handle->currentsize = decomp_size;

	return GEFETCH_OK;
}
