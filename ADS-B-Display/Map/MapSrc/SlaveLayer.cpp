//---------------------------------------------------------------------------


#pragma hdrstop

#include "SlaveLayer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

SlaveLayer::SlaveLayer() {
	m_Caps = 0;
}

SlaveLayer::~SlaveLayer() {
}

int SlaveLayer::GetCap(int cap) {
	return m_Caps & cap;
}

/*int SlaveLayer::BindOvertexture(Vector3d &worldPoint1, Vector3d &worldPoint2, Vector3d &worldPoint3) {
	return 0;
}*/

void SlaveLayer::Overdraw(Region *rgn) {
}
