#include "gefetch_internal.h"

/**
 * Return size of last downloaded file
 */
size_t gefetch_get_data_size(gefetch *handle) {
	return handle->currentdata ? handle->currentsize : 0;
}

/**
 * Return pointed to buffer containing last downloaded file
 */
void *gefetch_get_data_ptr(gefetch_t handle) {
	return handle->currentdata;
}
