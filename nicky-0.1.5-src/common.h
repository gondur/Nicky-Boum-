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

#ifndef __COMMON_H__
#define __COMMON_H__

#include "util.h"

typedef struct anim_data_t {
	int8 unk0;
	int16 lifes;
	int8 cycles;
	int8 unk3;
	uint8 unk4;
	uint8 unk5;
	uint8 unk6;
	uint8 init_sprite_num;
	uint8 colliding_opcode;
	uint8 logic_opcode;
	int8 sound_num;
	uint8 unkB;
	uint16 sprite_num;
	uint16 sprite_flags;
	uint16 unk10;
	uint16 unk12;
	uint16 anim_w;
	uint16 anim_h;
	uint16 score;
	int16 bounding_box_x1;
	int16 bounding_box_x2;
	int16 bounding_box_y1;
	int16 bounding_box_y2;
	const uint8 *move_data_ptr;
	uint16 unk26;
	uint16 unk28;
	struct anim_data_t *anim_data1_ptr;
	struct anim_data_t *anim_data2_ptr;
	struct anim_data_t *anim_data3_ptr;
	uint16 dx;
	uint16 dy;
	struct anim_data_t *anim_data4_ptr;
	uint16 unk3E;
	uint16 unk40;
	uint16 unk42;
} anim_data_t; /* sizeof == 0x44 */

typedef struct object_state_t {
	uint8 displayed;
	int16 life;
	int8 anim_data_cycles;
	int8 unk3;
	int16 map_pos_x; /* init_map_pos_x ? */
	int16 map_pos_y; /* init_map_pos_y ? */
	anim_data_t *anim_data1_ptr;
	anim_data_t *anim_data2_ptr;
	anim_data_t *anim_data3_ptr;
	anim_data_t *anim_data4_ptr;
	const uint8 *move_data_ptr;
	int16 distance_pos_x;
	int16 distance_pos_y;
	int16 pos_x;
	int16 pos_y;
	uint16 ref_ref_index;
	uint8 unk26;
	uint8 unk27;
	uint8 transparent_flag;
	uint8 tile_num;
	uint8 visible;
	/* XXX */
	int16 anim_data3_ptr_;
	int16 anim_data4_ptr_;
	int16 anim_data4_seg_;
} object_state_t; /* sizeof == 0x2C */

typedef struct anim_frame_t {
	uint8 frames_count;
	uint8 unk1;
	int16 frame_w;
	int16 frame_h;
	int16 unk6;
	int16 unk8;
	int16 frame_w2;
	int16 delta_y;
	int16 frame_h2;
	int16 sprite_num;
	int16 unk12;
	struct anim_frame_t *next_anim_frame;
	int16 unk16;
	int16 unk18;
	int16 unk1A;
	int16 pos_x;
	int16 pos_y;
} anim_frame_t; /* sizeof == 0x20 */

#endif /* __COMMON_H__ */
