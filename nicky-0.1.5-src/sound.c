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
#include <string.h>
#include "sound.h"
#include "fileio.h"
#include "systemstub.h"


static int sfx_file_slot;
static const uint8 *sfx_file_data;
static int mod_file_slot;
static int ins_file_slot;
static const uint8 *ins_file_data;


void snd_init() {
	sfx_file_slot = -1;
	sfx_file_data = 0;
	mod_file_slot = -1;
	ins_file_slot = -1;
	ins_file_data = 0;
}

void snd_stop() {
	sys_stop_module();
	if (sfx_file_slot >= 0) {
		fio_close(sfx_file_slot);
		sfx_file_slot = -1;
	}
	if (mod_file_slot >= 0) {
		fio_close(mod_file_slot);
		mod_file_slot = -1;
	}
	if (ins_file_slot >= 0) {
		fio_close(ins_file_slot);
		ins_file_slot = -1;
	}
}

void snd_play_song(int song) {
	sys_stop_module();
	if (mod_file_slot >= 0) {
		fio_close(mod_file_slot);
		mod_file_slot = -1;
	}
#ifndef NICKY_DEBUG
	if (song >= 0 && song < MAX_SND_MODULES) {
		if (ins_file_slot < 0) {
			ins_file_slot = fio_open("INSTRUMENTS.RAW", FIO_READ, 1);
			ins_file_data = fio_fetch(ins_file_slot);
		}
		if (ins_file_data != 0) {
			const uint8 *module_data = 0;
			print_debug(DBG_SOUND, "snd_play_song(%d, '%s')", song, snd_module_table[song]);
			mod_file_slot = fio_open(snd_module_table[song], FIO_READ, 1);
			module_data = fio_fetch(mod_file_slot);
			sys_play_module(module_data, ins_file_data);
		}
	}
#endif
}

void snd_play_sfx(int sfx) {
	print_debug(DBG_SOUND, "snd_play_sfx(%d)", sfx);
	if (sfx >= 0 && sfx < MAX_SND_SAMPLES) {
		const sample_data_t *sd = &snd_sample_data_table[sfx];
		if (sfx_file_slot < 0) {
			sfx_file_slot = fio_open("SFXS.RAW", FIO_READ, 1);
			sfx_file_data = fio_fetch(sfx_file_slot);
		}
		if (sfx_file_data) {
			sys_play_sfx((const int8 *)sfx_file_data + sd->offs, sd->size, PAULA_FREQ / sd->period);
		}
	}
}
