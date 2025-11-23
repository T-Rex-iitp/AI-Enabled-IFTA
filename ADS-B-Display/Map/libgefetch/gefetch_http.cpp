#include "gefetch_internal.h"
#include "Request.h"

/**
 * Internal function to form and execute single HTTP request
 *
 * @param method request method (METHOD_POST | METHOD_GET)
 * @param url full URL for request
 * @param postdata (POST only) pointer to data to send to server
 * @param postsize (POST only) length of postdata
 * @param code address of long to store HTTP response code to
 */
gefetch_error gefetch_do_http_request(gefetch *handle, int method, const char *url, void *postdata, size_t postsize, long *code) {
	long * res;

	/* free last downloaded file, if any */
	if (handle->currentdata)
		free(handle->currentdata);

	handle->currentdata = 0;
	handle->currentsize = 0;

 Request req;
 HTTPRequest ret;

 if (!req.SendRequest(method==METHOD_POST,url,(char *)postdata, postsize,&ret,(char *)handle->sid, code))
 {
  handle->currentdata=(unsigned char *)ret.message;
  handle->currentsize=ret.messageLength;
  return GEFETCH_OK;
 }
 else  return GEFETCH_FETCH_FAILED;
}
