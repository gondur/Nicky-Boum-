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

#include "game.h"
#include "sound.h"
#include "sequence.h"


anim_frame_t nicky_anim_frames_table[52] = {
	/* 0 */
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 0, 0, &nicky_anim_frames_table[1], 0, 2, 13, 12, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 1, 0, &nicky_anim_frames_table[2], 0, 2, 13, 12, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 2, 0, &nicky_anim_frames_table[3], 0, 2, 13, 12, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 3, 0, &nicky_anim_frames_table[4], 0, 2, 13, 12, 5 },
	/* 4 */
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 4, 0, &nicky_anim_frames_table[5], 0, 2, 13, 12, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 5, 0, &nicky_anim_frames_table[6], 0, 2, 13, 12, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 6, 0, &nicky_anim_frames_table[7], 0, 2, 13, 12, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 7, 0, &nicky_anim_frames_table[0], 0, 2, 13, 12, 5 },
	/* 8 */
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 8, 0, &nicky_anim_frames_table[9], 0, 0, 13, 0, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 9, 0, &nicky_anim_frames_table[10], 0, 0, 13, 0, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 10, 0, &nicky_anim_frames_table[11], 0, 0, 13, 0, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 11, 0, &nicky_anim_frames_table[12], 0, 0, 13, 0, 5 },
	/* 12 */
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 12, 0, &nicky_anim_frames_table[13], 0, 0, 13, 0, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 13, 0, &nicky_anim_frames_table[14], 0, 0, 13, 0, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 14, 0, &nicky_anim_frames_table[15], 0, 0, 13, 0, 5 },
	{ 1, 0, 16, 29, 124, 0, 16, 1, 29, 15, 0, &nicky_anim_frames_table[8], 0, 0, 13, 0, 5 },
	/* 16 */
	{ 2, 0, 16, 29, 124, 0, 16, 1, 29, 19, 0, &nicky_anim_frames_table[17], 0, 2, 13, 0, 0 },
	{ 2, 0, 16, 29, 124, 0, 16, 1, 29, 18, 0, &nicky_anim_frames_table[18], 0, 2, 13, 0, 0 },
	{ 2, 0, 16, 29, 124, 0, 16, 1, 29, 17, 0, &nicky_anim_frames_table[19], 0, 2, 13, 0, 0 },
	{ 0, 0, 16, 29, 124, 0, 16, 1, 29, 16, 0, 0, 0, 2, 13, 12, 5 },
	/* 20 */
	{ 2, 0, 16, 29, 124, 0, 16, 1, 29, 16, 0, &nicky_anim_frames_table[21], 0, 0, 13, 0, 0 },
	{ 2, 0, 16, 29, 124, 0, 16, 1, 29, 17, 0, &nicky_anim_frames_table[22], 0, 0, 13, 0, 0 },
	{ 2, 0, 16, 29, 124, 0, 16, 1, 29, 18, 0, &nicky_anim_frames_table[23], 0, 0, 13, 0, 0 },
	{ 0, 0, 16, 29, 124, 0, 16, 1, 29, 19, 0, 0, 0, 0, 13, 0, 5 },
	/* 24 */
	{ 0, 0, 16, 29, 124, 0, 16, 1, 29, 20, 0, 0, 0, 2, 0, 12, 5 }, /* jump/fall right side */
	{ 0, 0, 16, 29, 124, 0, 16, 1, 29, 21, 0, 0, 0, 2, 0, 0, 5 }, /* jump/fall left side */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 22, 0, &nicky_anim_frames_table[27], 0, 0, 0, 0, 0 }, /* stairs */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 23, 0, &nicky_anim_frames_table[28], 0, 0, 0, 0, 0 }, /* stairs */
	/* 28 */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 24, 0, &nicky_anim_frames_table[29], 0, 0, 0, 0, 0 }, /* stairs */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 25, 0, &nicky_anim_frames_table[26], 0, 0, 0, 0, 0 }, /* stairs */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 25, 0, &nicky_anim_frames_table[31], 0, 2, 0, 0, 0 }, /* stairs */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 24, 0, &nicky_anim_frames_table[32], 0, 2, 0, 0, 0 }, /* stairs */
	/* 32 */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 23, 0, &nicky_anim_frames_table[33], 0, 2, 0, 0, 0 }, /* stairs */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 22, 0, &nicky_anim_frames_table[30], 0, 2, 0, 0, 0 }, /* stairs */
	{ 2, 0, 16, 29, 124, 0, 16, 5, 29, 26, 0, &nicky_anim_frames_table[35], 0, 2, 13, 12, 8 },
	{ 2, 0, 16, 29, 124, 0, 16, 8, 29, 27, 0, &nicky_anim_frames_table[36], 0, 2, 13, 12, 8 },
	/* 36 */
	{ 0, 0, 16, 29, 124, 0, 16, 11, 29, 28, 0, 0, 0, 2, 13, 12, 13 },
	{ 2, 0, 16, 29, 124, 0, 16, 5, 29, 29, 0, &nicky_anim_frames_table[38], 0, 0, 13, 0, 8 },
	{ 2, 0, 16, 29, 124, 0, 16, 8, 29, 30, 0, &nicky_anim_frames_table[39], 0, 0, 13, 0, 8 },
	{ 0, 0, 16, 29, 124, 0, 16, 11, 29, 31, 0, 0, 0, 0, 13, 0, 13 },
	/* 40 */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 19, 0, &nicky_anim_frames_table[41], 0, 2, 13, 0, 0 },
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 18, 0, &nicky_anim_frames_table[42], 0, 2, 13, 0, 0 },
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 32, 0, &nicky_anim_frames_table[43], 0, 2, 13, 0, 0 }, /* use megabomb */
	{ 4, 1, 16, 29, 124, 0, 16, 1, 29, 33, 0, &nicky_anim_frames_table[44], 0, 2, 13, 0, 0 }, /* use megabomb */
	/* 44 */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 32, 0, &nicky_anim_frames_table[45], 0, 2, 13, 0, 0 }, /* use megabomb */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 18, 0, &nicky_anim_frames_table[23], 0, 0, 13, 0, 0 },
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 16, 0, &nicky_anim_frames_table[47], 0, 0, 13, 0, 0 },
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 17, 0, &nicky_anim_frames_table[48], 0, 0, 13, 0, 0 },
	/* 48 */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 32, 0, &nicky_anim_frames_table[49], 0, 2, 13, 0, 0 }, /* use megabomb */
	{ 4, 1, 16, 29, 124, 0, 16, 1, 29, 33, 0, &nicky_anim_frames_table[50], 0, 2, 13, 0, 0 }, /* use megabomb */
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 32, 0, &nicky_anim_frames_table[51], 0, 2, 13, 0, 0 },
	{ 3, 0, 16, 29, 124, 0, 16, 1, 29, 17, 0, &nicky_anim_frames_table[19], 0, 2, 13, 0, 0 }
};

action_op_pf action_op_table[NUM_ACTION_OPCODES] = {
	/* 0x00 */
	game_action_op_nop,
	game_action_op_nop,
	game_action_op_nop,
	game_action_op_use_megabomb,
	/* 0x04 */
	game_action_op4,
	game_action_op5,
	game_action_op6
};

update_nicky_anim_helper_pf update_nicky_anim_helper_table[NUM_NICKY_ANIM_HELPERS] = {
	/* 0x00 */
	game_update_nicky_anim_helper0,
	game_update_nicky_anim_helper1, /* GAME_DIR_RIGHT */
	game_update_nicky_anim_helper2, /* GAME_DIR_LEFT */
	0, /* GAME_DIR_RIGHT | GAME_DIR_LEFT */
  	/* 0x04 */
 	game_update_nicky_anim_helper3, /* GAME_DIR_UP */
 	game_update_nicky_anim_helper4, /* GAME_DIR_UP | GAME_DIR_RIGHT */
 	game_update_nicky_anim_helper5, /* GAME_DIR_UP | GAME_DIR_LEFT */
 	0, /* GAME_DIR_UP | GAME_DIR_LEFT | GAME_DIR_RIGHT */
  	/* 0x08 */
 	game_update_nicky_anim_helper6, /* GAME_DIR_DOWN */
 	game_update_nicky_anim_helper6, /* GAME_DIR_DOWN | GAME_DIR_RIGHT */
 	game_update_nicky_anim_helper6  /* GAME_DIR_DOWN | GAME_DIR_LEFT */
};

logic_op_pf logic_op_table[NUM_LOGIC_OPCODES] = {
	/* 0x00 */
	game_logic_op0,
	game_logic_op1,
	game_logic_op2,
	game_logic_op3,
	/* 0x04 */
	game_logic_op4,
	game_logic_op5,
	game_logic_op6,
	game_logic_op7,
	/* 0x08 */
	game_logic_op8,
	game_logic_op9,
	game_logic_op10,
	game_logic_op11,
	/* 0x0C */
	game_logic_op12,
	game_logic_op13,
	game_logic_op14,
	game_logic_op15,
	/* 0x10 */
	game_logic_op16,
	game_logic_op17,
	game_logic_op18,
	game_logic_op19
};

colliding_op_pf colliding_op_table[NUM_COLLIDING_OPCODES] = {
	/* 0x00 */
	game_collides_op_0_24,
	game_collides_op_potion,
	game_collides_op_life_bonus,
	game_collides_op_key_bonus,
	/* 0x04 */
	game_collides_op_spring,
	game_collides_op_wood_bonus,
	game_collides_op_shield_bonus,
	game_collides_op_magnifying_glass,
	/* 0x08 */
	game_collides_op_bomb_bonus,
	game_collides_op_megabomb_bonus,
	game_collides_op_red_ball,
	game_collides_op_blue_ball,
	/* 0x0C */
	game_collides_op_apple,
	game_collides_op_teleport,
	game_collides_op_misc_bonus,
	game_collides_op_bonus_box,
	/* 0x10 */
	game_collides_op_blue_box,
	game_collides_op_unk,
	game_collides_op_unk,
	game_collides_op_door,
	/* 0x14 */
	game_collides_op_eye,
	game_collides_op_21_28,
	game_collides_op_unk,
	game_collides_op_unk,
	/* 0x18 */
	game_collides_op_0_24,
	game_collides_op_go_to_next_level,
	game_collides_op_glass_pieces,
	game_collides_op_27,
	/* 0x1C */
	game_collides_op_21_28,
	game_collides_op_29,
	game_collides_op_unk
};

const uint16 nicky_move_offsets_table[] = {
	0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA,
	0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA,
	0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA,
	0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFA, 0xFFFB,
	0xFFFA, 0xFFFA, 0xFFFA, 0xFFFB, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE, 0xFFFF, 0xFFFF,
	0x0000, 0x0001, 0x0001, 0x0002, 0x0002, 0x0003, 0x0004, 0x7777
};

/* death movement */
const uint16 nicky_move_offsets_table5[] = {
	0x0000, 0xFFFC, 0x0000, 0xFFFC, 0x0000, 0xFFFD, 0x0000, 0xFFFD, 0x0000, 0xFFFD,
	0x0000, 0xFFFD, 0x0000, 0xFFFD, 0x0000, 0xFFFE, 0x0000, 0xFFFE, 0x0000, 0xFFFF,
	0x0000, 0xFFFE, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001, 0x0000, 0x0001,
	0x0000, 0x0002, 0x0000, 0x0002, 0x0000, 0x0002, 0x0000, 0x0003, 0x0000, 0x0003,
	0x0000, 0x0003, 0x0000, 0x0003, 0x0000, 0x0004, 0x0000, 0x0004, 0x0000, 0x0004,
	0x0000, 0x0004, 0x0000, 0x0005, 0x0000, 0x0004, 0x0000, 0x0005, 0x0000, 0x0005,
	0x0000, 0x0005, 0x0000, 0x0005, 0x0000, 0x0005, 0x0000, 0x0005, 0x0000, 0x0005,
	0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006,
	0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006,
	0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006,
	0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006, 0x0000, 0x0006,
	0x0000, 0x0006, 0x0000, 0x0006, 0x8888, 0x8888
};

const uint8 tile_map_quad_table[204] = {
	0x36, 0x36, 0xB0, 0xB0, 0x16, 0x85, 0x17, 0x9A, 0x00, 0x1F, 0x00, 0x0B, 0x36, 0x85, 0x36, 0x9A, 
	0x14, 0xAF, 0x16, 0x8F, 0x16, 0x9A, 0x16, 0x36, 0x36, 0x85, 0x15, 0x2A, 0x00, 0x00, 0x00, 0x00, 
	0x36, 0x9A, 0x36, 0x36, 0x04, 0x07, 0xAF, 0x07, 0x04, 0x8C, 0x4D, 0x64, 0x00, 0x49, 0x00, 0x35, 
	0x16, 0xAF, 0x17, 0xAF, 0xB4, 0x9A, 0x36, 0x36, 0x36, 0xAF, 0x36, 0xAF, 0x36, 0x36, 0xB1, 0xB1, 
	0x36, 0x36, 0x00, 0x11, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x85, 0x9A, 0xC4, 0x8F, 0xAF, 
	0x36, 0x36, 0x1C, 0x36, 0xC7, 0x9A, 0x8F, 0xAF, 0x1C, 0x36, 0x1D, 0x36, 0x4A, 0x4C, 0x4C, 0x60, 
	0x4A, 0x60, 0x60, 0x4C, 0x4A, 0x4C, 0xCB, 0xDF, 0x14, 0x38, 0x14, 0x4A, 0x14, 0x14, 0x14, 0x14, 
	0xEA, 0xEA, 0xEA, 0xEA, 0x4A, 0x60, 0x38, 0x4C, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x30, 
	0x01, 0x01, 0x30, 0x30, 0x66, 0x66, 0x66, 0x66, 0x5C, 0x20, 0x5C, 0x20, 0x01, 0x30, 0x01, 0x30, 
	0x01, 0x30, 0x01, 0x01, 0x01, 0x70, 0x01, 0x01, 0x01, 0x30, 0xE9, 0x30, 0x01, 0x01, 0xE9, 0x30, 
	0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x30, 0x19, 0x30, 0xAB, 0x30, 0x18, 0x30, 0x19, 0x30, 0xAB, 
	0x30, 0xBE, 0x30, 0xBF, 0x45, 0xC4, 0x47, 0xC5, 0x5C, 0x20, 0x5C, 0x20, 0x5C, 0x08, 0x5C, 0x08, 
	0x5C, 0x08, 0x5C, 0x8B, 0x5C, 0x8A, 0x5C, 0x08, 0x14, 0x39, 0x14, 0x39
};

const uint16 cos_table[512] = {
	0x0100, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FE, 0x00FE, 0x00FE,
	0x00FD, 0x00FD, 0x00FC, 0x00FC, 0x00FB, 0x00FB, 0x00FA, 0x00F9, 0x00F9, 0x00F8, 0x00F7,
	0x00F6, 0x00F5, 0x00F4, 0x00F4, 0x00F3, 0x00F2, 0x00F1, 0x00EF, 0x00EE, 0x00ED, 0x00EC,
	0x00EB, 0x00EA, 0x00E8, 0x00E7, 0x00E6, 0x00E4, 0x00E3, 0x00E1, 0x00E0, 0x00DE, 0x00DD,
	0x00DB, 0x00D9, 0x00D8, 0x00D6, 0x00D4, 0x00D3, 0x00D1, 0x00CF, 0x00CD, 0x00CB, 0x00C9,
	0x00C7, 0x00C5, 0x00C3, 0x00C1, 0x00BF, 0x00BD, 0x00BB, 0x00B9, 0x00B7, 0x00B5, 0x00B2,
	0x00B0, 0x00AE, 0x00AB, 0x00A9, 0x00A7, 0x00A4, 0x00A2, 0x009F, 0x009D, 0x009B, 0x0098,
	0x0095, 0x0093, 0x0090, 0x008E, 0x008B, 0x0088, 0x0086, 0x0083, 0x0080, 0x007E, 0x007B,
	0x0078, 0x0075, 0x0073, 0x0070, 0x006D, 0x006A, 0x0067, 0x0064, 0x0061, 0x005F, 0x005C,
	0x0059, 0x0056, 0x0053, 0x0050, 0x004D, 0x004A, 0x0047, 0x0044, 0x0041, 0x003E, 0x003B,
	0x0038, 0x0035, 0x0031, 0x002E, 0x002B, 0x0028, 0x0025, 0x0022, 0x001F, 0x001C, 0x0019,
	0x0015, 0x0012, 0x000F, 0x000C, 0x0009, 0x0006, 0x0003, 0x0000, 0xFFFD, 0xFFFA, 0xFFF7,
	0xFFF4, 0xFFF1, 0xFFEE, 0xFFEB, 0xFFE7, 0xFFE4, 0xFFE1, 0xFFDE, 0xFFDB, 0xFFD8, 0xFFD5,
	0xFFD2, 0xFFCF, 0xFFCB, 0xFFC8, 0xFFC5, 0xFFC2, 0xFFBF, 0xFFBC, 0xFFB9, 0xFFB6, 0xFFB3,
	0xFFB0, 0xFFAD, 0xFFAA, 0xFFA7, 0xFFA4, 0xFFA1, 0xFF9F, 0xFF9C, 0xFF99, 0xFF96, 0xFF93,
	0xFF90, 0xFF8D, 0xFF8B, 0xFF88, 0xFF85, 0xFF82, 0xFF80, 0xFF7D, 0xFF7A, 0xFF78, 0xFF75,
	0xFF72, 0xFF70, 0xFF6D, 0xFF6B, 0xFF68, 0xFF65, 0xFF63, 0xFF61, 0xFF5E, 0xFF5C, 0xFF59,
	0xFF57, 0xFF55, 0xFF52, 0xFF50, 0xFF4E, 0xFF4B, 0xFF49, 0xFF47, 0xFF45, 0xFF43, 0xFF41,
	0xFF3F, 0xFF3D, 0xFF3B, 0xFF39, 0xFF37, 0xFF35, 0xFF33, 0xFF31, 0xFF2F, 0xFF2D, 0xFF2C,
	0xFF2A, 0xFF28, 0xFF27, 0xFF25, 0xFF23, 0xFF22, 0xFF20, 0xFF1F, 0xFF1D, 0xFF1C, 0xFF1A,
	0xFF19, 0xFF18, 0xFF16, 0xFF15, 0xFF14, 0xFF13, 0xFF12, 0xFF11, 0xFF0F, 0xFF0E, 0xFF0D,
	0xFF0C, 0xFF0C, 0xFF0B, 0xFF0A, 0xFF09, 0xFF08, 0xFF07, 0xFF07, 0xFF06, 0xFF05, 0xFF05,
	0xFF04, 0xFF04, 0xFF03, 0xFF03, 0xFF02, 0xFF02, 0xFF02, 0xFF01, 0xFF01, 0xFF01, 0xFF01,
	0xFF01, 0xFF01, 0xFF01, 0xFF00, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01,
	0xFF02, 0xFF02, 0xFF02, 0xFF03, 0xFF03, 0xFF04, 0xFF04, 0xFF05, 0xFF05, 0xFF06, 0xFF07,
	0xFF07, 0xFF08, 0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF0F, 0xFF11,
	0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF18, 0xFF19, 0xFF1A, 0xFF1C, 0xFF1D, 0xFF1F,
	0xFF20, 0xFF22, 0xFF23, 0xFF25, 0xFF27, 0xFF28, 0xFF2A, 0xFF2C, 0xFF2D, 0xFF2F, 0xFF31,
	0xFF33, 0xFF35, 0xFF37, 0xFF39, 0xFF3B, 0xFF3D, 0xFF3F, 0xFF41, 0xFF43, 0xFF45, 0xFF47,
	0xFF49, 0xFF4B, 0xFF4E, 0xFF50, 0xFF52, 0xFF55, 0xFF57, 0xFF59, 0xFF5C, 0xFF5E, 0xFF61,
	0xFF63, 0xFF65, 0xFF68, 0xFF6B, 0xFF6D, 0xFF70, 0xFF72, 0xFF75, 0xFF78, 0xFF7A, 0xFF7D,
	0xFF80, 0xFF82, 0xFF85, 0xFF88, 0xFF8B, 0xFF8D, 0xFF90, 0xFF93, 0xFF96, 0xFF99, 0xFF9C,
	0xFF9F, 0xFFA1, 0xFFA4, 0xFFA7, 0xFFAA, 0xFFAD, 0xFFB0, 0xFFB3, 0xFFB6, 0xFFB9, 0xFFBC,
	0xFFBF, 0xFFC2, 0xFFC5, 0xFFC8, 0xFFCB, 0xFFCF, 0xFFD2, 0xFFD5, 0xFFD8, 0xFFDB, 0xFFDE,
	0xFFE1, 0xFFE4, 0xFFE7, 0xFFEB, 0xFFEE, 0xFFF1, 0xFFF4, 0xFFF7, 0xFFFA, 0xFFFD, 0x0000,
	0x0003, 0x0006, 0x0009, 0x000C, 0x000F, 0x0012, 0x0015, 0x0019, 0x001C, 0x001F, 0x0022,
	0x0025, 0x0028, 0x002B, 0x002E, 0x0031, 0x0035, 0x0038, 0x003B, 0x003E, 0x0041, 0x0044,
	0x0047, 0x004A, 0x004D, 0x0050, 0x0053, 0x0056, 0x0059, 0x005C, 0x005F, 0x0061, 0x0064,
	0x0067, 0x006A, 0x006D, 0x0070, 0x0073, 0x0075, 0x0078, 0x007B, 0x007E, 0x0080, 0x0083,
	0x0086, 0x0088, 0x008B, 0x008E, 0x0090, 0x0093, 0x0095, 0x0098, 0x009B, 0x009D, 0x009F,
	0x00A2, 0x00A4, 0x00A7, 0x00A9, 0x00AB, 0x00AE, 0x00B0, 0x00B2, 0x00B5, 0x00B7, 0x00B9,
	0x00BB, 0x00BD, 0x00BF, 0x00C1, 0x00C3, 0x00C5, 0x00C7, 0x00C9, 0x00CB, 0x00CD, 0x00CF,
	0x00D1, 0x00D3, 0x00D4, 0x00D6, 0x00D8, 0x00D9, 0x00DB, 0x00DD, 0x00DE, 0x00E0, 0x00E1,
	0x00E3, 0x00E4, 0x00E6, 0x00E7, 0x00E8, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00F9,
	0x00FA, 0x00FB, 0x00FB, 0x00FC, 0x00FC, 0x00FD, 0x00FD, 0x00FE, 0x00FE, 0x00FE, 0x00FF,
	0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF
};

const uint16 sin_table[512] = {
	0x0000, 0x0003, 0x0006, 0x0009, 0x000C, 0x000F, 0x0012, 0x0015, 0x0019, 0x001C, 0x001F,
	0x0022, 0x0025, 0x0028, 0x002B, 0x002E, 0x0031, 0x0035, 0x0038, 0x003B, 0x003E, 0x0041,
	0x0044, 0x0047, 0x004A, 0x004D, 0x0050, 0x0053, 0x0056, 0x0059, 0x005C, 0x005F, 0x0061,
	0x0064, 0x0067, 0x006A, 0x006D, 0x0070, 0x0073, 0x0075, 0x0078, 0x007B, 0x007E, 0x0080,
	0x0083, 0x0086, 0x0088, 0x008B, 0x008E, 0x0090, 0x0093, 0x0095, 0x0098, 0x009B, 0x009D,
	0x009F, 0x00A2, 0x00A4, 0x00A7, 0x00A9, 0x00AB, 0x00AE, 0x00B0, 0x00B2, 0x00B5, 0x00B7,
	0x00B9, 0x00BB, 0x00BD, 0x00BF, 0x00C1, 0x00C3, 0x00C5, 0x00C7, 0x00C9, 0x00CB, 0x00CD,
	0x00CF, 0x00D1, 0x00D3, 0x00D4, 0x00D6, 0x00D8, 0x00D9, 0x00DB, 0x00DD, 0x00DE, 0x00E0,
	0x00E1, 0x00E3, 0x00E4, 0x00E6, 0x00E7, 0x00E8, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE,
	0x00EF, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9,
	0x00F9, 0x00FA, 0x00FB, 0x00FB, 0x00FC, 0x00FC, 0x00FD, 0x00FD, 0x00FE, 0x00FE, 0x00FE,
	0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x0100, 0x00FF, 0x00FF, 0x00FF,
	0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FE, 0x00FE, 0x00FE, 0x00FD, 0x00FD, 0x00FC, 0x00FC,
	0x00FB, 0x00FB, 0x00FA, 0x00F9, 0x00F9, 0x00F8, 0x00F7, 0x00F6, 0x00F5, 0x00F4, 0x00F4,
	0x00F3, 0x00F2, 0x00F1, 0x00EF, 0x00EE, 0x00ED, 0x00EC, 0x00EB, 0x00EA, 0x00E8, 0x00E7,
	0x00E6, 0x00E4, 0x00E3, 0x00E1, 0x00E0, 0x00DE, 0x00DD, 0x00DB, 0x00D9, 0x00D8, 0x00D6,
	0x00D4, 0x00D3, 0x00D1, 0x00CF, 0x00CD, 0x00CB, 0x00C9, 0x00C7, 0x00C5, 0x00C3, 0x00C1,
	0x00BF, 0x00BD, 0x00BB, 0x00B9, 0x00B7, 0x00B5, 0x00B2, 0x00B0, 0x00AE, 0x00AB, 0x00A9,
	0x00A7, 0x00A4, 0x00A2, 0x009F, 0x009D, 0x009B, 0x0098, 0x0095, 0x0093, 0x0090, 0x008E,
	0x008B, 0x0088, 0x0086, 0x0083, 0x0080, 0x007E, 0x007B, 0x0078, 0x0075, 0x0073, 0x0070,
	0x006D, 0x006A, 0x0067, 0x0064, 0x0061, 0x005F, 0x005C, 0x0059, 0x0056, 0x0053, 0x0050,
	0x004D, 0x004A, 0x0047, 0x0044, 0x0041, 0x003E, 0x003B, 0x0038, 0x0035, 0x0031, 0x002E,
	0x002B, 0x0028, 0x0025, 0x0022, 0x001F, 0x001C, 0x0019, 0x0015, 0x0012, 0x000F, 0x000C,
	0x0009, 0x0006, 0x0003, 0x0000, 0xFFFD, 0xFFFA, 0xFFF7, 0xFFF4, 0xFFF1, 0xFFEE, 0xFFEB,
	0xFFE7, 0xFFE4, 0xFFE1, 0xFFDE, 0xFFDB, 0xFFD8, 0xFFD5, 0xFFD2, 0xFFCF, 0xFFCB, 0xFFC8,
	0xFFC5, 0xFFC2, 0xFFBF, 0xFFBC, 0xFFB9, 0xFFB6, 0xFFB3, 0xFFB0, 0xFFAD, 0xFFAA, 0xFFA7,
	0xFFA4, 0xFFA1, 0xFF9F, 0xFF9C, 0xFF99, 0xFF96, 0xFF93, 0xFF90, 0xFF8D, 0xFF8B, 0xFF88,
	0xFF85, 0xFF82, 0xFF80, 0xFF7D, 0xFF7A, 0xFF78, 0xFF75, 0xFF72, 0xFF70, 0xFF6D, 0xFF6B,
	0xFF68, 0xFF65, 0xFF63, 0xFF61, 0xFF5E, 0xFF5C, 0xFF59, 0xFF57, 0xFF55, 0xFF52, 0xFF50,
	0xFF4E, 0xFF4B, 0xFF49, 0xFF47, 0xFF45, 0xFF43, 0xFF41, 0xFF3F, 0xFF3D, 0xFF3B, 0xFF39,
	0xFF37, 0xFF35, 0xFF33, 0xFF31, 0xFF2F, 0xFF2D, 0xFF2C, 0xFF2A, 0xFF28, 0xFF27, 0xFF25,
	0xFF23, 0xFF22, 0xFF20, 0xFF1F, 0xFF1D, 0xFF1C, 0xFF1A, 0xFF19, 0xFF18, 0xFF16, 0xFF15,
	0xFF14, 0xFF13, 0xFF12, 0xFF11, 0xFF0F, 0xFF0E, 0xFF0D, 0xFF0C, 0xFF0C, 0xFF0B, 0xFF0A,
	0xFF09, 0xFF08, 0xFF07, 0xFF07, 0xFF06, 0xFF05, 0xFF05, 0xFF04, 0xFF04, 0xFF03, 0xFF03,
	0xFF02, 0xFF02, 0xFF02, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF00,
	0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF01, 0xFF02, 0xFF02, 0xFF02, 0xFF03,
	0xFF03, 0xFF04, 0xFF04, 0xFF05, 0xFF05, 0xFF06, 0xFF07, 0xFF07, 0xFF08, 0xFF09, 0xFF0A,
	0xFF0B, 0xFF0C, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF0F, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15,
	0xFF16, 0xFF18, 0xFF19, 0xFF1A, 0xFF1C, 0xFF1D, 0xFF1F, 0xFF20, 0xFF22, 0xFF23, 0xFF25,
	0xFF27, 0xFF28, 0xFF2A, 0xFF2C, 0xFF2D, 0xFF2F, 0xFF31, 0xFF33, 0xFF35, 0xFF37, 0xFF39,
	0xFF3B, 0xFF3D, 0xFF3F, 0xFF41, 0xFF43, 0xFF45, 0xFF47, 0xFF49, 0xFF4B, 0xFF4E, 0xFF50,
	0xFF52, 0xFF55, 0xFF57, 0xFF59, 0xFF5C, 0xFF5E, 0xFF61, 0xFF63, 0xFF65, 0xFF68, 0xFF6B,
	0xFF6D, 0xFF70, 0xFF72, 0xFF75, 0xFF78, 0xFF7A, 0xFF7D, 0xFF80, 0xFF82, 0xFF85, 0xFF88,
	0xFF8B, 0xFF8D, 0xFF90, 0xFF93, 0xFF96, 0xFF99, 0xFF9C, 0xFF9F, 0xFFA1, 0xFFA4, 0xFFA7,
	0xFFAA, 0xFFAD, 0xFFB0, 0xFFB3, 0xFFB6, 0xFFB9, 0xFFBC, 0xFFBF, 0xFFC2, 0xFFC5, 0xFFC8,
	0xFFCB, 0xFFCF, 0xFFD2, 0xFFD5, 0xFFD8, 0xFFDB, 0xFFDE, 0xFFE1, 0xFFE4, 0xFFE7, 0xFFEB,
	0xFFEE, 0xFFF1, 0xFFF4, 0xFFF7, 0xFFFA, 0xFFFD
};

const uint8 move_delta_table[854] = {
	0x00, 0xF8, 0x00, 0xF9, 0x00, 0xFB, 0x00, 0xFB, 0x00, 0xFD, 0x00, 0xFD, 0x00, 0xFF, 0x00, 0xFF, 
	0x00, 0x01, 0x00, 0x01, 0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x05, 0x00, 0x07, 0x00, 0x08, 
	0x00, 0x09, 0x00, 0x09, 0x00, 0x0B, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 
	0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 
	0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 
	0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x88, 0x88, 0x00, 0xFB, 0x00, 0xFC, 0x00, 0xFD, 0x00, 0xFD, 
	0x00, 0xFE, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x03, 
	0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 0x00, 0x08, 0x00, 0x0A, 
	0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 
	0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 
	0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 0x00, 0x0D, 
	0x00, 0x0D, 0x88, 0x88, 0x00, 0xF4, 0x00, 0xF4, 0x00, 0xF4, 0x00, 0xF5, 0x00, 0xF6, 0x00, 0xF6, 
	0x00, 0xF8, 0x00, 0xF9, 0x00, 0xFB, 0x00, 0xFB, 0x00, 0xFE, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x02, 
	0x00, 0x04, 0x00, 0x05, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0C, 
	0x00, 0x0C, 0x00, 0x0C, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x00, 0xF6, 0xFF, 0xF7, 
	0xFF, 0xF6, 0xFF, 0xF8, 0xFF, 0xF8, 0xFE, 0xF8, 0xFE, 0xFA, 0xFE, 0xFA, 0xFE, 0xFC, 0xFD, 0xFD, 
	0xFE, 0xFE, 0xFD, 0xFF, 0xFE, 0x01, 0xFD, 0x01, 0xFE, 0x03, 0xFD, 0x05, 0xFE, 0x05, 0xFE, 0x06, 
	0xFE, 0x07, 0xFF, 0x08, 0xFF, 0x09, 0xFF, 0x09, 0xFF, 0x0A, 0x00, 0x09, 0x00, 0x02, 0x00, 0x00, 
	0x00, 0x00, 0x88, 0x88, 0x00, 0xF5, 0x00, 0xF5, 0x00, 0xF5, 0x00, 0xF5, 0x00, 0xF5, 0x00, 0xF6, 
	0x00, 0xF7, 0x00, 0xF6, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF9, 0x00, 0xFA, 0x00, 0xFB, 
	0x00, 0xFC, 0x00, 0xFC, 0x00, 0xFD, 0x00, 0xFE, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 
	0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x05, 0x00, 0x06, 0x00, 0x06, 0x00, 0x07, 0x00, 0x08, 
	0x00, 0x09, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0B, 0x00, 0x0B, 0x00, 0x0B, 
	0x00, 0x0B, 0x00, 0x03, 0x88, 0x88, 0x00, 0xF5, 0x00, 0xF7, 0x00, 0xF7, 0x00, 0xF8, 0x00, 0xF9, 
	0x00, 0xFB, 0x00, 0xFB, 0x00, 0xFD, 0x00, 0xFD, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x01, 
	0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x05, 0x00, 0x07, 0x00, 0x08, 0x00, 0x09, 0x00, 0x09, 
	0x00, 0x0B, 0x00, 0x0C, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 
	0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 
	0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x00, 0x0E, 0x88, 0x88, 0x00, 0xF6, 
	0xFE, 0xF7, 0xFF, 0xF6, 0xFD, 0xF8, 0xFD, 0xF8, 0xFC, 0xF8, 0xFC, 0xFA, 0xFB, 0xFA, 0xFB, 0xFC, 
	0xFA, 0xFD, 0xFA, 0xFE, 0xFA, 0xFF, 0xFB, 0x01, 0xFA, 0x01, 0xFA, 0x03, 0xFB, 0x05, 0xFB, 0x05, 
	0xFC, 0x06, 0xFC, 0x07, 0xFC, 0x08, 0xFE, 0x09, 0xFE, 0x09, 0xFF, 0x0A, 0xFF, 0x09, 0x00, 0x02, 
	0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF9, 0x00, 0xFA, 0x00, 0xFA, 
	0x00, 0xFB, 0x00, 0xFC, 0x00, 0xFD, 0x00, 0xFD, 0x00, 0xFE, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x06, 
	0x00, 0x07, 0x00, 0x08, 0x00, 0x08, 0x00, 0x0A, 0x88, 0x88, 0x00, 0xFC, 0x01, 0xFD, 0x00, 0xFC, 
	0x01, 0xFD, 0x02, 0xFC, 0x01, 0xFD, 0x02, 0xFD, 0x02, 0xFC, 0x02, 0xFD, 0x03, 0xFD, 0x03, 0xFD, 
	0x03, 0xFD, 0x03, 0xFE, 0x04, 0xFD, 0x03, 0xFE, 0x04, 0xFE, 0x04, 0xFE, 0x04, 0xFE, 0x05, 0xFE, 
	0x04, 0xFF, 0x05, 0xFF, 0x04, 0xFF, 0x05, 0xFF, 0x05, 0x00, 0x04, 0xFF, 0x05, 0x00, 0x05, 0x00, 
	0x05, 0x00, 0x04, 0x01, 0x05, 0x01, 0x05, 0x00, 0x04, 0x02, 0x05, 0x01, 0x04, 0x01, 0x04, 0x02, 
	0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x04, 0x03, 0x03, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 
	0x02, 0x03, 0x02, 0x03, 0x02, 0x04, 0x02, 0x03, 0x01, 0x03, 0x01, 0x04, 0x01, 0x04, 0x01, 0x03, 
	0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x00, 0xFD, 0xFF, 0xFD, 0x00, 0xFC, 
	0xFE, 0xFD, 0xFF, 0xFD, 0xFE, 0xFE, 0xFE, 0xFD, 0xFE, 0xFD, 0xFE, 0xFE, 0xFD, 0xFD, 0xFD, 0xFE, 
	0xFD, 0xFE, 0xFC, 0xFF, 0xFD, 0xFE, 0xFC, 0xFF, 0xFC, 0xFF, 0xFC, 0xFF, 0xFD, 0xFF, 0xFC, 0x00, 
	0xFC, 0x00, 0xFC, 0x00, 0xFC, 0x01, 0xFC, 0x00, 0xFC, 0x01, 0xFC, 0x02, 0xFD, 0x01, 0xFC, 0x02, 
	0xFD, 0x02, 0xFD, 0x02, 0xFD, 0x02, 0xFE, 0x02, 0xFD, 0x03, 0xFE, 0x03, 0xFE, 0x03, 0xFF, 0x03, 
	0xFF, 0x03, 0xFF, 0x03, 0xFF, 0x03, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 
	0x00, 0xFD, 0x01, 0xFD, 0x00, 0xFC, 0x02, 0xFD, 0x01, 0xFD, 0x02, 0xFE, 0x02, 0xFD, 0x02, 0xFD, 
	0x02, 0xFE, 0x03, 0xFD, 0x03, 0xFE, 0x03, 0xFE, 0x04, 0xFF, 0x03, 0xFE, 0x04, 0xFF, 0x04, 0xFF, 
	0x04, 0xFF, 0x03, 0xFF, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x01, 0x04, 0x00, 0x04, 0x01, 
	0x04, 0x02, 0x03, 0x01, 0x04, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x02, 0x02, 0x03, 0x03, 
	0x02, 0x03, 0x02, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x00, 0x03, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x00, 0x88, 0x88
};

const uint8 move_delta_table2[204] = {
	0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06, 
	0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 
	0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 
	0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 
	0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 
	0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 
	0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 
	0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 
	0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x88, 0x88, 0x88, 0x88
};

anim_data_t anim_data0[23] = {
	{ 1, 0, 0, 0, 1, 0, 0, 82, 0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 0, 0, 8, 0, 9, move_delta_table2, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 1, 0, 0, 84, 0, 1, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 9, 0, 9, move_delta_table2, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 1, 0, 0, 30, 0, 1, 0, 0, 0, 0, 0, 0, 16, 16, 0, 1, 15, 1, 15, move_delta_table2, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 20, 0, 0, 1, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 8, 0, 8, 0, 6, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 20, 0, 0, 1, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 14, 14, 0, 2, 12, 2, 12, 0, 5, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 4, 0, 0, 0, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[6], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 4, 0, 0, 0, 0, 91, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[7], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 4, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[8], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 5, 0, 0, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[9], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 5, 0, 0, 0, 0, 94, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[10], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 5, 0, 0, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[11], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 1, 0, 0, 0, 0, 64, 2, 0, -1, 0, 0, 0, 0, 0, 26, 26, 0, -8, 34, -8, 34, 0, 0, 0, &anim_data0[12], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 65, 0, 0, -1, 0, 0, 0, 0, 0, 28, 28, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[13], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 66, 0, 0, -1, 0, 0, 0, 0, 0, 30, 30, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[14], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 67, 0, 0, -1, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[15], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 68, 0, 0, -1, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[16], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 69, 0, 0, -1, 0, 0, 0, 0, 0, 31, 31, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[17], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 70, 0, 0, -1, 0, 0, 0, 0, 0, 29, 29, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[18], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 71, 0, 0, -1, 0, 0, 0, 0, 0, 29, 29, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[19], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 72, 0, 0, -1, 0, 0, 0, 0, 0, 27, 27, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data0[20], 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 2, 0, 0, 0, 0, 73, 0, 0, -1, 0, 0, 0, 0, 0, 24, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0, 0, 0, 34, 0, 0, -1, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, move_delta_table, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

anim_data_t anim_data_starshield[8] = {
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[6], 0, 0, 0, 0, 0, 0, 0, 68 },
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[7], 0, 0, 0, 0, 0, 0, 0, 68 },
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[0], 0, 0, 0, 0, 0, 0, 0, 68 },
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[1], 0, 0, 0, 0, 0, 0, 0, 68 },
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[2], 0, 0, 0, 0, 0, 0, 0, 68 },
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[3], 0, 0, 0, 0, 0, 0, 0, 68 },
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[4], 0, 0, 0, 0, 0, 0, 0, 68 },
	{ 1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 0, 0, 0, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, &anim_data_starshield[5], 0, 0, 0, 0, 0, 0, 0, 68 }
};

const char *snd_module_table[MAX_SND_MODULES] = {
	"SONGDEAD.MOD",
	"SONGMON.MOD",
	"SONGPRES.MOD",
	"SONGWIN.MOD",
	"SONG1.MOD",
	"SONG2.MOD",
	"SONG3.MOD",
	"SONG4.MOD"
};

const sample_data_t snd_sample_data_table[MAX_SND_SAMPLES] = {
	/* 0 */
	{ 0x0000, 0x0136, 0x190, 0x3F, 0x1, 0x3 },
	{ 0x0136, 0x21DC, 0x1AC, 0x3F, 0x1, 0x8 },
	{ 0x2312, 0x0174, 0x1AC, 0x3F, 0x1, 0x8 },
	{ 0x2486, 0x0200, 0x1AC, 0x3F, 0x1, 0x3 },
	/* 4 */
	{ 0x2686, 0x06A0, 0x1AC, 0x3F, 0x1, 0x5 },
	{ 0x2D26, 0x0710, 0x1AC, 0x3F, 0x1, 0x4 },
	{ 0x3436, 0x19A0, 0x0CE, 0x3F, 0x1, 0x5 },
	{ 0x4DD6, 0x044C, 0x1AC, 0x3F, 0x1, 0x5 },
	/* 8 */
	{ 0x5222, 0x07E8, 0x1AC, 0x3F, 0x1, 0xA },
	{ 0x5A0A, 0x03B2, 0x1AC, 0x3F, 0x1, 0x3 },
	{ 0x5DBC, 0x11F6, 0x1AC, 0x3F, 0x1, 0x7 },
	{ 0x6FB2, 0x0B54, 0x1AC, 0x3F, 0x1, 0x8 },
	/* 12 */
	{ 0x7B06, 0x2A82, 0x1F4, 0x3F, 0x1, 0xA },
	{ 0xA588, 0x031C, 0x1AC, 0x3F, 0x1, 0x1 },
	{ 0xA8A4, 0x1C24, 0x0CE, 0x3F, 0x1, 0x9 },
	{ 0xC4C8, 0x1DD4, 0x0F0, 0x3F, 0x1, 0xA },
	/* 16 */
	{ 0xE29C, 0x0CCE, 0x1AC, 0x3F, 0x1, 0x6 },
	{ 0x0136, 0x21DC, 0x320, 0x3F, 0x1, 0x9 },
	{ 0x0136, 0x21DC, 0x118, 0x3F, 0x1, 0x9 },
	{ 0x0136, 0x21DC, 0x44C, 0x3F, 0x1, 0x9 }
};

const uint16 mpl_period_table[576] = {
	856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
	428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
	214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
	850, 802, 757, 715, 674, 637, 601, 567, 535, 505, 477, 450,
	425, 401, 379, 357, 337, 318, 300, 284, 268, 253, 239, 225,
	213, 201, 189, 179, 169, 159, 150, 142, 134, 126, 119, 113,
	844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474, 447,
	422, 398, 376, 355, 335, 316, 298, 282, 266, 251, 237, 224,
	211, 199, 188, 177, 167, 158, 149, 141, 133, 125, 118, 112,
	838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470, 444,
	419, 395, 373, 352, 332, 314, 296, 280, 264, 249, 235, 222,
	209, 198, 187, 176, 166, 157, 148, 140, 132, 125, 118, 111,
	832, 785, 741, 699, 660, 623, 588, 555, 524, 495, 467, 441,
	416, 392, 370, 350, 330, 312, 294, 278, 262, 247, 233, 220,
	208, 196, 185, 175, 165, 156, 147, 139, 131, 124, 117, 110,
	826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463, 437,
	413, 390, 368, 347, 328, 309, 292, 276, 260, 245, 232, 219,
	206, 195, 184, 174, 164, 155, 146, 138, 130, 123, 116, 109,
	820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460, 434,
	410, 387, 365, 345, 325, 307, 290, 274, 258, 244, 230, 217,
	205, 193, 183, 172, 163, 154, 145, 137, 129, 122, 115, 109,
	814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457, 431,
	407, 384, 363, 342, 323, 305, 288, 272, 256, 242, 228, 216,
	204, 192, 181, 171, 161, 152, 144, 136, 128, 121, 114, 108,
	907, 856, 808, 762, 720, 678, 640, 604, 570, 538, 504, 480,
	453, 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240,
	226, 214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120,
	900, 850, 802, 757, 715, 675, 636, 601, 567, 535, 505, 477,
	450, 425, 401, 379, 357, 337, 318, 300, 284, 268, 253, 238,
	225, 212, 200, 189, 179, 169, 159, 150, 142, 134, 126, 119,
	894, 844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474,
	447, 422, 398, 376, 355, 335, 316, 298, 282, 266, 251, 237,
	223, 211, 199, 188, 177, 167, 158, 149, 141, 133, 125, 118,
	887, 838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470,
	444, 419, 395, 373, 352, 332, 314, 296, 280, 264, 249, 235,
	222, 209, 198, 187, 176, 166, 157, 148, 140, 132, 125, 118,
	881, 832, 785, 741, 699, 660, 623, 588, 555, 524, 494, 467,
	441, 416, 392, 370, 350, 330, 312, 294, 278, 262, 247, 233,
	220, 208, 196, 185, 175, 165, 156, 147, 139, 131, 123, 117,
	875, 826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463,
	437, 413, 390, 368, 347, 338, 309, 292, 276, 260, 245, 232,
	219, 206, 195, 184, 174, 164, 155, 146, 138, 130, 123, 116,
	868, 820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460,
	434, 410, 387, 365, 345, 325, 307, 290, 274, 258, 244, 230,
	217, 205, 193, 183, 172, 163, 154, 145, 137, 129, 122, 115,
	862, 814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457,
	431, 407, 384, 363, 342, 323, 305, 288, 272, 256, 242, 228,
	216, 203, 192, 181, 171, 161, 152, 144, 136, 128, 121, 114
};
