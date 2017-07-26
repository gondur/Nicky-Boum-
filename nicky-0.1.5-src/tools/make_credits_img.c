
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

static const uint8 credits_text1[] = {
	0x43, 0x4F, 0x4E, 0x47, 0x52, 0x41, 0x54, 0x55, 0x4C, 0x41, 0x54, 0x49, 0x4F, 0x4E, 0x53, 0x5B, 
	0x01, 0x20, 0x5A, 0x4F, 0x4C, 0x44, 0x52, 0x41, 0x4E, 0x45, 0x20, 0x54, 0x48, 0x45, 0x20, 0x53, 
	0x4F, 0x52, 0x43, 0x45, 0x52, 0x45, 0x53, 0x53, 0x5B, 0x53, 0x45, 0x4E, 0x54, 0x20, 0x42, 0x41, 
	0x43, 0x4B, 0x01, 0x20, 0x54, 0x4F, 0x20, 0x54, 0x48, 0x45, 0x20, 0x4E, 0x4F, 0x54, 0x48, 0x49, 
	0x4E, 0x47, 0x4E, 0x45, 0x53, 0x53, 0x5B, 0x44, 0x45, 0x46, 0x45, 0x41, 0x54, 0x45, 0x44, 0x20, 
	0x42, 0x59, 0x01, 0x20, 0x54, 0x48, 0x45, 0x20, 0x56, 0x41, 0x4C, 0x4F, 0x52, 0x4F, 0x55, 0x53, 
	0x20, 0x4E, 0x49, 0x43, 0x4B, 0x59, 0x5C, 0x01, 0x01, 0x4F, 0x4E, 0x20, 0x54, 0x48, 0x45, 0x20, 
	0x46, 0x4C, 0x4F, 0x4F, 0x52, 0x20, 0x41, 0x54, 0x20, 0x54, 0x48, 0x45, 0x20, 0x50, 0x4C, 0x41, 
	0x43, 0x45, 0x20, 0x57, 0x48, 0x45, 0x52, 0x45, 0x01, 0x20, 0x53, 0x48, 0x45, 0x20, 0x48, 0x41, 
	0x53, 0x20, 0x44, 0x49, 0x53, 0x41, 0x50, 0x50, 0x45, 0x41, 0x52, 0x45, 0x44, 0x5B, 0x4E, 0x49, 
	0x43, 0x4B, 0x59, 0x20, 0x50, 0x49, 0x43, 0x4B, 0x53, 0x20, 0x55, 0x50, 0x01, 0x20, 0x41, 0x20, 
	0x4C, 0x49, 0x54, 0x54, 0x4C, 0x45, 0x20, 0x47, 0x4F, 0x4C, 0x44, 0x45, 0x4E, 0x20, 0x4B, 0x45, 
	0x59, 0x5C, 0x01, 0x01, 0x53, 0x45, 0x41, 0x52, 0x43, 0x48, 0x49, 0x4E, 0x47, 0x20, 0x54, 0x48, 
	0x52, 0x4F, 0x55, 0x47, 0x48, 0x20, 0x54, 0x48, 0x45, 0x20, 0x43, 0x41, 0x53, 0x54, 0x4C, 0x45, 
	0x01, 0x20, 0x52, 0x4F, 0x4F, 0x4D, 0x53, 0x5B, 0x4E, 0x49, 0x43, 0x4B, 0x59, 0x20, 0x44, 0x49, 
	0x53, 0x43, 0x4F, 0x56, 0x45, 0x52, 0x53, 0x20, 0x49, 0x4E, 0x20, 0x41, 0x20, 0x44, 0x41, 0x4D, 
	0x50, 0x01, 0x20, 0x44, 0x55, 0x4E, 0x47, 0x45, 0x4F, 0x4E, 0x5C, 0x5C, 0x5C, 0x41, 0x20, 0x43, 
	0x48, 0x45, 0x53, 0x54, 0x5C, 0x00
};

static const uint8 credits_text2[] = {
	0x48, 0x49, 0x53, 0x20, 0x48, 0x41, 0x4E, 0x44, 0x20, 0x54, 0x52, 0x45, 0x4D, 0x42, 0x4C, 0x49, 
	0x4E, 0x47, 0x20, 0x57, 0x49, 0x54, 0x48, 0x20, 0x48, 0x4F, 0x50, 0x45, 0x5B, 0x4E, 0x49, 0x43, 
	0x4B, 0x59, 0x01, 0x20, 0x43, 0x4F, 0x4D, 0x45, 0x53, 0x20, 0x4E, 0x45, 0x41, 0x52, 0x20, 0x41, 
	0x4E, 0x44, 0x20, 0x49, 0x4E, 0x53, 0x45, 0x52, 0x54, 0x53, 0x20, 0x54, 0x48, 0x45, 0x20, 0x4B, 
	0x45, 0x59, 0x01, 0x20, 0x49, 0x4E, 0x54, 0x4F, 0x20, 0x54, 0x48, 0x45, 0x20, 0x4C, 0x4F, 0x43, 
	0x4B, 0x5C, 0x01, 0x01, 0x01, 0x54, 0x48, 0x45, 0x20, 0x43, 0x48, 0x45, 0x53, 0x54, 0x20, 0x4F, 
	0x50, 0x45, 0x4E, 0x53, 0x20, 0x57, 0x49, 0x54, 0x48, 0x4F, 0x55, 0x54, 0x20, 0x44, 0x49, 0x46, 
	0x46, 0x49, 0x43, 0x55, 0x4C, 0x54, 0x59, 0x01, 0x20, 0x55, 0x4E, 0x46, 0x4F, 0x4C, 0x44, 0x49, 
	0x4E, 0x47, 0x20, 0x49, 0x54, 0x53, 0x20, 0x53, 0x45, 0x43, 0x52, 0x45, 0x54, 0x5C, 0x01, 0x4E, 
	0x49, 0x43, 0x4B, 0x59, 0x20, 0x44, 0x49, 0x53, 0x43, 0x4F, 0x56, 0x45, 0x52, 0x53, 0x20, 0x41, 
	0x4E, 0x20, 0x45, 0x4C, 0x49, 0x58, 0x49, 0x52, 0x5B, 0x01, 0x20, 0x54, 0x48, 0x45, 0x20, 0x4F, 
	0x4E, 0x4C, 0x59, 0x20, 0x4D, 0x45, 0x44, 0x49, 0x43, 0x49, 0x4E, 0x45, 0x20, 0x54, 0x4F, 0x20, 
	0x54, 0x48, 0x45, 0x20, 0x45, 0x56, 0x49, 0x4C, 0x20, 0x53, 0x50, 0x45, 0x4C, 0x4C, 0x01, 0x20, 
	0x43, 0x41, 0x53, 0x54, 0x20, 0x42, 0x59, 0x20, 0x5A, 0x4F, 0x4C, 0x44, 0x52, 0x41, 0x4E, 0x45, 
	0x20, 0x4F, 0x4E, 0x20, 0x48, 0x49, 0x53, 0x20, 0x47, 0x52, 0x41, 0x4E, 0x44, 0x46, 0x41, 0x54, 
	0x48, 0x45, 0x52, 0x01, 0x20, 0x44, 0x55, 0x52, 0x49, 0x4E, 0x47, 0x20, 0x48, 0x49, 0x53, 0x20, 
	0x43, 0x41, 0x50, 0x54, 0x49, 0x56, 0x49, 0x54, 0x59, 0x5C, 0x00
};

static const uint8 credits_text3[] = {
	0x01, 0x01, 0x47, 0x52, 0x41, 0x42, 0x42, 0x49, 0x4E, 0x47, 0x20, 0x54, 0x48, 0x45, 0x20, 0x50, 
	0x52, 0x45, 0x43, 0x49, 0x4F, 0x55, 0x53, 0x20, 0x46, 0x4C, 0x41, 0x53, 0x4B, 0x5B, 0x4E, 0x49, 
	0x43, 0x4B, 0x59, 0x01, 0x20, 0x54, 0x41, 0x4B, 0x45, 0x53, 0x20, 0x54, 0x48, 0x45, 0x20, 0x50, 
	0x41, 0x54, 0x48, 0x20, 0x48, 0x4F, 0x4D, 0x45, 0x5C, 0x01, 0x01, 0x01, 0x43, 0x52, 0x4F, 0x53, 
	0x53, 0x49, 0x4E, 0x47, 0x20, 0x54, 0x48, 0x52, 0x4F, 0x55, 0x47, 0x48, 0x20, 0x54, 0x48, 0x45, 
	0x20, 0x46, 0x4F, 0x52, 0x45, 0x53, 0x54, 0x5B, 0x48, 0x45, 0x20, 0x54, 0x48, 0x49, 0x4E, 0x4B, 
	0x53, 0x01, 0x20, 0x4F, 0x46, 0x20, 0x48, 0x49, 0x53, 0x20, 0x47, 0x52, 0x41, 0x4E, 0x44, 0x46, 
	0x41, 0x54, 0x48, 0x45, 0x52, 0x20, 0x57, 0x48, 0x4F, 0x20, 0x57, 0x49, 0x4C, 0x4C, 0x20, 0x42, 
	0x45, 0x20, 0x53, 0x4F, 0x4F, 0x4E, 0x01, 0x20, 0x48, 0x45, 0x41, 0x4C, 0x45, 0x44, 0x5C, 0x00
};

static const uint8 credits_text4[] = {
	0x47, 0x52, 0x45, 0x45, 0x54, 0x49, 0x4E, 0x47, 0x53, 0x20, 0x54, 0x4F, 0x01, 0x01, 0x44, 0x41, 
	0x4E, 0x49, 0x45, 0x4C, 0x20, 0x4C, 0x41, 0x4D, 0x42, 0x49, 0x4E, 0x01, 0x4D, 0x49, 0x43, 0x48, 
	0x45, 0x4C, 0x45, 0x20, 0x4C, 0x41, 0x4D, 0x42, 0x49, 0x4E, 0x01, 0x4D, 0x55, 0x52, 0x49, 0x45, 
	0x4C, 0x20, 0x4C, 0x45, 0x43, 0x48, 0x45, 0x4E, 0x45, 0x01, 0x53, 0x59, 0x4C, 0x56, 0x49, 0x41, 
	0x4E, 0x45, 0x20, 0x56, 0x4F, 0x59, 0x4E, 0x4E, 0x45, 0x54, 0x01, 0x50, 0x41, 0x54, 0x52, 0x49, 
	0x43, 0x45, 0x20, 0x4C, 0x41, 0x4D, 0x42, 0x49, 0x4E, 0x01, 0x50, 0x41, 0x54, 0x52, 0x49, 0x43, 
	0x4B, 0x20, 0x52, 0x45, 0x47, 0x4E, 0x41, 0x55, 0x4C, 0x44, 0x01, 0x56, 0x41, 0x4E, 0x45, 0x53, 
	0x53, 0x41, 0x20, 0x4C, 0x41, 0x4D, 0x42, 0x49, 0x4E, 0x01, 0x53, 0x41, 0x4E, 0x44, 0x52, 0x49, 
	0x4E, 0x45, 0x20, 0x46, 0x45, 0x41, 0x55, 0x56, 0x45, 0x41, 0x55, 0x01, 0x41, 0x4C, 0x41, 0x49, 
	0x4E, 0x20, 0x52, 0x45, 0x47, 0x4E, 0x41, 0x55, 0x4C, 0x44, 0x01, 0x50, 0x49, 0x45, 0x52, 0x52, 
	0x45, 0x20, 0x56, 0x4F, 0x59, 0x4E, 0x4E, 0x45, 0x54, 0x01, 0x4C, 0x55, 0x44, 0x4F, 0x56, 0x49, 
	0x43, 0x01, 0x50, 0x41, 0x54, 0x52, 0x49, 0x43, 0x49, 0x41, 0x20, 0x47, 0x41, 0x4C, 0x49, 0x42, 
	0x45, 0x52, 0x54, 0x01, 0x54, 0x48, 0x49, 0x45, 0x52, 0x52, 0x59, 0x20, 0x42, 0x4C, 0x41, 0x4E, 
	0x4F, 0x54, 0x00
};

typedef struct credits_page_t {
	int x;
	int y;
	const uint8 *text_data;
} credits_page_t;

static credits_page_t credits_page_table[4] = {
	{ 24, 40, credits_text1 },
	{ 24, 34, credits_text2 },
	{ 24, 34, credits_text3 },
	{ 88, 14, credits_text4 }
};

static uint8 img_data_temp1[320 * 200];
static uint8 img_data_temp2[160 * 200];

static void decode_img_pc(const uint8 *src, uint8 *dst) {
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
				*dst++ = c;
			}
		}
		src += 40 * 4;
	}
}

static void encode_img_pc(const uint8 *src, uint8 *dst) {
	int y, x, m;
	for (y = 0; y < 200; ++y) {
		int mask = 0x80;
		memset(dst, 0, 320);
		for (x = 0; x < 320; ++x) {
			uint8 color = *src++;
			for (m = 0; m < 4; ++m) {
				if (color & (1 << m)) {
					dst[m * 40 + (x / 8)] |= mask;
				}
			}
			mask >>= 1;
			if (mask == 0) {
				mask = 0x80;
			}
		}
		dst += 160;
	}
}

static void blit_sprite(const uint8 *font_data, uint8 spr, int x, int y, uint8 *dst) {
	int j;
	const uint8 *spr_data = font_data + read_uint16LE(font_data + spr * 2);
	const int spr_w = read_uint16LE(spr_data + 0);
	const int spr_h = read_uint16LE(spr_data + 2);
	spr_data += 4;
	dst += y * 320;
	for (j = 0; j < spr_h; ++j) {
		int i;
		int cy = y + j;
		for (i = 0; i < (spr_w + 7) / 8; ++i) {
			int b;
			uint8 data[4];
			data[0] = *spr_data++;
			data[1] = *spr_data++;
			data[2] = *spr_data++;
			data[3] = *spr_data++;
			for (b = 0; b < 8; ++b) {
				int p;
				int cx = x + i * 8 + b;
				if (cx >= 0 && cx < 320 && cy >= 0 && cy < 200) {
					uint8 color = 0;
					for (p = 0; p < 4; ++p) {
						if (data[p] & (1 << (7 - b))) {
							color |= 1 << p;
						}
					}
					if (color != 0) {
						dst[cx] = color;
					}
				}
			}
		}
		dst += 320;
	}
}

static void draw_credits_text(int x, int y, const uint8 *text_data, const uint8 *font_data, uint8 *img_data) {
	int left_x = x;
	while (1) {
		uint8 ch = *text_data++;
		if (ch == 0) {
			break;
		} else if (ch == 1) {
			x = left_x;
			y += 12;
		} else {
			if (ch != 0x20) {
				blit_sprite(font_data, ch - 0x40, x, y, img_data);
			}
			x += 8;
		}
	}
}

static void make_credits_img(const uint8 *img_data, const uint8 *font_data) {
	int i;
	for (i = 0; i < 4; ++i) {
		FILE *fp;
		char img_filename[20];
		const credits_page_t *cp = &credits_page_table[i];
		decode_img_pc(img_data, img_data_temp1);
		draw_credits_text(cp->x, cp->y, cp->text_data, font_data, img_data_temp1);
		encode_img_pc(img_data_temp1, img_data_temp2);
		sprintf(img_filename, "I%02d.IMG.dec", 8 + i);
		fp = fopen(img_filename, "wb");
		if (fp) {
			fwrite(img_data_temp2, 160 * 200, 1, fp);
			fclose(fp);
		}
	}
}

static uint8 *read_file(const char *filename) {
	uint8 *buf = 0;
	FILE *fp = fopen(filename, "rb");
	if (fp) {
		int size;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buf = (uint8 *)malloc(size);
		if (buf) {
			int sz = fread(buf, size, 1, fp);
			if (sz != 1) {
				printf("Error reading '%s'\n", filename);
				free(buf);
				buf = 0;
			}
		}
		fclose(fp);
	}
	return buf;
}
	
int main(int argc, char *argv[]) {
	uint8 *fontend_spr = read_file("pics_nicky1/FONTEND.SPR");
	uint8 *i07_img = read_file("pics_nicky1/I07.IMG.dec");
	if (!fontend_spr) {
		printf("Error opening 'FONTEND.SPR'\n");
		return -1;
	}
	if (!i07_img) {
		printf("Error opening 'I07.IMG'\n");
		return -1;
	}
	make_credits_img(i07_img, fontend_spr);
	return 0;
}