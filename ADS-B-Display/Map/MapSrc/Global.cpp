//---------------------------------------------------------------------------


#pragma hdrstop

#include "Global.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)


#ifdef WIN32
#include <windows.h>
int gettimeofday(struct timeval *tv, void *)
{
    // not quite correct translation, but for logging it's enough
    FILETIME ft;
    GetSystemTimeAsFileTime(  &ft );
    tv->tv_sec = ft.dwHighDateTime;
    tv->tv_usec = ft.dwLowDateTime;
    return 0;
}
#endif

#ifdef DEBUG
void debug(const char *fmt, ...) {
	struct timeval tv;
	gettimeofday(&tv, 0);

	fprintf(stderr, "%ld.%06ld ", tv.tv_sec, tv.tv_usec);

	va_list arglist;
	va_start(arglist, fmt);
	vfprintf(stderr, fmt, arglist);
	va_end(arglist);
}
#endif

void warning(const char *fmt, ...) {
	struct timeval tv;
	gettimeofday(&tv, 0);

	fprintf(stderr, "%ld.%06ld ", tv.tv_sec, tv.tv_usec);

	va_list arglist;
	va_start(arglist, fmt);
	vfprintf(stderr, fmt, arglist);
	va_end(arglist);
}

void fatal(const char *fmt, ...) {
	struct timeval tv;
	gettimeofday(&tv, 0);

	fprintf(stderr, "%ld.%06ld ", tv.tv_sec, tv.tv_usec);

	va_list arglist;
	va_start(arglist, fmt);
	vfprintf(stderr, fmt, arglist);
	va_end(arglist);

	exit(1);
}
