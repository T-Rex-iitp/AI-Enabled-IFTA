//---------------------------------------------------------------------------

#pragma hdrstop
#include <stdio.h>
#include <string.h>
#include "TimeFunctions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__int64   GetCurrentTimeInMsec(void)
{
  FILETIME FileTime;
  FILETIME LocalFileTime;
  __int64  ReturnValue;

  GetSystemTimeAsFileTime(&FileTime);
  FileTimeToLocalFileTime(&FileTime,&LocalFileTime);
  memcpy(&ReturnValue,&LocalFileTime,sizeof(__int64));
  ReturnValue/=10000;
  return(ReturnValue);
}
//---------------------------------------------------------------------------
char * TimeToChar(__int64 hmsm)
{
char h,m,s;
int ms;
static char timdsp[] = "HH:MM:SS:MMM\0"; /* always the right time  */
h = (hmsm /  3600000)%24;                /* yes: compute hour      */
m = ((hmsm/1000) % 3600)/60;             /* minutes                */
s = (hmsm/1000) % 60;                    /* seconds                */
ms= hmsm %1000;
timdsp[0] = h / 10 + '0';                /* update timdsp with hour*/
timdsp[1] = h % 10 + '0';
timdsp[3] = m / 10 + '0';                /* minutes                */
timdsp[4] = m % 10 + '0';
timdsp[6] = s / 10 + '0';                /* seconds                */
timdsp[7] = s % 10 + '0';
timdsp[9] = ms /100 + '0';               /* m secs                 */
timdsp[10] = (ms /10) %10 + '0';
timdsp[11] = ms % 10 + '0';
return timdsp;
}
//---------------------------------------------------------------------------
