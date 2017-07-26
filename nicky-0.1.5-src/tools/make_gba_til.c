
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

#define MAX_SPRITE_W        300
#define MAX_SPRITE_H        300
#define MAX_TILS_ENTRIES    256
#define MAX_SPRITE_ENTRIES   40


typedef struct {
	int w, h;
	uint8 mode;
} gba_mode_t;

typedef struct {
	int tile_w, tile_h;
	uint8 gba_mode;
	uint8 num_tiles;
	uint16 size;
	uint8 *ptr;
} til_sprite_entry_t;

typedef struct {
	uint8 num_sprite_entries;
	uint8 num_vspr;
	uint8 num_hspr;
	til_sprite_entry_t sprite_entries_table[MAX_SPRITE_ENTRIES];
} til_entry_t;

static const uint8 TILB_signature[] = {
	0x54, 0x49, 0x4C, 0x42
};

static const uint8 TILS_signature[] = {
	0x54, 0x49, 0x4C, 0x53
};

static const char *decor_files_table[] = {
	"DECOR1.BLK",
	"DECOR2.BLK",
	"DECOR3.BLK",
	"DECOR4.BLK"
};

static const char *sprite_files_table[] = {
	"DIGITS.SPR",
	"LIFEBAR.SPR",
	"NICKY.SPR",
	"S01.SPR",
	"S02.SPR",
	"S03.SPR",
	"S04.SPR",
	"S11.SPR",
	"S12.SPR",
	"S13.SPR",
	"S14.SPR"
};

static const gba_mode_t gba_modes_table[] = {
	{  8,  8,  0 },	
	{ 16, 16,  1 },
	{ 32, 32,  2 },
	{ 64, 64,  3 },
	{ 16,  8,  4 },
	{ 32,  8,  5 },
	{ 32, 16,  6 },
	{ 64, 32,  7 },
	{  8, 16,  8 },
	{  8, 32,  9 },
	{ 16, 32, 10 },
	{ 32, 64, 11 }
};

static char src_data_path[256], dst_data_path[256];
static int num_til_entries;
static til_entry_t til_entries_table[MAX_TILS_ENTRIES];
static uint8 temp_sprite_buffer[MAX_SPRITE_W * MAX_SPRITE_H];


static uint8 *read_file(const char *file) {
	FILE *fp;
	int size;
	uint8 *p = 0;
	char file_path[256];
	sprintf(file_path, "%s/%s", src_data_path, file);
	fp = fopen(file_path, "rb");
	if (!fp) {
		printf("ERROR: unable to open file '%s'\n", file);
	} else {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		p = (uint8 *)malloc(size);
		if (p) {
			fread(p, size, 1, fp);
		}
		fclose(fp);
	}
	return p;
}

static uint8 find_gba_mode(int w, int h) {
	int i;
	for (i = 0; i < sizeof(gba_modes_table) / sizeof(gba_modes_table[0]); ++i) {
		const gba_mode_t *gm = &gba_modes_table[i];
		if (gm->w == w && gm->h == h) {
			return gm->mode;
		}
	}
	printf("ERROR: invalid sprite dimensions w=%d h=%d\n", w, h);
	return -1;
}

static void write_uint16(uint16 n, FILE *fp) {
	fputc(n & 0xFF, fp);
	fputc(n >> 8, fp);
}

static void write_uint32(uint32 n, FILE *fp) {
	write_uint16(n & 0xFFFF, fp);
	write_uint16(n >> 16, fp);
}

static void make_TIL_filename(const char *filename, char *til_filename) {
	char *p;
	strcpy(til_filename, filename);
	p = strrchr(til_filename, '.');
	assert(p);
	strcpy(p + 1, "TIL");
}

static uint8 *pack4b_cpy(uint8 *dst, const uint8 *src, int count) {
	int i;
	assert((count & 1) == 0);
	for (i = 0; i < count; ++i) {
		uint8 b = *src++;
		assert((b & 0xF0) == 0);
		if (i & 1) {
			*dst |= b << 4;
			++dst;
		} else {
			*dst = b;
		}
	}
	return dst;
}

static void make_tiles(uint8 *dst, int dst_w, int dst_h, const uint8 *src, int src_w, int src_h, int src_pitch) {
	int scale_w = src_w / dst_w;
	int scale_h = src_h / dst_h;
	int j, i, h;
	assert(dst_w <= src_w && dst_h <= src_h);
	for (j = 0; j < scale_h; ++j) {
		for (i = 0; i < scale_w; ++i) {
			const uint8 *src_ptr = src + i * dst_w;
			for (h = 0; h < dst_h; ++h) {
				dst = pack4b_cpy(dst, src_ptr, dst_w);
				src_ptr += src_pitch;
			}
		}
		src += dst_h * src_pitch;
	}
}

static void decode_SPR(const uint8 *src, int w, int h, uint8 *dst, int dst_pitch) {
	int j, i, b, p;
	for (j = 0; j < h; ++j) {
		for (i = 0; i < (w + 7) / 8; ++i) {
			uint8 data[4];
			data[0] = *src++;
			data[1] = *src++;
			data[2] = *src++;
			data[3] = *src++;
			for (b = 0; b < 8; ++b) {
				uint8 color = 0;
				const uint8 mask = 1 << (7 - b);
				for (p = 0; p < 4; ++p) {
					if (data[p] & mask) {
						color |= 1 << p;
					}
				}
				dst[i * 8 + b] = color;
			}
		}
		dst += dst_pitch;
	}
}

static int round_up_dim(int d) {
	static const int dim_table[] = { 8, 16, 32 };
	int i;
	assert(d > 0);
	for (i = 0; i < 3; ++i) {
		if (d <= dim_table[i]) {
			return dim_table[i];
		}
	}	
	return 32;
}

static void convert_SPR(const uint8 *spr) {
	int i;
	int num_sprites = read_uint16LE(spr) / 2;
	num_til_entries = 0;
	for (i = 0; i < num_sprites; ++i) {
		til_entry_t *te;
		int j, k;
		const uint8 *src;
		int cur_spr_h, cur_spr_w;
		const uint8 *spr_data = spr + read_uint16LE(spr + i * 2);
		const int spr_w = read_uint16LE(spr_data + 0);
		const int spr_h = read_uint16LE(spr_data + 2);
	
		assert(num_til_entries < MAX_TILS_ENTRIES);
		assert(spr_w <= MAX_SPRITE_W && spr_h <= MAX_SPRITE_H);
		te = &til_entries_table[num_til_entries];
		
		if (spr_w == 0 || spr_h == 0) {
			++num_til_entries;
			memset(te, 0, sizeof(til_entry_t));
			continue;
		}

		memset(temp_sprite_buffer, 0, sizeof(temp_sprite_buffer));
		decode_SPR(spr_data + 4, spr_w, spr_h, temp_sprite_buffer, MAX_SPRITE_W);
		
		te->num_sprite_entries = 0;
		te->num_vspr = (spr_h + 31) / 32;
		te->num_hspr = (spr_w + 31) / 32;
		
		assert(te->num_vspr != 0 && te->num_hspr != 0);
		
		src = temp_sprite_buffer;
		cur_spr_h = spr_h;
		for (j = 0; j < te->num_vspr; ++j, cur_spr_h -= 32) {
			cur_spr_w = spr_w;
			for (k = 0; k < te->num_hspr; ++k, cur_spr_w -= 32) {
				til_sprite_entry_t *tse;
				assert(te->num_sprite_entries < MAX_SPRITE_ENTRIES);
				tse = &te->sprite_entries_table[te->num_sprite_entries];
				tse->tile_h = round_up_dim(cur_spr_h);
				tse->tile_w = round_up_dim(cur_spr_w);
				tse->gba_mode = find_gba_mode(tse->tile_w, tse->tile_h);
				tse->num_tiles = (tse->tile_h / 8) * (tse->tile_w / 8);
				tse->size = tse->tile_h * tse->tile_w / 2;
				tse->ptr = (uint8 *)malloc(tse->size);
				if (tse->ptr) {
					memset(tse->ptr, 0, tse->size);
					make_tiles(tse->ptr, 8, 8, src + k * 32, tse->tile_w, tse->tile_h, MAX_SPRITE_W);
				}
				++te->num_sprite_entries;
			}
			src += 32 * MAX_SPRITE_W;
		}
		++num_til_entries;
	}
}

static void write_TILS_file(const char *filename) {
	FILE *fp;
	char til_filename[256], file_path[256];
	make_TIL_filename(filename, til_filename);
	printf("Writing '%s'... ", til_filename);
	sprintf(file_path, "%s/%s", dst_data_path, til_filename);
	fp = fopen(file_path, "wb");
	if (!fp) {
		printf("Error\n");
	} else {
		int i, j;
		uint32 offs;
		fwrite(TILS_signature, 4, 1, fp);
		offs = num_til_entries * 4;
		for (i = 0; i < num_til_entries; ++i) {
			til_entry_t *te = &til_entries_table[i];
			write_uint32(offs, fp);
			offs += 4;
			for (j = 0; j < te->num_sprite_entries; ++j) {
				til_sprite_entry_t *tse = &te->sprite_entries_table[j];
				offs += tse->size + 8;
			}
		}
		for (i = 0; i < num_til_entries; ++i) {
			til_entry_t *te = &til_entries_table[i];
			fputc(te->num_sprite_entries, fp);
			fputc(te->num_vspr, fp);
			fputc(te->num_hspr, fp);
			fputc(0, fp); /* padding */
			for (j = 0; j < te->num_sprite_entries; ++j) {
				til_sprite_entry_t *tse = &te->sprite_entries_table[j];
				write_uint16(tse->tile_h, fp);
				write_uint16(tse->tile_w, fp);
				fputc(tse->gba_mode, fp);
				fputc(tse->num_tiles, fp);
				write_uint16(tse->size, fp);
				if (tse->size != 0) {
					fwrite(tse->ptr, tse->size, 1, fp);
					free(tse->ptr);
				}
				assert((tse->size & 3) == 0);
			}
		}
		fclose(fp);
		printf("Ok\n");
		num_til_entries = 0;
	}
}

static void decode_BLK(const uint8 *src, uint8 *dst, int dst_pitch) {
	int y, d, b, p;
	for (y = 0; y < 16; ++y) {
		uint8 data[2][4];
		data[0][0] = *src++;
		data[1][0] = *src++;
		data[0][1] = *src++;
		data[1][1] = *src++;
		data[0][2] = *src++;
		data[1][2] = *src++;
		data[0][3] = *src++;
		data[1][3] = *src++;
		for (d = 0; d < 2; ++d) {
			for (b = 0; b < 8; ++b) {
				uint8 color = 0;
				const uint8 mask = 1 << (7 - b);
				for (p = 0; p < 4; ++p) {
					if (data[d][p] & mask) {
						color |= 1 << p;
					}
				}
				dst[d * 8 + b] = color;
			}
		}
		dst += dst_pitch;
	}
}

static void convert_BLK(const uint8 *blk) {
	int tile_num;
	til_entry_t *te;
	til_sprite_entry_t *tse;
	num_til_entries = 0;
	for (tile_num = 0; tile_num < 256; ++tile_num) {
		assert(num_til_entries < MAX_TILS_ENTRIES);
		te = &til_entries_table[tile_num];
		te->num_sprite_entries = 1;
		te->num_vspr = 1;
		te->num_hspr = 1;
		tse = &te->sprite_entries_table[0];
		tse->gba_mode = find_gba_mode(8, 8);
		tse->num_tiles = 4;
		tse->size = 8 * 8 / 2 * 4;
		tse->ptr = (uint8 *)malloc(tse->size);
		if (tse->ptr) {
			memset(temp_sprite_buffer, 0, sizeof(temp_sprite_buffer));
			decode_BLK(blk, temp_sprite_buffer, MAX_SPRITE_W);
			memset(tse->ptr, 0, tse->size);
			make_tiles(tse->ptr, 8, 8, temp_sprite_buffer, 16, 16, MAX_SPRITE_W);
		}
		++num_til_entries;
		blk += 16 * 16 / 2;
	}
}

static void write_TILB_file(const char *filename) {
	FILE *fp;
	char til_filename[256], file_path[256];
	make_TIL_filename(filename, til_filename);
	printf("Writing '%s'... ", til_filename);
	sprintf(file_path, "%s/%s", dst_data_path, til_filename);
	fp = fopen(file_path, "wb");
	if (!fp) {
		printf("Error\n");
	} else {
		int i;
		fwrite(TILB_signature, 4, 1, fp);
		for (i = 0; i < num_til_entries; ++i) {
			til_sprite_entry_t *tse = &til_entries_table[i].sprite_entries_table[0];
			fwrite(tse->ptr, tse->size, 1, fp);
			free(tse->ptr);
		}
		fclose(fp);
		printf("Ok\n");
		num_til_entries = 0;
	}
}

int main(int argc, char *argv[]) {
	if (argc >= 3) {
		int i;
		strcpy(src_data_path, argv[1]);
		strcpy(dst_data_path, argv[2]);
		for (i = 0; i < sizeof(sprite_files_table) / sizeof(sprite_files_table[0]); ++i) {
			uint8 *spr_buf = read_file(sprite_files_table[i]);
			if (spr_buf) {
				convert_SPR(spr_buf);
				free(spr_buf);
				write_TILS_file(sprite_files_table[i]);
			}
		}
		for (i = 0; i < sizeof(decor_files_table) / sizeof(decor_files_table[0]); ++i) {
			uint8 *blk_buf = read_file(decor_files_table[i]);
			if (blk_buf) {
				convert_BLK(blk_buf);
				free(blk_buf);
				write_TILB_file(decor_files_table[i]);
			}
		}
	}
	return 0;
}
