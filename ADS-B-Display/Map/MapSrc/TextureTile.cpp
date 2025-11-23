//---------------------------------------------------------------------------


#pragma hdrstop

#include "TextureTile.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
/* *tors */
TextureTile::TextureTile(int x, int y, int level, TextureTilePtr parent): Tile(x, y, level) {
	m_Texture = 0;
	m_Parent = parent;
	m_Child[0] = m_Child[1] = m_Child[2] = m_Child[3] = TextureTilePtr(0);
}

TextureTile::~TextureTile() {
	delete m_Texture;
}

/* coord ops */
int TextureTile::GetType() {
	return TILETYPE_TEXTURE;
}       

/* quadtree ops */
TextureTilePtr TextureTile::GetParent() {
	return m_Parent;
}

TextureTilePtr TextureTile::GetChild(int n) {
	return m_Child[n];
}

TextureTilePtr TextureTile::GetChild(int x, int y) {
	return m_Child[ ((y & 1) << 1) | ((x^y) & 1) ];
}

void TextureTile::SetChild(int n, TextureTilePtr child) {
	m_Child[n] = child;
}

void TextureTile::SetChild(int x, int y, TextureTilePtr child) {
	m_Child[ ((y & 1) << 1) | ((x^y) & 1) ] = child;
}

int TextureTile::IsLeaf() {
	return	m_Child[0] == 0 &&
		m_Child[1] == 0 &&
		m_Child[2] == 0 &&
		m_Child[3] == 0;
}

/* load/save */
void TextureTile::Load(RawBuffer *data, int keep) {
	Texture *tex = new Texture;

	try {
		tex->LoadJPEG(TEXTURE_SOURCE_MEM, data->Data(), data->Size());
	} catch (...) {
		delete tex;
		delete data;
		throw;
	}

	m_Texture = tex;
	Tile::Load(data, keep);
}

int TextureTile::IsLoaded() {
	return (IsNull() || m_Texture);
}

void TextureTile::SetTexture() {
	Touch();

	if (IsReady())
		m_Texture->SetTexture();
}

void TextureTile::Unload() {
	if (m_Texture)
		m_Texture->Unload();
}

