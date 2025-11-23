#ifndef __GEFETCH_INTERNAL_H__
#define __GEFETCH_INTERNAL_H__
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "gefetch.h"

#ifdef WIN32

#define snprintf _snprintf
#endif

enum {
	METHOD_POST,
	METHOD_GET,
};

/* metadata header */
typedef struct gefetch_meta_header {
	unsigned char	unknown1[12];
	unsigned char	nentries[2];
	unsigned char	unknown2[18];
} gefetch_meta_header;

/* metadata entry */
typedef struct gefetch_meta_entry {
	unsigned char	flags;
#define GEFETCH_META_FLAG_CHILD0	0x01
#define GEFETCH_META_FLAG_CHILD1	0x02
#define GEFETCH_META_FLAG_CHILD2	0x04
#define GEFETCH_META_FLAG_CHILD3	0x08

#define GEFETCH_META_FLAG_UNKNOWN1	0x10
#define GEFETCH_META_FLAG_UNKNOWN2	0x20
#define GEFETCH_META_FLAG_UNKNOWN3	0x40
#define GEFETCH_META_FLAG_TOPOGRAPHY	0x80
	unsigned char	unknown1[3];
	unsigned char	imagenode;
	unsigned char	unknown2[1];
	unsigned char	toponode;
	unsigned char	unknown3[13];
	unsigned char	neigbournodes[8];
	unsigned char	unknown4[4];
} gefetch_meta_entry;

typedef struct gefetch_meta gefetch_meta;

/* metadata chunk */
struct gefetch_meta {
	/* coords */
	int		x;
	int		y;
	int		level;

	/* pointer to next chunk (to form a single linked list) */
	gefetch_meta	*next;
	gefetch_meta	*prev;

	/* count of meta entries */
	int		nentries;

	/* array of meta entries */
	gefetch_meta_entry entries[1];
};

/* gefetch handle */
typedef struct gefetch {
	/* URL to connect to */
	char		*url;

	/* SessionID */
	unsigned char	*sid;


	/* buffer for file being downloaded */
	size_t		currentsize;
	unsigned char	*currentdata;

	/* buffer for dbroot file */
	size_t		dbrootsize;
	unsigned char	*dbrootdata;

	/* meta catalogue */
	size_t		metasize;
	size_t		maxmetasize;
	gefetch_meta	*firstmeta;
	gefetch_meta	*lastmeta;
} gefetch;

/* http request forming and handling (gefetch_http.c) */
gefetch_error gefetch_do_http_request(gefetch *handle, int method, const char *url, void *postdata, size_t postsize, long *code);

/* high level routine to fetch generic data (gefetch_fetch.h) */
gefetch_error gefetch_fetch(gefetch *handle, char *url);

/* routine to convert (x, y, level) triple to string used in urls (gefetch_fetch.h) */
gefetch_error gefetch_coords_to_string(int x, int y, int level, char *buffer, size_t buffersize);

/* metadata handling (gefetch_meta.h) */
void gefetch_remove_extra_meta(gefetch *handle, size_t extraspace);
gefetch_error gefetch_parse_meta(gefetch *handle, gefetch_meta *meta, int x, int y, int level, gefetch_meta_entry **resentry);
gefetch_error gefetch_find_meta(gefetch *handle, int x, int y, int level, gefetch_meta **resmeta);
gefetch_error gefetch_fetch_meta(gefetch *handle, int x, int y, int level);

/* authentication (gefetch_auth.c) */
gefetch_error gefetch_authenticate(gefetch *handle);

/* decryption (gefetch_crypt.c) */
gefetch_error gefetch_decipher_current_file(gefetch *handle);

/* decompression (gefetch_zlib.c) */
gefetch_error gefetch_decompress_current_file(gefetch *handle);

#endif
