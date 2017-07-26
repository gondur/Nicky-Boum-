/* Nicky - Nicky Boum engine rewrite
 * Copyright (C) 2006 Gregory Montoir
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "fileio.h"
#include "sqx_decoder.h"

#define MAX_FILEIO_SLOTS 30


typedef struct {
	const char *filename;
	int size;
} pc_datafile_t;

typedef struct {
	int used;
	FILE *fp;
	int pos;
	int size;
	uint8 *data;
	fio_open_mode_e mode;
	pc_datafile_t *pc_df;
} fileio_slot_t;


static const char *data_directory;
static fileio_slot_t fileio_slots_table[MAX_FILEIO_SLOTS];
static const pc_datafile_t pc_datafiles_table[] = {
	{ "DECOR1.BLK", 32768 },
	{ "DECOR1.CDG", 20000 },
 	{ "DECOR1.REF", 2048 },
	{ "DECOR1A.CDG", 20000 },
	{ "DECOR2.BLK", 32768 },
	{ "DECOR2.CDG", 20000 },
 	{ "DECOR2.REF", 2048 },
	{ "DECOR2A.CDG", 20000 },
	{ "DECOR3.BLK", 32768 },
	{ "DECOR3.CDG", 20000 },
 	{ "DECOR3.REF", 2048 },
	{ "DECOR3A.CDG", 20000 },
	{ "DECOR4.BLK", 32768 },
	{ "DECOR4.CDG", 20000 },
 	{ "DECOR4.REF", 2048 },
	{ "DECOR4A.CDG", 20000 },
 	{ "POSIT1.REF", 4100 },
 	{ "POSIT1A.REF", 3880 },
 	{ "POSIT2.REF", 3920 },
 	{ "POSIT2A.REF", 4000 },
 	{ "POSIT3.REF", 3960 },
 	{ "POSIT3A.REF", 3960 },
 	{ "POSIT4.REF", 3820 },
 	{ "POSIT4A.REF", 4090 },
	{ "REF1.REF", 16728 },
	{ "REF2.REF", 17544 },
	{ "REF3.REF", 18428 },
	{ "REF4.REF", 19176 },
	{ "S01.SPR", 45740 },
	{ "S02.SPR", 57128 },
	{ "S03.SPR", 58958 },
	{ "S04.SPR", 59630 },
 	{ "S11.SPR", 7280 },
	{ "S12.SPR", 13824 },
	{ "S13.SPR", 39502 },
	{ "S14.SPR", 25232 }
};

static int find_free_slot() {
	int i, slot = -1;
	for (i = 0; i < MAX_FILEIO_SLOTS; ++i) {
		if (!fileio_slots_table[i].used) {
			slot = i;
			break;
		}
	}
	return slot;
}

static int pc_datafile_compare(const void *a, const void *b) {
	const char *key = (const char *)a;
	const pc_datafile_t *pc_df = (const pc_datafile_t *)b;
	return strcmp(key, pc_df->filename);
}

void fio_init(const char *data_path) {
	data_directory = data_path;
	memset(fileio_slots_table, 0, sizeof(fileio_slots_table));
}

int fio_open(const char *filename, fio_open_mode_e mode, int error_flag) {
	static const char *modes_table[] = { "rb", "wb" };
	int slot = find_free_slot();
	if (slot < 0) {
		print_error("Unable to find free slot for '%s'", filename);
	} else {
		char *p;
		char buf[512];
		fileio_slot_t *file_slot = &fileio_slots_table[slot];
		memset(file_slot, 0, sizeof(fileio_slot_t));
		sprintf(buf, "%s/%s", data_directory, filename);
		p = buf + strlen(data_directory) + 1;
		string_upper(p);
		file_slot->fp = fopen(buf, modes_table[mode]);
		if (!file_slot->fp) {
			if (error_flag) {
				print_error("Unable to open file '%s'", filename);
			}
			print_warning("Unable to open file '%s'", filename);
			slot = -1;
		} else {
			file_slot->mode = mode;
			fseek(file_slot->fp, 0, SEEK_END);
			file_slot->size = ftell(file_slot->fp);
			fseek(file_slot->fp, 0, SEEK_SET);
			file_slot->used = 1;
			file_slot->pc_df = bsearch(filename, pc_datafiles_table, ARRAYSIZE(pc_datafiles_table), sizeof(pc_datafile_t), pc_datafile_compare);
		}
	}
	return slot;
}

void fio_close(int slot) {
	if (slot >= 0) {
		fileio_slot_t *file_slot = &fileio_slots_table[slot];
		assert(file_slot->used);
		free(file_slot->data);
		fclose(file_slot->fp);
		memset(file_slot, 0, sizeof(fileio_slot_t));
	}
}

int fio_size(int slot) {
	int size = 0;
	if (slot >= 0) {
		fileio_slot_t *file_slot = &fileio_slots_table[slot];
		assert(file_slot->used);
		if (file_slot->pc_df) {
			size = file_slot->pc_df->size;
		} else {
			size = file_slot->size;
		}
	}
	return size;
}

const uint8 *fio_fetch(int slot) {
	uint8 *data = 0;
	if (slot >= 0) {
		fileio_slot_t *file_slot = &fileio_slots_table[slot];
		assert(file_slot->used && file_slot->mode == FIO_READ);
		if (!file_slot->data) {
			file_slot->data = (uint8 *)malloc(file_slot->size);
			if (!file_slot->data) {
				print_error("Unable to allocate %d bytes", file_slot->size);
			} else {
				const pc_datafile_t *df = file_slot->pc_df;
				int cur_pos = ftell(file_slot->fp);
				fseek(file_slot->fp, 0, SEEK_SET);
				fread(file_slot->data, 1, file_slot->size, file_slot->fp);
				fseek(file_slot->fp, cur_pos, SEEK_SET);
				if (df) {
					uint8 *dst = (uint8 *)malloc(df->size);
					if (!dst) {
						print_error("Unable to allocate %d bytes", df->size);
						free(file_slot->data);
						file_slot->data = 0;
					} else {
						int decoded_size = sqx_decode(file_slot->data + 2, dst);
						if (decoded_size != df->size) {
							print_error("Failed to decode file_slot %d", slot);
							free(dst);
							dst = 0;
						}
						free(file_slot->data);
						file_slot->data = dst;
					}
				}
			}
		}
		data = file_slot->data;
	}
	return data;
}

void fio_write(int slot, const uint8 *data, int len) {
	if (slot >= 0) {
		fileio_slot_t *file_slot = &fileio_slots_table[slot];
		assert(file_slot->used && file_slot->mode == FIO_WRITE);
		fwrite(data, len, 1, file_slot->fp);
	}
}
