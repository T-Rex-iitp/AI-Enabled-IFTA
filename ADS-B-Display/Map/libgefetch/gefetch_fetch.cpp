
#include <stdio.h>
#include <stdlib.h>
#include "gefetch_internal.h"
/**
 * Fetch arbitary URI from Google Earth's servers
 *
 * @param uri URI requested (usually starting with /)
 */
gefetch_error gefetch_fetch_uri(gefetch *handle, char *uri) {
	/* form full url */
	char urlbuf[1024];
	if (_snprintf(urlbuf, sizeof(urlbuf), "%s%s", handle->url, uri) >= sizeof(urlbuf))
		return GEFETCH_SMALL_BUFFER;

	/* fetch */
	return gefetch_fetch(handle, urlbuf);
}

/**
 * Internal function to fetch data from Google Earth's servers
 *
 * Given full URL, does all required HTTP requests to server to
 * get requested data. Automatically fetches dbRoot and does
 * authentication if needed. Also decrypts data.
 *
 * @param URL full URL for request
 */
gefetch_error gefetch_fetch(gefetch *handle, char *url) {
	gefetch_error result;
	long code;

	/* try to get a file */
	if ((result = gefetch_do_http_request(handle, METHOD_GET, url, 0, 0, &code)) != GEFETCH_OK)
		return result;

	/* success */
	if (code == 200) {
		return GEFETCH_OK;
	}

	/* not found */
	if (code == 404)
		return GEFETCH_NOT_FOUND;

	/* other result codes */
	return GEFETCH_FETCH_FAILED;
}
