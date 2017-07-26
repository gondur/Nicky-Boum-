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
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "game.h"
#include "resource.h"
#include "systemstub.h"

#define MAX_RES_SLOTS 10

#ifdef NICKY_GBA_VERSION
#define BLK_EXT "TIL"
#define SPR_EXT "TIL"
#else
#define BLK_EXT "BLK"
#define SPR_EXT "SPR"
#endif


uint8 res_decor_cdg[20000] GCC_EXT;
anim_data_t res_ref_ref[282] GCC_EXT;

const uint8 *res_digits_spr, *res_lifebar_spr, *res_nicky_spr, *res_level_spr, *res_monster_spr, *res_decor_blk, *res_decor_ref, *res_posit_ref, *res_decor_pal, *res_flash_pal;
static int res_slot_count;
static int res_slot_table[MAX_RES_SLOTS];
static int level_data_loaded = 0;

static struct anim_data_t *get_ref_ptr(int16 num_ref, int anim_count) {
	anim_data_t *ad = 0;
	if (num_ref < 0) {
		ad = (anim_data_t *)0xFFFFFFFF;
	} else if (num_ref > 0) {
		assert(num_ref < anim_count);
		ad = &res_ref_ref[num_ref];
	}
	return ad;
}

static const uint8 *get_move_delta_ptr(uint16 move) {
	static const int offsets_table[] = { 0, 88, 92, 180, 236, 292, 374, 462, 518, 570, 682, 768 };
	const uint8 *md = 0;
	if (move != 0) {
		--move;
		assert(move < ARRAYSIZE(offsets_table));
		md = move_delta_table + offsets_table[move];
	}
	return md;
}

static void load_ref_ref(const uint8 *data, int size) {
	int i, anim_count;
	
	memset(res_ref_ref, 0, sizeof(res_ref_ref));
	anim_count = size / 0x44;
	assert(anim_count <= ARRAYSIZE(res_ref_ref));
	for (i = 0; i < anim_count; ++i) {
		anim_data_t *ad = &res_ref_ref[i];
		ad->unk0 = *data++;
		ad->lifes = *data++;
		ad->cycles = *data++;
		ad->unk3 = *data++;
		ad->unk4 = *data++;
		ad->unk5 = *data++;
		ad->unk6 = *data++;
		ad->init_sprite_num = *data++;
		ad->colliding_opcode = *data++;
		ad->logic_opcode = *data++;
		ad->sound_num = *data++;
		ad->unkB = *data++;
		ad->sprite_num = read_uint16LE(data); data += 2;
		ad->sprite_flags = read_uint16LE(data); data += 2;
		ad->unk10 = read_uint16LE(data); data += 2;
		ad->unk12 = read_uint16LE(data); data += 2;
		ad->anim_w = read_uint16LE(data); data += 2;
		ad->anim_h = read_uint16LE(data); data += 2;
		ad->score = read_uint16LE(data); data += 2;
		ad->bounding_box_x1 = read_uint16LE(data); data += 2;
		ad->bounding_box_x2 = read_uint16LE(data); data += 2;
		ad->bounding_box_y1 = read_uint16LE(data); data += 2;
		ad->bounding_box_y2 = read_uint16LE(data); data += 2;
		ad->move_data_ptr = get_move_delta_ptr(read_uint16LE(data)); data += 2;
		data += 2;
		ad->unk26 = read_uint16LE(data); data += 2;
		ad->unk28 = read_uint16LE(data); data += 2;
		ad->anim_data1_ptr = get_ref_ptr(read_uint16LE(data), anim_count); data += 2;
		data += 2;
		ad->anim_data2_ptr = get_ref_ptr(read_uint16LE(data), anim_count); data += 2;
		data += 2;
		ad->anim_data3_ptr = get_ref_ptr(read_uint16LE(data), anim_count); data += 2;
		data += 2;
		ad->dx = read_uint16LE(data); data += 2;
		ad->dy = read_uint16LE(data); data += 2;
		ad->anim_data4_ptr = get_ref_ptr(read_uint16LE(data), anim_count); data += 2;
		data += 2;
		ad->unk3E = read_uint16LE(data); data += 2;
		ad->unk40 = read_uint16LE(data); data += 2;
		ad->unk42 = read_uint16LE(data); data += 2;
	}
}

static const uint8 *fetch_level_data(const char *filename) {
	int slot_tmp;
	
	slot_tmp = fio_open(filename, FIO_READ, 1);
	assert(res_slot_count < MAX_RES_SLOTS);
	res_slot_table[res_slot_count] = slot_tmp;
	++res_slot_count;
	return fio_fetch(slot_tmp);
}

void res_load_level_data(int level) {
	int slot_tmp;
	char filename[10];
	const uint8 *file_data;
	static const char *suffixes_table[] = { "1", "1A", "2", "2A", "3", "3A", "4", "4A" };
	
	print_debug(DBG_RESOURCE, "res_load_level_data(%d)", level);
	
	res_unload_level_data();
		
	/* set level palette */
	sprintf(filename, "DECOR%s.PAL", suffixes_table[level & ~1]);
	res_decor_pal = fetch_level_data(filename);
	sys_set_palette_spr(res_decor_pal, 16, 0);

	/* set nicky palette */
	slot_tmp = fio_open("NICKY.PAL", FIO_READ, 0);
	file_data = fio_fetch(slot_tmp);
	sys_set_palette_spr(file_data, 16, 1);
	fio_close(slot_tmp);	
	
	/* set digits palette */
	slot_tmp = fio_open("DIGITS.PAL", FIO_READ, 1);
	file_data = fio_fetch(slot_tmp);
	sys_set_palette_spr(file_data, 16, 2);
	fio_close(slot_tmp);
	
	/* set lifebar palette */
	slot_tmp = fio_open("LIFEBAR.PAL", FIO_READ, 1);
	file_data = fio_fetch(slot_tmp);
	sys_set_palette_spr(file_data, 16, 3);
	fio_close(slot_tmp);

	/* load bomb/flash palette */
	res_flash_pal = fetch_level_data("FLASH.PAL");
	
	/* background */
	sprintf(filename, "DEGRA%s.COP", suffixes_table[level & ~1]);
	slot_tmp = fio_open(filename, FIO_READ, 1);
	file_data = fio_fetch(slot_tmp);
	sys_set_copper_pal(file_data + 8 * 2, 160);
	fio_close(slot_tmp);
	
	/* tiles gfx */
	sprintf(filename, "DECOR%s." BLK_EXT, suffixes_table[level & ~1]);
	res_decor_blk = fetch_level_data(filename);
	sys_set_tile_data(res_decor_blk);

	/* tile flags */
	sprintf(filename, "DECOR%s.REF", suffixes_table[level & ~1]);
	res_decor_ref = fetch_level_data(filename);

	/* tile map (will be modified, make a copy) */
	sprintf(filename, "DECOR%s.CDG", suffixes_table[level]);
	slot_tmp = fio_open(filename, FIO_READ, 1);
	file_data = fio_fetch(slot_tmp);
	memcpy(res_decor_cdg, file_data, fio_size(slot_tmp));
	fio_close(slot_tmp);

	/* nicky sprites */
	res_nicky_spr = fetch_level_data("NICKY." SPR_EXT);

	/* level sprites */
	sprintf(filename, "S0%s." SPR_EXT, suffixes_table[level & ~1]);
	res_level_spr = fetch_level_data(filename);
	
	/* digits sprites */
	res_digits_spr = fetch_level_data("DIGITS." SPR_EXT);
	
	/* lifebar sprites */
	res_lifebar_spr = fetch_level_data("LIFEBAR." SPR_EXT);
	
	/* monster sprites */
	sprintf(filename, "S1%s." SPR_EXT, suffixes_table[level & ~1]);
	res_monster_spr = fetch_level_data(filename);
	
	/* monster flags */
	sprintf(filename, "POSIT%s.REF", suffixes_table[level]);
	res_posit_ref = fetch_level_data(filename);
	
	/* monster anim_data */
	sprintf(filename, "REF%s.REF", suffixes_table[level & ~1]);
	slot_tmp = fio_open(filename, FIO_READ, 1);
	file_data = fio_fetch(slot_tmp);
	load_ref_ref(file_data, fio_size(slot_tmp));
	fio_close(slot_tmp);
	
	level_data_loaded = 1;
}


void res_unload_level_data() {
	print_debug(DBG_RESOURCE, "res_unload_level_data()");
	
	if (level_data_loaded) {
		int i;
		for (i = 0; i < res_slot_count; ++i) {
			fio_close(res_slot_table[i]);
		}
		memset(res_slot_table, 0, sizeof(res_slot_table));
		res_slot_count = 0;
		level_data_loaded = 0;
	}
}
