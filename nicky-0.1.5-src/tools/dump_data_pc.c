
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

static uint32 read_u32(FILE *fp) {
	uint8 a = fgetc(fp);
	uint8 b = fgetc(fp);
	uint8 c = fgetc(fp);
	uint8 d = fgetc(fp);
	return (d << 24) | (c << 16) | (b << 8) | a;
}

static uint16 read_u16(FILE *fp) {
	uint8 a = fgetc(fp);
	uint8 b = fgetc(fp);
	return (b << 8) | a;
}

void dump(FILE *fp, int off1, int len, int w, const char *name) {
	int off2 = off1 + len;
	uint16 i = 0;
	assert(off1 < off2);
	fseek(fp, off1, SEEK_SET);
	while (ftell(fp) < off2) {
		switch (w) {
		case 1:	
			if (i == 16) {
				printf("\n\t");
				i = 0;
			}
			printf("0x%02X, ", fgetc(fp));
			break;
		case 2:
			if (i == 10) {
				printf("\n\t");
				i = 0;
			}
			printf("0x%04X, ", read_u16(fp));
			break;
		case 4:
			if (i == 8) {
				printf("\n\t");
				i = 0;
			}
			printf("0x%06X, ", read_u32(fp));
			break;
		default:
			printf("Unhandled width = %d\n", w);
			break;
		}
		++i;
	}
}

void dump_credits_text(FILE *fp, int off, int str_len, const char *name) {
	fseek(fp, off, SEEK_SET);
	printf("const char *%s = \"\n", name);
	while (str_len--) {
		char c = fgetc(fp);
		switch (c) {
		case 1:
			printf("\\n");
			break;
		case 0x5B:
			printf("!");
			break;
		default:
			printf("%c", c);
			break;
		}
	}
	printf("\";\n");
}

void dump_rotation_table(FILE *fp, int offset, int n, const char *name) {
	int line_count = 10;
	fseek(fp, offset, SEEK_SET);
	while (n--) {
		uint8 hi = fgetc(fp);
		uint8 lo = fgetc(fp);
		if (line_count == 10) {
			printf("\n\t");
			line_count = 0;
		}
		printf("0x%04X, ", (hi << 8) | lo);
		++line_count;
	}
}

void dump_nicky_anim_frames_table(FILE *fp, int off, int n, const char *name) {
	int i;
	fseek(fp, off, SEEK_SET);
	for (i = 0; i < n; ++i) {
		int next_anim_frame = 0;
		printf("\t{ %d, ", fgetc(fp)); /* db frames_count */
		printf("%d, ", fgetc(fp)); /* db field_1 */
		printf("%d, ", read_u16(fp)); /* dw field_2 */
		printf("%d, ", read_u16(fp)); /* dw field_4 */
		printf("%d, ", read_u16(fp)); /* dw field_6 */
		printf("%d, ", read_u16(fp)); /* dw field_8 */
		printf("%d, ", read_u16(fp)); /* dw field_A */
		printf("%d, ", read_u16(fp)); /* dw dy */
		printf("%d, ", read_u16(fp)); /* dw field_E */
		printf("%d, ", read_u16(fp)); /* dw sprite_data_ptr */
		printf("%d, ", read_u16(fp)); /* dw field_12 */
		next_anim_frame = read_u16(fp);
		if (next_anim_frame != 0) {
			printf("&%s[%d], ", name, (next_anim_frame - 0xC1B4) / 0x20); /* dw next_anim_frame */
		} else {
			printf("0, ");
		}
		printf("%d, ", read_u16(fp)); /* dw field_16 */
		printf("%d, ", read_u16(fp)); /* dw field_18 */
		printf("%d, ", read_u16(fp)); /* dw field_1A */
		printf("%d, ", read_u16(fp)); /* dw field_1C */
		printf("%d },\n", read_u16(fp)); /* dw field_1E */
	}
}

static void print_anim_table_entry(const char *name, int n, int base_offs) {
	if (n > 0) {
		printf("&%s[%d], ", name, (n - base_offs) / 0x44);
	} else {
		printf("0, ");
	}
}

void dump_anim_data(FILE *fp, int off, int n, int base_offs, const char *name) {
	int i, j;
	fseek(fp, off, SEEK_SET);
	printf("%s = \n", name);
	for (i = 0; i < n; ++i) {
		printf("\t{ %d, ", (int8)fgetc(fp)); /* db field_0 */
		printf("%d, ", fgetc(fp)); /* db field_1 */
		printf("%d, ", fgetc(fp)); /* db cycles */
		printf("%d, ", fgetc(fp)); /* db field_3 */
		printf("%d, ", fgetc(fp)); /* db field_4 */
		printf("%d, ", fgetc(fp)); /* db field_5 */
		printf("%d, ", fgetc(fp)); /* db field_6 */
		printf("%d, ", fgetc(fp)); /* db field_7 */
		printf("%d, ", fgetc(fp)); /* db colliding_opcode */
		printf("%d, ", fgetc(fp)); /* db unk9 */
		printf("%d, ", (int8)fgetc(fp)); /* db sound_num */
		printf("%d, ", fgetc(fp)); /* db unkB */
		/* sprite_num - bounding_box_y2 */
		for (j = 0; j < 11; ++j) {
			printf("%d, ", (int16)read_u16(fp));
		}
		printf("&move_delta_table[%d], ", (int16)read_u16(fp)); /* dw move_data_ptr */
		read_u16(fp); /*printf("%d, ", (int16)read_u16(fp));*/ /* dw move_data_seg */
		printf("%d, ", (int16)read_u16(fp)); /* dw unk26 */
		printf("%d, ", (int16)read_u16(fp)); /* dw unk28 */
		print_anim_table_entry(name, read_u16(fp), base_offs); /* dw anim_data1_ptr */
		read_u16(fp); /* printf("%d, ", (int16)read_u16(fp));*/ /* dw anim_data1_seg */
		print_anim_table_entry(name, read_u16(fp), base_offs); /* dw anim_data2_ptr */
		read_u16(fp); /* printf("%d, ", (int16)read_u16(fp));*/ /* dw anim_data2_seg */
		print_anim_table_entry(name, read_u16(fp), base_offs); /* dw anim_data3_ptr */
		read_u16(fp); /* printf("%d, ", (int16)read_u16(fp));*/ /* dw anim_data3_seg */
		printf("%d, ", (int16)read_u16(fp)); /* dw unk36 */
		printf("%d, ", (int16)read_u16(fp)); /* dw unk38 */
		print_anim_table_entry(name, read_u16(fp), base_offs); /* dw anim_data4_ptr */
		read_u16(fp); /* printf("%d, ", (int16)read_u16(fp));*/ /* dw anim_data4_seg */
		printf("%d, ", (int16)read_u16(fp)); /* dw unk3E */
		printf("%d, ", (int16)read_u16(fp)); /* dw unk40 */
		printf("%d, ", (int16)read_u16(fp)); /* dw unk42 */
		printf("},\n");
	}
	printf("\n");
}

void dump_pal_nicky2(FILE *fp) {
	static const int offsets[] = { 0x170DD, 0x170FD, 0x1711D, 0x17149, 0x17169, 0x17189, 0x171A9 };
	int i;
	FILE *fp_out;
	for (i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i) {
		char pal_filename[20];
		sprintf(pal_filename, "I%02d.PAL", i);
		fp_out = fopen(pal_filename, "wb");
		if (fp_out) {
			int j;
			fseek(fp, offsets[i], SEEK_SET);
			for (j = 0; j < 16; ++j) {
				uint16 color = read_u16(fp);
				fputc(color >> 8, fp_out);
				fputc(color & 0xFF, fp_out);
			}
			fclose(fp_out);
		}
	}
}

void dump_move_offsets(FILE *fp, int offs, int n, const char *name) {
	int i;
	printf("%s[] = {\n", name);
	fseek(fp, offs, SEEK_SET);
	for (i = 0; i < n; ++i) {
		uint16 mo = read_u16(fp);
		printf("0x%04X, ", mo);
	}
	printf("\n");
}

void dump_nicky1() {
	FILE *fp = fopen("../_nicky1/nicky.exe", "rb");
	if (fp) {
#if 0
		dump_credits_text(fp, 0x17191, 278, "_credits_text1");
		dump_credits_text(fp, 0x172A7, 267, "_credits_text2");
		dump_credits_text(fp, 0x173B2, 144, "_credits_text3");
		dump_credits_text(fp, 0x17442, 212, "_credits_text4");
		dump_rotation_table(fp, 0x163A3, 128 + 512, "_game_rotation_table");
		dump_nicky_anim_frames_table(fp, 0x14614, 52, "_game_nicky_anim_frames");
		dump(fp, 0x15448, 204, 1, "_game_tile_map_quad_table");
		dump(fp, 0x1552C, 854, 1, "_game_move_delta_table");
		dump(fp, 0x1552C, 854, 1, "_game_move_delta_table");
		dump_move_offsets(fp, 0x160D8, 58, "_game_nicky_move_offsets_table");
		dump_move_offsets(fp, 0x15882, 126, "_game_nicky_move_offsets_table5");
		dump(fp, 0x1597E, 204, 1, "_game_move_delta_table2");
#endif
		dump_anim_data(fp, 0x14C94, 23, 0xC834, "_game_anim_data0");
		dump_anim_data(fp, 0x152B0, 6, 0xCE50, "_game_anim_data_starshield");
		fclose(fp);
	}
}

void dump_nicky2() {
	FILE *fp = fopen("../_nicky2/nicky2.exe", "rb");
	if (fp) {
#if 0
		dump_pal_nicky2(fp);
#endif
		fclose(fp);
	}
}

int main(int argc, char* argv[]) {
	dump_nicky1();
	dump_nicky2();
	return 0;
}
