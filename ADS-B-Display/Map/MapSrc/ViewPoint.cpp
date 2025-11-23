//---------------------------------------------------------------------------


#pragma hdrstop

#include "ViewPoint.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
Viewpoint::Viewpoint() {
	m_Translation = 0.0;
	m_TranslationTime = DEFAULT_TRANSLATION_TIME;
}

Viewpoint::Viewpoint(Eye &eye) {
	m_CurrentEye = eye;
	m_SourceEye = eye;
	m_TargetEye = eye;

	m_Translation = 1.0;
	m_TranslationTime = DEFAULT_TRANSLATION_TIME;
}

Viewpoint::~Viewpoint() {
}

void Viewpoint::SetCurrentCoordinates(Eye &eye) {
	m_CurrentEye = eye;
	m_SourceEye = eye;
	m_TargetEye = eye;

	m_Translation = 1.0;
}

void Viewpoint::SetTargetCoordinates(Eye &eye) {
	m_SourceEye = m_CurrentEye;
	m_TargetEye = eye;

	m_Translation = 0.0;
}

void Viewpoint::Animate(double delta) {
	if (m_Translation >= 1.0)
		return;

	m_Translation += delta/m_TranslationTime;

	if (m_Translation >= 1.0) {
		m_Translation = 1.0;
		m_CurrentEye = m_TargetEye;
		return;
	}

	double trans = TranslationFunction(m_Translation);
	m_CurrentEye = m_SourceEye * (1.0 - trans) + m_TargetEye * trans;
}

double Viewpoint::TranslationFunction(double percent) {
	/*
	 * 1-(1-x)^n is most suitable here
	 */
	return percent * (2.0 - percent);
}

Eye Viewpoint::GetEye() {
	return m_CurrentEye;
}

