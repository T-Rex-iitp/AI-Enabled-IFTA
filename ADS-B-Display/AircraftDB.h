//---------------------------------------------------------------------------

#ifndef AircraftDBH
#define AircraftDBH
#include "ght_hash_table.h"

#define  AC_DB_NUM_FIELDS          27
#define  AC_DB_ICAO                 0
#define  AC_DB_Registration         1
#define  AC_DB_ManufacturerICAO     2
#define  AC_DB_ManufacturerName     3
#define  AC_DB_Model                4
#define  AC_DB_TypeCode             5
#define  AC_DB_SerialNumber         6
#define  AC_DB_LineNumber           7
#define  AC_DB_ICAOAircraftType     8
#define  AC_DB_Operator             9
#define  AC_DB_OperatorCallSign    10
#define  AC_DB_OperatorICAO        11
#define  AC_DB_OperatorIATA        12
#define  AC_DB_Owner               13
#define  AC_DB_TestReg             14
#define  AC_DB_Registered          15
#define  AC_DB_RegUntil            16
#define  AC_DB_Status              17
#define  AC_DB_Built               18
#define  AC_DB_FirstFlightDate     19
#define  AC_DB_Seatconfiguration   20
#define  AC_DB_Engines             21
#define  AC_DB_Modes               22
#define  AC_DB_ADSB                23
#define  AC_DB_ACARS               24
#define  AC_DB_Notes               25
#define  AC_DB_CategoryDescription 26

typedef struct
{
  uint32_t    ICAO24;
  AnsiString  Fields[AC_DB_NUM_FIELDS];
} TAircraftData;

bool InitAircraftDB(AnsiString FileName);
const char * GetAircraftDBInfo(uint32_t addr);
//---------------------------------------------------------------------------
#endif
