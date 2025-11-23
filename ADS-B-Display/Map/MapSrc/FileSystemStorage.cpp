//---------------------------------------------------------------------------


#pragma hdrstop

#include "FileSystemStorage.h"
#include <dir.h>
#include <stdio.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)
#define _CRT_SECURE_NO_WARNINGS



FilesystemStorage::FilesystemStorage(std::string root,bool UseGE) {
	m_StorageRoot = root;
	m_UseGE=UseGE;
}

FilesystemStorage::~FilesystemStorage() {
}

void FilesystemStorage::Process(TilePtr tile) {
	if (!tile->IsLoaded()) { /* loading */
	   std::string path;
		if (m_UseGE)
		 path = m_StorageRoot + PathFromCoordsGE(tile->GetX(), tile->GetY(), tile->GetLevel(), tile->GetType());
		else
         path = m_StorageRoot + PathFromCoordsNASA(tile->GetX(), tile->GetY(), tile->GetLevel(), tile->GetType());
		try {
			int f;

			if ((f = open(path.c_str(), O_RDONLY | O_BINARY)) == -1)
			{
			   // Removed Exception DP and added return
			   //	throw SysException("cannot open file in filesystem storage", errno);
			   return;

			}

			RawBuffer *buf = new RawBuffer(f);

			try {
				tile->Load(buf, m_pSaveStorage != 0);
			} catch (...) {
				close(f);
				delete buf;
				throw;
			}

			close(f);
		} catch(SysException &) {
			/* unable to load current tile, i.e. tile is not in a storage
			 * THIS is not an error */
		} catch(std::exception &) {
			/* unable to load current tile, i.e. tile is not in a storage
			 * THIS is not an error */
		}
	} else if (tile->IsSaveable()) { /* saving */
		std::string subpath;
		if (m_UseGE)
		   subpath = PathFromCoordsGE(tile->GetX(), tile->GetY(), tile->GetLevel(), tile->GetType());
		else
		   subpath = PathFromCoordsNASA(tile->GetX(), tile->GetY(), tile->GetLevel(), tile->GetType());

		/* store file, creating needed directories */
		std::string::size_type pos = 0;
		while (pos != std::string::npos) {
			if ((pos = subpath.find('\\', pos+1)) == std::string::npos) {
				std::string path = m_StorageRoot + subpath;
				int f;
				if ((f = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666)) == -1) {
					throw SysException("cannot open file in filesystem storage", errno);
				}

				/* for null tile, just create corresponding file */
				if (!tile->IsNull()) {
					RawBuffer *buf = tile->ReleaseRawData();
					if (write(f, buf->Data(), (unsigned int)buf->Size()) != (int)buf->Size()) {
						unlink(path.c_str());	/* ensure no broken files get into cache */
						delete buf;
						close(f);
						throw SysException("cannot write file into filesystem storage", errno);
					}

					delete buf;
				}

				close(f);
                printf("Saved Tile %s\n",path.c_str());
				return;
			} else {
				std::string path = m_StorageRoot + subpath.substr(0, pos);
				if (mkdir(path.c_str()) != 0 && errno != EEXIST)
					throw SysException("cannot mkdir in filesystem storage", errno);
			}
		}
		throw Exception("shouldn't get here");
	}
}

std::string FilesystemStorage::PathFromCoordsGE(int x, int y, int level, int type) {
	std::string path;
	std::string name;

	char *ext = (char *)"";

	if (type == TILETYPE_TEXTURE)
		ext =(char *) ".jpg";

	int deepness = 0;
	for (; level >= 0; level--) {
		int middle = 1 << level;

		if (x < middle && y < middle) {
			path += "0";
			name += "0";
		} else if (x >= middle && y < middle) {
			path += "1";
			name += "1";
		} else if (x < middle && y >= middle) {
			path += "3";
			name += "3";
		} else {
			path += "2";
			name += "2";
		}

		if ((++deepness) % 4 == 0)
			path += "\\";

		x %= middle;
		y %= middle;
	}

	if (deepness % 4 != 0)
		path += "\\";

	return std::string("\\") + path + name + ext;
}
std::string FilesystemStorage::PathFromCoordsNASA(int x, int y, int level, int type) {
	std::string path;
	std::string name;

	int i;
	char *ext =(char *) "";
	ext = (char *)".jpg";

	for (i = 0; i <= level; i++) {
		int bit = 1 << (level-i);

		if (x & bit) {
			if (y & bit)    name += "2";
			else            name += "1";
		} else {
			if (y & bit)    name += "3";
			else            name += "0";
		}

		if (i < (level+1 & ~0x3)) {
			if (x & bit) {
				if (y & bit)    path += "2";
				else            path += "1";
			} else {
				if (y & bit)    path += "3";
				else            path += "0";
			}

			if (i % 4 == 3)
			path += "\\";
		}
	}

	return std::string("\\") + path + name + ext;
}


