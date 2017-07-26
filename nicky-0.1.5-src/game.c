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

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "fileio.h"
#include "input.h"
#include "resource.h"
#include "sequence.h"
#include "sound.h"
#include "systemstub.h"


int inventory_enabled_flag;
player_state_t player_state;
int new_level_flag;
int quit_level_flag;
uint16 random_seed;

static int current_level;
static game_state_t game_state;
static decor_state_t decor_state;
static int player_pos_x, player_pos_y;

static object_state_t objects_table[453] GCC_EXT;
static object_state_t *objects_table_ptr1 = &objects_table[0];
static object_state_t *objects_table_ptr2 = &objects_table[410]; /* bullets */
static object_state_t *objects_table_ptr3 = &objects_table[437]; /* bomb */
static object_state_t *objects_table_ptr4 = &objects_table[438];
static object_state_t *objects_table_ptr5 = &objects_table[446];
static object_state_t *objects_table_ptr6 = &objects_table[448]; /* apples */
static object_state_t *objects_table_ptr7 = &objects_table[452];
static object_state_t *objects_table_ptr8 = &objects_table[421]; /* bonuses */
static object_state_t *objects_table_ptr9 = &objects_table[426];
static object_state_t *objects_list_cur, *objects_list_head, *objects_list_last;
static const uint16 *nicky_move_offsets_ptr;

static int16 nicky_temp_pos, nicky_temp_pos2, nicky_temp_pos3;
static int bonus_count;
static anim_data_t *cur_anim_data_ptr;
static int16 cur_anim_data_count;
static anim_data_t *ref_ref_1, *ref_ref_7, *ref_ref_43;
static int16 cycle_score;
static uint8 update_decor_counter;
static uint8 throw_apple_var;
static int16 bounding_box_x1, bounding_box_x2, bounding_box_y1, bounding_box_y2;
static uint16 unk_ref_index;
static int16 megabomb_origin_x, megabomb_origin_y;
static int16 collision_state_counter;

/* flags */
static int execute_action_op_flag;
static int update_nicky_anim_flag;
static int restart_level_flag;
static int monster_fight_flag;
static int boss4_state;
static uint8 change_pal_flag;

/* sound */
static int8 _snd_current_sfx_num, _snd_current_sfx_priority;

/* screen */
static tile_anim_t _screen_blinking_tile;
static uint8 _screen_cdg_offsets[256] GCC_EXT;
static int _screen_tile_origin_x, _screen_tile_origin_y;
static int _screen_tile_map_offs_x, _screen_tile_map_offs_y;
static int _screen_tile_map_offs_x_shl4, _screen_tile_map_offs_y_shl4;


void game_init() {
	memset(&game_state, 0, sizeof(game_state));
	memset(&decor_state, 0, sizeof(decor_state));
	memset(&player_state, 0, sizeof(player_state));
	game_state.starshield_object_state = &objects_table[417];
	inventory_enabled_flag = 1;
	restart_level_flag = 0;
	quit_level_flag = 0;
	throw_apple_var = 0;
	bounding_box_x1 = bounding_box_x2 = bounding_box_y1 = bounding_box_y2 = 0;
	collision_state_counter = 0;
	random_seed = (uint16)time(0);
}

#ifdef NICKY_DEBUG
static void soundfx_test() {
	int cur_sfx = -1;
	while (1) {
		int prev_sfx = cur_sfx;
		int *key_mask = sys_get_key_mask();
		if (*key_mask & SKM_LEFT) {
			--cur_sfx;
			if (cur_sfx < 0) {
				cur_sfx = 0;
			}
			*key_mask &= ~SKM_LEFT;
		}
		if (*key_mask & SKM_RIGHT) {
			++cur_sfx;
			if (cur_sfx > MAX_SND_SAMPLES - 1) {
				cur_sfx = MAX_SND_SAMPLES - 1;
			}
			*key_mask &= ~SKM_RIGHT;
		}
		if (*key_mask & SKM_QUIT) {
			break;
		}
		if (prev_sfx != cur_sfx) {
			printf("current sfx = %d\n", cur_sfx);
			snd_play_sfx(cur_sfx);
		}
		sys_process_events();
	}
}
#endif

void game_run(int start_level_num) {
	/* display microids logo */
	sys_set_screen_mode(SSM_BITMAP);
	seq_display_image("I00.BMP", 4000);
	while (!sys_exit_flag) {
		seq_play_intro();
		game_prepare_level(start_level_num);
		while (!sys_exit_flag) {
			game_run_cycle();
			if (quit_level_flag >= 2) {
				inp_end_level();
				game_handle_game_over();
				break;
			} else if (new_level_flag) {
				inp_end_level();
				if (current_level == 7) {
					seq_play_ending();
/*					game_handle_highscore_screen();*/
					break;
				} else {
					++current_level;
/*					game_print_level_password();*/
					game_prepare_level(current_level);
					continue;
				}
			}
			sys_update_screen();
			sys_process_events();
			sys_clear_sprite_list();
		}
	}
}

void game_destroy() {
}

uint16 game_get_random_number(uint16 mod) {
	uint16 r1 = random_seed;
	uint16 r2 = r1;
	assert(mod != 0);
	r1 <<= 1;
	r1 += r2;
	r1 <<= 1;
	r1 += r2 * 3;
	r1 <<= 2;
	r1 += r2;
	r1 <<= 2;
	r1 += r2;
	r1 <<= 2;
	r1 += r2;
	r1 <<= 1;
	r1 += r2;
	r1 <<= 2;
	r1 += r2;
	r1 <<= 2;
	r1 += r2;
	random_seed = r1 + 1;
	return random_seed % mod;
}

void game_prepare_level(int level) {
	print_debug(DBG_GAME, "game_prepare_level(%d)", level);
	sys_set_screen_mode(SSM_TILED);
	current_level = level;
	game_state.score = 0;
	game_init_player_state();
	res_load_level_data(current_level);
	sys_set_tilemap_data(res_decor_cdg, _screen_cdg_offsets);
	game_init_level();
	game_init_nicky();
	snd_play_song(SND_MUS_SONGLEVEL + (current_level >> 1));
	inp_start_level(current_level);
}

void game_init_level() {
	print_debug(DBG_GAME, "game_init_level() level=%d", current_level);
	new_level_flag = 0;
	_snd_current_sfx_num = -1;
	_snd_current_sfx_priority = 0;
	player_state.keys_count = 0;
	player_pos_x = 96;
	player_pos_y = 99;
	_screen_tile_origin_x = 0;
	_screen_tile_origin_y = 0;
#if 0 /* boss fight debug */
	_screen_tile_origin_x = 18400;
	_screen_tile_origin_y = 40;
#endif
	player_state.tilemap_offset = 0;
	player_state.unk38 = 0;
	player_state.has_spring = 0;
	nicky_move_offsets_ptr = &nicky_move_offsets_table[40];
	monster_fight_flag = 0;
	boss4_state = 0;
	quit_level_flag = 0;
	game_state.unk12 = 0;
	ref_ref_1 = &res_ref_ref[1];
	ref_ref_7 = &res_ref_ref[7];
	ref_ref_43 = &res_ref_ref[43];
	game_init_ref_ref(res_ref_ref, 0x8000);
	game_init_level_helper();
	memset(objects_table, 0, sizeof(objects_table));
	game_init_objects_from_positref(res_posit_ref, 0);
	game_disable_nicky_shield();
	game_init_level_start_screen();
	cycle_score = 0;
	update_nicky_anim_flag = 1;
}

void game_init_nicky() {
	player_state.pos_x = player_pos_x;
	player_state.pos_y = player_pos_y;
	player_state.unk1 = 0;
	player_state.hdir_flags = 1;
	player_state.vdir_flags = 0;
	player_state.anim_frames_count = 0;
	player_state.unk1C = 0;
	player_state.unk28 = 0;
	player_state.increment_life_bar = 1;
	player_state.dead_flag = 0;
	player_state.energy = 0;
	player_state.unk2A = 0;
	player_state.action_code = 0;
	player_state.anim_frame_ptr = &nicky_anim_frames_table[19];
	game_enable_nicky_shield();
	player_state.shield_duration = 475;
	unk_ref_index = 0;
	execute_action_op_flag = 0;
	change_pal_flag = 0;
}

void game_enable_nicky_shield() {
	int i;
	object_state_t *os = game_state.starshield_object_state;
	anim_data_t *ad = &anim_data_starshield[0];
	for (i = 0; i < 4; ++i) {
		os->anim_data1_ptr = ad;
		os->pos_x = -16;
		os->pos_y = -16;
		os->displayed = 1;
		os->distance_pos_x = 0;
		os->distance_pos_y = 0;
		os->map_pos_x = i * 256;
		os->map_pos_y = 1;
		++os;
		++ad;
	}
	player_state.shield_duration = 0;
}

void game_run_cycle() {
	inp_copy_state();
	game_adjust_objects_position();
	game_update_nicky_anim();
	game_handle_nicky_shield();
	game_play_sound();
	game_throw_apple();
	game_adjust_tilemap();
	game_execute_logic_op();
	game_handle_objects2();
	game_update_helper3();
	game_handle_objects6();
	game_execute_action_op();
	game_handle_projectiles_collisions();
	game_update_helper4();
	game_execute_colliding_op();
	game_update_tile_offsets();
	game_update_score();
	game_update_life_bar();
	game_handle_level_change();
	game_draw_nicky();
	if (inventory_enabled_flag) {
		game_redraw_inventory();
	}
	sys_add_to_sprite_list(res_lifebar_spr, player_state.energy * 36 >> 6, 4, 4, SSF_VISIBLE, 3);
}

void game_init_player_state() {
	memset(&player_state, 0, sizeof(player_state_t));
	player_state.dim_w = 16;
	player_state.dim_h = 29;
	player_state.pos_dx_1 = 10;
	player_state.pos_dy_1 = 28;
	player_state.anim_frame_ptr = &nicky_anim_frames_table[19];
	player_state.hdir_flags = 1;
	player_state.vdir_flags = 0;
	player_state.pos_x = -16;
	player_state.pos_y = -16;
	player_state.lifes_count = 5;
	cur_anim_data_ptr = anim_data0;
	cur_anim_data_count = 3;
	nicky_move_offsets_ptr = &nicky_move_offsets_table[40];
}

void game_update_nicky_anim() {
	player_state.decor_ref_flag2 = 0;
	player_state.decor_ref_flag0 = 0;
	decor_state.unk24 = 0;
	if (update_nicky_anim_flag) {
		if (inp_direction_mask & GAME_DIR_UP) {
			if (player_state.vdir_flags == 3) {
				player_state.vdir_flags = 0;
			}
		} else {
			player_state.vdir_flags |= 1;
		}
		assert(inp_direction_mask < 11 && inp_direction_mask != 3 && inp_direction_mask != 7);
		(*update_nicky_anim_helper_table[inp_direction_mask])();
		if (player_state.hdir_flags & 8) {
			game_set_next_nicky_anim_frame();
			if (player_state.anim_frame_ptr->frames_count == 0) {
				player_state.hdir_flags &= ~8;
			}
		}
		if (player_state.decor_ref_flag2) {
			if (snd_sample_data_table[0].priority >= _snd_current_sfx_priority) {
				_snd_current_sfx_priority = snd_sample_data_table[0].priority;
				_snd_current_sfx_num = 0;
			}
		}
	} else if (player_state.dead_flag) {
		print_debug(DBG_GAME, "game_update_nicky_anim() player_state.dead_flag player_state.pos_y=%d", player_state.pos_y);
		if (player_state.pos_y >= 161) {
			if (quit_level_flag) {
				quit_level_flag = 2;
			} else {
				restart_level_flag = 1;
			}
		} else {
			uint16 dx = player_state.move_offsets_data_ptr[0];
			uint16 dy = player_state.move_offsets_data_ptr[1];
			if (dx == 0x8888) {
				player_state.pos_y = 161;
			} else {
				player_state.move_offsets_data_ptr += 2;
				player_state.pos_x += (int16)dx;
				player_state.pos_y += (int16)dy;
			}
		}
	} else if (player_state.action_code) {
		if (player_state.action_code == 1) {
			anim_frame_t *af = &nicky_anim_frames_table[40];
			if (player_state.hdir_flags & 1) {
				af = &nicky_anim_frames_table[46];
			}
			player_state.anim_frame_ptr = af;
			player_state.anim_frames_count = 2;
			player_state.action_code = 2;
		}
		if (player_state.action_code < 3) {
			if (player_state.anim_frame_ptr->unk1) {
				player_state.action_code = 3;
				execute_action_op_flag = 1;
			}
		}
		game_set_next_nicky_anim_frame();
	}
}

void game_adjust_player_position() {
	const int x_end = 20000 - (GAME_SCREEN_W / 16) * 50; /* 19000 */
	int x = player_state.tilemap_offset / 50;
	int y = player_state.tilemap_offset % 50;
	player_pos_x = x * 16;
	player_pos_y = y * 16 + 3;
	y -= 8;
	if (y < 0) {
		y = 0;
	} else if (y > 40) {
		y = 40;
	}
	x = x * 50 - 500;
	if (x < 0) {
		x = 0;
	} else if (x > x_end) {
		x = x_end;
	}
	if (monster_fight_flag) {
		y = 40;
		x = x_end;
	}
	_screen_tile_origin_x = x;
	_screen_tile_origin_y = y;
	print_debug(DBG_GAME, "game_adjust_player_position() offs=%d origin=%d,%d", player_state.tilemap_offset, _screen_tile_origin_x, _screen_tile_origin_y);
}

void game_handle_nicky_shield() {
	if (player_state.shield_duration > 0) {
		int i;
		int16 delta = 0;
		object_state_t *os = game_state.starshield_object_state;
		if (player_state.shield_duration <= 50) {
			os->map_pos_y = -1;
		}
		delta = os->map_pos_y;
		if (delta == 0) {
			goto next_shield_step;
		} else if (delta > 0) {
			if (os->distance_pos_x >= 30) {
				os->map_pos_y = 0;
			}
			goto next_shield_step;
		} else if (os->distance_pos_x > -1) {
			player_state.shield_duration = 24;
			goto next_shield_step;
		}
		
		player_state.shield_duration = 0;
		for (i = 0; i < 4; ++i) {
			os->displayed = 0;
			++os;
		}
		player_state.unk2A = 0;
		return;

next_shield_step:
		--player_state.shield_duration;
		for (i = 0; i < 4; ++i) {
			os->distance_pos_x += delta;
			os->distance_pos_y += delta;
			os->map_pos_x = (os->map_pos_x + 24) & 0x3FF;
			os->pos_x = ((int16)cos_table[os->map_pos_x >> 1] * os->distance_pos_x) >> 8;
			os->pos_x += player_state.pos_x + 4;
			os->pos_y = ((int16)sin_table[os->map_pos_x >> 1] * os->distance_pos_y) >> 8;
			os->pos_y += player_state.pos_y;
			--os->anim_data_cycles;
			if (os->anim_data_cycles <= 0) {
				anim_data_t *ad = os->anim_data1_ptr;
				os->anim_data_cycles = ad->cycles;
				os->anim_data1_ptr = ad->anim_data1_ptr;
			}
			++os;
		}
	}
}

void game_play_sound() {
	if (_snd_current_sfx_num >= 0) {
		snd_play_sfx(_snd_current_sfx_num);
	}
	_snd_current_sfx_num = -1;
	_snd_current_sfx_priority = 0;
}

void game_handle_objects6() {
	int i;
	object_state_t *os = objects_table_ptr6;
	game_draw_bomb_object();
	game_handle_objects5();
	/* draw apples */
	for (i = 0; i < 5; ++i) {
		if (os->displayed) {
			os->pos_y += (int16)read_uint16BE(os->move_data_ptr + 2) - decor_state.delta_y;
			os->move_data_ptr += 4;
			os->pos_x += os->distance_pos_x - decor_state.delta_x;
			if (os->pos_x >= -4 && os->pos_x <= GAME_SCREEN_W && os->pos_y >= -4 && os->pos_y <= GAME_SCREEN_H) {
				game_draw_object(os);
			} else {
				os->displayed = 0;
			}
		}
		++os;
	}
}

void game_draw_bomb_object() {
	object_state_t *os = objects_table_ptr3;
	if (os->displayed) {
		anim_data_t *ad = os->anim_data1_ptr;
		if (ad->cycles != 0 && --os->anim_data_cycles == 0) {
			if (ad->anim_data1_ptr == 0) {
				os->displayed = 0;
				return;
			}
			ad = ad->anim_data1_ptr;
			os->anim_data1_ptr = ad;
			os->anim_data_cycles = ad->cycles;
			if (ad->colliding_opcode == 2) {
				os->pos_x -= 8;
				os->pos_y -= 8;
				os->displayed = 2;
				snd_play_sfx(SND_SAMPLE_1);
			}
		}
		os->pos_x -= decor_state.delta_x;
		os->pos_y -= decor_state.delta_y;
		if (os->pos_x >= 0 && os->pos_x <= GAME_SCREEN_W && os->pos_y >= 0 && os->pos_y <= GAME_SCREEN_H) {
			game_draw_object(os);
		} else {
			os->displayed = 0;
		}
	}
}

void game_handle_objects5() {
	int i;
	object_state_t *os = objects_table_ptr5;
	for (i = 0; i < 2; ++i) {
		if (os->displayed) {
			os->pos_y += os->distance_pos_y - decor_state.delta_y;
			os->pos_x += os->distance_pos_x - decor_state.delta_x;
			if (os->pos_x >= -4 && os->pos_x <= GAME_SCREEN_W && os->pos_y >= -4 && os->pos_y <= GAME_SCREEN_H) {
				game_draw_object(os);
			} else {
				os->displayed = 0;
			}
		}
		++os;
	}
}

void game_draw_object(object_state_t *os) {
	anim_data_t *ad = os->anim_data1_ptr;
	if (ad->anim_h != 0) {
		const uint8 *spr = res_level_spr;
		if ((ad->sprite_flags & 0xF000) == 0x9000) {
			spr = res_monster_spr;
		}
		if (os->transparent_flag) {
			os->transparent_flag = 0;
			sys_add_to_sprite_list(spr, ad->sprite_num, os->pos_x, os->pos_y, SSF_HITMODE, 0);
		} else {
			sys_add_to_sprite_list(spr, ad->sprite_num, os->pos_x, os->pos_y, SSF_VISIBLE, 0);
		}
	}
}

void game_shake_screen() {
	/* sample_data19 */
/*	print_warning("game_shake_screen() UNIMPLEMENTED");*/
}

void game_update_decor_after_megabomb() {
	int j, offs;
	object_state_t *os;
	
	offs = decor_state.tile_block_x + decor_state.tile_block_y;
	for (j = 0; j < GAME_SCREEN_W / 16; ++j) {
		int i;
		for (i = 0; i < GAME_SCREEN_H / 16; ++i) {
			uint8 tile_num = res_decor_cdg[offs + i];
			if (res_decor_ref[tile_num * 8] & 0x10) {
				uint8 new_tile_num = res_decor_ref[tile_num * 8 + 6];
				if (new_tile_num != 0) {
					res_decor_cdg[offs + i] = new_tile_num;
				}
			}
		}
		offs += 50;
	}
	for (os = objects_list_cur; os <= objects_list_last; ++os) {
		anim_data_t *_bx = os->anim_data1_ptr;
		if (os->displayed != 2)
			continue;
		if ((os->unk26 & 4) == 0 || (os->unk26 & 8))
			continue;
		if (os->pos_y >= GAME_SCREEN_H || os->pos_y + _bx->bounding_box_y2 < 0)
			continue;
		if (os->pos_x >= GAME_SCREEN_W || os->pos_x + _bx->bounding_box_x2 < 0)
			continue;
		if (_bx->lifes == 0)
			continue;
		os->transparent_flag = 32;
		os->life -= 8;
		if (os->life > 0)
			continue;
		if (_bx->score != 0) {
			cycle_score += _bx->score;
			unk_ref_index = 0;
		}
		if (_bx->colliding_opcode == 18) {
			game_change_decor_tile_map_line(os, _bx);
		}
		if (_bx->anim_data3_ptr == 0) {
			os->displayed = (os->visible != 0) ? 1 : 0;
		} else {
			if (os->visible == 0) {
				os->anim_data2_ptr = 0;
			}
			os->transparent_flag = 0;
			game_init_object_from_anim3(os, _bx);
		}
	}
}

void game_draw_nicky() {
	int i;
	anim_frame_t *af;
	object_state_t *os;

	/* add nicky sprite */
	af = player_state.anim_frame_ptr;
	sys_add_to_sprite_list(res_nicky_spr, af->sprite_num, player_state.pos_x, player_state.pos_y, SSF_VISIBLE, 1);
	/* add starshield */
	os = game_state.starshield_object_state;
	for (i = 0; i < 4; ++i) {
		if (os->displayed) {
			anim_data_t *ad = os->anim_data1_ptr;
			sys_add_to_sprite_list(res_nicky_spr, ad->sprite_num, os->pos_x, os->pos_y, SSF_VISIBLE, 1);
		}
		++os;
	}
}

void game_translate_object_tables(int16 dx, int16 dy) {
	game_translate_object_table(objects_table_ptr1, 410, dx, dy);
	game_translate_object_table(objects_table_ptr2, 7, dx, dy);
	game_translate_object_table(game_state.starshield_object_state, 4, dx, dy);
	game_translate_object_table(objects_table_ptr8, 16, dx, dy);
	game_translate_object_table(objects_table_ptr3, 1, dx, dy);
	game_translate_object_table(objects_table_ptr4, 8, dx, dy);
	game_translate_object_table(objects_table_ptr5, 2, dx, dy);
	game_translate_object_table(objects_table_ptr6, 4, dx, dy);
	game_translate_object_table(objects_table_ptr7, 1, dx, dy);
}

void game_translate_object_table(object_state_t *os, int num, int16 dx, int16 dy) {
	int i;
	for (i = 0; i < num && (os->displayed & 0x80) == 0; ++i) {
		if (os->displayed) {
			os->pos_x += dx;
			os->pos_y += dy;
		}
		++os;
	}
}

void game_redraw_inventory() {
	game_draw_inventory_item(GAME_INV_MEGABOMBS, player_state.megabombs_count);
	game_draw_inventory_item(GAME_INV_BOMBS, player_state.bombs_count);
	game_draw_inventory_item(GAME_INV_KEYS, player_state.keys_count);
	game_draw_inventory_item(GAME_INV_LIFES, player_state.lifes_count);
}

uint8 game_set_decor_tile_flags4_3_2_0(uint8 b, int offset) {
	uint8 m;
	int ref_offset = res_decor_cdg[offset] * 8;
	if (res_decor_ref[ref_offset + 4] & 0xFF) {
		++player_state.decor_ref_flag4;
	}
	if (res_decor_ref[ref_offset + 3] & 0xFF) {
		++player_state.decor_ref_flag3;
	}
	if (res_decor_ref[ref_offset + 2] & 0xFF) {
		player_state.decor_ref_flag2 = 1;
		res_decor_cdg[offset] = res_decor_ref[ref_offset + 2];
	}
	m = res_decor_ref[ref_offset];
	if (b & m) {
		player_state.decor_ref_flag0 |= b;
	}
	return m;
}

void game_set_decor_tile_flag0(uint8 b, int offset) {
	int ref_offset = res_decor_cdg[offset] * 8;
	if (b & res_decor_ref[ref_offset]) {
		player_state.decor_ref_flag0 |= b;
	}
}

void game_set_next_nicky_anim_frame() {
	anim_frame_t *af = player_state.anim_frame_ptr;
	if (af->frames_count) {
		--player_state.anim_frames_count;
		if (player_state.anim_frames_count <= 0) {
			af = af->next_anim_frame;
			player_state.anim_frame_ptr = af;
			player_state.unk3C = 0;
			player_state.anim_frames_count = af->frames_count;
		}
	}
}

void game_disable_nicky_shield() {
	int i;
	object_state_t *os = game_state.starshield_object_state;
	anim_data_t *ad = &anim_data_starshield[0];
	for (i = 0; i < 4; ++i) {
		os->anim_data1_ptr = ad;
		os->displayed = 0;
		os->life = 0;
		os->anim_data_cycles = 0;
		os->unk3 = 0;
		os->move_data_ptr = 0;
		os->distance_pos_x = 0;
		os->distance_pos_y = 0;
		os->transparent_flag = 0;
		os->pos_x = -16;
		os->pos_y = -16;
		++os;
		++ad;
	}
}

void game_init_level_helper() {
	anim_data_t *ad = anim_data0;
	while (ad->unk0 >= 0) {
		ad->unk42 = ((ad->anim_w + 15) >> 4) * 2 * ad->anim_h;
		ad->unk10 = ad->sprite_num = ad->init_sprite_num;
		ad->unk12 = ad->sprite_flags = 0x8000;
		++ad;
	}
}

void game_init_level_helper2() {
	if (monster_fight_flag == 0) {
		update_nicky_anim_flag = 0;
		memset(&decor_state, 0, sizeof(decor_state));
		decor_state.tile_map_offs_x = _screen_tile_origin_x;
		_screen_tile_map_offs_x = _screen_tile_origin_x / 50;
		_screen_tile_map_offs_x_shl4 = _screen_tile_map_offs_x << 4;
		decor_state.tile_map_offs_y = _screen_tile_origin_y;
		_screen_tile_map_offs_y = _screen_tile_origin_y;
		_screen_tile_map_offs_y_shl4 = _screen_tile_map_offs_y << 4;
		decor_state.tile_map_end_offs_x = 20000 - (GAME_SCREEN_W / 16) * 50 + 50; /* 19050 */
		decor_state.tile_map_end_offs_y = 40;
		decor_state.tile_block_x = decor_state.tile_map_offs_x;
		decor_state.tile_block_y = decor_state.tile_map_offs_y;
		game_set_tile_offsets();
		game_update_cur_objects_ptr();
		game_reset_objects8();
	}
}

void game_init_level_start_screen() {
	game_init_level_helper2();
	if (player_state.dead_flag) {
		game_init_nicky();
	}
	player_state.pos_x = player_pos_x - _screen_tile_map_offs_x_shl4;
	player_state.pos_y = player_pos_y - _screen_tile_map_offs_y_shl4;
	player_state.pos_dx_2 = 0;
	player_state.pos_dy_2 = 0;
	player_state.anim_frame_ptr = &nicky_anim_frames_table[19];
	player_state.unk3C = 0;
	if ((player_state.hdir_flags & 1) == 0) {
		player_state.anim_frame_ptr = &nicky_anim_frames_table[23];
		player_state.unk3C = 0;
	}
	player_state.hdir_flags &= 1;
	if (monster_fight_flag == 0) {
		int i;
		sys_clear_sprite_list();
		game_handle_nicky_shield();
		for (i = 0; i < 2; ++i) {
			game_adjust_tilemap();
			game_execute_logic_op();
	/*		game_draw_nicky();
			sys_clear_sprite_list();
			game_sync();*/
		}
/*		sys_fade_in_palette();*/
	}
	++update_nicky_anim_flag;
}

void game_init_ref_ref(anim_data_t *ad, uint16 flags) {
	if ((ad->sprite_flags & 0xF000) == 0) {
		while (ad->unk0 >= 0) {
			ad->unk10 = ad->sprite_num;
			ad->sprite_flags |= flags;
			ad->unk12 = ad->sprite_flags;
			/* pointer conversions handled in load_ref_ref() */
			++ad;
		}
	}
}

void game_init_objects_from_positref(const uint8 *posit_ref, uint16 objects_offset) {
	object_state_t *os;
	objects_list_head = objects_table_ptr1 + objects_offset;
	os = objects_list_head;
	while (1) {
		anim_data_t *ad;
		uint16 num = read_uint16LE(posit_ref);
		if (num & 0x8000) {
			break;
		}
		ad = &res_ref_ref[num];
		assert(os < objects_table_ptr2);
		os->anim_data1_ptr = ad;
		os->anim_data2_ptr = ad;
		os->anim_data3_ptr = ad;
		os->displayed = ad->unk0;
		os->life = ad->lifes;
		os->anim_data_cycles = ad->cycles;
		os->unk3 = ad->unk3;
		os->move_data_ptr = ad->move_data_ptr;
		os->distance_pos_x = ad->unk26;
		os->distance_pos_y = ad->unk28;
		os->anim_data4_ptr = ad->anim_data2_ptr;
		os->unk26 = ad->unk4;
		os->unk27 = ad->unk5;
		os->map_pos_x = read_uint16LE(posit_ref + 2);
		os->map_pos_y = read_uint16LE(posit_ref + 4);
		os->ref_ref_index = read_uint16LE(posit_ref + 6);
		os->tile_num = posit_ref[9];
		os->visible = posit_ref[8];
		os->transparent_flag = 0;
		++os;
		posit_ref += 10;
	}
	os->displayed = 0xFF;
	os->map_pos_x = 30000;
	assert(os - objects_table <= 417);
}

void game_execute_logic_op_helper1(int x) {
	if (nicky_temp_pos3 <= objects_list_cur->map_pos_x) {
		if (objects_list_head != objects_list_cur) {
			--objects_list_cur;
		}
	}
}

void game_execute_logic_op_helper2(object_state_t *_di, anim_data_t *ad) {
	if (ad->anim_data4_ptr) {
		int i;
		anim_data_t *_bx = ad->anim_data4_ptr;
		anim_data_t *_bp = ad;
		object_state_t *_si = objects_table_ptr2;
		for (i = 0; i < 7; ++i) {
			if (_si->displayed == 0) {
				_di->unk3 = _bp->unk3;
				_si->anim_data1_ptr = _bx;
				_si->pos_x = _di->pos_x + _bp->unk3E;
				_si->pos_y = _di->pos_y + _bp->unk40;
				_si->displayed = _bx->unk0;
				_si->life = _bx->lifes;
				_si->distance_pos_x = _bx->unk26;
				_si->distance_pos_y = _bx->unk28;
				_si->move_data_ptr = _bx->move_data_ptr;
				_si->anim_data_cycles = _bx->cycles;
				_si->unk3 = _bx->unk3;
				_si->unk26 = _bx->unk4;
				_si->unk27 = _bx->unk5;
				game_play_anim_data_sfx(_bx);
				if ((_bx->unk4 & 0x10) == 0) {
					game_move_object(_si);
				}
				return;
			}
			++_si;
		}
	}
}

void game_execute_logic_op() {
	int16 _bp, _cx, _dx, _ax;
	object_state_t *os;
	
	nicky_temp_pos3 = _screen_tile_map_offs_x_shl4 - 280;
	_bp = _screen_tile_map_offs_x_shl4 + GAME_SCREEN_W + 280;
	_cx = _screen_tile_map_offs_y_shl4 - 140;
	_dx = _screen_tile_map_offs_y_shl4 + GAME_SCREEN_H + 150;
	game_execute_logic_op_helper1(_bp);
	os = objects_list_cur;
loop:
	if (os->displayed & 0x80) goto loc_1269A;
	if (os->displayed == 2) goto loc_12617;
	if (os >= objects_list_last) {
		if (_bp < os->map_pos_x) goto loc_1269A;
	} else {
		if (_bp < os->map_pos_x) goto next;
	}
	_ax = nicky_temp_pos3;
	if (_ax <= os->map_pos_x && _dx >= os->map_pos_y && _cx <= os->map_pos_y && os->displayed) {
		if (os->anim_data2_ptr == 0) {
			os->displayed = 0;
		} else {
			anim_data_t *ad = os->anim_data2_ptr;
			if (os > objects_list_last) {
				objects_list_last = os;
			}
			os->pos_y = os->map_pos_y - _screen_tile_map_offs_y_shl4 + decor_state.delta_y;
			_ax = os->map_pos_x - _screen_tile_map_offs_x_shl4 + decor_state.delta_x;
			if ((ad->unk4 & 0x80) || _ax > GAME_SCREEN_W || _ax + ad->anim_w <= 0 || os->pos_y > GAME_SCREEN_H || os->pos_y + ad->anim_h <= 0) {
				anim_data_t *_si;
				os->pos_x = _ax;
				os->anim_data1_ptr = ad;
				os->anim_data3_ptr = ad;
				os->anim_data4_ptr = ad->anim_data2_ptr;
				os->distance_pos_x = ad->unk26;
				os->move_data_ptr = ad->move_data_ptr;
				os->displayed = ad->unk0;
				os->life = ad->lifes;
				os->distance_pos_y = ad->unk28;
				os->anim_data_cycles = ad->cycles;
				os->unk3 = ad->unk3;
				os->displayed = 2;
				os->unk26 = ad->unk4;
				os->unk27 = ad->unk5;
				os->transparent_flag = 0;
loc_12617:
				_si = os->anim_data1_ptr;
				assert(_si->logic_opcode < NUM_LOGIC_OPCODES);
				_si = (*logic_op_table[_si->logic_opcode])(os, _si);
				if (os->displayed == 0) {
					os->displayed = (os->visible != 0) ? 1 : 0;
				} else {
					if (_si->unk3 != 0 && --os->unk3 <= 0) {
						os->unk3 = 1;
						if ((os->unk26 & 0x20) || (os->pos_x <= GAME_SCREEN_W && os->pos_x >= 0 && os->pos_y <= GAME_SCREEN_H && os->pos_y >= 0)) {
							game_execute_logic_op_helper2(os, _si);
						}
					}
					if (os->pos_x >= -280 && os->pos_x <= GAME_SCREEN_W + 280 && os->pos_y >= -140 && os->pos_y <= GAME_SCREEN_H + 150) {
						game_draw_object(os);
					} else {
						os->displayed = 1;
					}
				}
			}
		}
	}
next:
	++os;
	goto loop;

loc_1269A:
	if (os <= objects_list_last && os != objects_list_head) {
		--objects_list_last;
	}
}

void game_update_nicky_anim_helper0() {
	int16 _bx, _cx, _dx;
	int16 _bp = 0;
	
	if (player_state.unk1 == 2) { /* jumping */
		_bp = player_state.move_offsets_data_ptr[0];
		if (_bp < 0) goto loc_119D5;
		if (_bp == 0x7777) {
			_bp = 5;
		} else {
			player_state.hdir_flags |= 4;
			player_state.move_offsets_data_ptr++;
		}
	}
	decor_state.unk24 = _bp;
	nicky_temp_pos = player_state.pos_x;
	_cx = (player_state.pos_x + 4 + player_state.pos_dx_2) >> 4;
	_cx = (_cx * 50) + decor_state.tile_map_offs_x;
	player_state.pos_y += _bp;
	_dx = player_state.pos_y + player_state.pos_dy_2;
/*	print_debug(DBG_GAME, "game_update_nicky_anim_helper0() player_state.unk1=%d _bp=%d pos=%d,%d (dx2=%d,dy2=%d)", player_state.unk1, _bp, player_state.pos_x, player_state.pos_y, player_state.pos_dx_2, player_state.pos_dy_2);*/
	if (_dx > 0 && (_dx + player_state.dim_h) < 167) {
		nicky_temp_pos3 = _dx + player_state.dim_h;
		_dx = (nicky_temp_pos3 >> 4) + decor_state.tile_map_offs_y;
		_cx += _dx;
		player_state.decor_ref_flag0 &= ~1;
		player_state.decor_ref_flag3 = 0;
		player_state.decor_ref_flag4 = 0;
		game_set_decor_tile_flags4_3_2_0(1, _cx);
		game_set_decor_tile_flag0(4, _cx);
		_cx = (nicky_temp_pos + player_state.pos_dx_1 + player_state.pos_dx_2) >> 4;
		_cx = (_cx * 50) + decor_state.tile_map_offs_x + _dx;
		game_set_decor_tile_flag0(8, _cx);
		if (game_set_decor_tile_flags4_3_2_0(1, _cx) & 0x40) {
			player_state.unk38 = _cx;
		}
		if (player_state.decor_ref_flag4 == 2) {
			player_state.unk2A = 1;
		}
		if (player_state.decor_ref_flag3 == 2) {
			goto loc_1194B;
		}
		if ((player_state.decor_ref_flag0 & 1) == 0) {
			if (player_state.decor_ref_flag3 == 1) {
				goto loc_11959;
			} else {
				goto loc_1191B;
			}
		}
		if ((player_state.decor_ref_flag0 & 0xC) == 0) {
			nicky_temp_pos3 &= 0xF;
			if (nicky_temp_pos3 > nicky_temp_pos2) {
				goto loc_1191B;
			}
		}
		_bx = (player_state.pos_y + player_state.pos_dy_2 + player_state.dim_h) & ~0xF;
		_bx -= player_state.dim_h;
		_bx -= player_state.pos_dy_2;
		player_state.pos_y = _bx;
		goto loc_11959;
	}
loc_1191B:
	player_state.hdir_flags &= 0x8D;
	if (player_state.hdir_flags & 8) {
		return;
	}
	/* falling */
	if (player_state.hdir_flags & 1) {
		player_state.anim_frame_ptr = &nicky_anim_frames_table[24];
	} else {
		player_state.anim_frame_ptr = &nicky_anim_frames_table[25];
	}
	if (player_state.unk1 != 2) {
		player_state.unk1 = 2;
		player_state.move_offsets_data_ptr = &nicky_move_offsets_table[50];
		player_state.hdir_flags |= 4;
	}
	return;

loc_1194B:
	if ((inp_direction_mask & 0xC) && (player_state.hdir_flags & 0x30)) {
		goto loc_119C8;
	}
	
loc_11959:
	player_state.unk1 = 1;
	player_state.hdir_flags &= 0xFB;
	if (player_state.hdir_flags & 1) {
		if (inp_direction_mask & GAME_DIR_DOWN) {
			if ((player_state.hdir_flags & 0x40) == 0) {
				player_state.anim_frame_ptr = &nicky_anim_frames_table[34];
				player_state.anim_frames_count = 2;
				player_state.hdir_flags |= 0x40;
			}
		} else {
			if (inp_direction_mask & GAME_DIR_RIGHT) {
				goto loc_119C8;
			}
			player_state.hdir_flags &= 0xBF;
			player_state.anim_frame_ptr = &nicky_anim_frames_table[19];
		}
	} else {
		if (inp_direction_mask & GAME_DIR_DOWN) {
			if ((player_state.hdir_flags & 0x40) == 0) {
				player_state.anim_frame_ptr = &nicky_anim_frames_table[37];
				player_state.anim_frames_count = 2;
				player_state.hdir_flags |= 0x40;
			}
		} else {
			if (inp_direction_mask & GAME_DIR_LEFT) {
				goto loc_119C8;
			}
			player_state.hdir_flags &= 0xBF;
			player_state.anim_frame_ptr = &nicky_anim_frames_table[23];
		}
	}

	game_set_next_nicky_anim_frame();
	player_state.hdir_flags &= 0xC1;

loc_119C8:
	player_state.vdir_flags |= 2;
	player_state.unk1 = 0;
	player_state.hdir_flags &= 0xFB;
	return;

loc_119D5:
	player_state.move_offsets_data_ptr++;
	decor_state.unk24 = _bp;
	player_state.hdir_flags &= 0xFB;
	nicky_temp_pos = player_state.pos_x;
	_cx = (player_state.pos_x + 4 + player_state.pos_dx_2) >> 4;
	_cx = (_cx * 50) + decor_state.tile_map_offs_x;
	player_state.pos_y += _bp;
	
	_dx = player_state.pos_y + 4 + player_state.pos_dy_2;
	if (_dx <= 0) goto loc_11A5C;
	if (_dx > 136) goto loc_11A5C;
	_dx = (_dx >> 4) + decor_state.tile_map_offs_y;
	_cx += _dx;
	player_state.decor_ref_flag0 &= ~2;
	game_set_decor_tile_flags4_3_2_0(2, _cx);
	_cx = (nicky_temp_pos + player_state.pos_dx_1 + player_state.pos_dx_2) >> 4;
	_cx = (_cx * 50) + decor_state.tile_map_offs_x + _dx;
	game_set_decor_tile_flags4_3_2_0(2, _cx);
	if (player_state.decor_ref_flag0 & 2) {
loc_11A5C:
		player_state.hdir_flags |= 4;
		player_state.move_offsets_data_ptr = &nicky_move_offsets_table[50];
	}
	player_state.hdir_flags &= 0x8D;
	if (player_state.hdir_flags & 8) {
		return;
	}
	/* jumping */
	if (player_state.hdir_flags & 1) {
		player_state.anim_frame_ptr = &nicky_anim_frames_table[24];
	} else {
		player_state.anim_frame_ptr = &nicky_anim_frames_table[25];
	}
}

void game_update_nicky_anim_helper1() {
	int16 _cx, _dx;
	_cx = player_state.pos_x + player_state.dim_w + player_state.pos_dx_2;
	_cx = (_cx >> 4) * 50 + decor_state.tile_map_offs_x;
	nicky_temp_pos = _cx;
	_dx = player_state.pos_y + player_state.pos_dy_1 + player_state.pos_dy_2;
	if (_dx <= GAME_SCREEN_H) {
		_dx = (_dx >> 4) + decor_state.tile_map_offs_y;
		_cx += _dx;
		player_state.decor_ref_flag0 &= ~4;
		game_set_decor_tile_flags4_3_2_0(4, _cx);
		if ((player_state.decor_ref_flag0 & 4) == 0) {
			_cx = nicky_temp_pos;
			_dx = player_state.pos_y + 12 + player_state.pos_dy_2;
			_dx = (_dx >> 4) + decor_state.tile_map_offs_y;
			_cx += _dx;
			game_set_decor_tile_flags4_3_2_0(4, _cx);
			if ((player_state.decor_ref_flag0 & 4) == 0 && player_state.pos_x < GAME_SCREEN_W - 16) {
				player_state.pos_x += GAME_SCROLL_DX;
/*				player_state.pos_x += 1;*/
			}
		}
		if ((player_state.hdir_flags & 1) == 0) {
			if (player_state.hdir_flags <= 7) {
				player_state.anim_frame_ptr = &nicky_anim_frames_table[16];
				player_state.hdir_flags |= 9;
				player_state.hdir_flags &= 0xBD;
				player_state.anim_frames_count = 2;
			}
		} else {
			if ((player_state.hdir_flags & 0xC) == 0) {
				if (player_state.hdir_flags <= 0xF || (inp_direction_mask & 0xC) == 0) {
					if ((player_state.hdir_flags & 2) == 0) {
						player_state.anim_frame_ptr = &nicky_anim_frames_table[0];
						player_state.hdir_flags |= 2;
						player_state.hdir_flags &= 0x8F;
						player_state.anim_frames_count = 2;
					}
					game_set_next_nicky_anim_frame();
				}
			}
		}
	}
	game_update_nicky_anim_helper0();
}

void game_update_nicky_anim_helper2() {
	int16 _cx, _dx;
	_cx = (player_state.pos_x + player_state.pos_dx_2) >> 4;
	_cx = (_cx * 50) + decor_state.tile_map_offs_x;
	nicky_temp_pos = _cx;
	_dx = player_state.pos_y + player_state.pos_dy_1 + player_state.pos_dy_2;
	if (_dx <= GAME_SCREEN_H) {
		_dx = (_dx >> 4) + decor_state.tile_map_offs_y;
		_cx += _dx;
		player_state.decor_ref_flag0 &= ~8;
		game_set_decor_tile_flags4_3_2_0(8, _cx);
		if ((player_state.decor_ref_flag0 & 8) == 0) {
			_cx = nicky_temp_pos;
			_dx = player_state.pos_y + 12 + player_state.pos_dy_2;
			_dx = (_dx >> 4) + decor_state.tile_map_offs_y;
			_cx += _dx;
			game_set_decor_tile_flags4_3_2_0(8, _cx);
			if ((player_state.decor_ref_flag0 & 8) == 0 && player_state.pos_x > 0) {
				player_state.pos_x -= GAME_SCROLL_DX;
/*				player_state.pos_x -= 1;*/
			}
		}
		if (player_state.hdir_flags & 1) {
			if (player_state.hdir_flags <= 7) {
				player_state.anim_frame_ptr = &nicky_anim_frames_table[20];
				player_state.hdir_flags |= 8;
				player_state.hdir_flags &= 0xBC;
				player_state.anim_frames_count = 2;
			}
		} else {
			if ((player_state.hdir_flags & 0x8C) == 0) {
				if (player_state.hdir_flags <= 0xF || (inp_direction_mask & 0xC) == 0) {
					if ((player_state.hdir_flags & 2) == 0) {
						player_state.anim_frame_ptr = &nicky_anim_frames_table[8];
						player_state.hdir_flags |= 2;
						player_state.hdir_flags &= 0x8F;
						player_state.anim_frames_count = 2;
					}
					game_set_next_nicky_anim_frame();
				}
			}
		}
	}
	game_update_nicky_anim_helper0();
}

void game_update_nicky_anim_helper3() {
	game_update_nicky_anim_helper8();
	game_update_nicky_anim_helper0();
}

void game_update_nicky_anim_helper4() {
	game_update_nicky_anim_helper8();
	game_update_nicky_anim_helper1();
}

void game_update_nicky_anim_helper5() {
	game_update_nicky_anim_helper8();
	game_update_nicky_anim_helper2();
}

void game_update_nicky_anim_helper6() {
	game_update_nicky_anim_helper7();
	game_update_nicky_anim_helper0();
}

void game_update_nicky_anim_helper7() {
	int16 _cx, _dx;
	
	nicky_temp_pos = player_state.pos_x;
	_cx = (player_state.pos_x + 4 + player_state.pos_dx_2) >> 4;
	_cx = _cx * 50 + decor_state.tile_map_offs_x;
	
	_dx = player_state.pos_y + player_state.dim_h + player_state.pos_dy_2;
	if (_dx > GAME_SCREEN_H) return;
	
	_dx = (_dx >> 4) + decor_state.tile_map_offs_y;
	_cx += _dx;

	player_state.decor_ref_flag0 = 0;
	player_state.decor_ref_flag3 = 0;
	game_set_decor_tile_flags4_3_2_0(1, _cx);
	_cx = (nicky_temp_pos + player_state.pos_dx_1 + player_state.pos_dx_2) >> 4;
	_cx = _cx * 50 + decor_state.tile_map_offs_x + _dx;
	game_set_decor_tile_flags4_3_2_0(1, _cx);
	if (player_state.decor_ref_flag0 & 1) {
		player_state.hdir_flags &= ~0x30;
		return;
	}
	if (player_state.decor_ref_flag3 != 2) {
		if (player_state.decor_ref_flag3 == 0) {
			player_state.hdir_flags &= ~0x30;
		}
		return;
	}
	if (player_state.pos_y <= 161) {
		player_state.pos_y += GAME_SCROLL_DY;
	}
	if ((player_state.hdir_flags & 0x20) == 0) {
		player_state.anim_frame_ptr = &nicky_anim_frames_table[30];
		player_state.hdir_flags |= 0x20;
		player_state.hdir_flags &= 0xA1;
		player_state.unk1 = 0;
		player_state.anim_frames_count = 2;
	}
	game_set_next_nicky_anim_frame();
}

void game_update_nicky_anim_helper8() {
	int16 _cx, _dx;
	nicky_temp_pos = player_state.pos_x;
	_cx = (player_state.pos_x + 4 + player_state.pos_dx_2) >> 4;
	_cx = _cx * 50 + decor_state.tile_map_offs_x;
	nicky_temp_pos2 = _cx;
	_dx = player_state.pos_y + player_state.pos_dy_1 + player_state.pos_dy_2;
	if (_dx > 0 && _dx <= GAME_SCREEN_H) {
		_dx = (_dx >> 4) + decor_state.tile_map_offs_y;
		_cx += _dx;
		player_state.decor_ref_flag0 = 0;
		player_state.decor_ref_flag3 = 0;
		game_set_decor_tile_flags4_3_2_0(2, _cx);
		_cx = (nicky_temp_pos + player_state.pos_dx_1 + player_state.pos_dx_2) >> 4;
		_cx = _cx * 50 + decor_state.tile_map_offs_x + _dx;
		game_set_decor_tile_flags4_3_2_0(2, _cx);
		if (player_state.decor_ref_flag0 & 2) {
			player_state.hdir_flags &= ~0x30;
			return;
		}
		if (player_state.decor_ref_flag3 != 2) {
			if (player_state.decor_ref_flag3 == 0) {
				goto loc_1159B;
			}
			player_state.hdir_flags &= ~0x30;
			return;
		}
		if (player_state.pos_y > 0) {
			_cx = nicky_temp_pos2;
			_dx = (player_state.pos_y + 4 + player_state.pos_dy_2) >> 4;
			_dx += decor_state.tile_map_offs_y;
			_cx += _dx;
			game_set_decor_tile_flags4_3_2_0(2, _cx);
			if (player_state.decor_ref_flag0 & 2) {
				player_state.hdir_flags &= ~0x30;
				return;
			}
			player_state.pos_y -= GAME_SCROLL_DY;
		}
		if ((player_state.hdir_flags & 0x10) == 0) {
			player_state.anim_frame_ptr = &nicky_anim_frames_table[26];
			player_state.hdir_flags |= 0x10;
			player_state.hdir_flags &= 0x91;
			player_state.anim_frames_count = 2;
		}
		game_set_next_nicky_anim_frame();
		return;
	}
loc_1159B:
	player_state.hdir_flags &= ~0x30;
	if (player_state.vdir_flags == 0 && player_state.unk1 == 0) {
		player_state.unk1 = 2;
		player_state.move_offsets_data_ptr = nicky_move_offsets_ptr;
	}
}

/* used to display stairs */
void game_change_decor_tile_map_line(const object_state_t *os, const anim_data_t *ad) {
	int i;
	int offs = ((os->map_pos_x + ad->anim_w / 2) >> 4) * 50 + ((os->map_pos_y + ad->anim_h - 8) >> 4);
	for (i = 0; i < 10; ++i) {
		res_decor_cdg[offs - i] = os->ref_ref_index;
	}
}

/* used to restore the decor after using a bomb / a door */
void game_change_decor_tile_map_quad(const object_state_t *os) {
	int offs = (os->map_pos_x >> 4) * 50 + (os->map_pos_y >> 4);
	if ((os->tile_num & 0x80) == 0) {
		static const int offsets_table[] = { 0, 1, 50, 51 };
		int i;
		for (i = 0; i < 4; ++i) {
			res_decor_cdg[offs + offsets_table[i]] = tile_map_quad_table[os->tile_num * 4 + i];
		}
	}
}

void game_change_decor_tile_map(const object_state_t *os) {
	int offs = (os->map_pos_x >> 4) * 50 + (os->map_pos_y >> 4);
	res_decor_cdg[offs] = os->tile_num;
}

void game_update_score() {
	if (cycle_score != 0) {
		game_state.extra_life_score += cycle_score;
		if (game_state.extra_life_score > 200000) {
			if (player_state.lifes_count < 9) {
				++player_state.lifes_count;
			}
			game_state.extra_life_score -= 200000;
			snd_play_sfx(SND_SAMPLE_15);
		}
		game_state.score += cycle_score;
		if (game_state.score > 9999999) {
			game_state.score = 9999999;
		}
	}
	cycle_score = 0;
}

void loc_11EDE(object_state_t *os, anim_data_t *ad) {
	os->distance_pos_x = (player_state.hdir_flags & 1) ? ad->unk26 : -ad->unk26;
	os->move_data_ptr = ad->move_data_ptr;
	os->anim_data1_ptr = ad;
	os->displayed = 1;
	os->pos_x = player_state.pos_x + player_state.anim_frame_ptr->pos_x;
	os->pos_y = player_state.pos_y + player_state.anim_frame_ptr->pos_y;
}

void game_throw_apple() {
	if (update_nicky_anim_flag) {
		game_throw_apple_helper1();
		game_throw_apple_helper2();
		if (inp_fire_button == 0) {
			inp_fire_button_num_cycles_pressed = 0;
			goto loc_11E99;
		}
		if (inp_fire_button_num_cycles_pressed <= 20) {
			++inp_fire_button_num_cycles_pressed;
			goto loc_11E99;
		}
		if (player_state.hdir_flags & 0x40) {
			anim_data_t *ad = &anim_data0[5]; /* _bx */
			object_state_t *os = objects_table_ptr3; /* _di */
			if (player_state.bombs_count == 0 || os->displayed) goto throw_apple;
			inp_fire_button_num_cycles_pressed = 0;
			os->anim_data1_ptr = ad;
			os->displayed = ad->unk0;
			os->life = ad->lifes;
			os->anim_data_cycles = ad->cycles;
			os->unk3 = ad->unk3;
			os->unk26 = ad->unk4;
			os->unk27 = ad->unk5;
			os->pos_x = player_state.pos_x + player_state.anim_frame_ptr->unk18;
			os->pos_y = player_state.pos_y + player_state.anim_frame_ptr->unk1A;
			--player_state.bombs_count;
			return;
		} else {
			if ((player_state.hdir_flags & 0x7E) == 0 && player_state.unk1 == 0 && player_state.megabombs_count != 0 && player_state.action_code == 0) {
				update_nicky_anim_flag = 0;
				player_state.action_code = 1;
			}
			inp_fire_button_num_cycles_pressed = 0;
			return;
		}
	}
throw_apple:
	inp_fire_button_num_cycles_pressed = 0;
loc_11E99:
	if (player_state.unk1B) {
		--player_state.unk1B;
		return;
	}
	if (inp_fire_button == 0) {
		throw_apple_var = 1;
		return;
	}
	if (throw_apple_var) {
		int i;
		object_state_t *os = objects_table_ptr6;
		anim_data_t *ad = cur_anim_data_ptr;
		throw_apple_var = 0;
		for (i = 0; i < cur_anim_data_count; ++i) {
			if (os->displayed == 0 && (player_state.hdir_flags & 0x30) == 0) {
				player_state.unk1B = 3;
				loc_11EDE(os, ad);
				break;
			}
			++os;
		}
	}
}

void game_adjust_objects_position() {
	int _cx, _bx, _ax = 0;
/*	update_helper6_var = 0;*/
		
	if (player_state.dead_flag) goto loc_11CDA;
	
	/* X coord */
/*	if (monster_fight_flag) goto loc_11C1F;*/
	if (player_state.pos_x <= 148 - 50) {
		const int16 x_start = monster_fight_flag ? 19000 : 0;
		if (decor_state.tile_map_offs_x <= x_start) goto loc_11B90;
		game_translate_object_tables(GAME_SCROLL_DX, 0);
		player_state.pos_x += GAME_SCROLL_DX;
		decor_state.unk18 += GAME_SCROLL_DX;
		if (decor_state.unk18 >= 16) {
			decor_state.tile_map_offs_x -= 50;
		}
	}
	if (player_state.pos_x >= 188 - 50) {
		if (decor_state.tile_map_offs_x >= decor_state.tile_map_end_offs_x) goto loc_11B90;
		_cx = decor_state.tile_map_offs_x + 50;
		_ax = GAME_SCROLL_DX;
		if (_cx >= decor_state.tile_map_end_offs_x && GAME_SCROLL_DX > decor_state.unk18) {
			_ax = decor_state.unk18;
		}
		game_translate_object_tables(-_ax, 0);
		player_state.pos_x -= _ax;
		decor_state.unk18 -= _ax;
		if (decor_state.unk18 < 0) {
			decor_state.tile_map_offs_x += 50;
		}
	}
loc_11B90:
	decor_state.unk18 &= 0xF;
	_ax = _bx = decor_state.unk18;
	player_state.pos_dx_2 = -_bx;
	_ax = 0;
/*loc_11C1F:*/

	/* Y coord */
	if (monster_fight_flag) goto loc_11CDA;
	_ax = 0;
	_bx = decor_state.tile_map_offs_y;
	if (player_state.pos_y <= 64) {
		if (_bx <= 0) goto loc_11CDA;
		_ax = -GAME_SCROLL_DY;
		if (decor_state.unk24 < 0) {
			_ax = decor_state.unk24;
		}
		if (_bx <= 1) {
			_cx = _ax;
			_ax = -_ax + decor_state.unk1A;
			if (_ax > 16) {
				_cx = decor_state.unk1A - 16;
			}
			_ax = _cx;
		}
		game_translate_object_tables(0, -_ax);
		player_state.pos_y -= _ax;
		decor_state.unk1A -= _ax;
		if (decor_state.unk1A >= 16) {
			--decor_state.tile_map_offs_y;
		}
	}
/*loc_11C94:*/
	if (player_state.pos_y >= 87) {
		--_bx;
		if (_bx >= decor_state.tile_map_end_offs_y) goto loc_11CDA;
		_ax = GAME_SCROLL_DY;
		_cx = decor_state.unk24;
		if (_cx > 0) {
			_ax = _cx;
		}
		_cx = _bx + 1;
		if (_cx >= decor_state.tile_map_end_offs_y && _ax > decor_state.unk1A) {
			_ax = decor_state.unk1A;
		}
		game_translate_object_tables(0, -_ax);
		player_state.pos_y -= _ax;
		decor_state.unk1A -= _ax;
		if (decor_state.unk1A < 0) {
			++decor_state.tile_map_offs_y;
		}
	}

loc_11CDA:
	decor_state.unk1A &= 0xF;
	_bx = _ax = decor_state.unk1A;
	player_state.pos_dy_2 = -_bx;

	decor_state.unk20 = player_state.pos_dx_2;
	decor_state.unk22 = player_state.pos_dy_2;
	decor_state.tile_block_x = decor_state.tile_map_offs_x;
	decor_state.tile_block_y = decor_state.tile_map_offs_y;
}

void game_adjust_tilemap() {
	_screen_tile_map_offs_y_shl4 = (decor_state.tile_map_offs_y << 4) - decor_state.unk1A;
	_screen_tile_map_offs_x_shl4 = ((decor_state.tile_map_offs_x / 50) << 4) - decor_state.unk18;
/*	print_debug(DBG_GAME, "game_adjust_tilemap() map_offs_y_shl4=%d map_offs_x_shl4=%d", _screen_tile_map_offs_y_shl4, _screen_tile_map_offs_x_shl4);*/
	sys_set_tilemap_origin(_screen_tile_map_offs_x_shl4, _screen_tile_map_offs_y_shl4);
	if (monster_fight_flag < 2) {
		if (current_level & 1) {
			if (_screen_tile_map_offs_x_shl4 >= (380 << 4) && _screen_tile_map_offs_y_shl4 >= (40 << 4)) {
				++monster_fight_flag;
				print_debug(DBG_GAME, "game_setup_monster() monster_fight_flag=%d", monster_fight_flag);
				if (monster_fight_flag >= 2) {
					game_setup_monster();
				}
			}
		}
	}
}

void game_handle_level_change() {
	if (restart_level_flag) {
		print_debug(DBG_GAME, "game_handle_level_change() restart_level_flag=%d", restart_level_flag);
		restart_level_flag = 0;
		game_init_level_start_screen();
	} else {
		/* tiles blinking */
		if (player_state.tile_blinking_duration > 0) {
			uint16 _cx, _ax;
			--player_state.tile_blinking_duration;
			++_screen_blinking_tile.anim;
			if (_screen_blinking_tile.anim > 1) {
				_screen_blinking_tile.anim = 0;
			}
			_cx = _screen_blinking_tile.tile_num;
			_ax = _screen_blinking_tile.anim;
			_screen_cdg_offsets[_cx] = _screen_blinking_tile.tiles_table[_ax];
			if (player_state.tile_blinking_duration == 0) {
				_screen_cdg_offsets[_cx] = _screen_blinking_tile.tiles_table[0];
			}
		}
		if (player_state.has_spring > 0) {
			--player_state.has_spring;
			if (player_state.has_spring == 0) {
				nicky_move_offsets_ptr = &nicky_move_offsets_table[40];
			}
		}
		if (player_state.has_wood > 0) {
			--player_state.has_wood;
		}
		if (player_state.has_red_ball > 0) {
			--player_state.has_red_ball;
		}
		if (player_state.action_code >= 5 || change_pal_flag) {
			change_pal_flag ^= 1;
			if (change_pal_flag) {
				sys_set_palette_spr(res_flash_pal, 16, 0);
			} else {
				sys_set_palette_spr(res_decor_pal, 16, 0);
			}
		}
		if (game_state.unk12 >= 60) {
			++game_state.unk12;
			if (game_state.unk12 >= 200) {
				new_level_flag = 1;
			}
		}
	}
}

void game_update_decor() {
	int i;
	object_state_t *os = objects_table_ptr5;
	for (i = 0; i < 2; ++i) {
		if (os->displayed) {
			game_update_decor_helper(os);
		} else {
			--update_decor_counter;
		}
		++os;
	}
}

void game_update_decor_helper(object_state_t *os) {
	if (os->life <= 0) {
		os->displayed = 0;
		--update_decor_counter;
	} else {
		anim_data_t *ad = os->anim_data1_ptr;
		nicky_temp_pos2 = (ad->anim_h >> 1) + os->pos_y + decor_state.unk22;
		nicky_temp_pos  = (ad->anim_w >> 1) + os->pos_x + decor_state.unk20;
		if (os->distance_pos_y >= 0) {
			if (os->distance_pos_x >= 0) {
				if (game_get_decor_tile_ref(nicky_temp_pos, nicky_temp_pos2) & 0x10) {
					if (game_get_decor_tile_ref(nicky_temp_pos + 6, nicky_temp_pos2 - 5) & 0x10) {
						if (game_get_decor_tile_ref(nicky_temp_pos - 6, nicky_temp_pos2 + 5) & 0x10) {
							os->distance_pos_y = -os->distance_pos_y;
						}
						os->distance_pos_x = -os->distance_pos_x;
						--os->life;
					} else {
						os->distance_pos_y = -os->distance_pos_y;
						--os->life;
					}
				}
			} else {
				if (game_get_decor_tile_ref(nicky_temp_pos, nicky_temp_pos2) & 0x10) {
					if (game_get_decor_tile_ref(nicky_temp_pos - 6, nicky_temp_pos2 - 5) & 0x10) {
						if (game_get_decor_tile_ref(nicky_temp_pos + 6, nicky_temp_pos2 + 5) & 0x10) {
							os->distance_pos_y = -os->distance_pos_y;
						}
						os->distance_pos_x = -os->distance_pos_x;
						--os->life;
						
					} else {
						os->distance_pos_x = -os->distance_pos_x;
						--os->life;
					}
				}
			}
		} else {
			if (os->distance_pos_x >= 0) {
				if (game_get_decor_tile_ref(nicky_temp_pos, nicky_temp_pos2) & 0x10) {
					if (game_get_decor_tile_ref(nicky_temp_pos + 6, nicky_temp_pos2 + 5) & 0x10) {
						if (game_get_decor_tile_ref(nicky_temp_pos - 6, nicky_temp_pos2 - 5) & 0x10) {
							os->distance_pos_y = -os->distance_pos_y;
						}
						os->distance_pos_x = -os->distance_pos_x;
						--os->life;
					} else {
						os->distance_pos_y = -os->distance_pos_y;
						--os->life;
					}
					
				}
			} else {
				if (game_get_decor_tile_ref(nicky_temp_pos - 6, nicky_temp_pos2 + 5) & 0x10) {
					if (game_get_decor_tile_ref(nicky_temp_pos + 6, nicky_temp_pos2 - 5) & 4) {
						os->distance_pos_y = -os->distance_pos_y;
					}
					os->distance_pos_x = -os->distance_pos_x;
					--os->life;
				} else {
					os->distance_pos_y = -os->distance_pos_y;
					--os->life;
				}
			}
		}
	}
}

uint8 game_get_decor_tile_ref(int x, int y) {
	int offs = (x >> 4) * 50 + decor_state.tile_block_x + (y >> 4) + decor_state.tile_block_y;
	uint8 tile_num = res_decor_cdg[offs];
	return res_decor_ref[tile_num * 8];
}

int game_projectile_collides(object_state_t *os) {
	int i;
	anim_data_t *ad;
	object_state_t *_si;
	
	if ((os->unk26 & 1) == 0) return 1;
	ad = os->anim_data1_ptr;
	if (os->pos_y >= GAME_SCREEN_H || os->pos_y + ad->bounding_box_y2 < 0) return 1;
	if (os->pos_x >= GAME_SCREEN_W || os->pos_x + ad->bounding_box_x2 < 0) return 1;
	bounding_box_x1 = os->pos_x + ad->bounding_box_x1;
	bounding_box_x2 = os->pos_x + ad->bounding_box_x2;
	bounding_box_y1 = os->pos_y + ad->bounding_box_y1;
	bounding_box_y2 = os->pos_y + ad->bounding_box_y2;
	update_decor_counter = 7;
	_si = objects_table_ptr5;
	for (i = 0; i < 7; ++i, ++_si) {
		if (_si->displayed == 0) {
			--update_decor_counter;
			if (update_decor_counter == 0) {
				return 0;
			}
		} else {			
			int _bp = _si->pos_x;
			int _dx = _si->pos_y;
			anim_data_t *ad2 = os->anim_data1_ptr;
			if (_dx + ad2->bounding_box_y1 > bounding_box_y2 || _dx + ad2->bounding_box_y2 < bounding_box_y1) {
				continue;
			}
			if (_bp + ad2->bounding_box_x1 > bounding_box_x2 || _bp + ad2->bounding_box_x2 < bounding_box_x1) {
				continue;
			}
			_si->displayed = 0;
			if (ad->lifes == 0) {
				return 0;
			}
			if (ad->colliding_opcode == 17) {
				game_handle_projectiles_collisions_helper3(os, ad);
			}
			os->transparent_flag ^= 1;
			--os->life;
			print_debug(DBG_GAME, "life counter = %d", os->life);
			if (os->life > 0) {
				return 0;
			}
			if (ad->colliding_opcode != 17) {
				unk_ref_index = 0;
			}
			cycle_score += ad->score;
			if (ad->anim_data3_ptr == 0) {
				os->displayed = (os->visible != 0) ? 1 : 0;
				return 0;
			} else {
				if (os->visible == 0) {
					os->anim_data2_ptr = 0;
				}
				os->transparent_flag = 0;
				game_play_anim_data_sfx(game_init_object_from_anim3(os, ad));
			}
		}
	}
	return 1;
}

void game_handle_projectiles_collisions() {
	object_state_t *os;
	
	update_decor_counter = 7;
	game_update_decor();
	game_handle_projectiles_collisions_helper2(objects_table_ptr6, 1, 4);
	game_handle_projectiles_collisions_helper1(objects_table_ptr7);
	if (update_decor_counter == 0) {
		return;
	}
	for (os = objects_list_cur; os <= objects_list_last; ++os) {
		if (os->displayed == 2) {
			if (!game_projectile_collides(os)) {
				break;
			}
		}
	}
}

void game_execute_colliding_op() {
	if (!player_state.dead_flag) {
		object_state_t *os = objects_list_cur;
		anim_frame_t *af = player_state.anim_frame_ptr;
		player_state.unk3E = 0;
		bounding_box_y1 = player_state.pos_y + af->delta_y;
		bounding_box_y2 = player_state.pos_y + player_state.dim_h;
		bounding_box_x1 = player_state.pos_x + 2;
		bounding_box_x2 = player_state.pos_x + 14;
		game_execute_colliding_op_helper1();
		game_execute_colliding_op_helper2();
		while (1) {
			if (os->displayed == 2) {
				if (os->unk26 & 2) {
					anim_data_t *ad = os->anim_data1_ptr;
					if (os->pos_y + ad->bounding_box_y2 >= bounding_box_y1 && os->pos_y + ad->bounding_box_y1 <= bounding_box_y2) {
						if (os->pos_x + ad->bounding_box_x2 >= bounding_box_x1 && os->pos_x + ad->bounding_box_x1 <= bounding_box_x2) {
							assert(ad->colliding_opcode < NUM_COLLIDING_OPCODES);
							(*colliding_op_table[ad->colliding_opcode])(os, ad);
						}
					}
				}
			}
			if (os < objects_list_last) {
				++os;
			} else {
				if (player_state.unk3E != 0) {
					player_state.unk1 = 2;
					player_state.hdir_flags &= ~4;
					player_state.move_offsets_data_ptr = nicky_move_offsets_ptr + 3;
					player_state.unk16 = 0;
				}
				break;
			}
		}
	}
}

/* bullets collision */
void game_execute_colliding_op_helper1() {
	int i;
	object_state_t *os = objects_table_ptr2;
	update_decor_counter = 7;
	/* collision with decor */
	game_handle_projectiles_collisions_helper2(objects_table_ptr2, 0, 7);
	if (update_decor_counter == 0) {
		return;
	}
	/* collision with nicky */
	for (i = 0; i < 7; ++i, ++os) {
		if (os->displayed) {
			anim_data_t *ad = os->anim_data1_ptr;
			if (ad->unk4 & 1) {
				if (os->pos_y + ad->bounding_box_y2 < bounding_box_y1 || os->pos_y + ad->bounding_box_y1 > bounding_box_y2) {
					continue;
				}
				if (os->pos_x + ad->bounding_box_x2 < bounding_box_x1 || os->pos_x + ad->bounding_box_x1 > bounding_box_x2) {
					continue;
				}
				player_state.unk2A = 1;
				if (ad->anim_data3_ptr != 0) {
					game_init_object_from_anim3(os, ad);
				} else {
					os->displayed = 0;
				}
				break;
			}
		}
	}
}

/* bonus collision */
void game_execute_colliding_op_helper2() {
	if (bonus_count != 0) {
		int i;
		object_state_t *os = objects_table_ptr8;
		for (i = 0; i < 16; ++i, ++os) {
			if (os->displayed) {
				if (os->unk26 & 2) {
					anim_data_t *ad = os->anim_data1_ptr;
					if (os->pos_y + ad->bounding_box_y2 < bounding_box_y1 || os->pos_y + ad->bounding_box_y1 > bounding_box_y2) {
						continue;
					}
					if (os->pos_x + ad->bounding_box_x2 < bounding_box_x1 || os->pos_x + ad->bounding_box_x1 > bounding_box_x2) {
						continue;
					}
					if (ad->colliding_opcode != 14) {
						os->transparent_flag ^= 2;
						player_state.unk2A = 1;
					} else {
						cycle_score += ad->score;
						if (ad->anim_data3_ptr != 0) {
							game_play_anim_data_sfx(game_init_object_from_anim3(os, ad));
						} else {
							os->displayed = 0;
						}
					}
					break;
				}
			}
		}
	}
}

void game_reset_objects8() {
	int i;
	object_state_t *os = objects_table_ptr8;
	player_state.unk1B = 0;
	bonus_count = 0;
	execute_action_op_flag = 0;
	for (i = 0; i < 32; ++i) {
		os->displayed = 0;
		os->life = 0;
		os->distance_pos_x = 0;
		os->move_data_ptr = 0;
		os->anim_data_cycles = 0;
		os->unk3 = 0;
		os->distance_pos_y = 0;
		os->transparent_flag = 0;
		++os;		
	}
}

void game_play_anim_data_sfx(const anim_data_t *ad) {
	if (ad->sound_num >= 0) {
		uint8 priority = snd_sample_data_table[ad->sound_num].priority;
		if (priority >= _snd_current_sfx_priority) {
			_snd_current_sfx_priority = priority;
			_snd_current_sfx_num = ad->sound_num;
		}
	}
}

void game_set_tile_offsets() {
	static const uint16 game_state_unk16[8] = { 149, 129, 149, 129, 0, 0, 165, 166 };
	const uint16 *p;
	int i, tile_num;

	memset(_screen_cdg_offsets, 0, sizeof(_screen_cdg_offsets));
	for (i = 0; i < 256; ++i) {
		_screen_cdg_offsets[i] = i;
	}
	tile_num = 240;
	for (i = 0; i < 4; ++i) {
		int j;
		tile_anim_t *ta = &game_state.tile_anims_table[i];
		ta->tile_num = tile_num;
		ta->anim = 0;
		for (j = 0; j < 4; ++j) {
			ta->tiles_table[j] = tile_num;
			++tile_num;
		}
	}
	game_state.unk15 = 0;
	
	p = game_state_unk16 + (current_level & ~1);
	_screen_blinking_tile.tile_num = p[0];
	_screen_blinking_tile.anim = 0;
	_screen_blinking_tile.tiles_table[0] = _screen_cdg_offsets[p[0]];
	_screen_blinking_tile.tiles_table[1] = _screen_cdg_offsets[p[1]];
}

void game_update_life_bar() {
	if (player_state.increment_life_bar) {
		/* XXX original used 44 */
		if (player_state.energy >= 64) {
			player_state.increment_life_bar = 0;
			return;
		}
		++player_state.energy;
		if (player_state.pos_y >= GAME_SCREEN_H) {
			player_state.pos_y = 161;
		}
		return;
	}
	if (player_state.pos_y >= GAME_SCREEN_H) {
		player_state.pos_y = 161;
	} else {
		if (player_state.shield_duration > 0) {
			return;
		}
		if (update_nicky_anim_flag == 0) {
			return;
		}
		if (player_state.unk2A == 0) {
			return;
		}
	}
	print_debug(DBG_GAME, "game_update_life_bar() energy=%d", player_state.energy);
	--player_state.unk2A;
	if (player_state.energy) {
		--player_state.energy;
		return;
	}
	if (player_state.dead_flag) {
		return;
	}
	--player_state.lifes_count;
	if (player_state.lifes_count == 0) {
		quit_level_flag = 1;
	}
	if (player_state.has_blue_ball) {
		player_state.has_blue_ball = 0;
	} else {
		cur_anim_data_ptr = &anim_data0[0];
		cur_anim_data_count = 3;
	}
	player_state.tilemap_offset = player_state.unk38 - 2;
	game_adjust_player_position();
	player_state.dead_flag = 1;
	update_nicky_anim_flag = 0;
	player_state.move_offsets_data_ptr = nicky_move_offsets_table5;
	player_state.unk16 = 0;
	snd_play_sfx(SND_SAMPLE_8);
}

void loc_11F71(object_state_t *os, anim_data_t *ad) {
	if (player_state.unk1C) {
		--player_state.unk1C;
	} else if (os->displayed == 0 && inp_fire_button && (player_state.hdir_flags & 0x30) == 0) {
		os->anim_data1_ptr = ad;
		player_state.unk1C = 8;
		os->displayed = ad->unk0;
		os->life = ad->lifes;
		os->anim_data_cycles = ad->cycles;
		os->unk3 = ad->unk3;
		os->unk26 = ad->unk4;
		os->unk27 = ad->unk5;
		os->pos_x = player_state.pos_x;
		os->pos_y = player_state.pos_y;
		os->distance_pos_y = ad->unk28;
		os->distance_pos_x = (player_state.hdir_flags & 1) ? ad->unk26 : -ad->unk26;
		os->pos_x += player_state.anim_frame_ptr->pos_x;
		os->pos_y += player_state.anim_frame_ptr->pos_y;
	}
}

void game_throw_apple_helper1() {
	object_state_t *os = objects_table_ptr5;
	if (player_state.has_blue_ball) {
		loc_11F71(os, &anim_data0[3]);
	}
	if (player_state.has_red_ball) {
		++os;
		loc_11F71(os, &anim_data0[4]);
	}
}

void game_throw_apple_helper2() {
	if (player_state.has_wood > 0) {
		if (player_state.unk28 > 0) {
			--player_state.unk28;
		} else if (inp_fire_button) {
			object_state_t *os = objects_table_ptr7;
			if (os->displayed == 0 && (player_state.hdir_flags & 0x30) == 0) {
				player_state.unk28 = 5;
				loc_11EDE(os, &anim_data0[2]);
			}
		}
	}
}

void game_update_helper3() {
	if (bonus_count) {
		if (boss4_state) {
			game_update_helper8();
		} else {
			game_draw_bonuses();
		}
	}
}

static void game_update_helper7(object_state_t *os) {
	if (os->displayed) {
		int i;
		for (i = 0; i < 4; ++i) {
			int16 delta = os->anim_data3_ptr_;
			os->anim_data4_seg_ += os->map_pos_y;
			os->anim_data4_ptr_ += os->map_pos_x;
			if (delta < os->anim_data4_ptr_) {
				os->anim_data4_ptr_ -= delta;
				os->pos_x += os->distance_pos_x;
			}
			if (delta < os->anim_data4_seg_) {
				os->anim_data4_seg_ -= delta;
				os->pos_y += os->distance_pos_y;
			}
		}
		os->pos_x -= decor_state.delta_x;
		if (os->pos_x >= -280 && os->pos_x <= GAME_SCREEN_W + 280) {
			os->pos_y -= decor_state.delta_y;
			if (os->pos_y >= -140 && os->pos_y <= GAME_SCREEN_H + 150) {
				return;
			}
		}
	}
	os->displayed = 0;
	--bonus_count;
}

void game_update_helper8() {
	int i;
	int16 _bp, _dx, _bx;
	object_state_t *os = objects_table_ptr8;
	bonus_count = 9;
	if (boss4_state == 2) {
		game_move_object(os);
		boss4_state = 3;
	} else if (boss4_state == 3) {
		game_update_helper7(os);
	}
	_bp = os->pos_x;
	_dx = os->pos_y;
	++os;
	for (i = 0; i < 8; ++i, ++os) {
		anim_data_t *ad;
		if (os->displayed == 0) {
hide_object:
			os->displayed = 0;
			--bonus_count;
			continue;
		}
		ad = os->anim_data1_ptr;
		if (ad->logic_opcode != 19) {
			ad = game_logic_op_helper1(os, ad);
			if (os->displayed == 0) {
				goto hide_object;
			}
		} else {
			/* boss level 9 */
			if (ad->cycles != 0 && --os->anim_data_cycles == 0) {
				if (ad->anim_data1_ptr == 0) {
					goto hide_object;
				}
				ad = ad->anim_data1_ptr;
				os->anim_data1_ptr = ad;
				os->anim_data_cycles = ad->cycles;
			}
			_bx = os->map_pos_x;
			if (os->distance_pos_x <= 48) {
				if (os->distance_pos_x == 0) {
					game_play_anim_data_sfx(ad);
				}
				os->distance_pos_x += 4;
				os->distance_pos_y += 4;
				if (os->distance_pos_x >= 40) {
					boss4_state = 2;
				}
			} else {
				if (os->map_pos_y <= 26) {
					os->map_pos_y += 2;
				}
				_bx -= os->map_pos_y;
				_bx &= 0x3FF;
				os->map_pos_x = _bx;
			}
			os->pos_x = ((int16)cos_table[_bx >> 1] * os->distance_pos_x) >> 8;
			os->pos_x += _bp - decor_state.delta_x;
			os->pos_y = ((int16)sin_table[_bx >> 1] * os->distance_pos_y) >> 8;
			os->pos_y += _dx - decor_state.delta_y;
		}
		if (os->pos_x >= -280 && os->pos_x <= GAME_SCREEN_W + 280 && os->pos_y >= -140 && os->pos_y <= GAME_SCREEN_H + 150) {
			game_draw_object(os);
		} else {
			goto hide_object;
		}
	}
}

void game_draw_bonuses() {
	int i;
	object_state_t *os = objects_table_ptr8;
	bonus_count = 16;
	for (i = 0; i < 16; ++i) {
		if (os->displayed) {
			anim_data_t *ad = os->anim_data1_ptr;
			if (ad->logic_opcode == 2) {
				game_logic_op2(os, ad);
				if (os->displayed) {
					goto loc_1328B;
				}
				goto loc_132AC;
			}
			if (os->unk27 != 0 && --os->unk27 == 0) {
				if (os->anim_data4_ptr == 0) {
					goto loc_132AC;
				}
				ad = os->anim_data4_ptr;
				os->anim_data1_ptr = ad;
				os->anim_data3_ptr = ad;
				os->anim_data4_ptr = ad->anim_data2_ptr;
				os->distance_pos_x = ad->unk26;
				os->distance_pos_y = ad->unk28;
				os->move_data_ptr = ad->move_data_ptr;
				os->anim_data_cycles = ad->cycles;
				os->unk3 = ad->unk3;
				os->unk26 = ad->unk4;
				os->unk27 = ad->unk5;
				goto loc_13288;
			}
			if (ad->cycles != 0 && --os->anim_data_cycles == 0) {
				if (ad->anim_data1_ptr == 0) {
					goto loc_132AC;
				}
				if (ad->anim_data1_ptr == (anim_data_t *)0xFFFFFFFF) {
					if (ad->anim_data3_ptr == 0) {
						goto loc_132AC;
					}
					ad = ad->anim_data3_ptr;
				} else {
					ad = ad->anim_data1_ptr;
				}
				os->anim_data1_ptr = ad;
				os->anim_data_cycles = ad->cycles;
			}
loc_13288:
			loc_1282D(os, ad);
loc_1328B:
/*			print_debug(DBG_GAME, "bonus.x=%d bonus.y=%d", os->pos_x, os->pos_y);*/
			if (os->pos_x >= -280 && os->pos_x <= GAME_SCREEN_W + 280 && os->pos_y >= -140 && os->pos_y <= GAME_SCREEN_H + 150) {
				game_draw_object(os);
			} else {
loc_132AC:
				os->displayed = 0;
				--bonus_count;
			}
		}
		++os;
	}
}

/* cycle tile offsets */
void game_update_tile_offsets() {
	int i;
	tile_anim_t *ta;
	
	--game_state.unk15;
	if (game_state.unk15 < 0) {
		game_state.unk15 = 3;
		for (i = 0; i < 4; ++i) {
			ta = &game_state.tile_anims_table[i];
			++ta->anim;
			if (ta->anim == 4) {
				ta->anim = 0;
			}
			_screen_cdg_offsets[ta->tile_num] = ta->tiles_table[ta->anim];
		}
	}
}

/* bomb_collision */
void game_update_helper4() {
	object_state_t *os = objects_table_ptr3;
	if (os->displayed == 2) {
		bounding_box_y1 = os->pos_y + os->anim_data1_ptr->bounding_box_y1;
		bounding_box_x1 = os->pos_x + os->anim_data1_ptr->bounding_box_x1;
		bounding_box_y2 = os->pos_y + os->anim_data1_ptr->bounding_box_y2;
		bounding_box_x2 = os->pos_x + os->anim_data1_ptr->bounding_box_x2;
		for (os = objects_list_cur; os <= objects_list_last; ++os) {
			if (os->displayed == 2) {
				if (os->unk26 & 4) {
					anim_data_t *ad = os->anim_data1_ptr;
					if (os->pos_y + ad->bounding_box_y2 < bounding_box_y1 || os->pos_y + ad->bounding_box_y1 > bounding_box_y2) {
						continue;
					}
					if (os->pos_x + ad->bounding_box_x2 < bounding_box_x1 || os->pos_x + ad->bounding_box_x1 > bounding_box_x2) {
						continue;
					}
					os->transparent_flag = 16;
					os->life -= 4;
					if (os->life == 0 || os->life < 0) {
						cycle_score += ad->score;
						if (ad->colliding_opcode == 18) {
							game_change_decor_tile_map_line(os, ad);
						}
						if (ad->unk4 & 8) {
							game_change_decor_tile_map_quad(os);
						}
						if (ad->unk4 & 0x40) {
							object_state_t *os2 = os + 1;
							anim_data_t *ad2 = &res_ref_ref[os->ref_ref_index];
							os2->anim_data2_ptr = ad2;
							os2->displayed = ad2->unk0;
						}
						if (ad->anim_data3_ptr == 0) {
							os->displayed = (os->visible != 0) ? 1 : 0;
						} else {
							if (os->visible == 0) {
								os->anim_data2_ptr = 0;
							}
							os->transparent_flag = 0;
							game_init_object_from_anim3(os, ad);
						}
						objects_table_ptr3->displayed = 1;
						return;	
					}
				}
			}
		}
	}
}

void game_setup_monster() {
	const uint8 *p, *boss_posit_ref;
	anim_data_t *ad, *boss_ref_ref;
	
	ad = &res_ref_ref[0];
	while (ad->unk0 != -1) {
		++ad;
	}
	++ad;
	boss_ref_ref = ad;

	p = res_posit_ref;
	while (read_uint16LE(p) != 0xFFFF) {
		p += 10;
	}
	p += 10;
	boss_posit_ref = p;
	
	snd_play_song(SND_MUS_SONGMON);
	
	game_init_ref_ref(boss_ref_ref, 0x9000);
	memset(objects_table, 0, 417 * sizeof(object_state_t));
	/* XXX original offset was 10000 / 0x2C, weird... */
	game_init_objects_from_positref(boss_posit_ref, 227);
	game_update_cur_objects_ptr();
	game_state.unk12 = 0;
	game_state.unk13 = 0;
}

void game_draw_inventory_item(int item, int count) {	
	static const int icon_map_table[] = { 34, 33, 28, 27 };
	const int y = 13;
	const int x = 5 + item * 18;
	sys_add_to_sprite_list(res_level_spr, icon_map_table[item], x, y, SSF_VISIBLE, 0);
	sys_add_to_sprite_list(res_digits_spr, count, x + 10, y + 8, SSF_VISIBLE, 2);
}

void game_update_cur_objects_ptr() {
	int x1, x2;
	object_state_t *os;
	
	objects_list_cur = objects_list_head;
	objects_list_last = objects_list_head;
	x1 = _screen_tile_map_offs_x_shl4 - 280;
	x2 = _screen_tile_map_offs_x_shl4 + GAME_SCREEN_W + 280;
	
	os = objects_list_cur;
	while ((os->displayed & 0x80) == 0) {
		if (os->displayed) {
			os->displayed = 1;
		}
		++os;
	}
	
	os = objects_list_cur;
	while ((os->displayed & 0x80) == 0 && x1 > os->map_pos_x) {
		++objects_list_cur;
		os = objects_list_cur;
	}
	
	os = objects_list_cur;
	while ((os->displayed & 0x80) == 0 && x2 >= os->map_pos_x) {
		if (os->anim_data2_ptr == 0) {
			os->displayed = 0;
		}
		if (os->displayed != 0) {
			anim_data_t *_si = os->anim_data2_ptr;
			os->displayed = 2;
			os->pos_x = os->map_pos_x - _screen_tile_map_offs_x_shl4;
			os->pos_y = os->map_pos_y - _screen_tile_map_offs_y_shl4;
			os->anim_data1_ptr = _si;
			os->anim_data3_ptr = _si;
			os->anim_data4_ptr = _si->anim_data2_ptr;
			os->distance_pos_x = _si->unk26;
			os->distance_pos_y = _si->unk28;
			os->move_data_ptr = _si->move_data_ptr;
			os->anim_data_cycles = _si->cycles;
			os->unk3 = _si->unk3;
			os->life = _si->lifes;
			os->unk26 = _si->unk4;
			os->unk27 = _si->unk5;
			os->transparent_flag = 0;
		}
		objects_list_last = os;
		++os;
	}
}

void game_handle_objects2() {
	int i;
	object_state_t *os = objects_table_ptr2;
	for (i = 0; i < 7; ++i, ++os) {
		if (os->displayed) {
			anim_data_t *ad = os->anim_data1_ptr;
			if (ad->cycles != 0 && --os->anim_data_cycles == 0) {
				if (ad->anim_data1_ptr == 0) {
					os->displayed = 0;
					continue;
				}
				ad = ad->anim_data1_ptr;
				os->anim_data1_ptr = ad;
				os->anim_data_cycles = ad->cycles;
				os->unk26 = ad->unk4;
			}
			if (os->unk26 & 0x10) {
				if (ad->move_data_ptr != 0) {
					if (os->move_data_ptr[0] == 0x88) {
						os->move_data_ptr = ad->move_data_ptr;
					}
					os->pos_x += (int8)os->move_data_ptr[0];
					os->pos_y += (int8)os->move_data_ptr[1];
					os->move_data_ptr += 2;
				} else {
					os->pos_x += ad->unk26;
					os->pos_y += ad->unk28;
				}
			} else {
				int j;
				int16 delta = os->anim_data3_ptr_;
				for (j = 0; j < 4; ++j) {
					os->anim_data4_seg_ += os->map_pos_y;
					os->anim_data4_ptr_ += os->map_pos_x;
					if (delta < os->anim_data4_ptr_) {
						os->anim_data4_ptr_ -= delta;
						os->pos_x += os->distance_pos_x;
					}
					if (delta < os->anim_data4_seg_) {
						os->anim_data4_seg_ -= delta;
						os->pos_y += os->distance_pos_y;
					}
				}
			}
			os->pos_x -= decor_state.delta_x;
			if (os->pos_x >= -8 && os->pos_x <= GAME_SCREEN_W) {
				os->pos_y -= decor_state.delta_y;
				if (os->pos_y >= -8 && os->pos_y <= GAME_SCREEN_H) {
					game_draw_object(os);
					continue;
				}
			}
			os->displayed = 0;
		}
	}
}

anim_data_t *game_init_object_from_anim3(object_state_t *os, anim_data_t *ad) {
	anim_data_t *next = ad->anim_data3_ptr;
	os->pos_x += ad->dx;
	os->pos_y += ad->dy;
	os->anim_data_cycles = next->cycles;
	os->unk3 = ad->unk3;
	os->unk26 = next->unk4;
	os->unk27 = next->unk5;
	os->life = next->lifes;
	os->distance_pos_x = next->unk26;
	os->move_data_ptr = next->move_data_ptr;
	os->anim_data4_ptr = next->anim_data2_ptr;
	os->distance_pos_y = next->unk28;
	os->anim_data1_ptr = next;
	os->anim_data3_ptr = next;
	return next;
}

/* construction du pont */
void game_handle_projectiles_collisions_helper1(object_state_t *os) {
	if (os->displayed) {
		anim_data_t *ad = os->anim_data1_ptr;
		int y = os->pos_y + (ad->anim_h >> 1) + decor_state.unk22;
		int x = os->pos_x + (ad->anim_w >> 1) + decor_state.unk20;
		int offs = (x >> 4) * 50 + decor_state.tile_block_x + (y >> 4) + decor_state.tile_block_y;
		int tile_offs = res_decor_cdg[offs] * 8;
		if (res_decor_ref[tile_offs] & 0x20) {
			if (res_decor_ref[tile_offs + 5]) {
				res_decor_cdg[offs] = res_decor_ref[tile_offs + 5];
				snd_play_sfx(SND_SAMPLE_9);
				--update_decor_counter;
				os->displayed = 0;
			}
		}
	}
}

void game_handle_projectiles_collisions_helper2(object_state_t *os, int _bp, int count) {
	while (count--) {
		anim_data_t *ad = os->anim_data1_ptr;
		if (os->displayed && (ad->unk4 & 1)) {
			int y = os->pos_y + decor_state.unk22 + (ad->anim_h >> 1);
			int x = os->pos_x + decor_state.unk20 + (ad->anim_w >> 1);
			int offs = (x >> 4) * 50 + decor_state.tile_block_x + (y >> 4) + decor_state.tile_block_y;
			int tile_offs = res_decor_cdg[offs] * 8;
			if (res_decor_ref[tile_offs] & 0x10) {
				if (res_decor_ref[tile_offs + 5] != 0 && _bp != 0) {
					res_decor_cdg[offs] = res_decor_ref[tile_offs + 5];
					if (snd_sample_data_table[0].priority >= _snd_current_sfx_priority) {
						_snd_current_sfx_priority = snd_sample_data_table[0].priority;
						_snd_current_sfx_num = 0;
					}
				}
				if (ad->anim_data3_ptr != 0) {
					game_init_object_from_anim3(os, ad);
				} else {
					--update_decor_counter;
					os->displayed = 0;
				}
				return;
			}
		} else {
			--update_decor_counter;
		}
		++os;
	}
}

void game_logic_op_helper2(object_state_t *_di, anim_data_t *_si, anim_data_t *ad) {
	int i;
	anim_data_t *_game_logic_op_helper2_temp_var = ad;
	object_state_t *bx = objects_table_ptr8;
	for (i = 0; i < 16; ++i) {
		if (bx->displayed == 0) {
			bx->anim_data1_ptr = _si;
			bx->anim_data3_ptr = _si;
			bx->displayed = _si->unk0;
			bx->life = _si->lifes;
			bx->anim_data_cycles = _si->cycles;
			bx->unk3 = _si->unk3;
			bx->displayed = 2;
			bx->anim_data4_ptr = _si->anim_data2_ptr;
			bx->move_data_ptr = _si->move_data_ptr;
			bx->unk26 = _si->unk4;
			bx->unk27 = _si->unk5;
			bx->pos_x = _di->pos_x + _game_logic_op_helper2_temp_var->unk3E;
			bx->pos_y = _di->pos_y + _game_logic_op_helper2_temp_var->unk40;
			bx->move_data_ptr += game_get_random_number(collision_state_counter) * 2;
			bx->distance_pos_y = 0;
			bx->distance_pos_x = game_get_random_number(5) - 2;
			bonus_count = 1;
			++_di->ref_ref_index;
			game_play_anim_data_sfx(_si);
			return;
		}
		++bx;
	}
	++_di->ref_ref_index;
}

/* called when hitting bonus cone */
void game_handle_projectiles_collisions_helper3(object_state_t *os, anim_data_t *ad) {
	if (os->ref_ref_index == 0) {
		os->life += unk_ref_index & 0xFF;
	}
	collision_state_counter = 6;
	game_logic_op_helper2(os, &ad->anim_data4_ptr[os->ref_ref_index], ad);
}

void game_handle_game_over() {
	int i;
	
	player_state.tilemap_offset = player_state.unk38 - 2;
	game_adjust_player_position();
	game_disable_nicky_shield();
	player_state.pos_x = -30;
	game_draw_nicky();
	for (i = 0; i < 2; ++i) {
		anim_data_t *ad = &res_ref_ref[123 + i];
		object_state_t *os = &objects_table_ptr8[i];
		os->pos_x = (GAME_SCREEN_W - 64) / 2;
		os->pos_y = (i == 0) ? 48 : 100;
		os->anim_data1_ptr = ad;
		os->anim_data3_ptr = ad;
		os->anim_data4_ptr = ad->anim_data2_ptr;
		os->distance_pos_x = ad->unk26;
		os->distance_pos_y = ad->unk28;
		os->move_data_ptr = ad->move_data_ptr;
		os->displayed = ad->unk0;
		os->life = ad->lifes;
		os->anim_data_cycles = ad->cycles;
		os->unk3 = ad->unk3;
		++os->displayed;
		os->unk26 = ad->unk4;
		os->unk27 = ad->unk5;
		os->transparent_flag = 0;
	}
	while (1) {
		sys_update_screen();
		sys_clear_sprite_list();
		++objects_table_ptr8[0].life;
		if (objects_table_ptr8[0].life > 2) {
			break;
		}
		game_adjust_tilemap();
		game_execute_logic_op();
		game_draw_bonuses();
	}
	snd_play_song(SND_MUS_SONGDEAD);
	sys_wait_for_keys(8000, SKM_ACTION);
/*	sys_fade_out_palette();*/
/*	game_handle_highscore_screen();*/
}

void game_move_object(object_state_t *os) {
	int dy, dx;
	os->distance_pos_x = 0;
	os->distance_pos_y = 0;
	
	dy = player_state.pos_y + 4 + player_state.anim_frame_ptr->delta_y - os->pos_y;
	os->map_pos_y = (dy < 0) ? -dy : dy;
	
	dx = player_state.pos_x - os->pos_x;
	os->map_pos_x = (dx < 0) ? -dx : dx;
	
	os->anim_data3_ptr_ = (os->map_pos_x > os->map_pos_y) ? os->map_pos_x : os->map_pos_y;
	if (dx != 0) {
		if (dx < 0) {
			--os->distance_pos_x;
		} else {
			++os->distance_pos_x;
		}
	}
	if (dy != 0) {
		if (dy < 0) {
			--os->distance_pos_y;
		} else {
			++os->distance_pos_y;
		}
	}
	os->anim_data4_ptr_ = 0;
	os->anim_data4_seg_ = 0;
}

void loc_1282D(object_state_t *os, anim_data_t *ad) {
	int x, y;
	if (ad->move_data_ptr) {
		if (os->move_data_ptr[0] == 0x88) {
			os->move_data_ptr = ad->move_data_ptr;
		}
		x = (int8)os->move_data_ptr[0];
		y = (int8)os->move_data_ptr[1];
		os->move_data_ptr += 2;
	} else {
		x = os->distance_pos_x;
		y = os->distance_pos_y;
	}
	os->pos_x += x - decor_state.delta_x;
	os->pos_y += y - decor_state.delta_y;
}

/* --- ACTION_OP */

void game_execute_action_op() {
	if (execute_action_op_flag) {
		assert(player_state.action_code < NUM_ACTION_OPCODES);
		(*action_op_table[player_state.action_code])();
	}
}

void game_action_op_nop() {
}

void game_action_op_use_megabomb() {
	anim_data_t *ad = &anim_data0[21];
	object_state_t *os = objects_table_ptr4;
	player_state.action_code = 4;
	os->anim_data1_ptr = ad;
	os->displayed = ad->unk0;
	os->life = ad->lifes;
	os->anim_data_cycles = 0xE;
	os->unk3 = ad->unk3;
	os->move_data_ptr = ad->move_data_ptr;
	os->pos_x = player_state.pos_x - 6;
	os->pos_y = player_state.pos_y - 16;
	--player_state.megabombs_count;
}

void game_action_op4() {
	object_state_t *os = objects_table_ptr4;
	--os->anim_data_cycles;
	if (os->anim_data_cycles <= 0) {
		player_state.action_code = 5;
	}
	os->pos_x += (int8)os->move_data_ptr[0] - decor_state.delta_x;
	os->pos_y += (int8)os->move_data_ptr[1] - decor_state.delta_y;
	os->move_data_ptr += 2;
	game_draw_object(os);
}

void game_action_op5() {
	int i, x;
	anim_data_t *ad = &anim_data0[21];
	object_state_t *os = objects_table_ptr4;
	megabomb_origin_x = os->pos_x; /* os->move_data_ptr = os->pos_x;*/
	megabomb_origin_y = os->pos_y; /* os->move_data_seg = os->pos_y;*/
	x = 0;
	for (i = 0; i < 8; ++i) {
		os->anim_data1_ptr = ad;
		os->pos_x = megabomb_origin_x;
		os->pos_y = megabomb_origin_y;
		os->displayed = 1;
		os->anim_data_cycles = 30;
		os->unk3 = 8;
		os->distance_pos_x = 0;
		os->distance_pos_y = 0;
		os->map_pos_x = x;
		x += 128;
		x &= ~1;
		++os;
	}
	player_state.action_code = 6;
	game_shake_screen();
	game_update_decor_after_megabomb();
	game_action_op6();
}

void game_action_op6() {
	int i;
	
	object_state_t *os = objects_table_ptr4;
	if ((player_state.vdir_flags & 0x80) == 0 && player_state.vdir_flags != 0) {
		--os->unk3;
		if ((os->unk3 & 3) == 0) {
			snd_play_sfx(SND_SAMPLE_19);
		}
	}
	/* _bp = os->move_data_ptr;*/
	/* _dx = os->move_data_seg;*/
	--os->anim_data_cycles;
	if (os->anim_data_cycles <= 0) {
		update_nicky_anim_flag = 1;
		player_state.action_code = 0;
		execute_action_op_flag = 0;
		os->displayed = 0;
	} else {
		for (i = 0; i < 8; ++i) {
			os->distance_pos_x += 8;
			os->distance_pos_y += 8;
			os->map_pos_x = (os->map_pos_x + 12) & 0x3FF;
			os->pos_x = ((int16)cos_table[os->map_pos_x >> 1] * os->distance_pos_x) >> 8;
			os->pos_x += megabomb_origin_x - decor_state.delta_x;
			os->pos_y = ((int16)sin_table[os->map_pos_x >> 1] * os->distance_pos_y) >> 8;
			os->pos_y += megabomb_origin_y - decor_state.delta_y;
			game_draw_object(os);
			++os;
		}
	}
}

/* --- LOGIC_OP */

anim_data_t *game_logic_op_unk(object_state_t *os, anim_data_t *ad) {
	print_warning("game_logic_op_unk() logic_opcode=%d UNIMPLEMENTED", ad->logic_opcode);
	return ad;
}

static void loc_12923(object_state_t *os, anim_data_t *ad) {
	os->anim_data1_ptr = ad;
	os->anim_data3_ptr = ad;
	os->anim_data4_ptr = ad->anim_data2_ptr;
	os->move_data_ptr = ad->move_data_ptr;
	os->distance_pos_x = ad->unk26;
	os->distance_pos_y = ad->unk28;
	os->anim_data_cycles = ad->cycles;
	os->unk3 = ad->unk3;
	os->unk26 = ad->unk4;
	os->unk27 = ad->unk5;
	loc_1282D(os, ad);
}

static anim_data_t *loc_12B2B(object_state_t *os, anim_data_t *ad) {
	int x, y, offs;
	x = os->pos_x - 2;
	if (os->distance_pos_x != 0) {
		uint8 _al;
		if (os->distance_pos_x > 0) {
			x += ad->anim_w + 4;
		}
		x += decor_state.unk20 - decor_state.delta_x;
		x = (x >> 4) * 50 + decor_state.tile_block_x;
		offs = x;
		offs += (os->pos_y + decor_state.unk22 - decor_state.delta_y) >> 4;
		offs += decor_state.tile_block_y;
		_al = res_decor_cdg[offs];
		if (res_decor_ref[_al * 8] & 1) {
			os->distance_pos_x = 0;
		} else {
			offs = x;
			offs += (os->pos_y + ad->anim_h + decor_state.unk22 - decor_state.delta_y) >> 4;
			offs += decor_state.tile_block_y;
			_al = res_decor_cdg[offs];
			if (res_decor_ref[_al * 8] & 1) {
				os->distance_pos_x = 0;
			}
		}
	}
	y = os->pos_y - 2;
	if (os->distance_pos_y != 0) {
		uint8 _al;
		if (os->distance_pos_y > 0) {
			y += ad->anim_h + 4;
		}
		y += decor_state.unk22 - decor_state.delta_y;
		y = (y >> 4) + decor_state.tile_block_y;
		offs = os->pos_x + decor_state.unk20 - decor_state.delta_x;
		offs = (offs >> 4) * 50 + decor_state.tile_block_x;
		offs += y;
		_al = res_decor_cdg[offs];
		if (res_decor_ref[_al * 8] & 1) {
			os->distance_pos_y = 0;
		} else {
			offs = os->pos_x + ad->anim_w + decor_state.unk20 - decor_state.delta_x;
			offs = (offs >> 4) * 50 + decor_state.tile_block_x;
			offs += y;
			_al = res_decor_cdg[offs];
			if (res_decor_ref[_al * 8] & 1) {
				os->distance_pos_y = 0;
			}
		}
	}
	if (ad->cycles != 0 && --os->anim_data_cycles == 0) {
		anim_data_t *_si = ad->anim_data1_ptr;
		if (_si == (anim_data_t *)0xFFFFFFFF) {
			_si = os->anim_data3_ptr;
		}
		if (_si == 0) {
			os->displayed = 0;
		} else {
			ad = _si;
			os->anim_data1_ptr = _si;
			os->anim_data_cycles = _si->cycles;
			loc_1282D(os, _si);
		}
	} else {
		loc_1282D(os, ad);
	}
	return ad;
}

anim_data_t *game_logic_op_helper1(object_state_t *os, anim_data_t *ad) {
	if (ad->cycles != 0 && --os->anim_data_cycles == 0) {
		anim_data_t *_si = ad->anim_data1_ptr;
		if (_si == (anim_data_t *)0xFFFFFFFF) {
			_si = os->anim_data3_ptr;
		}
		if (_si == 0) {
			os->displayed = 0;
			return ad;
		}
		ad = _si;
		os->anim_data1_ptr = ad;
		os->distance_pos_x = ad->unk26;
		os->distance_pos_y = ad->unk28;
		os->anim_data_cycles = ad->cycles;
	}
	loc_1282D(os, ad);
	return ad;
}

/* setup_objects with 'os' as origin */
static void loc_13132(object_state_t *os, anim_data_t *ad, object_state_t *_bx, int count) {
	int i;
	for (i = 0; i < count; ++i, ++_bx) {
		if (_bx->displayed != 2) {
			int dy;
			anim_data_t *_di;
			_bx->pos_x = os->pos_x + ad->unk3E;
			_bx->pos_y = os->pos_y + ad->unk40;
			_di = ad->anim_data4_ptr;
			_bx->anim_data1_ptr = _di;
			_bx->anim_data3_ptr = _di;
			_bx->anim_data4_ptr = _di->anim_data2_ptr;
			_bx->distance_pos_x = _di->unk26;
			_bx->distance_pos_y = _di->unk28;
			_bx->displayed = _di->unk0;
			_bx->life = _di->lifes;
			_bx->anim_data_cycles = _di->cycles;
			_bx->unk3 = _di->unk3;
			_bx->displayed = 2;
			_bx->unk26 = _di->unk4;
			_bx->unk27 = _di->unk5;
			if (_di->unkB != 0) {
				_bx->distance_pos_x = -(game_get_random_number(_di->unkB) + 1);
			}
			dy = ad->init_sprite_num;
			if ((_di->unk6 & 4) && (dy >= 2)) {
				dy = game_get_random_number(dy >> 1) * 2;
			}
			_bx->move_data_ptr = &_di->move_data_ptr[dy];
			game_play_anim_data_sfx(_di);
			break;
		}
	}
}

static void game_logic_op_helper3(object_state_t *os, anim_data_t *ad, object_state_t *_bx, int count) {
	if (os->pos_x <= GAME_SCREEN_W && os->pos_x >= 0 && os->pos_y <= GAME_SCREEN_H && os->pos_y >= -28) {
		loc_13132(os, ad, _bx, count);
	}
}

static void game_logic_op_helper4(object_state_t *os, anim_data_t *ad) {
	os->anim_data1_ptr = ad;
	os->anim_data3_ptr = ad;
	os->anim_data4_ptr = ad->anim_data2_ptr;
	os->distance_pos_x = ad->unk26;
	os->distance_pos_y = ad->unk28;
	os->move_data_ptr = &ad->move_data_ptr[ad->init_sprite_num];
	os->displayed = ad->unk0;
	os->life = ad->lifes;
	os->anim_data_cycles = ad->cycles;
	os->unk3 = ad->unk3;
	os->displayed = 2;
	os->unk26 = ad->unk4;
	os->unk27 = ad->unk5;
	game_change_decor_tile_map_quad(os);
}

static anim_data_t *game_logic_op_helper5(object_state_t *os, anim_data_t *ad) {
	--game_state.unk13;
	if (game_state.unk13 <= 0) {
		int i;
		object_state_t *_bx = os + 1;
		game_state.unk13 = game_get_random_number(5) + 1;
		for (i = 0; i < 9; ++i, ++_bx) {
			if (_bx->displayed != 2) {
				if (ad->anim_data3_ptr) {
					anim_data_t *_dx = ad->anim_data3_ptr;
					_bx->pos_x = os->pos_x + game_get_random_number(ad->anim_w) - 10;
					_bx->pos_y = os->pos_y + game_get_random_number(ad->anim_h) - 10;
					_bx->anim_data1_ptr = _dx;
					_bx->anim_data3_ptr = _dx;
					_bx->anim_data4_ptr = _dx->anim_data2_ptr;
					_bx->distance_pos_x = _dx->unk26;
					_bx->distance_pos_y = _dx->unk28;
					_bx->move_data_ptr = _dx->move_data_ptr;
					_bx->displayed = _dx->unk0;
					_bx->life = _dx->lifes;
					_bx->anim_data_cycles = _dx->cycles;
					_bx->unk3 = _dx->unk3;
					_bx->displayed = 2;
					_bx->unk26 = _dx->unk4;
					_bx->unk27 = _dx->unk5;
					_bx->transparent_flag = 0;
					if (_dx->sound_num >= 0) {
						snd_play_sfx(_dx->sound_num);
					}
				}
				if (game_state.unk12 < 60) {
					++game_state.unk12;
					break;
				}
				os->displayed = 0;
				if (current_level == 7) {
					anim_data_t *__bx;
					ad = os->anim_data1_ptr; /* _si */
					__bx = ad->anim_data4_ptr;
					os->anim_data1_ptr = __bx;
					os->anim_data3_ptr = __bx;
					os->anim_data4_ptr = __bx->anim_data2_ptr;
					os->distance_pos_x = __bx->unk26;
					os->distance_pos_y = __bx->unk28;
					os->move_data_ptr = &__bx->move_data_ptr[__bx->init_sprite_num];
					os->displayed = __bx->unk0;
					os->life = __bx->lifes;
					os->anim_data_cycles = __bx->cycles;
					os->unk3 = __bx->unk3;
					os->displayed = 2;
					os->unk26 = __bx->unk4;
					os->unk27 = __bx->unk5;
					os->pos_y += ad->unk40;
					os->pos_x += ad->unk3E;
					break;
				}
			}
		}
	}
	return ad;
}

anim_data_t *game_logic_op0(object_state_t *os, anim_data_t *ad) {
	if (os->unk27 == 0 || --os->unk27 != 0) {
		ad = game_logic_op_helper1(os, ad);
	} else {
		if (os->anim_data4_ptr == 0) {
			os->displayed = 0;
		} else {
			ad = os->anim_data4_ptr;
			loc_12923(os, ad);
		}
	}
	return ad;
}

anim_data_t *game_logic_op1(object_state_t *os, anim_data_t *ad) {
	int _ax, _bx;
	uint8 _al, _bl;
	
	_ax = os->pos_x;
	if (ad->unk6 & 2) {
		_ax += ad->anim_w;
	} else {
		_ax -= 2;
	}
	_ax += decor_state.unk20 - decor_state.delta_x;
	_ax = (_ax >> 4) * 50 + decor_state.tile_block_x;
	_bx = os->pos_y + ad->anim_h + decor_state.unk22 - decor_state.delta_y;
	_bx = (_bx >> 4) + decor_state.tile_block_y;
	_bx += _ax;

	_al = res_decor_cdg[_bx - 1];
	_bl = res_decor_cdg[_bx];
	if ((res_decor_ref[_bl * 8] & 1) && (res_decor_ref[_al * 8] & 1) == 0) {
		if (ad->unk6 & 0x80) {
			if (ad->anim_data1_ptr == 0) {
				os->displayed = 0;
			} else {
				ad = ad->anim_data1_ptr;
				loc_12923(os, ad);
			}
		} else {
			ad = game_logic_op_helper1(os, ad);
		}
	} else {
		ad = os->anim_data4_ptr;
		loc_12923(os, ad);
	}
	return ad;
}

anim_data_t *game_logic_op2(object_state_t *os, anim_data_t *ad) {
	int dy;
	if (os->move_data_ptr[1] == 0x88) {
		os->displayed = 0;
	} else {
		uint8 tile_num;
		int16 _ax, _bx, _cx, _dx;
		
		dy = (int8)os->move_data_ptr[1];
		os->move_data_ptr += 2;
		os->pos_y += dy - decor_state.delta_y;
		_bx = os->pos_y,
		_dx = ad->anim_h / 2;
		if (dy >= 0) {
			_bx += ad->anim_h;
			_dx = -_dx;
		}
		_bx += decor_state.unk22;
		_cx = _bx;
		_bx = (_bx >> 4) + decor_state.tile_block_y;
		_ax = os->distance_pos_x;
		if (_ax != 0) {
			if (_ax > 0) {
				_ax += ad->bounding_box_x2;
			}
			_cx += _dx;
			_cx = (_cx >> 4) + decor_state.tile_block_y;
			_ax -= decor_state.delta_x;
			_ax += os->pos_x + decor_state.unk20;
			_ax = (_ax >> 4) * 50 + decor_state.tile_block_x + _cx;
			tile_num = res_decor_cdg[_ax];
			if (res_decor_ref[tile_num * 8] & 1) {
				os->distance_pos_x = 0;
			}		
		}
/*loc_1335D:*/
		_ax = os->distance_pos_x - decor_state.delta_x;
		os->pos_x += _ax;
		_ax = os->pos_x + decor_state.unk20;
		_cx = _ax;
		_ax = (_ax >> 4) * 50 + decor_state.tile_block_x + _bx;
		tile_num = res_decor_cdg[_ax];
		if ((res_decor_ref[tile_num * 8] & 1) == 0) {
			_ax = ((ad->bounding_box_x2 + _cx) >> 4) * 50 + decor_state.tile_block_x + _bx;
			tile_num = res_decor_cdg[_ax];
			if ((res_decor_ref[tile_num * 8] & 0x10) == 0) {
				goto loc_13479;
			}
		}
/*loc_133D3:*/
		if (dy <= 0) {
			os->move_data_ptr = &ad->move_data_ptr[ad->init_sprite_num];
			goto loc_13479;
		}
		++os->unk27;
		if (os->unk27 < 4) goto loc_1344F;
		if (os->anim_data4_ptr == 0) goto loc_134B2;
		ad = os->anim_data4_ptr;
		os->anim_data1_ptr = ad;
		os->anim_data3_ptr = ad;
		os->anim_data4_ptr = ad->anim_data2_ptr;
		os->distance_pos_x = ad->unk26;
		os->move_data_ptr = ad->move_data_ptr;
		os->distance_pos_y = ad->unk28;
		os->anim_data_cycles = ad->cycles;
		os->unk3 = ad->unk3;
		os->unk26 = ad->unk4;
		os->unk27 = ad->unk5;
		goto loc_13461;
loc_1344F:
		os->move_data_ptr = &ad->move_data_ptr[os->unk27 * 4];
loc_13461:
		_bx = (os->pos_y + decor_state.unk22 + ad->anim_h) & ~0xF;
		_bx -= ad->anim_h;
		_bx -= decor_state.unk22;
		os->pos_y = _bx;
/*		print_debug(DBG_GAME, "game_logic_op2() pos_y=%d", os->pos_y);*/
loc_13479:
		if (ad->cycles != 0 && --os->anim_data_cycles == 0) {
			anim_data_t *_si = ad->anim_data1_ptr;
			if (_si == (anim_data_t *)0xFFFFFFFF) {
				_si = os->anim_data3_ptr;
			}
			if (_si == 0) {
				os->displayed = 0;
				return ad;
			}
			ad = _si;
			os->anim_data1_ptr = ad;
			os->anim_data_cycles = ad->cycles;
		}
		return ad;
loc_134B2:
		os->displayed = 0;
	}
	return ad;
}

anim_data_t *game_logic_op3(object_state_t *os, anim_data_t *ad) {
	int16 _ax, _bx;
	uint8 tile_num;
	
	_ax = os->pos_x + decor_state.unk20 - decor_state.delta_x;
	_ax = (_ax >> 4) * 50 + decor_state.tile_block_x;
	_bx = os->pos_y + ad->anim_h + decor_state.unk22 - decor_state.delta_y;
	_bx = (_bx >> 4) + decor_state.tile_block_y;
	_bx += _ax;
	
	tile_num = res_decor_cdg[_bx];
	if ((res_decor_ref[tile_num * 8] & 0x10) == 0) {
		anim_data_t *ad2 = os->anim_data4_ptr;
		if (ad2 == 0) {
			os->displayed = 0;
			return ad;
		}
		os->anim_data1_ptr = ad2;
		os->anim_data3_ptr = ad2;
		os->anim_data4_ptr = ad2->anim_data2_ptr;
		os->move_data_ptr = &ad2->move_data_ptr[ad->init_sprite_num];
		os->anim_data_cycles = ad2->cycles;
		os->unk3 = ad2->unk3;
		os->unk26 = ad2->unk4;
		os->unk27 = ad2->unk5;
		ad = ad2;
	}
	os->pos_x -= decor_state.delta_x;
	os->pos_y -= decor_state.delta_y;
	return ad;
}

/* squirrels */
anim_data_t *game_logic_op4(object_state_t *os, anim_data_t *ad) {
	int offs;
	uint8 _al, _bl;
	int16 x = os->pos_x;
	if (ad->unk6 & 2) {
		x += ad->anim_w + 5;
	} else {
		x -= 5;
	}
	offs = ((x + decor_state.unk20 - decor_state.delta_x) >> 4) * 50 + decor_state.tile_block_x;
	offs += ((os->pos_y + ad->anim_h + decor_state.unk22 - decor_state.delta_y) >> 4) + decor_state.tile_block_y;
	
	_al = res_decor_cdg[offs - 1];
	_bl = res_decor_cdg[offs];
	if ((res_decor_ref[_bl * 8] & 1) == 0) {
		ad = os->anim_data4_ptr;
		loc_12923(os, ad);
		return ad;
	}
	if ((res_decor_ref[_al * 8] & 1) != 0) {
		ad = os->anim_data4_ptr;
		loc_12923(os, ad);
		return ad;
	}
	
	if ((ad->unk6 & 2) == 0) {
		if (os->pos_x < player_state.pos_x || os->pos_x - 150 > player_state.pos_x) goto loc_12AD2;
loc_12AA6:
		if (os->pos_y + ad->anim_h < player_state.pos_y || os->pos_y > player_state.pos_y + player_state.dim_h) goto loc_12AD2;
		++ad;
		loc_12923(os, ad);
		return ad;
	}
	if (os->pos_x >= player_state.pos_x) goto loc_12AD2;
	if (os->pos_x + 150 >= player_state.pos_x) goto loc_12AA6;
loc_12AD2:
	if (ad->unk6 & 0x80) {
		ad = ad->anim_data1_ptr;
		loc_12923(os, ad);
		return ad;
	}
	ad = game_logic_op_helper1(os, ad);
	return ad;
}

/* birds */
anim_data_t *game_logic_op5(object_state_t *os, anim_data_t *ad) {
	int x, y;
	os->distance_pos_x = 2;
	os->distance_pos_y = 2;
	y = player_state.pos_y - 8;
	if (os->pos_y >= y) {
		if (os->pos_y + 4 <= y) {
			os->distance_pos_y = 0;
		} else {
			os->distance_pos_y = -os->distance_pos_y;
		}
	}
	x = player_state.pos_x - 8;
	if (os->pos_x >= x) {
		if (os->pos_x <= x + 4) {
			os->distance_pos_x = 0;
		} else {
			os->distance_pos_x = -os->distance_pos_x;
		}
	}
	return loc_12B2B(os, ad);
}

/* glass piece */
anim_data_t *game_logic_op6(object_state_t *os, anim_data_t *ad) {
	uint8 _al;
	int x, y, offs;
	uint8 dy = os->move_data_ptr[1];
	if (dy == 0x88) {
		os->displayed = 0;
		return ad;
	}
	os->move_data_ptr += 2;
	os->pos_y += (int8)dy - decor_state.delta_y;
	y = os->pos_y;
	if ((dy & 0x80) == 0) {
		y += ad->anim_h;
	}
	y = ((y + decor_state.unk22) >> 4) + decor_state.tile_block_y; /* _bx */
	os->pos_x += os->distance_pos_x - decor_state.delta_x;
	x = os->pos_x + decor_state.unk20; /* _cx */
	offs = (x >> 4) * 50 + decor_state.tile_block_x + y;
	_al = res_decor_cdg[offs];
	if ((res_decor_ref[_al * 8] & 1) == 0) {
		offs = ((x + ad->bounding_box_x2) >> 4) * 50 + decor_state.tile_block_x + y;
		_al = res_decor_cdg[offs];
		if ((res_decor_ref[_al * 8] & 1) == 0) {
			goto loc_12DF1;
		}
	}
	os->pos_y = ((os->pos_y + decor_state.unk22 + ad->anim_h) & ~0xF) - ad->anim_h - decor_state.unk22;
	if (os->anim_data4_ptr == 0) {
		os->displayed = 0;
	} else {
		int i;
		object_state_t *_di = os;
		anim_data_t *_bx = os->anim_data4_ptr;
		x = os->pos_x; /* _bp */
		y = os->pos_y; /* _dx */
		for (i = 0; i < 2; ++i) {
			os->anim_data1_ptr = _bx;
			os->anim_data3_ptr = _bx;
			os->anim_data4_ptr = _bx->anim_data2_ptr;
			os->distance_pos_x = _bx->unk26;
			os->distance_pos_y = _bx->unk28;
			os->move_data_ptr = _bx->move_data_ptr;
			os->displayed = _bx->unk0;
			os->life = _bx->lifes;
			os->anim_data_cycles = _bx->cycles;
			os->unk3 = _bx->unk3;
			os->displayed = 2;
			os->unk26 = _bx->unk4;
			os->unk27 = _bx->unk5;
			os->pos_x = x;
			os->pos_y = y;
			++os;
			++_bx;
		}
		game_play_anim_data_sfx(_di->anim_data1_ptr);
	}
loc_12DF1:
	return ad;
}

/* boss level 2 */
anim_data_t *game_logic_op7(object_state_t *os, anim_data_t *ad) {
	int i, _bp;
	object_state_t *_bx = os + 1;
	_bp = 0;
	game_shake_screen();
	for (i = 0; i < 9; ++i, ++_bx) {
		if (_bx->displayed != 2) {
			anim_data_t *_dx = ad->anim_data4_ptr;
			if (_dx == 0) {
				goto loc_12EA0;
			}
			_bx->pos_y = _bx->map_pos_y - _screen_tile_map_offs_y_shl4 + decor_state.delta_y;
			_bx->pos_x = _bx->map_pos_x - _screen_tile_map_offs_x_shl4 + decor_state.delta_x;
			_bx->pos_x += game_get_random_number(32);
			_bx->anim_data1_ptr = _dx;
			_bx->anim_data3_ptr = _dx;
			_bx->anim_data4_ptr = _dx->anim_data2_ptr;
			_bx->distance_pos_x = _dx->unk26;
			_bx->distance_pos_y = _dx->unk28;
			_bx->move_data_ptr = &_dx->move_data_ptr[_dx->init_sprite_num];
			_bx->displayed = _dx->unk0;
			_bx->life = _dx->lifes;
			_bx->anim_data_cycles = _dx->cycles;
			_bx->unk3 = _dx->unk3;
			_bx->displayed = 2;
			_bx->unk26 = _dx->unk4;
			_bx->unk27 = _dx->unk5;
			++_bp;
			if (_bp == 4) goto loc_12EA0;
		}
	}
loc_12EA0:
	return game_logic_op0(os, ad);
}

/* boss explosion */
anim_data_t *game_logic_op8(object_state_t *os, anim_data_t *ad) {
	ad = game_logic_op_helper5(os, ad);
	return game_logic_op0(os, ad);
}

/* plant */
anim_data_t *game_logic_op9(object_state_t *os, anim_data_t *ad) {
	++os->tile_num;
	if ((os->tile_num & 1) && os->pos_x <= GAME_SCREEN_W && os->pos_x >= 0 && os->pos_y <= GAME_SCREEN_H && os->pos_y >= 0) {
		collision_state_counter = 8;
		game_logic_op_helper2(os, ad->anim_data4_ptr, ad);
	}
	return game_logic_op0(os, ad);
}

/* boss level6 */
anim_data_t *game_logic_op10(object_state_t *os, anim_data_t *ad) {
	game_shake_screen();
	return game_logic_op0(os, ad);
}

/* boss level4 */
anim_data_t *game_logic_op11(object_state_t *os, anim_data_t *ad) {
	++os->tile_num;
	if ((os->tile_num & 1) && ad->anim_data4_ptr) {
		loc_13132(os, ad, os + 1, 9);
	}
	return game_logic_op0(os, ad);
}

/* level5 */
anim_data_t *game_logic_op12(object_state_t *os, anim_data_t *ad) {
	--os->unk27;
	if (os->unk27 <= 0) {
		os->unk27 = ad->unk5;
		os->distance_pos_x = game_get_random_number(5) - 2;
		os->distance_pos_y = game_get_random_number(5) - 2;
	}
	return loc_12B2B(os, ad);
}

/* bat */
anim_data_t *game_logic_op13(object_state_t *os, anim_data_t *ad) {
	--os->unk27;
	if (os->unk27 <= 0) {
		ad = os->anim_data3_ptr + 1;
		loc_12923(os, ad);
	} else {
		ad = game_logic_op1(os, ad);
	}
	return ad;
}

/* levels 5,6 - scarecrow */
anim_data_t *game_logic_op14(object_state_t *os, anim_data_t *ad) {
	if (ad->anim_data4_ptr) {
		game_logic_op_helper3(os, ad, os - 5, 5);
	}
	return game_logic_op0(os, ad);
}

/* levels 5,6 - gouttes plafond */
anim_data_t *game_logic_op15(object_state_t *os, anim_data_t *ad) {
	if (ad->anim_data4_ptr) {
		game_logic_op_helper3(os, ad, objects_table_ptr8, 16);
		bonus_count = 1;
	}
	return game_logic_op0(os, ad);
}

/* block falling from the ceiling */
anim_data_t *game_logic_op16(object_state_t *os, anim_data_t *ad) {
	os->anim_data2_ptr = 0;
	os->displayed = 0;
	(os + 1)->pos_x = os->pos_x;
	(os + 1)->pos_y = os->pos_y;
	(os + 1)->anim_data2_ptr = os->anim_data4_ptr;
	game_logic_op_helper4(os + 1, os->anim_data4_ptr);
	loc_1282D(os, ad);
	return ad;
}

/* levels 6,7 rondins bois avec pics */
anim_data_t *game_logic_op17(object_state_t *os, anim_data_t *ad) {
	int y, x;
	y = os->pos_y;
	if (ad->unk6 & 2) {
		y += ad->anim_h;
	}
	y += decor_state.unk22 - decor_state.delta_y;
	y = (y >> 4) + decor_state.tile_block_y;
	
	x = os->pos_x + decor_state.unk20 - decor_state.delta_x;
	x = (x >> 4) * 50 + decor_state.tile_block_x;

	if (res_decor_ref[res_decor_cdg[y + x] * 8] & 0x10) {
		loc_12923(os, os->anim_data4_ptr);
	} else {
		ad = game_logic_op_helper1(os, ad);
	}
	return ad;
}

/* falling platforms levels 7,8 */
anim_data_t *game_logic_op18(object_state_t *os, anim_data_t *ad) {
	os->anim_data2_ptr = ad;
	game_logic_op_helper4(os, ad->anim_data2_ptr);
	loc_1282D(os, ad);
	return ad;
}

/* boss level8 */
anim_data_t *game_logic_op19(object_state_t *os, anim_data_t *ad) {
	int i;
	anim_data_t *_dx = ad->anim_data4_ptr;
	anim_data_t *_bp = _dx->anim_data4_ptr;
	object_state_t *_bx = objects_table_ptr8;
	for (i = 0; i < 9; ++i, ++_bx) {
		_bx->pos_x = os->pos_x + ad->unk3E;
		_bx->pos_y = os->pos_y + ad->unk40;
		_bx->displayed = _dx->unk0;
		_bx->life = _dx->lifes;
		_bx->anim_data_cycles = _dx->cycles;
		_bx->unk3 = _dx->unk3;
		_bx->distance_pos_x = 0;
		_bx->distance_pos_y = 0;
		_bx->anim_data1_ptr = _dx;
		_bx->unk26 = _dx->unk4;
		_bx->unk27 = _dx->unk5;
		_dx = _bp;
	}
	_bx = objects_table_ptr8 + 1;
	for (i = 0; i < 8; ++i, ++_bx) {
		_bx->map_pos_x = (i * 128) & ~1;
		_bx->map_pos_y = 0;
	}
	bonus_count = 1;
	boss4_state = 1;
	return game_logic_op_helper1(os, ad);
}

/* --- COLLIDES_OP */

void game_collides_play_sound(object_state_t *os) {
	anim_data_t *ad = os->anim_data1_ptr;
	os->displayed = 0;
	if (ad->sound_num >= 0) {
		snd_play_sfx(ad->sound_num);
	}
}

void loc_141E9(object_state_t *os, anim_data_t *ad) {
	os->anim_data2_ptr = ad->anim_data2_ptr;
	if (ad->anim_data3_ptr == 0) {
		os->displayed = 0;
	} else {
		ad = ad->anim_data3_ptr;
		os->anim_data_cycles = ad->cycles;
		os->unk3 = ad->unk3;
		os->unk26 = ad->unk4;
		os->unk27 = ad->unk5;
		os->move_data_ptr = &ad->move_data_ptr[ad->init_sprite_num];
		os->distance_pos_x = ad->unk26;
		os->distance_pos_y = ad->unk28;
		os->anim_data4_ptr = ad->anim_data2_ptr;
		os->anim_data1_ptr = ad;
		os->anim_data3_ptr = ad;
		game_play_anim_data_sfx(ad);
	}
}

void loc_142A7(object_state_t *os, anim_data_t *ad) { /* _di, _bx */
	if (ad->anim_data3_ptr == 0) {
		os->displayed = 0;
		if (os->visible == 0) {
			return;
		}
		++os->displayed;
		game_play_anim_data_sfx(os->anim_data1_ptr);
	} else {
		if (os->visible == 0) {
			os->anim_data2_ptr = 0;
		}
		game_play_anim_data_sfx(game_init_object_from_anim3(os, ad));
	}
}

void game_collides_op_unk(object_state_t *os, anim_data_t *ad) {
	print_warning("game_collides_op_unk() colliding_opcode=%d UNIMPLEMENTED", ad->colliding_opcode);
}

/* snails */
void game_collides_op_0_24(object_state_t *os, anim_data_t *ad) {
	int16 _ax, _cx;
	if (ad->lifes == 0 || (player_state.hdir_flags & 4) == 0) {
		game_collides_op_27(os, ad);
		return;
	}
	_ax = player_state.dim_h / 2 + player_state.pos_y;
	_cx = os->pos_y + ad->bounding_box_y1;
	if (_ax > _cx) {
		game_collides_op_27(os, ad);
		return;
	}
	os->transparent_flag = 32;
	os->life -= 8;
	if (os->life > 0) {
		player_state.unk3E = 1;
	} else {
		int i, j;
		object_state_t *_si = objects_table_ptr9;
		os->transparent_flag = 0;
		cycle_score += ad->score;
		player_state.unk3E = 1;
		for (i = 0; i < 11; ++i) {
			if (_si->displayed == 0) {
				anim_data_t *_dx;
				++bonus_count;
				if (unk_ref_index >= 10) {
					unk_ref_index = 0;
				}
				++unk_ref_index;
				_dx = &ref_ref_7[unk_ref_index - 1];
				_si->anim_data1_ptr = _si->anim_data3_ptr = _dx;
				_si->displayed = _dx->unk0;
				_si->life = _dx->lifes;
				_si->anim_data_cycles = _dx->cycles;
				_si->unk3 = _dx->unk3;
				_si->displayed = 2;
				_si->anim_data4_ptr = _dx->anim_data2_ptr;
				_si->move_data_ptr = _dx->move_data_ptr;
				_si->unk26 = _dx->unk4;
				_si->unk27 = _dx->unk5;
				_si->pos_x = os->pos_x;
				_si->pos_y = os->pos_y;
				
				_si->pos_x += (ad->anim_w >> 1) - (_dx->anim_w >> 1);
				_si->distance_pos_x = 0;
				_si->distance_pos_y = 0;
				
				if (ad->colliding_opcode != 24) {
					loc_142A7(os, ad);
					return;
				}
				_si = os + 1;
				_dx = ad->anim_data4_ptr;
				for (j = 0; j < 2; ++j) {
					_si->anim_data1_ptr = _dx;
					_si->anim_data3_ptr = _dx;
					_si->displayed = _dx->unk0;
					_si->life = _dx->lifes;
					_si->anim_data_cycles = _dx->cycles;
					_si->unk3 = _dx->unk3;
					_si->displayed = 2;
					_si->anim_data4_ptr = _dx->anim_data2_ptr;
					_si->move_data_ptr = _dx->move_data_ptr;
					_si->distance_pos_x = _dx->unk26;
					_si->distance_pos_y = _dx->unk28;
					_si->unk26 = _dx->unk4;
					_si->unk27 = _dx->unk5;
					_si->pos_x = os->pos_x + ad->unk3E;
					_si->pos_y = os->pos_y + ad->unk40;
					_si->anim_data2_ptr = _dx->anim_data2_ptr;
					++_si;
					++_dx;
				}
				loc_142A7(os, ad);
				return;
			}
			++_si;
		}
	}
}

void game_collides_op_potion(object_state_t *os, anim_data_t *ad) {
	player_state.increment_life_bar = 1;
	player_state.unk2A = 0;
	game_collides_play_sound(os);
}

void game_collides_op_life_bonus(object_state_t *os, anim_data_t *ad) {
	if (player_state.lifes_count < 9) {
		++player_state.lifes_count;
	}
	game_collides_play_sound(os);
}

void game_collides_op_key_bonus(object_state_t *os, anim_data_t *ad) {
	if (player_state.keys_count < 9) {
		++player_state.keys_count;
	}
	game_collides_play_sound(os);
}

void game_collides_op_spring(object_state_t *os, anim_data_t *ad) {
	nicky_move_offsets_ptr = &nicky_move_offsets_table[35];
	player_state.has_spring = 2000;
	game_collides_play_sound(os);
}

void game_collides_op_wood_bonus(object_state_t *os, anim_data_t *ad) {
	player_state.has_wood = 1600;
	os->anim_data2_ptr = os->anim_data1_ptr;
	loc_142A7(os, ad);
}

void game_collides_op_shield_bonus(object_state_t *os, anim_data_t *ad) {
	game_enable_nicky_shield();
	player_state.shield_duration = 1900;
	game_collides_play_sound(os);
}

void game_collides_op_magnifying_glass(object_state_t *os, anim_data_t *ad) {
	player_state.tile_blinking_duration = 2000;
	game_collides_play_sound(os);
}

void game_collides_op_bomb_bonus(object_state_t *os, anim_data_t *ad) {
	if (player_state.bombs_count < 9) {
		++player_state.bombs_count;
	}
	game_collides_play_sound(os);
}

void game_collides_op_megabomb_bonus(object_state_t *os, anim_data_t *ad) {
	if (player_state.megabombs_count < 9) {
		++player_state.megabombs_count;
	}
	game_collides_play_sound(os);
}

void game_collides_op_red_ball(object_state_t *os, anim_data_t *ad) {
	player_state.has_red_ball = 2000;
	game_collides_play_sound(os);
}

void game_collides_op_blue_ball(object_state_t *os, anim_data_t *ad) {
	player_state.has_blue_ball = 1;
	game_collides_play_sound(os);
}

void game_collides_op_apple(object_state_t *os, anim_data_t *ad) {
	cur_anim_data_ptr = &anim_data0[1];
	cur_anim_data_count = 4;
	game_collides_play_sound(os);
}

void game_collides_op_teleport(object_state_t *os, anim_data_t *ad) {
	player_state.tilemap_offset = os->ref_ref_index;
	game_adjust_player_position();
	restart_level_flag = 1;
	ad = os->anim_data1_ptr;
	if (ad->sound_num >= 0) {
		snd_play_sfx(ad->sound_num);
	}
}

void game_collides_op_misc_bonus(object_state_t *os, anim_data_t *ad) {
	os->anim_data2_ptr = 0;
	cycle_score += ad->score;
	if (ad->anim_data3_ptr == 0) {
		os->displayed = 0;
		if (os->visible != 0) {
			++os->displayed;
			game_play_anim_data_sfx(os->anim_data1_ptr);
		}
	} else {
		if (os->visible == 0) {
			os->anim_data2_ptr = 0;
		}
		game_play_anim_data_sfx(game_init_object_from_anim3(os, ad));
	}
}

void game_collides_op_bonus_box(object_state_t *os, anim_data_t *ad) {
	int i;
	object_state_t *_si = objects_table_ptr8;
	anim_data_t *_dx = ad->anim_data4_ptr;
	os->anim_data2_ptr = ad->anim_data3_ptr;
	collision_state_counter = -2;
	for (i = 0; i < 5; ++i) {
		_si->anim_data1_ptr = _dx;
		_si->anim_data3_ptr = _dx;
		_si->displayed = _dx->unk0;
		_si->life = _dx->lifes;
		_si->anim_data4_ptr = _dx->anim_data2_ptr;
		_si->move_data_ptr = _dx->move_data_ptr;
		_si->anim_data_cycles = _dx->cycles;
		_si->unk3 = _dx->unk3;
		_si->unk26 = _dx->unk4;
		_si->unk27 = _dx->unk5;
		_si->pos_x = os->pos_x + ad->unk3E - (4 - i);
		_si->pos_y = os->pos_y + ad->unk40;
		_si->distance_pos_y = 0;
		_si->distance_pos_x = collision_state_counter;
		++collision_state_counter;
		++_si;
		++_dx;
	}
	++bonus_count;
	loc_142A7(os, ad);
}

void game_collides_op_blue_box(object_state_t *os, anim_data_t *ad) {
	object_state_t *_si = os + 1;
	anim_data_t *_dx = &ref_ref_43[os->ref_ref_index];
	_si->anim_data2_ptr = _dx;
	_si->displayed = _dx->unk0;
	os->anim_data2_ptr = ad->anim_data3_ptr;
	loc_142A7(os, ad);
}

void game_collides_op_door(object_state_t *os, anim_data_t *ad) {
	print_debug(DBG_GAME, "game_collides_op_door() keys_count=%d", player_state.keys_count);
	if (player_state.keys_count) {
		--player_state.keys_count;
		game_change_decor_tile_map_quad(os);
		loc_141E9(os, ad);
	}
}

void game_collides_op_eye(object_state_t *os, anim_data_t *ad) {
	if ((player_state.unk1 & 2) == 0) {
		player_state.move_offsets_data_ptr = &nicky_move_offsets_table[50];
		player_state.unk16 = 0;
		player_state.hdir_flags |= 4;
	}
	game_play_anim_data_sfx(ad);
	game_change_decor_tile_map(os);
	os->displayed = 0;
	os = &objects_list_head[os->ref_ref_index];
	if (os->displayed > 0) {
		game_change_decor_tile_map_quad(os);
		loc_141E9(os, os->anim_data1_ptr);
	}
}

/* jumpers, 'bounding plants' */
void game_collides_op_21_28(object_state_t *os, anim_data_t *ad) {
	if (player_state.hdir_flags & 4) {
		int y1 = player_state.dim_h / 2 + player_state.pos_y;
		int y2 = os->pos_y + ad->bounding_box_y1;
		if (y1 <= y2) {
			player_state.pos_y = y2 - player_state.dim_h;
			player_state.unk1 = 2;
			player_state.hdir_flags &= ~4;
			player_state.move_offsets_data_ptr = nicky_move_offsets_table;
			player_state.unk16 = 0;
			if (ad->colliding_opcode != 21) {
				player_state.move_offsets_data_ptr = &nicky_move_offsets_table[35];
			}
			loc_142A7(os, ad);
		}
	}
}

void game_collides_op_go_to_next_level(object_state_t *os, anim_data_t *ad) {
	new_level_flag = 1;
	ad = os->anim_data1_ptr;
	if (ad->sound_num >= 0) {
		snd_play_sfx(ad->sound_num);
	}
}

void game_collides_op_glass_pieces(object_state_t *os, anim_data_t *ad) {
	object_state_t *_si = &objects_list_head[os->ref_ref_index];
	if (_si->displayed == 2 && _si->anim_data2_ptr == _si->anim_data1_ptr) {
		anim_data_t *_di = _si->anim_data2_ptr->anim_data3_ptr;
		_si->anim_data_cycles = _di->cycles;
		_si->unk3 = _di->unk3;
		_si->unk26 = _di->unk4;
		_si->unk27 = _di->unk5;
		_si->distance_pos_x = _di->unk26;
		_si->distance_pos_y = _di->unk28;
		_si->anim_data4_ptr = _di->anim_data2_ptr;
		_si->anim_data1_ptr = _di;
		_si->anim_data3_ptr = _di;
		_si->move_data_ptr = &_di->move_data_ptr[_di->init_sprite_num];
		loc_142A7(os, ad);
	}
}

/* pumpkin levels 5,6 */
void game_collides_op_27(object_state_t *os, anim_data_t *ad) {
	os->transparent_flag ^= 2;
	player_state.unk2A = 1;
}

/* falling platforms levels 7,8 */
void game_collides_op_29(object_state_t *os, anim_data_t *ad) {
	if (player_state.unk1 & 2) {
		loc_141E9(os, ad);
	}
}
