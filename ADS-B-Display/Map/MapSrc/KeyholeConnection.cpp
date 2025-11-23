//---------------------------------------------------------------------------


#pragma hdrstop

#include "KeyholeConnection.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#ifdef WIN32
#include <windows.h>
#define sleep(sec) Sleep(1000*(sec))
#endif

// https://skyvector.com/api/chartDataFPL


#define GOOGLE_URL               "http://mt1.google.com"
#define SKYVECTOR_URL            "http://t.skyvector.com"
#define SKYVECTOR_CHART_VPS      "301"
#define SKYVECTOR_CHART_IFR_LOW  "302"
#define SKYVECTOR_CHART_IFR_HIGH "304"
#define SKYVECTOR_KEY             "V7pMh4xRihf1nr61"
#define SKYVECTOR_EDITION         "2504"


KeyholeConnection::KeyholeConnection(int type)
{
  const char * url;
	if (type== GoogleMaps)
	{
	 ServerType= GoogleMaps;
	 url=GOOGLE_URL;
    }
	else if (type== SkyVector_VFR)
	{
	  ServerType= SkyVector;
	  url=SKYVECTOR_URL;
	  Key=SKYVECTOR_KEY;
	  Chart=SKYVECTOR_CHART_VPS;
	  Edition=SKYVECTOR_EDITION;
	}
	else if (type== SkyVector_IFR_Low)
	{
	  ServerType= SkyVector;
	  url=SKYVECTOR_URL;
	  Key=SKYVECTOR_KEY;
	  Chart=SKYVECTOR_CHART_IFR_LOW;
	  Edition=SKYVECTOR_EDITION;
	}
	else if (type== SkyVector_IFR_High)
	{
	  ServerType= SkyVector;
	  url=SKYVECTOR_URL;
	  Key=SKYVECTOR_KEY;
	  Chart=SKYVECTOR_CHART_IFR_HIGH;
	  Edition=SKYVECTOR_EDITION;
	}
	if ((m_GEFetch = gefetch_init(url)) == 0)
		throw Exception("gefetch_init() failed");
}

KeyholeConnection::~KeyholeConnection() {
	if (m_GEFetch)
		gefetch_cleanup(m_GEFetch);
}

void KeyholeConnection::Process(TilePtr tile) {
	gefetch_error res;
    if (ServerType== GoogleMaps)
    {
      res = gefetch_fetch_image_googlemaps(m_GEFetch, tile->GetX(), tile->GetY(), tile->GetLevel());
    }
    else if (ServerType== SkyVector)
	{
	  res = gefetch_fetch_image_skyvector(m_GEFetch,Key,Chart,Edition, tile->GetX(), tile->GetY(), tile->GetLevel());
    }

	if ((res == GEFETCH_NOT_FOUND) ||  (res == GEFETCH_INVALID_ZOOM))
	{
		tile->Null();
		return;
	}
	else if (res != GEFETCH_OK) {
		sleep(1);	/* don't do a DOS in case of any problems */
		throw Exception("gefetch_fetch_image() failed");
	}

	RawBuffer *buf = new RawBuffer(gefetch_get_data_ptr(m_GEFetch), gefetch_get_data_size(m_GEFetch));

	try {
		tile->Load(buf, m_pSaveStorage != 0);
	} catch (...) {
		delete buf;
		throw;
	}
}

