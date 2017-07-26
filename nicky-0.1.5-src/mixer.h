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

#ifndef __MIXER_H__
#define __MIXER_H__

#include "util.h"

#define MIXER_MAX_CHANNELS 2
#define MIXER_BUFFER_SIZE 2048
#define MIXER_FRAC_BITS 16

typedef struct {
	const int8 *chunk_data;
	uint32 chunk_size;
	uint32 chunk_pos;
	uint32 chunk_inc;
} mixer_channel_t;

extern void mixer_init(int sample_rate);
extern void mixer_play_sfx(const int8 *sample_data, uint32 sample_size, int sample_rate);
extern void mixer_mix_samples(int8 *dst, int len);

#endif
