
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

/* TODO:
 S00.SPR
 S1x.SPR
*/

#define ICE_TAG   "Ice!"
#define MAX_FILEDUMPS  60

typedef struct {
	int offs;
	int type;
	int size;
	const char *filename;
} file_dump_t;

enum {
	FDT_IMG = 0,
	FDT_PAL,
	FDT_COP,
	FDT_CDG,
	FDT_COD,
	FDT_BLK,
	FDT_MOD,
	FDT_REF,
	FDT_UNK
};

static const char *fileDumpExt[] = {
	"img",
	"pal",
	"cop",
	"cdg",
	"cod",
	"blk",
	"mod",
	"ref",
	"dump"
};

typedef struct {
	int unpacked_size;
	int packed_size;
	int type;
	const char *filename;	
} dump_data_t;

static const dump_data_t dumpData[] = {
	{ 7228, 844, FDT_MOD, "songdead" },
	{ 3132, 481, FDT_MOD, "songmon" },
	{ 5180, 957, FDT_MOD, "songpres" },
	{ 5180, 961, FDT_MOD, "songwin" },
	{ 11324, 2355, FDT_MOD, "song4" },
	{ 22588, 3835, FDT_MOD, "song1" },
	{ 21564, 2950, FDT_MOD, "song3" },
	{ 21564, 3491, FDT_MOD, "song2" },
	
	{ 352, 176, FDT_COP, "degra1" },
	{ 352, 180, FDT_COP, "degra2" },
	{ 352, 147, FDT_COP, "degra3" },
	{ 352, 177, FDT_COP, "degra4" },
	
	{ 0x12A04, 0x8DEA, FDT_REF, "ref1" }, /* ref1.ref + s01.spr */
	{ 0x1611E, 0x9EB2, FDT_REF, "ref2" }, /* ref2.ref + s02.spr */
	{ 0x16AB8, 0xAA8C, FDT_REF, "ref3" }, /* ref3.ref + s03.spr */
	{ 0x16DB2, 0xABA7, FDT_REF, "ref4" }, /* ref4.ref + s04.spr */
	
	{ 6000, 846, FDT_COD, "demo" },
};

/* 18 (s11.spr), 24, 36 monster sprites */
/* 00, 30, 46 ? */

static int lookForIceTags(const uint8 *p, int p_size, file_dump_t *fd, int max) {
	uint8 i;
	int n = 0;
	const uint8 *p_start = p;
	const uint8 *p_end = p + p_size;
	while (p < p_end) {
		if (memcmp(p, ICE_TAG, 4) == 0) {
			int size = read_uint32BE(p + 4);
			if (size >= 100 * 1024) {
				++p;
				printf("WARNING: file too big (%d) offs=0x%X\n", size, p - p_start);
			} else {
				int packed_size = read_uint32BE(p + 4);
				int unpacked_size = read_uint32BE(p + 8);
				int offset = p - p_start;
				switch (unpacked_size) {
				case 32000:
					assert(n < max);
					/* palette before the image */
					memset(fd, 0, sizeof(file_dump_t));
					fd->size = 32;
					fd->offs = offset - 4 - 32;
					fd->type = FDT_PAL;
					++n;
					++fd;
					break;
				}
				assert(n < max);
				memset(fd, 0, sizeof(file_dump_t));
				fd->size = packed_size;
				fd->offs = offset;
				switch (unpacked_size) {
				case 24100:
					/* DECORx.CDG + POSITx.REF (map + ?) */
					fd->type = FDT_CDG;
					break;
				case 34816:
					/* DECORx.BLK + DECORx.REF (data + flag) */
					fd->type = FDT_BLK;
					break;
				case 32000:
					/* I0x.IMG */
					fd->type = FDT_IMG;
					break;
				default:
					fd->type = FDT_UNK;
					for (i = 0; i < sizeof(dumpData) / sizeof(dumpData[0]); ++i) {
						const dump_data_t *dd = &dumpData[i];
						if (dd->unpacked_size == unpacked_size && dd->packed_size == packed_size) {
							fd->type = dd->type;
							fd->filename = dd->filename;
							break;
						}
					}
					break;
				}
				++n;
				++fd;
				/* skip data */
				p += size;
			}
		} else {
			++p;
		}
	}
	return n;
}

static void extractIceFiles(const uint8 *p, int p_size, file_dump_t *fd, int n) {
	int i, prev_offset = 0;
	for (i = 0; i < n; ++i) {
		FILE *fp;
		char filename[20];
		int size, offset, end, skip;
		if (fd->filename) {
			sprintf(filename, "dump/%s.%s", fd->filename, fileDumpExt[fd->type]);
		} else {
			sprintf(filename, "dump/%02d.%s", i, fileDumpExt[fd->type]);
		}
		size = fd->size;
		offset = fd->offs;
		printf("dump entry %d (%s) type=%d size=%d", i, filename, fd->type, size);
		if (memcmp(p + offset, ICE_TAG, 4) == 0) {
			int packed_size = read_uint32BE(p + offset + 4);
			int unpacked_size = read_uint32BE(p + offset + 8);
			assert(packed_size < unpacked_size);
			printf(" (%d)", unpacked_size);
		}
		fp = fopen(filename, "wb");
		assert(fp);
		if (offset + size > p_size) {
			size = p_size - offset;
			printf("WARNING: file truncated\n");
		}
		fwrite(p + offset, 1, size, fp);
		fclose(fp);
		end = offset + size;
		skip = offset - prev_offset;
		printf(" offset=0x%05X end=0x%05X skip=%d\n", offset, end, skip);
		prev_offset = end;
		++fd;
	}
}

static int fileSize(FILE *fp) {
	int sz, pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return sz;
}

static void dumpUnkParts(uint8 *p) {
	static const int unkPart[] = {
		0x0149C, 0x27A06, /* sounds, instruments, sprites */
		0xAD276, 0xD5A5C, /* code */
		0xD5D05, 0xDC000
	};
	uint8 i; 
	for (i = 0; i < sizeof(unkPart) / sizeof(unkPart[0]); i += 2) {
		FILE *fp;
		char filename[20];
		sprintf(filename, "dump/unk%d.raw", i);
		fp = fopen(filename, "wb");
		if (fp) {
			int sz = unkPart[i + 1] - unkPart[i];
			fwrite(p + unkPart[i], 1, sz, fp);
			fclose(fp);
			printf("dump unk entry %d sz = %d\n", i, sz);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc >= 1) {
		FILE *fp = fopen(argv[1], "rb");
		if (fp) {
			int adf_size = fileSize(fp);
			uint8 *p = (uint8 *)malloc(adf_size);
			if (p) {
				int n;
				file_dump_t fd[MAX_FILEDUMPS];
				fread(p, 1, adf_size, fp);
				n = lookForIceTags(p, adf_size, fd, MAX_FILEDUMPS);
				extractIceFiles(p, adf_size, fd, n);
				dumpUnkParts(p);
				free(p);
			}
			fclose(fp);
		}
	}
	return 0;
}
