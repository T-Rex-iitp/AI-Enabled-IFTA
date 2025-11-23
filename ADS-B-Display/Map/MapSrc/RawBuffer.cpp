//---------------------------------------------------------------------------


#pragma hdrstop

#include "RawBuffer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

RawBuffer::RawBuffer(void *data, size_t size) {
	m_Data = new unsigned char [size];
	memcpy(m_Data, data, size);
	m_Size = size;
}

RawBuffer::RawBuffer(int f) {
	struct stat st;
	if(fstat(f, &st) == -1)
		throw SysException("fstat() failed", errno);

	m_Data = new unsigned char [st.st_size];
	if(read(f, m_Data, st.st_size) != st.st_size) {
		delete[] m_Data;
		throw SysException("read() failed", errno);
	}
	m_Size = st.st_size;
}

RawBuffer::~RawBuffer() {
	delete[] m_Data;
}

size_t RawBuffer::Size() {
	return m_Size;
}

void *RawBuffer::Data() {
	return m_Data;
}

