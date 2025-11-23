#ifndef __GEFETCH_H__
#define __GEFETCH_H__
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* error codes */
typedef enum {
	/* no error */
	GEFETCH_OK,

	/* no memory */
	GEFETCH_NOMEM,

	/* curl perform failed */
	GEFETCH_INVALID_ZOOM,

    GEFETCH_INVALID_CHART,

	/* google returned corrupt data */
	GEFETCH_CORRUPT_DATA,

	/* constant size buffer too small for data */
	GEFETCH_SMALL_BUFFER,

	/* unable to fetch data requested */
	GEFETCH_FETCH_FAILED,

	/* data requested does not exist */
	GEFETCH_NOT_FOUND,
} gefetch_error;

typedef struct gefetch *gefetch_t;

/* init/cleanup */
gefetch_t	gefetch_init(const char * url);
void		gefetch_cleanup(gefetch_t handle);

/* options */
gefetch_error	gefetch_set_max_metasize(gefetch_t handle, size_t size);
gefetch_error	gefetch_set_url(gefetch_t handle, char *url);

/* fetch specific types of data */
gefetch_error	gefetch_fetch_uri(gefetch_t handle, char *uri);
gefetch_error	gefetch_fetch_image_googlemaps(gefetch_t handle, int x, int y, int level);
gefetch_error	gefetch_fetch_image_skyvector(gefetch_t handle, const char *key,const char *chart, const char * edition, int x, int y, int level);
/* retrieve data from library */
size_t		gefetch_get_data_size(gefetch_t handle);
void		*gefetch_get_data_ptr(gefetch_t handle);

#ifdef __cplusplus
}
#endif

#endif
