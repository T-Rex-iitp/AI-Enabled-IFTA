//---------------------------------------------------------------------------


#pragma hdrstop

#include "Tile.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
Tile::Tile(int x, int y, int level) {

	m_rcMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    if (m_rcMutex == NULL) {
        throw SysException("CreateMutex() failed", GetLastError());
    }
	m_X = x;
	m_Y = y;
	m_Level = level;

	m_LastUsed = Timer::Instance()->GetFrame();

	refcount = 0;

	m_RawData = 0;
	m_IsNull = 0;
}

Tile::~Tile() {
	delete m_RawData;
    CloseHandle(m_rcMutex);
}

int Tile::IncRef()
{
    WaitForSingleObject(m_rcMutex,INFINITE); 
    int rc = ++refcount;
    ReleaseMutex(m_rcMutex);
    return rc;
}

int Tile::DecRef()
{
    WaitForSingleObject(m_rcMutex,INFINITE);
    int rc = --refcount;
    ReleaseMutex(m_rcMutex);
    return rc;
}

/* coord ops */
int Tile::GetX() {
	return m_X;
}

int Tile::GetY() {
	return m_Y;
}

int Tile::GetLevel() {
	return m_Level;
}

int Tile::GetType() {
	return TILETYPE_NONE;
}

/* age ops */
void Tile::Touch() {
	m_LastUsed = Timer::Instance()->GetFrame();
}

ticks_t Tile::GetAge() {
	return Timer::Instance()->GetFrame() - m_LastUsed;
}

int Tile::IsOld() {
	return GetAge() > 2;
}

/* null ops */
void Tile::Null() {
	m_IsNull = 1;
}

int Tile::IsNull() {
	return m_IsNull;
}

/* load/save */
void Tile::Load(RawBuffer *data, int keep) {
	if (keep)
		m_RawData = data;
	else 
		delete data;
}

RawBuffer *Tile::ReleaseRawData() {
	RawBuffer *raw = m_RawData;
	m_RawData = 0;
	return raw;
}

int Tile::IsSaveable() {
	return m_RawData != 0;
}

int Tile::IsLoaded() {
	return IsNull();
}

int Tile::IsReady() {
	return !IsNull() && IsLoaded();
}

