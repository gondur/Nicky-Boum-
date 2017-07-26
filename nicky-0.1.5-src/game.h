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

#ifndef __GAME_H__
#define __GAME_H__

#include "util.h"
#include "common.h"

#define GAME_SCREEN_W          240
#define GAME_SCREEN_H          160
#define NUM_ACTION_OPCODES       7
#define NUM_NICKY_ANIM_HELPERS  11
#define NUM_LOGIC_OPCODES       20
#define NUM_COLLIDING_OPCODES   31
#define GAME_SCROLL_DX           4
#define GAME_SCROLL_DY           4

#define GAME_DIR_RIGHT 1
#define GAME_DIR_LEFT  2
#define GAME_DIR_UP    4
#define GAME_DIR_DOWN  8

#define GAME_INV_MEGABOMBS 0
#define GAME_INV_BOMBS     1
#define GAME_INV_KEYS      2
#define GAME_INV_LIFES     3

typedef struct {
	int16 delta_x;
	int16 delta_y;
	int16 tile_map_offs_x;
	int16 tile_map_offs_y;
	int16 tile_map_end_offs_x;
	int16 tile_map_end_offs_y;
	int16 unk18;
	int16 unk1A;
	int16 tile_block_x;
	int16 tile_block_y;
	int16 unk20;
	int16 unk22;
	int16 unk24;
} decor_state_t;

typedef struct {
	uint8 tile_num;
	uint8 anim;
	uint8 tiles_table[4];
} tile_anim_t;

typedef struct {
	object_state_t *starshield_object_state;
	int score;
	int extra_life_score;
	uint8 unk12;
	uint8 unk13;
	uint8 unk14;
	int8 unk15;
	tile_anim_t tile_anims_table[4];
} game_state_t;

typedef struct {
	uint8 unk0;
	uint8 unk1; /* mode 2:jumping */
	uint8 hdir_flags; /* 1:right, 0x10:climb */
	uint8 vdir_flags;
	int16 pos_x; /* screen related */
	int16 pos_y; /* screen related */
	int16 dim_w;
	int16 dim_h;
	int16 pos_dx_1;
	int16 pos_dy_1;
	int16 pos_dx_2;
	int16 pos_dy_2;
	const uint16 *move_offsets_data_ptr;
	int16 unk16;
	uint16 anim_frames_count;
	uint8 unk1B;
	uint8 unk1C;
	uint8 decor_ref_flag0;
	uint8 increment_life_bar;
	uint8 bombs_count;
	uint8 megabombs_count;
	uint8 keys_count;
	uint8 lifes_count;
	uint8 decor_ref_flag4;
	uint8 decor_ref_flag3;
	uint8 dead_flag;
	uint8 decor_ref_flag2;
	uint8 has_blue_ball;
	int8 unk28;
	uint8 energy;
	uint8 unk2A;
	uint8 action_code;
	int16 has_red_ball;
	int16 has_wood;
	int16 shield_duration;
	int16 tile_blinking_duration;
	int16 has_spring;
	int16 tilemap_offset;
	uint16 unk38;
	anim_frame_t *anim_frame_ptr;
	uint16 unk3C;
	uint8 unk3E;
	uint8 unk3F;
} player_state_t; /* sizeof == 0x40 */

typedef void (*action_op_pf)();
typedef void (*update_nicky_anim_helper_pf)();
typedef anim_data_t *(*logic_op_pf)(object_state_t *os, anim_data_t *ad);
typedef void (*colliding_op_pf)(object_state_t *os, anim_data_t *ad);

extern player_state_t player_state;
extern int inventory_enabled_flag;
extern int new_level_flag;
extern int quit_level_flag;
extern uint16 random_seed;
extern anim_frame_t nicky_anim_frames_table[52];
extern action_op_pf action_op_table[NUM_ACTION_OPCODES];
extern update_nicky_anim_helper_pf update_nicky_anim_helper_table[NUM_NICKY_ANIM_HELPERS];
extern logic_op_pf logic_op_table[NUM_LOGIC_OPCODES];
extern colliding_op_pf colliding_op_table[NUM_COLLIDING_OPCODES];
extern const uint16 nicky_move_offsets_table[];
extern const uint16 nicky_move_offsets_table5[];
extern const uint8 tile_map_quad_table[204];
extern const uint16 cos_table[512];
extern const uint16 sin_table[512];
extern const uint8 move_delta_table[854];
extern const uint8 move_delta_table2[204];
extern anim_data_t anim_data0[23];
extern anim_data_t anim_data_starshield[8];

extern void game_init();
extern void game_run(int start_level_num);
extern void game_destroy();
extern uint16 game_get_random_number(uint16 mod);
extern void game_prepare_level(int level);
extern void game_init_level();
extern void game_init_nicky();
extern void game_enable_nicky_shield();
extern void game_run_cycle();
extern void game_init_player_state();
extern void game_update_nicky_anim();
extern void game_adjust_player_position();
extern void game_handle_nicky_shield();
extern void game_play_sound();
extern void game_handle_objects6();
extern void game_draw_bomb_object();
extern void game_handle_objects5();
extern void game_draw_object(object_state_t *os);
extern void game_shake_screen();
extern void game_update_decor_after_megabomb();
extern void game_draw_nicky();
extern void game_translate_object_tables(int16 dx, int16 dy);
extern void game_translate_object_table(object_state_t *os, int num, int16 dx, int16 dy);
extern void game_redraw_inventory();
extern uint8 game_set_decor_tile_flags4_3_2_0(uint8 b, int offset);
extern void game_set_decor_tile_flag0(uint8 b, int offset);
extern void inp_copy_state();
extern void game_set_next_nicky_anim_frame();
extern void game_disable_nicky_shield();
extern void game_init_level_helper();
extern void game_init_level_helper2();
extern void game_init_level_start_screen();
extern void game_init_ref_ref(anim_data_t *ad, uint16 flags);
extern void game_init_objects_from_positref(const uint8 *posit_ref, uint16 objects_offset);
extern void game_execute_logic_op_helper1(int x);
extern void game_execute_logic_op_helper2(object_state_t *_di, anim_data_t *ad);
extern void game_execute_logic_op();
extern void game_update_nicky_anim_helper0();
extern void game_update_nicky_anim_helper1();
extern void game_update_nicky_anim_helper2();
extern void game_update_nicky_anim_helper3();
extern void game_update_nicky_anim_helper4();
extern void game_update_nicky_anim_helper5();
extern void game_update_nicky_anim_helper6();
extern void game_update_nicky_anim_helper7();
extern void game_update_nicky_anim_helper8();
extern void game_change_decor_tile_map_line(const object_state_t *os, const anim_data_t *ad);
extern void game_change_decor_tile_map_quad(const object_state_t *os);
extern void game_change_decor_tile_map(const object_state_t *os);
extern void game_update_score();
extern void game_throw_apple();
extern void game_adjust_objects_position();
extern void game_adjust_tilemap();
extern void game_handle_level_change();
extern void game_update_decor();
extern void game_update_decor_helper(object_state_t *os);
extern uint8 game_get_decor_tile_ref(int x, int y);
extern void game_handle_projectiles_collisions();
extern void game_execute_colliding_op();
extern void game_execute_colliding_op_helper1();
extern void game_execute_colliding_op_helper2();
extern void game_reset_objects8();
extern void game_play_anim_data_sfx(const anim_data_t *ad);
extern void game_set_tile_offsets();
extern void game_update_life_bar();
extern void game_throw_apple_helper1();
extern void game_throw_apple_helper2();
extern void game_update_helper3();
extern void game_update_helper8();
extern void game_draw_bonuses();
extern void game_update_tile_offsets();
extern void game_update_helper4();
extern void game_setup_monster();
extern void game_draw_inventory_item(int item, int count);
extern void game_update_cur_objects_ptr();
extern void game_handle_objects2();
extern anim_data_t *game_init_object_from_anim3(object_state_t *os, anim_data_t *ad);
extern void game_handle_projectiles_collisions_helper1(object_state_t *os);
extern void game_handle_projectiles_collisions_helper2(object_state_t *os, int _bp, int count);
extern void game_handle_projectiles_collisions_helper3(object_state_t *os, anim_data_t *ad);
extern void game_handle_game_over();
extern void game_move_object(object_state_t *os);
extern void loc_1282D(object_state_t *os, anim_data_t *ad);

extern void game_execute_action_op();
extern void game_action_op_nop();
extern void game_action_op_use_megabomb();
extern void game_action_op4();
extern void game_action_op5();
extern void game_action_op6();

extern anim_data_t *game_logic_op_helper1(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op_unk(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op0(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op1(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op2(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op3(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op4(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op5(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op6(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op7(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op8(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op9(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op10(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op11(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op12(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op13(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op14(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op15(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op16(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op17(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op18(object_state_t *os, anim_data_t *ad);
extern anim_data_t *game_logic_op19(object_state_t *os, anim_data_t *ad);

extern void game_collides_op_unk(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_0_24(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_potion(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_life_bonus(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_key_bonus(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_spring(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_wood_bonus(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_shield_bonus(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_magnifying_glass(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_bomb_bonus(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_megabomb_bonus(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_red_ball(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_blue_ball(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_apple(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_teleport(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_misc_bonus(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_bonus_box(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_blue_box(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_door(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_eye(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_21_28(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_go_to_next_level(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_glass_pieces(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_27(object_state_t *os, anim_data_t *ad);
extern void game_collides_op_29(object_state_t *os, anim_data_t *ad);
	
#endif /* __GAME_H__ */
