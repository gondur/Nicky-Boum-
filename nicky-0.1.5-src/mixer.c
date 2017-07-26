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

#include <string.h>
#include "mixer.h"


static int mixing_rate;
static mixer_channel_t channels_table[MIXER_MAX_CHANNELS];


void mixer_init(int sample_rate) {
	mixing_rate = sample_rate;
	memset(channels_table, 0, sizeof(channels_table));
}

void mixer_play_sfx(const int8 *sample_data, uint32 sample_size, int sample_period) {
	int i;
	/* find and setup a free channel */
	for (i = 0; i < ARRAYSIZE(channels_table); ++i) {
		mixer_channel_t *channel = &channels_table[i];
		if (channel->chunk_data) {
			if (channel->chunk_data == sample_data) {
				channel->chunk_pos = 0;
				break;
			}
		} else {
			channel->chunk_data = sample_data;
			channel->chunk_size = sample_size;
			channel->chunk_pos = 0;
			channel->chunk_inc = (sample_period << MIXER_FRAC_BITS) / mixing_rate;
			break;
		}
	}
}

void mixer_mix_samples(int8 *dst, int len) {
	int i;
	for (i = 0; i < ARRAYSIZE(channels_table); ++i) {
		mixer_channel_t *channel = &channels_table[i];
		if (channel->chunk_data) {
			int j;
			for (j = 0; j < len; ++j) {
				int b = dst[j];
				int pos = channel->chunk_pos >> MIXER_FRAC_BITS;
				if (pos >= channel->chunk_size) {
					channel->chunk_data = 0;
					break;
				}
				b += channel->chunk_data[pos];
				if (b < -128) {
					b = -128;
				} else if (b > 127) {
					b = 127;
				}
				dst[j] = (int8)b;
				channel->chunk_pos += channel->chunk_inc;
			}
		}
	}
}
