//---------------------------------------------------------------------------


#pragma hdrstop

#include "MasterLayer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

MasterLayer::MasterLayer() {
}

MasterLayer::~MasterLayer() {
}

void MasterLayer::BindSlaveLayer(SlaveLayer *layer) {
	m_SlaveLayers.push_back(layer);
}

void MasterLayer::ClearSlaveLayers() {
	m_SlaveLayers.clear();
}

