#include "gefetch_internal.h"

#define DEFAULT_MAX_METASIZE 1*1024*1024

/**
 * Initialize gefetch library
 */
gefetch *gefetch_init(const char *url) {
	gefetch *handle;

	/* allocate structure */
	if ((handle = (gefetch*)malloc(sizeof(gefetch))) == 0)
		return 0;

	memset(handle, 0, sizeof(gefetch));

	/* init certain fields */
	handle->maxmetasize = DEFAULT_MAX_METASIZE;

	if ((handle->url = (char*)malloc(strlen(url)+1)) == 0) {
		gefetch_cleanup(handle);
		return 0;
	}

	strcpy(handle->url, url);

	return handle;
}

/**
 * Destroy gefetch handle and free all data used by library
 */
void gefetch_cleanup(gefetch *handle) {
	if (handle->url)
		free(handle->url);

	if (handle->sid)
		free(handle->sid);

	if (handle->currentdata)
		free(handle->currentdata);

	if (handle->dbrootdata)
		free(handle->dbrootdata);

	free(handle);
}

/**
 * Set different address of Google server
 *
 * @url new address (like http://kh.google.com)
 */
gefetch_error gefetch_set_url(gefetch *handle, char *url) {
	char *newurl = handle->url;
	if (strlen(handle->url) < strlen(url))
		if ((newurl = (char*)realloc(handle->url, strlen(url) + 1)) == 0)
			return GEFETCH_NOMEM;

	strcpy(newurl, url);

	handle->url = newurl;

	return GEFETCH_OK;
}
