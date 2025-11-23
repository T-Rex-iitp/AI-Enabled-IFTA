//---------------------------------------------------------------------------

#pragma hdrstop

#include "AircraftDB.h"
#include "csv.h"

#define DIM(array)         (sizeof(array) / sizeof(array[0]))
//---------------------------------------------------------------------------
#pragma package(smart_init)
const char *aircraft_get_country (uint32_t addr, bool get_short);
bool aircraft_is_helicopter (uint32_t addr, const char **type_ptr);
const char *aircraft_get_military (uint32_t addr);
bool aircraft_is_military (uint32_t addr, const char **country);

ght_hash_table_t *AircraftDBHashTable=NULL;
//---------------------------------------------------------------------------
static unsigned char mg_unhex_nimble(unsigned char c) {
  return (c >= '0' && c <= '9')   ? (unsigned char) (c - '0')
         : (c >= 'A' && c <= 'F') ? (unsigned char) (c - '7')
                                  : (unsigned char) (c - 'W');
}

//---------------------------------------------------------------------------
static unsigned long mg_unhexn(const char *s, size_t len) {
  unsigned long i = 0, v = 0;
  for (i = 0; i < len; i++) v <<= 4, v |= mg_unhex_nimble(((uint8_t *) s)[i]);
  return v;
}
//---------------------------------------------------------------------------
static void mg_unhex(const char *buf, size_t len, unsigned char *to) {
  size_t i;
  for (i = 0; i < len; i += 2) {
    to[i >> 1] = (unsigned char) mg_unhexn(&buf[i], 2);
  }
}

//---------------------------------------------------------------------------
 static int CSV_callback (struct CSV_context *ctx, const char *value)
{
  int    rc = 1;
  static bool  Init=true;
  static TAircraftData Record;

  if (Init)
    {
      for (int i=0; i < AC_DB_NUM_FIELDS; i++)
      {
		 Record.Fields[i]="?";
         Init=false;
      }
    }
   if(strlen(value)) Record.Fields[ctx->field_num]=value;
   if (ctx->field_num==0)
    {
     Record.ICAO24=mg_unhexn (value, strlen(value));
     //printf("%x %s\n",Record.ICAO24,Record.Fields[ctx->field_num].c_str());
    }
   else if (ctx->field_num == (ctx->num_fields - 1))
   {
	 TAircraftData *Data;
	 if (Record.ICAO24!=0)
	 {
	  Data =(TAircraftData *) ght_get(AircraftDBHashTable,sizeof(Record.ICAO24),&Record.ICAO24);
	  if (Data)
	  {
	   printf("Duplicate Aircraft Record %s %x\n",Data->Fields[0].c_str(),Record.ICAO24);
	  }
	  else
	  {
	   Data= new TAircraftData;
	   Data->ICAO24=Record.ICAO24;

	   for (int i=0; i < AC_DB_NUM_FIELDS; i++)  Data->Fields[i]=Record.Fields[i];
	   if (ght_insert(AircraftDBHashTable,Data,sizeof(Data->ICAO24), &Data->ICAO24) < 0)
			 {
			  printf("ght_insert Error-Should Not Happen");
			 }
	   // printf("Record Added %s\n",Data->Fields[0].c_str());
	  }
	 }
	 for (int i=0; i < AC_DB_NUM_FIELDS; i++) Record.Fields[i]="?";
   }
  return(rc);
}
//---------------------------------------------------------------------------
bool InitAircraftDB(AnsiString FileName)
{
  CSV_context  csv_ctx;

  AircraftDBHashTable = ght_create(600000);
  ght_set_rehash(AircraftDBHashTable, TRUE);

  if ( !AircraftDBHashTable)
	{
	  throw Sysutils::Exception("Create Hash Failed");
	}
  ght_set_rehash(AircraftDBHashTable, TRUE);

   memset (&csv_ctx, 0, sizeof(csv_ctx));
   csv_ctx.file_name = FileName.c_str();
   csv_ctx.delimiter = ',';
   csv_ctx.callback  = CSV_callback;
   csv_ctx.line_size = 2000;
   printf("Reading Aircraft DB\n");
   if (!CSV_open_and_parse_file(&csv_ctx))
    {
	  printf("Parsing of \"%s\" failed: %s\n", FileName.c_str(), strerror(errno));
      return (false);
	}

   printf("Done Reading Aircraft DB\n");
   return(true);
}
//---------------------------------------------------------------------------
const char * GetAircraftDBInfo(uint32_t addr)
{
  static char          buf [2048];
  const TAircraftData *a;
  a =(TAircraftData *) ght_get(AircraftDBHashTable,sizeof(addr),&addr);

  if (a)
   {

	const char *type2  =NULL   ;
	bool  isHelo=false;
;
	type2 = NULL;
	isHelo=aircraft_is_helicopter(addr, &type2);
	snprintf (buf,sizeof(buf),"addr:0x%06X, Reg:%s, Manufact-ICAO:%s, Manufact-Name:%s, Model:%s\n"
							  "Type:%s, Serial:%s, Line:%s, ICAO-Air-Type:%s, Op:%s, Op-CallSign:%s\n"
							  "Op-ICAO %s, OP-IATA:%s, Owner:%s, TestReg:%s, Reg:%s, Reg-Until: %s\n"
							  "Status:%s, Built:%s, First-Flight:%s, Seat-Config:%s, Engines:%s\n"
							  "Modes:%s, ADSB:%s, ACARS:%s, Notes:%s, Cat-Desc:%s, Country:%s\n"
							  "%s %s %s",
							   a->ICAO24,a->Fields[1].c_str(),
							   a->Fields[2].c_str(),a->Fields[3].c_str(),a->Fields[4].c_str(),
							   a->Fields[5].c_str(),a->Fields[6].c_str(),a->Fields[7].c_str(),
							   a->Fields[8].c_str(),a->Fields[9].c_str(),a->Fields[10].c_str(),
							   a->Fields[11].c_str(),a->Fields[12].c_str(),a->Fields[13].c_str(),
							   a->Fields[14].c_str(),a->Fields[15].c_str(),a->Fields[16].c_str(),
							   a->Fields[17].c_str(),a->Fields[18].c_str(),a->Fields[19].c_str(),
							   a->Fields[20].c_str(),a->Fields[21].c_str(),a->Fields[22].c_str(),
							   a->Fields[23].c_str(),a->Fields[24].c_str(),a->Fields[25].c_str(),
							   a->Fields[26].c_str(),aircraft_get_country (addr, false),
							   aircraft_is_military(addr, NULL) ? "Military " : "",
							   isHelo ? "Helo-" : " ",isHelo ? type2 : " ");


   }
   else snprintf (buf,sizeof(buf),"addr: 0x%06X, No Data",addr);

   return (buf);
}
//---------------------------------------------------------------------------
/*
 * Declare ICAO registration address ranges and country.
 * Mostly generated from the assignment table in the appendix to Chapter 9 of
 * Annex 10 Vol III, Second Edition, July 2007 (with amendments through 88-A, 14/11/2013)
 *
 * Rewritten from `web_root-Tar1090/flags.js` to lookup
 * the county.
 *
 * The low and high values used to lookup a (short/long) country
 * or military ranges.
 */
typedef struct ICAO_range {
        uint32_t    low;
        uint32_t    high;
        const char *cc_short;
        const char *cc_long;
      } ICAO_range;
//---------------------------------------------------------------------------
static const ICAO_range ICAO_ranges [] = {
    { 0x004000, 0x0043FF, "ZW", "Zimbabwe" },
    { 0x006000, 0x006FFF, "MZ", "Mozambique" },
    { 0x008000, 0x00FFFF, "ZA", "South Africa" },
    { 0x010000, 0x017FFF, "EG", "Egypt" },
    { 0x018000, 0x01FFFF, "LY", "Libya"  },
    { 0x020000, 0x027FFF, "MA", "Morocco" },
    { 0x028000, 0x02FFFF, "TN", "Tunisia" },
    { 0x030000, 0x0303FF, "BW", "Botswana" },
    { 0x032000, 0x032FFF, "BI", "Burundi" },
    { 0x034000, 0x034FFF, "CM", "Cameroon" },
    { 0x035000, 0x0353FF, "KM", "Comoros" },
    { 0x036000, 0x036FFF, "CG", "Congo" },
    { 0x038000, 0x038FFF, "CI", "Cote d'Ivoire" },
    { 0x03E000, 0x03EFFF, "GA", "Gabon" },
    { 0x040000, 0x040FFF, "ET", "Ethiopia" },
    { 0x042000, 0x042FFF, "GQ", "Equatorial Guinea" },
    { 0x044000, 0x044FFF, "GH", "Ghana" },
    { 0x046000, 0x046FFF, "GN", "Guinea" },
    { 0x048000, 0x0483FF, "GW", "Guinea-Bissau" },
    { 0x04A000, 0x04A3FF, "LS", "Lesotho" },
    { 0x04C000, 0x04CFFF, "KE", "Kenya" },
    { 0x050000, 0x050FFF, "LR", "Liberia" },
    { 0x054000, 0x054FFF, "MG", "Madagascar" },
    { 0x058000, 0x058FFF, "MW", "Malawi" },
    { 0x05A000, 0x05A3FF, "MV", "Maldives" },
    { 0x05C000, 0x05CFFF, "ML", "Mali" },
    { 0x05E000, 0x05E3FF, "MR", "Mauritania" },
    { 0x060000, 0x0603FF, "MU", "Mauritius" },
    { 0x062000, 0x062FFF, "NE", "Niger" },
    { 0x064000, 0x064FFF, "NG", "Nigeria" },
    { 0x068000, 0x068FFF, "UG", "Uganda" },
    { 0x06A000, 0x06A3FF, "QA", "Qatar" },
    { 0x06C000, 0x06CFFF, "CF", "Central African Republic" },
    { 0x06E000, 0x06EFFF, "RW", "Rwanda" },
    { 0x070000, 0x070FFF, "SN", "Senegal" },
    { 0x074000, 0x0743FF, "SC", "Seychelles" },
    { 0x076000, 0x0763FF, "SL", "Sierra Leone" },
    { 0x078000, 0x078FFF, "SO", "Somalia" },
    { 0x07A000, 0x07A3FF, "SZ", "Swaziland" },  // Now Eswatini
    { 0x07C000, 0x07CFFF, "SD", "Sudan" },
    { 0x080000, 0x080FFF, "TZ", "Tanzania" },
    { 0x084000, 0x084FFF, "TD", "Chad" },
    { 0x088000, 0x088FFF, "TG", "Togo" },
    { 0x08A000, 0x08AFFF, "ZM", "Zambia" },
    { 0x08C000, 0x08CFFF, "CD", "DR Congo" },
    { 0x090000, 0x090FFF, "AO", "Angola" },
    { 0x094000, 0x0943FF, "BJ", "Benin" },
    { 0x096000, 0x0963FF, "CV", "Cape Verde" },
    { 0x098000, 0x0983FF, "DJ", "Djibouti" },
    { 0x09A000, 0x09AFFF, "GM", "Gambia" },
    { 0x09C000, 0x09CFFF, "BF", "Burkina Faso" },
    { 0x09E000, 0x09E3FF, "ST", "Sao Tome & Principe" },
    { 0x0A0000, 0x0A7FFF, "DZ", "Algeria" },
    { 0x0A8000, 0x0A8FFF, "BS", "Bahamas" },
    { 0x0AA000, 0x0AA3FF, "BB", "Barbados" },
    { 0x0AB000, 0x0AB3FF, "BZ", "Belize" },
    { 0x0AC000, 0x0ACFFF, "CO", "Colombia" },
    { 0x0AE000, 0x0AEFFF, "CR", "Costa Rica" },
    { 0x0B0000, 0x0B0FFF, "CU", "Cuba" },
    { 0x0B2000, 0x0B2FFF, "SV", "El Salvador" },
    { 0x0B4000, 0x0B4FFF, "GT", "Guatemala" },
    { 0x0B6000, 0x0B6FFF, "GY", "Guyana" },
    { 0x0B8000, 0x0B8FFF, "HT", "Haiti" },
    { 0x0BA000, 0x0BAFFF, "HN", "Honduras" },
    { 0x0BC000, 0x0BC3FF, "VC", "Saint Vincent & the Grenadines" },
    { 0x0BE000, 0x0BEFFF, "JM", "Jamaica" },
    { 0x0C0000, 0x0C0FFF, "NI", "Nicaragua" },
    { 0x0C2000, 0x0C2FFF, "PA", "Panama" },
    { 0x0C4000, 0x0C4FFF, "DO", "Dominican Republic" },
    { 0x0C6000, 0x0C6FFF, "TT", "Trinidad & Tobago" },
    { 0x0C8000, 0x0C8FFF, "SR", "Suriname" },
    { 0x0CA000, 0x0CA3FF, "AG", "Antigua & Barbuda" },
    { 0x0CC000, 0x0CC3FF, "GD", "Grenada" },
    { 0x0D0000, 0x0D7FFF, "MX", "Mexico" },
    { 0x0D8000, 0x0DFFFF, "VE", "Venezuela" },
    { 0x100000, 0x1FFFFF, "RU", "Russia" },
    { 0x201000, 0x2013FF, "NA", "Namibia" },
    { 0x202000, 0x2023FF, "ER", "Eritrea" },
    { 0x300000, 0x33FFFF, "IT", "Italy" },
    { 0x340000, 0x37FFFF, "ES", "Spain" },
    { 0x380000, 0x3BFFFF, "FR", "France" },
    { 0x3C0000, 0x3FFFFF, "DE", "Germany" },

    // UK territories are officially part of the UK range
    // add extra entries that are above the UK and take precedence
    // this is a mess ... let's still try
    { 0x400000, 0x4001BF, "BM", "Bermuda" },
    { 0x4001C0, 0x4001FF, "KY", "Cayman Islands" },
    { 0x400300, 0x4003FF, "TC", "Turks & Caicos Islands" },
    { 0x424135, 0x4241F2, "KY", "Cayman Islands" },
    { 0x424200, 0x4246FF, "BM", "Bermuda" },
    { 0x424700, 0x424899, "KY", "Cayman Islands" },
    { 0x424B00, 0x424BFF, "IM", "Isle of Man" },
    { 0x43BE00, 0x43BEFF, "BM", "Bermuda" },
    { 0x43E700, 0x43EAFD, "IM", "Isle of Man" },
    { 0x43EAFE, 0x43EEFF, "GG", "Guernsey" },

    // catch all United Kingdom for the even more obscure stuff
    { 0x400000, 0x43FFFF, "GB", "United Kingdom" },
    { 0x440000, 0x447FFF, "AT", "Austria" },
    { 0x448000, 0x44FFFF, "BE", "Belgium" },
    { 0x450000, 0x457FFF, "BG", "Bulgaria" },
    { 0x458000, 0x45FFFF, "DK", "Denmark" },
    { 0x460000, 0x467FFF, "FI", "Finland" },
    { 0x468000, 0x46FFFF, "GR", "Greece" },
    { 0x470000, 0x477FFF, "HU", "Hungary" },
    { 0x478000, 0x47FFFF, "NO", "Norway" },
    { 0x480000, 0x487FFF, "NL", "Netherland" },
    { 0x488000, 0x48FFFF, "PL", "Poland" },
    { 0x490000, 0x497FFF, "PT", "Portugal" },
    { 0x498000, 0x49FFFF, "CZ", "Czechia" },  // previously 'Czech Republic'
    { 0x4A0000, 0x4A7FFF, "RO", "Romania" },
    { 0x4A8000, 0x4AFFFF, "SE", "Sweden" },
    { 0x4B0000, 0x4B7FFF, "CH", "Switzerland" },
    { 0x4B8000, 0x4BFFFF, "TR", "Turkey" },
    { 0x4C0000, 0x4C7FFF, "RS", "Serbia" },
    { 0x4C8000, 0x4C83FF, "CY", "Cyprus" },
    { 0x4CA000, 0x4CAFFF, "IE", "Ireland" },
    { 0x4CC000, 0x4CCFFF, "IS", "Iceland" },
    { 0x4D0000, 0x4D03FF, "LU", "Luxembourg" },
    { 0x4D2000, 0x4D2FFF, "MT", "Malta" },
    { 0x4D4000, 0x4D43FF, "MC", "Monaco" },
    { 0x500000, 0x5003FF, "SM", "San Marino" },
    { 0x501000, 0x5013FF, "AL", "Albania" },
    { 0x501C00, 0x501FFF, "HR", "Croatia" },
    { 0x502C00, 0x502FFF, "LV", "Latvia" },
    { 0x503C00, 0x503FFF, "LT", "Lithuania" },
    { 0x504C00, 0x504FFF, "MD", "Moldova"  },
    { 0x505C00, 0x505FFF, "SK", "Slovakia" },
    { 0x506C00, 0x506FFF, "SI", "Slovenia" },
    { 0x507C00, 0x507FFF, "UZ", "Uzbekistan" },
    { 0x508000, 0x50FFFF, "UA", "Ukraine" },
    { 0x510000, 0x5103FF, "BY", "Belarus" },
    { 0x511000, 0x5113FF, "EE", "Estonia" },
    { 0x512000, 0x5123FF, "MK", "Macedonia" },
    { 0x513000, 0x5133FF, "BA", "Bosnia & Herzegovina" },
    { 0x514000, 0x5143FF, "GE", "Georgia" },
    { 0x515000, 0x5153FF, "TJ", "Tajikistan" },
    { 0x516000, 0x5163FF, "ME", "Montenegro" },
    { 0x600000, 0x6003FF, "AM", "Armenia" },
    { 0x600800, 0x600BFF, "AZ", "Azerbaijan" },
    { 0x601000, 0x6013FF, "KG", "Kyrgyzstan" },
    { 0x601800, 0x601BFF, "TM", "Turkmenistan" },
    { 0x680000, 0x6803FF, "BT", "Bhutan" },
    { 0x681000, 0x6813FF, "FM", "Micronesia" },
    { 0x682000, 0x6823FF, "MN", "Mongolia" },
    { 0x683000, 0x6833FF, "KZ", "Kazakhstan" },
    { 0x684000, 0x6843FF, "PW", "Palau" },
    { 0x700000, 0x700FFF, "AF", "Afghanistan" },
    { 0x702000, 0x702FFF, "BD", "Bangladesh" },
    { 0x704000, 0x704FFF, "MM", "Myanmar" },
    { 0x706000, 0x706FFF, "KW", "Kuwait" },
    { 0x708000, 0x708FFF, "LA", "Laos" },
    { 0x70A000, 0x70AFFF, "NP", "Nepal" },
    { 0x70C000, 0x70C3FF, "OM", "Oman" },
    { 0x70E000, 0x70EFFF, "KH", "Cambodia"},
    { 0x710000, 0x717FFF, "SA", "Saudi Arabia" },
    { 0x718000, 0x71FFFF, "KR", "South Korea" },
    { 0x720000, 0x727FFF, "KP", "North Korea" },
    { 0x728000, 0x72FFFF, "IQ", "Iraq" },
    { 0x730000, 0x737FFF, "IR", "Iran" },
    { 0x738000, 0x73FFFF, "IL", "Israel" },
    { 0x740000, 0x747FFF, "JO", "Jordan" },
    { 0x748000, 0x74FFFF, "LB", "Lebanon" },
    { 0x750000, 0x757FFF, "MY", "Malaysia" },
    { 0x758000, 0x75FFFF, "PH", "Philippines" },
    { 0x760000, 0x767FFF, "PK", "Pakistan" },
    { 0x768000, 0x76FFFF, "SG", "Singapore" },
    { 0x770000, 0x777FFF, "LLK", "Sri Lanka" },
    { 0x778000, 0x77FFFF, "SY", "Syria" },
    { 0x789000, 0x789FFF, "HK", "Hong Kong" },
    { 0x780000, 0x7BFFFF, "CN", "China" },
    { 0x7C0000, 0x7FFFFF, "AU", "Australia" },
    { 0x800000, 0x83FFFF, "IN", "India" },
    { 0x840000, 0x87FFFF, "JP", "Japan" },
    { 0x880000, 0x887FFF, "TH", "Thailand" },
    { 0x888000, 0x88FFFF, "VN", "Viet Nam" },
    { 0x890000, 0x890FFF, "YE", "Yemen" },
    { 0x894000, 0x894FFF, "BH", "Bahrain" },
    { 0x895000, 0x8953FF, "BN", "Brunei" },
    { 0x896000, 0x896FFF, "AE", "United Arab Emirates" },
    { 0x897000, 0x8973FF, "SB", "Solomon Islands" },
    { 0x898000, 0x898FFF, "PG", "Papua New Guinea" },
    { 0x899000, 0x8993FF, "TW", "Taiwan" },
    { 0x8A0000, 0x8A7FFF, "ID", "Indonesia"  },
    { 0x900000, 0x9003FF, "MH", "Marshall Islands" },
    { 0x901000, 0x9013FF, "CK", "Cook Islands" },
    { 0x902000, 0x9023FF, "WS", "Samoa"  },
    { 0xA00000, 0xAFFFFF, "US", "United States" },
    { 0xC00000, 0xC3FFFF, "CA", "Canada" },
    { 0xC80000, 0xC87FFF, "NZ", "New Zealand" },
    { 0xC88000, 0xC88FFF, "FJ", "Fiji" },
    { 0xC8A000, 0xC8A3FF, "NR", "Nauru" },
    { 0xC8C000, 0xC8C3FF, "LC", "Saint Lucia" },
    { 0xC8D000, 0xC8D3FF, "TU", "Tonga" },
    { 0xC8E000, 0xC8E3FF, "KI", "Kiribati" },
    { 0xC90000, 0xC903FF, "VU", "Vanuatu" },
    { 0xE00000, 0xE3FFFF, "AR", "Argentina" },
    { 0xE40000, 0xE7FFFF, "BR", "Brazil" },
    { 0xE80000, 0xE80FFF, "CL", "Chile" },
    { 0xE84000, 0xE84FFF, "EC", "Ecuador" },
    { 0xE88000, 0xE88FFF, "PY", "Paraguay" },
    { 0xE8C000, 0xE8CFFF, "PE", "Peru" },
    { 0xE90000, 0xE90FFF, "UY", "Uruguay" },
    { 0xE94000, 0xE94FFF, "BO", "Bolivia" }
};
//---------------------------------------------------------------------------
const char *aircraft_get_country (uint32_t addr, bool get_short)
{
  const ICAO_range *r = ICAO_ranges + 0;
  uint16_t   i;

  for (i = 0; i < DIM(ICAO_ranges); i++, r++)
      if (addr >= r->low && addr <= r->high)
         return (get_short ? r->cc_short : r->cc_long);
  return (NULL);
}
//---------------------------------------------------------------------------
/**
 * Returns TRUE if the ICAO address is in one of these military ranges.
 */
static const ICAO_range military_range [] = {
     { 0xADF7C8,  0xAFFFFF, "US" },
     { 0x010070,  0x01008F, NULL },
     { 0x0A4000,  0x0A4FFF, NULL },
     { 0x33FF00,  0x33FFFF, NULL },
     { 0x350000,  0x37FFFF, NULL },
     { 0x3A8000,  0x3AFFFF, NULL },
     { 0x3B0000,  0x3BFFFF, NULL },
     { 0x3EA000,  0x3EBFFF, NULL },
     { 0x3F4000,  0x3FBFFF, NULL },
     { 0x400000,  0x40003F, NULL },
     { 0x43C000,  0x43CFFF, "UK" },
     { 0x444000,  0x446FFF, NULL },
     { 0x44F000,  0x44FFFF, NULL },
     { 0x457000,  0x457FFF, NULL },
     { 0x45F400,  0x45F4FF, NULL },
     { 0x468000,  0x4683FF, NULL },
     { 0x473C00,  0x473C0F, NULL },
     { 0x478100,  0x4781FF, NULL },
     { 0x480000,  0x480FFF, NULL },
     { 0x48D800,  0x48D87F, NULL },
     { 0x497C00,  0x497CFF, NULL },
     { 0x498420,  0x49842F, NULL },
     { 0x4B7000,  0x4B7FFF, NULL },
     { 0x4B8200,  0x4B82FF, NULL },
     { 0x506F00,  0x506FFF, NULL },
     { 0x70C070,  0x70C07F, NULL },
     { 0x710258,  0x71028F, NULL },
     { 0x710380,  0x71039F, NULL },
     { 0x738A00,  0x738AFF, NULL },
     { 0x7C822E,  0x7C84FF, NULL },
     { 0x7C8800,  0x7C88FF, NULL },
     { 0x7C9000,  0x7CBFFF, NULL },
     { 0x7CF800,  0x7CFAFF, "AU" },
     { 0x7D0000,  0x7FFFFF, NULL },
     { 0x800200,  0x8002FF, NULL },
     { 0xC0CDF9,  0xC3FFFF, "CA" },
     { 0xC87F00,  0xC87FFF, "NZ" },
     { 0xE40000,  0xE41FFF, NULL }
   };
//---------------------------------------------------------------------------
bool aircraft_is_military (uint32_t addr, const char **country)
{
  const ICAO_range *r = military_range + 0;
  uint16_t          i;

  for (i = 0; i < DIM(military_range); i++, r++)
      if (addr >= r->low && addr <= r->high)
      {
        if (country && r->cc_short)
           *country = r->cc_short;
        return (true);
      }
  return (false);
}
//---------------------------------------------------------------------------
/**
 * The types of a helicopter (incomplete).
 */
static bool is_helicopter_type (const char *type)
{
  const char *helli_types[] = { "H1P", "H2P", "H1T", "H2T" };
  uint16_t    i;

  if (type[0] != 'H')   /* must start with a 'H' */
     return (false);

  for (i = 0; i < DIM(helli_types); i++)
      if (!stricmp(type, helli_types[i]))
         return (true);
  return (false);
}
//---------------------------------------------------------------------------
/**
 * Figure out if an ICAO-address belongs to a helicopter.
 */
bool aircraft_is_helicopter (uint32_t addr, const char **type_ptr)
{
  const TAircraftData *a;

  if (type_ptr)
	 *type_ptr = NULL;

  a =(TAircraftData *) ght_get(AircraftDBHashTable,sizeof(addr),&addr);
  if (a && is_helicopter_type(a->Fields[AC_DB_ICAOAircraftType].c_str()))
  {
    if (type_ptr)
	   *type_ptr = a->Fields[AC_DB_ICAOAircraftType].c_str();
    return (true);
  }
  return (false);
}

//---------------------------------------------------------------------------
const char *aircraft_get_military (uint32_t addr)
{
  static char buf [20];
  const  char *cntry;
  bool   mil = aircraft_is_military (addr, &cntry);
  int    sz;

  if (!mil)
     return ("");

  sz = snprintf (buf, sizeof(buf), "Military");
  if (cntry)
     snprintf (buf+sz, sizeof(buf)-sz, " (%s)", cntry);
  return (buf);
}
//---------------------------------------------------------------------------
