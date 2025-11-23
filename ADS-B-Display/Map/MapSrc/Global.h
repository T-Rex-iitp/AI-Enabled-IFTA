//---------------------------------------------------------------------------

#ifndef GlobalH
#define GlobalH
#include "Timer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#include <time.h>
#define  _USE_MATH_DEFINES
#include <math.h>
#endif

#ifdef DEBUG
void debug(const char *fmt, ...);
#else
#define debug(format, args...)
#endif

void warning(const char *fmt, ...);
void fatal(const char *fmt, ...);

//---------------------------------------------------------------------------
#endif
