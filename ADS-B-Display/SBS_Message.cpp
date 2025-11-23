//---------------------------------------------------------------------------

#pragma hdrstop
#include "DisplayGUI.h"
#include "Aircraft.h"
#include "SBS_Message.h"
#include "TimeFunctions.h"
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#define SMALL_VAL        0.0001
#define BIG_VAL          9999999.0
#define VALID_POS(pos)   (fabs(pos->Longitude) >= SMALL_VAL && fabs(pos->Longitude) < 180.0 && \
						  fabs(pos->Latitude) >= SMALL_VAL && fabs(pos->Latitude) < 90.0)

#define SBS_MESSAGE_TYPE      0
#define SBS_TRANSMISSION_TYPE 1
#define SBS_SESSION_ID        2
#define SBS_AIRCRAFT_ID       3
#define SBS_HEX_INDENT        4
#define SBS_FLIGHT_ID         5
#define SBS_DATE_GENERATED    6
#define SBS_TIME_GENERATED    7
#define SBS_DATE_LOGGED       8
#define SBS_TIME_LOGGED       9
#define SBS_CALLSIGN          10
#define SBS_ALTITUDE          11
#define SBS_GROUND_SPEED      12
#define SBS_TRACK_HEADING     13
#define SBS_LATITUDE          14
#define SBS_LONGITUDE         15
#define SBS_VERTICAL_RATE     16
#define SBS_SQUAWK            17
#define SBS_ALERT             18
#define SBS_EMERGENCY         19
#define SBS_SBI               20
#define SBS_IS_ON_GROUND      21


static char *strsep (char **stringp, const char *delim);
static  int hexDigitVal(int c);
static const char *get_SBS_timestamp (void);
static void get_FILETIME_now (FILETIME *ft);
static uint32_t aircraft_get_addr (uint8_t a0, uint8_t a1, uint8_t a2);
//---------------------------------------------------------------------------
static  int hexDigitVal(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    else return -1;
}
//---------------------------------------------------------------------------
static char *strsep (char **stringp, const char *delim)
{
  char *start = *stringp;
  char *ptr;

  if (start == NULL)
    return NULL;

  /* Optimize the case of no delimiters.  */
  if (delim[0] == '\0')
    {
      *stringp = NULL;
      return start;
    }

  /* Optimize the case of one delimiter.  */
  if (delim[1] == '\0')
    ptr = strchr (start, delim[0]);
  else
    /* The general case.  */
    ptr = strpbrk (start, delim);
  if (ptr == NULL)
    {
      *stringp = NULL;
      return start;
    }

  *ptr = '\0';
  *stringp = ptr + 1;

  return start;
}
//---------------------------------------------------------------------------

/**
 * Return a double-timestamp for the SBS output.
 */
static const char *get_SBS_timestamp (void)
{
  int         ts_len;
  char        ts_buf [30];
  static char timestamp [2*sizeof(ts_buf)];

  FILETIME    ft;
  SYSTEMTIME  st;

  get_FILETIME_now (&ft);
  FileTimeToSystemTime (&ft, &st);
  ts_len = snprintf (ts_buf, sizeof(ts_buf), "%04u/%02u/%02u,%02u:%02u:%02u.%03u,",
                     st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

  /* Since the date,time,date,time is just a repeat, build the whole string once and
   * then add it to each MSG output.
   */
  strcpy (timestamp, ts_buf);
  strcat (timestamp, ts_buf);
  timestamp [ts_len-1] = '\0';    /* remove last ',' */
  return (timestamp);
}
//---------------------------------------------------------------------------
static void get_FILETIME_now (FILETIME *ft)
{
  GetSystemTimePreciseAsFileTime (ft);
}
//---------------------------------------------------------------------------
/**
 * Convert 24-bit big-endian (network order) to host order format.
 */
static uint32_t aircraft_get_addr (uint8_t a0, uint8_t a1, uint8_t a2)
{
  return ((a0 << 16) | (a1 << 8) | a2);
}
//char  msg [MODES_MAX_SBS_SIZE];
//---------------------------------------------------------------------------
bool ModeS_Build_SBS_Message (const modeS_message *mm, TADS_B_Aircraft *a, char *msg)
{
   char *p = msg;
  int   emergency = 0, ground = 0, alert = 0, spi = 0;
  const char *date_str;

  if (mm->msg_type == 4 || mm->msg_type == 5 || mm->msg_type == 21)
  {
    /**\note
     * identity is calculated/kept in base10 but is actually
     * octal (07500 is represented as 7500)
     */
    if (mm->identity == 7500 || mm->identity == 7600 || mm->identity == 7700)
       emergency = -1;
    if (mm->flight_status == 1 || mm->flight_status == 3)
       ground = -1;
    if (mm->flight_status == 2 || mm->flight_status == 3 || mm->flight_status == 4)
       alert = -1;
    if (mm->flight_status == 4 || mm->flight_status == 5)
       spi = -1;
  }

  /* Field 11 could contain the call-sign we can get from `aircraft_find_or_create()::reg_num`.
   *
   * Need to output the current date and time into the SBS output
   *          1   2 3 4 5      6 7          8            9          10           11 12   13 14  15       16       17 18 19 20 21 22
   * example: MSG,3,1,1,4CA7B6,1,2023/10/20,22:33:49.364,2023/10/20,22:33:49.403,  ,7250,  ,   ,53.26917,-2.17755,  ,  ,  ,  ,  ,0
   */
  date_str = get_SBS_timestamp();

  if (mm->msg_type == 0)
  {
    p += sprintf (p, "MSG,5,1,1,%06X,1,%s,,%d,,,,,,,,,,",
                  aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
                  date_str, mm->altitude);
  }
  else if (mm->msg_type == 4)
  {
    p += sprintf (p, "MSG,5,1,1,%06X,1,%s,,%d,,,,,,,%d,%d,%d,%d",
                  aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
                  date_str, mm->altitude, alert, emergency, spi, ground);
  }
  else if (mm->msg_type == 5)
  {
    p += sprintf (p, "MSG,6,1,1,%06X,1,%s,,,,,,,,%d,%d,%d,%d,%d",
                  aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
                  date_str, mm->identity, alert, emergency, spi, ground);
  }
  else if (mm->msg_type == 11)
  {
    p += sprintf (p, "MSG,8,1,1,%06X,1,%s,,,,,,,,,,,,",
                  aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]), date_str);
  }
  else if (mm->msg_type == 17 && mm->ME_type == 4)
  {
    p += sprintf (p, "MSG,1,1,1,%06X,1,%s,%s,,,,,,,,0,0,0,0",
                  aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
                  date_str, mm->flight);
  }
  else if (mm->msg_type == 17 && mm->ME_type >= 9 && mm->ME_type <= 18)
  {
	if ((!a->HaveLatLon) || !VALID_POS(a))
		 p += sprintf (p, "MSG,3,1,1,%06X,1,%s,,%d,,,,,,,0,0,0,0",
                       aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
                       date_str, mm->altitude);
    else p += sprintf (p, "MSG,3,1,1,%06X,1,%s,,%d,,,%1.5f,%1.5f,,,0,0,0,0",
                       aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
					   date_str, mm->altitude, a->Latitude, a->Longitude);
  }
  else if (mm->msg_type == 17 && mm->ME_type == 19 && mm->ME_subtype == 1)
  {
    int vr = (mm->vert_rate_sign == 0 ? 1 : -1) * 64 * (mm->vert_rate - 1);

    p += sprintf (p, "MSG,4,1,1,%06X,1,%s,,,%d,%d,,,%i,,0,0,0,0",
                  aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
                  date_str, (int)a->Speed, (int)a->Heading, vr);
  }
  else if (mm->msg_type == 21)
  {
    p += sprintf (p, "MSG,6,1,1,%06X,1,%s,,,,,,,,%d,%d,%d,%d,%d",
                  aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]),
                  date_str, mm->identity, alert, emergency, spi, ground);
  }
  else
	return(false);

  *p++ = '\0';
  return(true);
}
//---------------------------------------------------------------------------
#define DELIMITER ","
//---------------------------------------------------------------------------
static void trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return ;
}
//---------------------------------------------------------------------------
bool SBS_Message_Decode( char *msg)
{
   bool  HaveSpeed=false;
   bool  HaveHeading=false;
   bool  HaveLat=false;
   bool  HaveLon=false;
   TADS_B_Aircraft *ADS_B_Aircraft;
   int FieldNumber=0;
   uint32_t addr=0;
   __int64 CurrentTime=GetCurrentTimeInMsec();

   char *SBS_Fields[22];
   char FixHex[7];

   for (int i = 0; i < 22; i++)
	 {
		SBS_Fields[i] = strsep(&msg, DELIMITER);
		if (!msg && i < 21)
		  {
			return(false);
		  }
	 }

   if ((SBS_Fields[SBS_MESSAGE_TYPE]==0) || (stricmp(SBS_Fields[SBS_MESSAGE_TYPE],"MSG")!=0))
	{
	  printf("not a SBS MSG\n");
	  return(false);
	}

	if ((SBS_Fields[SBS_HEX_INDENT]==0) || (strlen(SBS_Fields[SBS_HEX_INDENT]) < 6) || (strlen(SBS_Fields[SBS_HEX_INDENT]) > 7))  // icao must be 6 characters
	   {
		if ((SBS_Fields[SBS_HEX_INDENT]==0) || (strlen(SBS_Fields[SBS_HEX_INDENT]) > 7))
		{
		 printf("invalid ICAO 1 Field is %s\n",SBS_Fields[SBS_HEX_INDENT]);
		 return(false);
		}
		else
		{
		 int current_length=strlen(SBS_Fields[SBS_HEX_INDENT]);
		 int padding_length = 6 - current_length;
		 memset(FixHex, '0', padding_length);
		 strcpy(FixHex +padding_length, SBS_Fields[SBS_HEX_INDENT]);
		 //printf("Fixed Hex Was %s now %s\n", SBS_Fields[SBS_HEX_INDENT],FixHex);
		 SBS_Fields[SBS_HEX_INDENT]=FixHex;
		}
       }
	char *icao = SBS_Fields[SBS_HEX_INDENT];
    int non_icao = 0;
    if (icao[0] == '~')
     {
      icao++;
      non_icao = 1;
     }
    unsigned char *chars = (unsigned char *)(unsigned char *) &addr;
    for (int j = 0; j < 6; j += 2)
       {
	   	int high = hexDigitVal(icao[j]);
        int low = hexDigitVal(icao[j + 1]);

        if (high == -1 || low == -1)
		  {
           printf("invalid ICAO 2\n");
           return(false);
          }

         chars[2 - j / 2] = (high << 4) | low;
       }
     //printf("%06X\n",(int)addr);
     if (non_icao) addr |= MODES_NON_ICAO_ADDRESS;

     ADS_B_Aircraft =(TADS_B_Aircraft *) ght_get(Form1->HashTable,sizeof(addr),&addr);
     if (ADS_B_Aircraft==NULL)
        {
         ADS_B_Aircraft= new TADS_B_Aircraft;
         ADS_B_Aircraft->ICAO=addr;
         snprintf(ADS_B_Aircraft->HexAddr,sizeof(ADS_B_Aircraft->HexAddr),"%06X",(int)addr);
         ADS_B_Aircraft->NumMessagesSBS=0;
         ADS_B_Aircraft->NumMessagesRaw=0;
         ADS_B_Aircraft->VerticalRate=0;
         ADS_B_Aircraft->HaveAltitude=false;
         ADS_B_Aircraft->HaveLatLon=false;
         ADS_B_Aircraft->HaveSpeedAndHeading=false;
         ADS_B_Aircraft->HaveFlightNum=false;
         ADS_B_Aircraft->HaveRoute=false;
         ADS_B_Aircraft->RequestedRoute=false;
         ADS_B_Aircraft->Route[0]=NULL;
         ADS_B_Aircraft->SpriteImage=Form1->CurrentSpriteImage;
         if (Form1->CycleImages->Checked)
              Form1->CurrentSpriteImage=(Form1->CurrentSpriteImage+1)%Form1->NumSpriteImages;
		 if (ght_insert(Form1->HashTable,ADS_B_Aircraft,sizeof(addr), &addr) < 0)
             {
			  printf("ght_insert Error-Should Not Happen");
             }
        }

      ADS_B_Aircraft->LastSeen =CurrentTime;
      ADS_B_Aircraft->NumMessagesSBS++;

	  if ((SBS_Fields[SBS_CALLSIGN]) && strlen(SBS_Fields[SBS_CALLSIGN]) > 0)
		 {
		   strncpy(ADS_B_Aircraft->FlightNum, SBS_Fields[SBS_CALLSIGN], 9);
		   ADS_B_Aircraft->FlightNum[8] = '\0';
		   ADS_B_Aircraft->HaveFlightNum = true;
		   for (unsigned i = 0; i < 8; ++i)
              {
			   if (ADS_B_Aircraft->FlightNum[i] == '\0')
				 {
				  ADS_B_Aircraft->FlightNum[i] = ' ';
				 }
				if (!(ADS_B_Aircraft->FlightNum[i] >= 'A' && ADS_B_Aircraft->FlightNum[i] <= 'Z') &&
					!(ADS_B_Aircraft->FlightNum[i] >= '0' && ADS_B_Aircraft->FlightNum[i] <= '9') &&
					  ADS_B_Aircraft->FlightNum[i] != ' ')
					{
					  // Bad callsign, ignore it
					  ADS_B_Aircraft->HaveFlightNum = false;
					  break;
					}
              }
           trimwhitespace(ADS_B_Aircraft->FlightNum);
         }
	  if ((SBS_Fields[SBS_ALTITUDE]) && strlen(SBS_Fields[SBS_ALTITUDE]) > 0)
		 {
		   char *endptr = NULL;
		   double tmp=strtod(SBS_Fields[SBS_ALTITUDE], &endptr);
		   if (endptr != SBS_Fields[SBS_ALTITUDE] && isfinite(tmp))
			  {
			   ADS_B_Aircraft->HaveAltitude=true;
			   ADS_B_Aircraft->Altitude=tmp;
			  }
		 }
	  if ((SBS_Fields[SBS_GROUND_SPEED]) && strlen(SBS_Fields[SBS_GROUND_SPEED]) > 0)
		 {
		  char *endptr = NULL;

		  double tmp=strtod(SBS_Fields[SBS_GROUND_SPEED], &endptr);
		  if (endptr != SBS_Fields[SBS_GROUND_SPEED] && isfinite(tmp))
			  {
			   ADS_B_Aircraft->Speed=tmp;
			  }
		 }
	  if ((SBS_Fields[SBS_TRACK_HEADING]) && strlen(SBS_Fields[SBS_TRACK_HEADING]) > 0)
		 {
		  char *endptr = NULL;

		  double tmp=strtod(SBS_Fields[SBS_TRACK_HEADING], &endptr);
		  if (endptr != SBS_Fields[SBS_TRACK_HEADING] && isfinite(tmp))
			  {
			   ADS_B_Aircraft->Heading=tmp;
			   ADS_B_Aircraft->HaveSpeedAndHeading=true;
			  }
		 }
	  if (SBS_Fields[SBS_LATITUDE]  && (strlen(SBS_Fields[SBS_LATITUDE]) > 0) &&
          SBS_Fields[SBS_LONGITUDE] && (strlen(SBS_Fields[SBS_LONGITUDE]) > 0))
		 {
		   char *endptr = NULL;
           double TempLat,TempLon;
		   TempLat=strtod(SBS_Fields[SBS_LATITUDE], &endptr);
		   if (endptr != SBS_Fields[SBS_LATITUDE] && isfinite(TempLat))
			  {
                TempLon=strtod(SBS_Fields[SBS_LONGITUDE], &endptr);
                if (endptr != SBS_Fields[SBS_LONGITUDE] && isfinite(TempLon))
                {
                  if ((ADS_B_Aircraft->Latitude< 90.0) && (ADS_B_Aircraft->Latitude>=-90.0) &&
                      (ADS_B_Aircraft->Longitude>= -180.0) && (ADS_B_Aircraft->Longitude<=180.0))
                  {
                   ADS_B_Aircraft->Latitude=TempLat;
                   ADS_B_Aircraft->Longitude=TempLon;
                   ADS_B_Aircraft->HaveLatLon=true;
                  }

                }
              }
		 }

		if ((SBS_Fields[SBS_VERTICAL_RATE]) && strlen(SBS_Fields[SBS_VERTICAL_RATE]) > 0)
		  {
			  char *endptr = NULL;

			  double tmp=strtod(SBS_Fields[SBS_VERTICAL_RATE], &endptr);
			  if (endptr != SBS_Fields[SBS_VERTICAL_RATE] && isfinite(tmp))
			  {
			   ADS_B_Aircraft->VerticalRate=tmp;
			  }
		  }
  return(true);
}
//---------------------------------------------------------------------------
