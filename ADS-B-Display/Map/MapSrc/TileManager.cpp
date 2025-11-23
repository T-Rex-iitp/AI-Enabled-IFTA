//---------------------------------------------------------------------------


#pragma hdrstop

#include "TileManager.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
TileManager::TileManager(TileStorage *ts): m_TextureRoot(new TextureTile(0, 0, 0, TextureTilePtr(0))) {
	m_FirstTileStorage = ts;
	m_nTextureTiles = 1;
}

TileManager::~TileManager() {
}

TextureTilePtr TileManager::GetTexture(int x, int y, int level) {
	TextureTilePtr cur = m_TextureRoot;

	for (int curlevel = 0; curlevel <= level; curlevel++) {
		int cx = (x >> (level - curlevel));
		int cy = (y >> (level - curlevel));

		int dx = cx & 1;
		int dy = cy & 1;

		if (cur->GetChild(dx, dy) == 0) {
			TextureTilePtr child = new TextureTile(cx, cy, curlevel, cur);

			cur->SetChild(dx, dy, child);
			m_nTextureTiles++;
			m_FirstTileStorage->Enqueue(TilePtr((Tile*)child.GetPtr()));	// XXX: unsafe. double check
			cur = child;
		} else
			cur = cur->GetChild(dx, dy);

		cur->Touch();
	}

	return cur;
}

TextureTilePtr TileManager::FindTextureToDrop(TextureTilePtr cur, TextureTilePtr best) {
	if (cur == 0)
		return best;

	if (cur->GetParent() != 0 && cur->IsLeaf() && cur->IsOld()) {
		if (best == 0 || cur->GetLevel() > best->GetLevel())
			best = cur;
		else if (best == 0 || cur->GetAge() > best->GetAge())
			best = cur;
	}

	for (int i = 0; i < 4; i++)
		best = FindTextureToDrop(cur->GetChild(i), best);

	return best;
}

int TileManager::Cleanup() {
	/*
	 * How cache cleanup works:
	 *
	 * 0. Tiles with children are not dropped
	 * 0. Tiles recently used (!IsOld) are not dropped
	 * 0. Root is not dropped
	 * 1. Tile with highest level is dropped first.
	 * 2. Of same level, Tile with higher age is dropped first.
	 *
	 * This ensures:
	 * - Minimal number of textures loaded
	 * - All visible tiles are always in memory
	 * - No reloading tiles on zoomout
	 */

	if (m_nTextureTiles <= DEFAULT_MAX_TEXTURES)
		return 0;

	TextureTilePtr victim = FindTextureToDrop(m_TextureRoot, 0);

	if (victim == 0)
		return 0;

	for (int i = 0; i < 4; i++)
		if (victim->GetParent()->GetChild(i) == victim) {
			victim->GetParent()->SetChild(i, 0);	/* after this, victim is doomed */
			victim->Unload();			/* be sure no opengl will be touched in threads XXX: review this */
			m_nTextureTiles--;
			return 1;
		}

	return 0;
		}


