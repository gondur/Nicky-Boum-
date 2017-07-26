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

#include <assert.h>
#include "fileio.h"
#include "sound.h"
#include "sequence.h"
#include "systemstub.h"


int seq_display_image(const char *image, int delay) {
	const uint8 *file_data;
	int slot, event_mask;
	
	slot = fio_open(image, FIO_READ, 1);
	file_data = fio_fetch(slot);
	if (read_uint16LE(file_data) == 0x4D42) {
		uint32 offs_bmp = read_uint16LE(file_data + 10);
		uint32 offs_pal = 14 + read_uint32LE(file_data + 14);
		int w = read_uint32LE(file_data + 18);
		int h = read_uint32LE(file_data + 22);
		int num_palette_colors = 1 << read_uint16LE(file_data + 28);
		assert(num_palette_colors <= 256);
		sys_set_palette_bmp(file_data + offs_pal, num_palette_colors);
		sys_blit_bitmap(file_data + offs_bmp, w, h);
	}
	sys_fade_in_palette();
	event_mask = sys_wait_for_keys(delay, SKM_ACTION);
	sys_fade_out_palette();
	fio_close(slot);
	return (event_mask & SKM_ACTION) != 0;
}

void seq_play_intro() {
	static const char *images_table[] = { "I01.BMP", "I02.BMP", "I03.BMP" };
	
	sys_set_screen_mode(SSM_BITMAP);
	snd_play_song(SND_MUS_SONGPRES);
	while (1) {
		int i;
		for (i = 0; i < ARRAYSIZE(images_table); ++i) {
			int quit_flag = seq_display_image(images_table[i], 8000);
			if (quit_flag || sys_exit_flag) {
				return;
			}
		}
	}
}

void seq_play_ending() {
	static const int delay_table[] = { 24000, 14000 };
	static const char *images_table[] = { "I07_1.BMP", "I04.BMP", "I07_2.BMP", "I05.BMP", "I07_3.BMP", "I06.BMP" };
	int i;
	
	sys_set_screen_mode(SSM_BITMAP);
	snd_play_song(SND_MUS_SONGWIN);
	for (i = 0; i < ARRAYSIZE(images_table); ++i) {
		seq_display_image(images_table[i], delay_table[i & 1]);
		if (sys_exit_flag) {
			break;
		}
	}
}
