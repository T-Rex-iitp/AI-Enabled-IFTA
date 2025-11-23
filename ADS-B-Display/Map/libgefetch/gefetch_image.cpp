#include <stdio.h>
#include <stdlib.h>
#include "gefetch_internal.h"

static int int_pow(int base, int exponent);
static int int_pow(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}
/**
 * Fetch single image from Google server
 *
 * @param x x coordinate of image [0..(2^level-1)]
 * @param y y coordinate of image [0..(2^level-1)]
 * @param level level of image [0..]
 */
gefetch_error gefetch_fetch_image_googlemaps(gefetch *handle, int x, int y, int level) {
	/* form full url */
	char urlbuf[1024];
	int correct = int_pow(2,level)-1;
	y=correct-y;

	if (_snprintf(urlbuf, sizeof(urlbuf),"%s/vt/lyrs=y&x=%d&y=%d&z=%d", handle->url,x,y,level) >= sizeof(urlbuf))
		return GEFETCH_SMALL_BUFFER;
	/* fetch */
	return gefetch_fetch(handle, urlbuf);
}

gefetch_error gefetch_fetch_image_skyvector(gefetch *handle,const char *key,const char *chart,const char * edition,int x, int y, int level) {
	/* form full url */
	char urlbuf[1024];
	int newlevel = 23 - (2 * level);
	int correct = int_pow(2,level)-1;
	int ichart=atoi(chart);

	y=correct-y;

	newlevel = 23 + 301-ichart - (2 * level);

	//printf("new level %d Level %d Ichart %d\n",newlevel,level,ichart);

	if (ichart==301)
	 {
	  if ((newlevel>21) || (newlevel<1)) return GEFETCH_INVALID_ZOOM;
	 }
	else if (ichart==302)
	 {
	   if ((newlevel>20) || (newlevel<2)) return GEFETCH_INVALID_ZOOM;
	 }
	else if (ichart==304)
	 {
       if ((newlevel>18) || (newlevel<2)) return GEFETCH_INVALID_ZOOM;
	 }
	 else return GEFETCH_INVALID_CHART;


	if (_snprintf(urlbuf, sizeof(urlbuf),"%s/%s/%s/%s/%d/%d/%d.jpg", handle->url,key,chart,edition,newlevel,x,y) >= sizeof(urlbuf))
	{
        printf("error\n");
		return GEFETCH_SMALL_BUFFER;
	}
	//printf("%s\n",urlbuf);
	/* fetch */
	return gefetch_fetch(handle, urlbuf);
}
