//---------------------------------------------------------------------------


#pragma hdrstop

#include "Timer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

Timer *Timer::m_Instance = 0;

Timer *Timer::Instance() {
	if (!m_Instance)
		m_Instance = new Timer;

	return m_Instance;
}

Timer::Timer() {
	m_Ticks = 0;
	m_UnsafeTicks = 0;
	m_Frame = 0;
}

void Timer::Initialize(ticks_t ticks) {
	m_Ticks = ticks;
	m_UnsafeTicks = (unsafe_ticks_t)ticks;
	m_Frame = 0;
}

unsafe_ticks_t Timer::Update(unsafe_ticks_t ticks) {
	unsafe_ticks_t delta;

	if (ticks < m_UnsafeTicks) {
		delta = (unsafe_ticks_t)((ticks_t)ticks + (ticks_t)UNSAFE_TICKS_LENGTH - (ticks_t)m_UnsafeTicks);
	} else {
		delta = ticks - m_UnsafeTicks;
	}

	m_UnsafeTicks = ticks;
	m_Ticks += delta;

	m_Frame++;

	return delta;
}

ticks_t Timer::GetTime() {
	return m_Ticks;
}

ticks_t Timer::GetFrame() {
	return m_Frame;
}


