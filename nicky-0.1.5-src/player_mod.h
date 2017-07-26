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
 
#ifndef __MOD_PLAYER_H__
#define __MOD_PLAYER_H__

#include "util.h"

#define MPL_NUM_SAMPLES    31
#define MPL_NUM_TRACKS      4
#define MPL_NUM_PATTERNS  128

typedef struct {
	uint16 len;
	uint8 fine_tune;
	int8 volume;
	uint16 loop_pos;
	uint16 loop_len;
	const int8 *data;
} mpl_sample_info_t;

typedef struct {
	mpl_sample_info_t sample_table[MPL_NUM_SAMPLES];
	uint8 num_patterns;
	const uint8 *pattern_order_table;
	const uint8 *pattern_table;
} mpl_module_info_t;

typedef struct {
	mpl_sample_info_t *sample;
	int8 volume;
	int pos;
	uint16 period;
	uint16 period_index;
	uint16 effect_data;
	int delay_counter;
} mpl_track_t;

extern const uint16 mpl_period_table[576];

extern void mpl_load(const uint8 *module_data, const uint8 *instrument_data);
extern void mpl_stop();
extern void mpl_play_callback(int mixing_rate, int8 *dst, int len);

#endif /* __MOD_PLAYER_H__ */
