
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

#define AMIGA_DATA_DIR "amiga_data/dump"
#define EXTRA_DATA_DIR "extra_data"

static const uint8 rev_bitmask_table[] = { 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1 };

static void make_LIFEBAR_SPR() {
	int i, b, x, y, n, offset;
	uint8 lifebar_bitmap[75 * 6];
	const int pitch = 75;
	FILE *fp_out;
	fp_out = fopen(EXTRA_DATA_DIR "/LIFEBAR.SPR", "wb");
	if (fp_out) {
		/* write offsets */
		offset = 37 * 2;
		for (i = 0; i < 37; ++i) {
			fputc(offset & 0xFF, fp_out);
			fputc(offset >> 8, fp_out);
			offset += 4 + ((75 + 7) & ~7) * 6 / 2;
		}
		/* fill with transparent color */
		memset(lifebar_bitmap, 0, sizeof(lifebar_bitmap));
		/* draw white border */
		for (i = 0; i <= 73; ++i) {
			lifebar_bitmap[pitch * 0 + i] = 1;
			lifebar_bitmap[pitch * 4 + i] = 1;
		}
		for (i = 1; i <= 3; ++i) {
			lifebar_bitmap[pitch * i + 0] = 1;
			lifebar_bitmap[pitch * i + 73] = 1;
		}
		/* draw black border */
		for (i = 1; i <= 73; ++i) {
			lifebar_bitmap[pitch * 5 + i] = 2;
		}
		for (i = 1; i <= 5; ++i) {
			lifebar_bitmap[pitch * i + 74] = 2;
		}

		/* 72 / 2 + 1 sprites */
		for (n = 0; n < 37; ++n) {
			const int color = 2 + n / 3;
			/* fill lifebar with color */
			for (i = 0; i < n * 2; i++) {
				lifebar_bitmap[pitch * 1 + 1 + i] = color;
				lifebar_bitmap[pitch * 2 + 1 + i] = color;
				lifebar_bitmap[pitch * 3 + 1 + i] = color;
			}
			/* write dimensions (LE16) */
			fputc(75, fp_out);
			fputc(0, fp_out);
			fputc(6, fp_out);
			fputc(0, fp_out);
			/* pack bitmap data */
			for (y = 0; y < 6; ++y) {
				for (b = 0; b < (75 + 7) / 8; ++b) {
					uint8 data[4];
					data[0] = 0;
					data[1] = 0;
					data[2] = 0;
					data[3] = 0;
					for (x = 0; x < 8; ++x) {
						uint8 color = 0;
						offset = b * 8 + x;
						if (offset < pitch) {
							color = lifebar_bitmap[pitch * y + offset];
						}
						if (color & 1) {
							data[0] |= rev_bitmask_table[x];
						}
						if (color & 2) {
							data[1] |= rev_bitmask_table[x];
						}
						if (color & 4) {
							data[2] |= rev_bitmask_table[x];
						}
						if (color & 8) {
							data[3] |= rev_bitmask_table[x];
						}
					}
					fwrite(data, 1, 4, fp_out);
				}
			}
		}
		fclose(fp_out);
	}
}

static void make_DIGITS_SPR() {
	/* SRC=> 1:transparent 0:black 2:white */
	/* DST=> 0: transparent 1:white 2:black */
	static const uint8 color_map[16] = { 2, 0, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	int offset;
	int w = 256;
	int h = 256;
	uint8 *tmp_buf;
	FILE *fp_out, *fp_in;
	fp_out = fopen(EXTRA_DATA_DIR "/DIGITS.SPR", "wb");
	assert(fp_out);
	fp_in = fopen("../docs/number_font_3.bmp", "rb");
	assert(fp_in);
	fseek(fp_in, 0x42, SEEK_SET);
	tmp_buf = (uint8 *)malloc(w * h / 2);
	if (tmp_buf) {
		int y, x, i;
		const uint8 *p;
		/* read bitmap data */
		fread(tmp_buf, 1, w * h / 2, fp_in);
		p = tmp_buf + w * h / 2;
		/* write offsets */
		offset = 10 * 2;
		for (i = 0; i < 10; ++i) {
			fputc(offset & 0xFF, fp_out);
			fputc(offset >> 8, fp_out);
			offset += 4 + 8 * 8 / 2;
		}
		for (i = 0; i < 10; ++i) {
			p -= w / 2;
			/* write dimensions (LE16) */
			fputc(8, fp_out);
			fputc(0, fp_out);
			fputc(8, fp_out);
			fputc(0, fp_out);
			/* convert and write data */
			for (y = 0; y < 8; ++y) {
				uint8 data[4];
				uint8 l[8];
				l[0] = p[0] >> 4;
				l[1] = p[0] & 0xF;
				l[2] = p[1] >> 4;
				l[3] = p[1] & 0xF;
				l[4] = p[2] >> 4;
				l[5] = p[2] & 0xF;
				l[6] = p[3] >> 4;
				l[7] = p[3] & 0xF;
				data[0] = 0;
				data[1] = 0;
				data[2] = 0;
				data[3] = 0;
				for (x = 0; x < 8; ++x) {
					uint8 color = color_map[l[x]];
					if (color & 1) {
						data[0] |= rev_bitmask_table[x];
					}
					if (color & 2) {
						data[1] |= rev_bitmask_table[x];
					}
				}
				fwrite(data, 1, 4, fp_out);
				p -= w / 2;
			}
		}
		free(tmp_buf);
	}
	fclose(fp_in);
	fclose(fp_out);
}

static void make_NICKY_SPR() {
	int i, y, x, offset;
	char filename[30];
	FILE *fp_out, *fp_in;
	uint8 buf[512];
	sprintf(filename, EXTRA_DATA_DIR "/NICKY.SPR");
	fp_out = fopen(filename, "wb");
	assert(fp_out);
	fp_in = fopen("amiga_data/Nicky Boum (1992)(Microids).adf", "rb");
	assert(fp_in);
	/* write offsets */
	offset = 43 * 2;
	for (i = 0; i < 34; ++i) {
		fputc(offset & 0xFF, fp_out);
		fputc(offset >> 8, fp_out);
		offset += 4 + 29 * 16 / 2;
	}
	for (i = 34; i < 35; ++i) {
		fputc(offset & 0xFF, fp_out);
		fputc(offset >> 8, fp_out);
		offset += 4;
	}
	for (i = 35; i < 43; ++i) {
		fputc(offset & 0xFF, fp_out);
		fputc(offset >> 8, fp_out);
		offset += 4 + 16 * 16 / 2;
	}
	/* nicky frames */
	fseek(fp_in, 0xD78F4, SEEK_SET);
	for (i = 0; i < 34; ++i) {
		const uint8 *src = buf;
		const int spr_w = 16;
		const int spr_h = 29;
		fread(buf, 1, 31 * 16 / 2, fp_in);
		/* write dimensions (LE16) */
		fputc(spr_w & 0xFF, fp_out);
		fputc(spr_w >> 8, fp_out);
		fputc(spr_h & 0xFF, fp_out);
		fputc(spr_h >> 8, fp_out);
		/* write data */
		for (y = 0; y < 29; ++y) {
			for (x = 0; x < 2; ++x) {
				uint8 data[4];
				data[0] = src[0];
				data[1] = src[2];
				data[2] = src[31 * 4 + 0];
				data[3] = src[31 * 4 + 2];
				++src;
				fwrite(data, 1, 4, fp_out);
			}
			src += 2;
		}
	}
	/* dummy frame (w == 0) (h == 0) */
	fputc(0, fp_out);
	fputc(0, fp_out);
	fputc(0, fp_out);
	fputc(0, fp_out);
	/* stars frames */
	fseek(fp_in, 0xD78F4 + 8428, SEEK_SET);
	for (i = 35; i < 39; ++i) {
		const uint8 *src = buf;
		const int spr_w = 16;
		const int spr_h = 16;
		fread(buf, 1, 18 * 16 / 4, fp_in);
		/* write dimensions (LE16) */
		fputc(spr_w & 0xFF, fp_out);
		fputc(spr_w >> 8, fp_out);
		fputc(spr_h & 0xFF, fp_out);
		fputc(spr_h >> 8, fp_out);
		/* write data */
		for (y = 0; y < 16; ++y) {
			for (x = 0; x < 2; ++x) {
				uint8 data[4];
				data[0] = src[0];
				data[1] = src[2];
				data[2] = 0;
				data[3] = 0;
				++src;
				fwrite(data, 1, sizeof(data), fp_out);
			}
			src += 2;
		}
	}
	/* duplicate frames 39 (== 37) and 40 (== 38) */
/*	fseek(fp_in, 0xD78F4 + 8428 + 18 * 16 / 4 * 2, SEEK_SET);*/
/*	for (i = 39; i < 41; ++i) {*/
	/* duplicate frames 35 to 39 */
	fseek(fp_in, 0xD78F4 + 8428, SEEK_SET);
	for (i = 39; i < 43; ++i) {
		const uint8 *src = buf;
		const int spr_w = 16;
		const int spr_h = 16;
		fread(buf, 1, 18 * 16 / 4, fp_in);
		/* write dimensions (LE16) */
		fputc(spr_w & 0xFF, fp_out);
		fputc(spr_w >> 8, fp_out);
		fputc(spr_h & 0xFF, fp_out);
		fputc(spr_h >> 8, fp_out);
		/* write data */
		for (y = 0; y < 16; ++y) {
			for (x = 0; x < 2; ++x) {
				uint8 data[4];
				data[0] = src[0];
				data[1] = src[2];
				data[2] = src[0] | src[2]; /* red pal */
				data[3] = 0;
				++src;
				fwrite(data, 1, sizeof(data), fp_out);
			}
			src += 2;
		}
	}
	fclose(fp_in);
	fclose(fp_out);
}

static void make_DEGRA_COP() {
	int i, len;
	uint8 buf[352];
	FILE *fp_out, *fp_in;
	for (i = 1; i <= 4; ++i) {
		char filename[30];
		sprintf(filename, EXTRA_DATA_DIR "/DEGRA%d.COP", i);
		fp_out = fopen(filename, "wb");
		assert(fp_out);
		sprintf(filename, AMIGA_DATA_DIR "/degra%d.cop.melt", i);
		fp_in = fopen(filename, "rb");
		assert(fp_in);
		fseek(fp_in, 0, SEEK_END);
		len = ftell(fp_in);
		assert(len == 352);
		fseek(fp_in, 0, SEEK_SET);
		fread(buf, 352, 1, fp_in);
		fwrite(buf, 352, 1, fp_out);
		fclose(fp_in);
		fclose(fp_out);
	}
}

static void make_LEVEL_PAL() {
	static const uint16 palData[] = {
/*	static const uint16 nickyPal[] = { */
		0x0000, 0x0468, 0x069B, 0x08BD, 0x0866, 0x0B66, 0x0D98, 0x0ECE,
		0x0EA4, 0x0E60, 0x0E00, 0x0420, 0x0644, 0x0A00, 0x0A60, 0x0C82,
/*	static const uint16 digitsPal[] = { */
		0x0000, 0x0FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/*	static const uint16 lifebarPal[] = { */
		0x0000, 0x0FFF, 0x0000, 0x0F00, 0x0F10, 0x0F20, 0x0F30, 0x0F40,
		0x0F50, 0x0F60, 0x0F70, 0x0F80, 0x0F90, 0x0FA0, 0x0FB0, 0x0FC0,
/*	static const uint16 _palLevel1[] = { */
		0x0000, 0x0E60, 0x0300, 0x0644, 0x0964, 0x0A86, 0x0CA8, 0x008D,
		0x0EA4, 0x0C00, 0x068E, 0x0466, 0x047A, 0x0880, 0x0AA0, 0x0ECE,
/*	static const uint16 _palLevel2[] = { */
		0x0000, 0x0E60, 0x0403, 0x0735, 0x0965, 0x0A86, 0x0CA8, 0x0068,
		0x0EA4, 0x0C00, 0x087C, 0x0466, 0x047A, 0x0880, 0x0AA0, 0x0ECE,
/*	static const uint16 _palLevel3[] = { */
		0x0000, 0x0E60, 0x0543, 0x0753, 0x0764, 0x0976, 0x0B99, 0x0568,
		0x0EA4, 0x0C00, 0x0885, 0x0553, 0x0663, 0x0B60, 0x0D80, 0x0ECE,
/*	static const uint16 _palLevel4[] = { */
		0x0000, 0x0E60, 0x0530, 0x0653, 0x0765, 0x0A88, 0x0CAA, 0x08AE,
		0x0EA4, 0x0C00, 0x0A8A, 0x0646, 0x0868, 0x0960, 0x0C86, 0x0ECE,
/*	static const uint16 _palFlash = { */
		0x0000, 0x0E00, 0x0300, 0x0600, 0x0900, 0x0A00, 0x0C00, 0x0000,
		0x0E00, 0x0C00, 0x0600, 0x0400, 0x0400, 0x0800, 0x0A00, 0x0E00,
/*	static const uint16 _palEnd1[] = { */
		0x0000, 0x0DB7, 0x0CA6, 0x0FEB, 0x0FEA, 0x0743, 0x0FD9, 0x0420,
		0x0530, 0x0EC8, 0x0B95, 0x0A84, 0x0973, 0x0862, 0x0751, 0x0640,
/*	static const uint16 _palEnd2[] = { */
		0x0000, 0x0622, 0x0400, 0x0842, 0x0A62, 0x0C82, 0x0CA4, 0x0CC6,
		0x0EE8, 0x0442, 0x0664, 0x0686, 0x06A8, 0x08CA, 0x08ED, 0x0EEE,
/*	static const uint16 _palEnd3[] = { */
		0x0000, 0x0C00, 0x0A00, 0x0140, 0x0562, 0x0786, 0x09AA, 0x0BDE,
		0x0800, 0x0420, 0x0640, 0x0862, 0x0A84, 0x0CA7, 0x0E00, 0x0EEE,
/*	static const uint16 _palEnd4[] = { */
		0x0000, 0x0822, 0x0A42, 0x0E84, 0x0EA6, 0x0255, 0x0367, 0x07AB,
		0x0EC8, 0x0620, 0x0C62, 0x058A, 0x0C00, 0x0ADE, 0x0254, 0x0EEE
	};
	static const char *palNames[] = {
		"NICKY.PAL",
		"DIGITS.PAL",
		"LIFEBAR.PAL",
		"DECOR1.PAL",
		"DECOR2.PAL",
		"DECOR3.PAL",
		"DECOR4.PAL",
		"FLASH.PAL",
		"END1.PAL",
		"END2.PAL",
		"END3.PAL",
		"END4.PAL"
	};
	int i, j;
	FILE *fp_out;
	for (i = 0; i < 7; ++i) {
		char filename[30];
		sprintf(filename, EXTRA_DATA_DIR "/%s", palNames[i]);
		fp_out = fopen(filename, "wb");
		assert(fp_out);
		for (j = 0; j < 16; ++j) {
			uint16 col = palData[i * 16 + j];
			fputc(col >> 0x8, fp_out);
			fputc(col & 0xFF, fp_out);
		}
		fclose(fp_out);
	}
}

static void make_IMAGE_PAL() {
	static const int palDumps[] = { 4, 6, 8, 10, 40, 42, 44, 38 };
	uint8 i;
	uint8 buf[32];
	FILE *fp_out, *fp_in;
	for (i = 0; i < sizeof(palDumps) / sizeof(palDumps[0]); ++i) {
		int len;
		char filename[30];
		sprintf(filename, EXTRA_DATA_DIR "/I%02d.PAL", i);
		fp_out = fopen(filename, "wb");
		assert(fp_out);
		sprintf(filename, AMIGA_DATA_DIR "/%02d.pal", palDumps[i]);
		fp_in = fopen(filename, "rb");
		assert(fp_in);
		fseek(fp_in, 0, SEEK_END);
		len = ftell(fp_in);
		assert(len == 32);
		fseek(fp_in, 0, SEEK_SET);
		fread(buf, 32, 1, fp_in);
		fwrite(buf, 32, 1, fp_out);
		fclose(fp_in);
		fclose(fp_out);
	}
}

int main(int argc, char *argv[]) {
	make_LIFEBAR_SPR();
	make_DIGITS_SPR();
	make_NICKY_SPR();
	make_DEGRA_COP();
	make_LEVEL_PAL();
	make_IMAGE_PAL();
	return 0;
}
