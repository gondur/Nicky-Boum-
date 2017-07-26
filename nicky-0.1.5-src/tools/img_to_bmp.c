#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"


typedef struct {
	uint8 b, g, r;
	uint8 reserved;
} rgb_quad_t;

typedef struct {
	uint16 type;
	uint32 size;
	uint16 reserved1;
	uint16 reserved2;
	uint32 off_bits;
} bmp_file_header_t;

typedef struct {
	uint32 size;
	uint32 w, h;
	uint16 planes;
	uint16 bit_count;
	uint32 compression;
	uint32 size_image;
	uint32 x_pels_per_meter;
	uint32 y_pels_per_meter;
	uint32 num_colors_used;
	uint32 num_colors_important;
} bmp_info_header_t;


static int file_size(FILE *fp) {
	int sz = 0;
	int pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return sz;
}

static void file_write_uint16(FILE *fp, uint16 n) {
	fputc(n & 0xFF, fp);
	fputc(n >> 8, fp);
}

static void file_write_uint32(FILE *fp, uint32 n) {
	fputc(n & 0xFF, fp);
	fputc(n >> 8, fp);
	fputc(n >> 16, fp);
	fputc(n >> 24, fp);
}

static void file_write_rgb_quad(FILE *fp, const rgb_quad_t *quad) {
	fputc(quad->b, fp);
	fputc(quad->g, fp);
	fputc(quad->r, fp);
	fputc(quad->reserved, fp);
}

static void file_write_bmp_file_header(FILE *fp, const bmp_file_header_t *file_header) {
	file_write_uint16(fp, file_header->type);
	file_write_uint32(fp, file_header->size);
	file_write_uint16(fp, file_header->reserved1);
	file_write_uint16(fp, file_header->reserved2);
	file_write_uint32(fp, file_header->off_bits);
}

static void file_write_bmp_info_header(FILE *fp, const bmp_info_header_t *info_header) {
	file_write_uint32(fp, info_header->size);
	file_write_uint32(fp, info_header->w);
	file_write_uint32(fp, info_header->h);
	file_write_uint16(fp, info_header->planes);
	file_write_uint16(fp, info_header->bit_count);
	file_write_uint32(fp, info_header->compression);
	file_write_uint32(fp, info_header->size_image);
	file_write_uint32(fp, info_header->x_pels_per_meter);
	file_write_uint32(fp, info_header->y_pels_per_meter);
	file_write_uint32(fp, info_header->num_colors_used);
	file_write_uint32(fp, info_header->num_colors_important);
}

static uint8 img_decode_buffer[32000];
static uint8 img_hflip_buffer[32000];

static void img_decode_pc(const uint8 *src, uint8 *dst) {
	int y, x, i, j;
	for (y = 0; y < 200; ++y) {
		for (x = 0; x < 40; ++x) {
			for (i = 0; i < 8; ++i) {
				const uint8 mask = 1 << (7 - i);
				uint8 c = 0;
				for (j = 0; j < 4; ++j) {
					if (src[j * 40 + x] & mask) {
						c |= 1 << j;
					}
				}
				if (i & 1) {
					*dst |= c;
					++dst;
				} else {
					*dst = c << 4;
				}
			}
		}
		src += 40 * 4;
	}
}

static void img_decode_amiga(const uint8 *src, uint8 *dst) {
	int y, x, i, j;
	for (y = 0; y < 200; ++y) {
		for (x = 0; x < 40; ++x) {
			for (i = 0; i < 8; ++i) {
				const uint8 mask = 1 << (7 - i);
				uint8 c = 0;
				for (j = 0; j < 4; ++j) {
					if (src[j * 200 * 40] & mask) {
						c |= 1 << j;
					}
				}
				if (i & 1) {
					*dst |= c;
					++dst;
				} else {
					*dst = c << 4;
				}
			}
			++src;
		}
	}
}

static void img_hflip(const uint8 *src, uint8 *dst) {
	int y;
	src += 200 * 320 / 2;
	for (y = 0; y < 200; ++y) {
		memcpy(dst, src, 160);
		dst += 160;
		src -= 160;
	}
}

static void img_convert(const uint8 *img, const uint8 *pal, FILE *fp, int amiga_flag) {
	int i;
	bmp_file_header_t file_header;
	bmp_info_header_t info_header;
	
	/* file header */
	memset(&file_header, 0, sizeof(file_header));
	file_header.type = 0x4D42;
	file_header.size = 14 + 40 + 4 * 16 + 32000;
	file_header.reserved1 = 0;
	file_header.reserved2 = 0;
	file_header.off_bits = 14 + 40 + 4 * 16;
	file_write_bmp_file_header(fp, &file_header);
	
	/* info header */
	memset(&info_header, 0, sizeof(info_header));
	info_header.size = 40;
	info_header.w = 320;
	info_header.h = 200;
	info_header.planes = 1;
	info_header.bit_count = 4;
	info_header.compression = 0;
	info_header.size_image = 320 * 200 / 2;
	info_header.x_pels_per_meter = 0;
	info_header.y_pels_per_meter = 0;
	info_header.num_colors_used = 0;
	info_header.num_colors_important = 0;
	file_write_bmp_info_header(fp, &info_header);
	
	/* palette */
	for (i = 0; i < 16; ++i) {
		uint8 r, g, b;
		rgb_quad_t quad;
		uint16 color = read_uint16BE(pal); pal += 2;
		r = (color >> 8) & 0xF;
		g = (color >> 4) & 0xF;
		b = (color >> 0) & 0xF;
		memset(&quad, 0, sizeof(quad));
		quad.b = (b << 4) | b;
		quad.g = (g << 4) | g;
		quad.r = (r << 4) | r;
		file_write_rgb_quad(fp, &quad);
	}
	
	/* bitmap bits */
	if (amiga_flag) {
		img_decode_amiga(img, img_decode_buffer);
	} else {
		img_decode_pc(img, img_decode_buffer);
	}
	
	/* hflip */
	img_hflip(img_decode_buffer, img_hflip_buffer);
	fwrite(img_hflip_buffer, 1, 32000, fp);
}

static void make_bmp_filename(const char *img_file, char *bmp_file) {
	char *p;
	strcpy(bmp_file, img_file);
	p = bmp_file + strlen(bmp_file) - 1;
	while (p >= bmp_file && *p != '.') {
		--p;
	}
	if (*p == '.') {
		strcpy(p + 1, "bmp");
	}
}

int main(int argc, char *argv[]) {
	if (argc >= 3) {
		int amiga_flag = (argc == 4) && strcmp(argv[3], "-a") == 0;
		FILE *fp_img = fopen(argv[1], "rb");
		FILE *fp_pal = fopen(argv[2], "rb");
		if (fp_img && fp_pal) {
			FILE *fp_out;
			char filename[30];
			make_bmp_filename(argv[1], filename);
			fp_out = fopen(filename, "wb");
			if (fp_out) {
				int sz;
				uint8 *img, *pal;
				sz = file_size(fp_img);
				assert(sz == 32000);
				img = (uint8 *)malloc(sz);
				assert(img);
				fread(img, 1, sz, fp_img);
				sz = file_size(fp_pal);
				assert(sz == 32);
				pal = (uint8 *)malloc(sz);
				assert(pal);
				fread(pal, 1, sz, fp_pal);
				img_convert(img, pal, fp_out, amiga_flag);
				free(img);
				free(pal);
				fclose(fp_out);
			}
			fclose(fp_img);
			fclose(fp_pal);
		}
	}
	return 0;
}
