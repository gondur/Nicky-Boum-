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

#include <SDL.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "mixer.h"
#include "scaler.h"
#include "player_mod.h"
#include "systemstub.h"


#define OSB_COPPER  0
#define OSB_TILES   1
#define OSB_SPRITES 2

#define DEFAULT_SCALER 2

#define TILES_SURFACE_W 512
#define TILES_SURFACE_H 256

#define MAX_SPRITES 128

#define SOUND_SAMPLE_RATE 22050
#define SOUND_BUFFER_SIZE 2048

#define AUTO_DIRTY_BLOCK_W 16
#define AUTO_DIRTY_BLOCK_H 16
#define MAX_AUTO_DIRTY_BLOCKS 150


typedef struct {
	const uint8 *data;
	int num;
	int x, y;
	int flags;
	int pal_num;
} sprite_t;

typedef struct {
	const char *name;
	scaler_pf proc;
	int factor;
} scaler_desc_t;

typedef struct {
	uint8 *ptr;
	int size;
} offscreen_buffer_t;

typedef struct {
	int table_size;
	uint32 *table_ptr[2];
} auto_dirty_blocks_t;


int sys_exit_flag;

static uint16 screen_palette[256];
static SDL_Surface *screen_surface, *offscreen_surface;
static offscreen_buffer_t offscreen_buffers_table[3];
static int tilemap_origin_x, tilemap_origin_y;
static int prev_tilemap_origin_x, prev_tilemap_origin_y;
static int key_mask;
static const uint8 *tile_data, *tilemap_lut, *tilemap_data;
static int screen_w, screen_h;
static sys_screen_mode_e current_mode;
static int scaler_num;
static int fullscreen_flag;
static int pause_flag;
static uint32 last_frame_time_stamp;
static sprite_t sprite_list[MAX_SPRITES];
static sprite_t *sprite_list_tail;
static int force_screen_redraw_flag;
static auto_dirty_blocks_t auto_dirty_blocks;

static const scaler_desc_t scaler_table[] = {
	{ "point1x", point1x, 1 },
	{ "point2x", point2x, 2 },
	{ "scale2x", scale2x, 2 },
	{ "point3x", point3x, 3 },
	{ "scale3x", scale3x, 3 },
	{ "point4x", point4x, 4 },
	{ "scale4x", scale4x, 4 }
};

static const uint8 rev_bitmask_table[] = {
	0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1
};


static void sound_callback(void *param, uint8 *buf, int len) {
	if (!pause_flag) {	
		memset(buf, 0, len);
		mpl_play_callback(SOUND_SAMPLE_RATE, (int8 *)buf, len);
		mixer_mix_samples((int8 *)buf, len);
	}
}

static void setup_screen_surface(int fullscreen, int scaler) {
	int w, h;
	SDL_PixelFormat *fmt;
	const scaler_desc_t *sd;

	scaler_num = scaler;
	fullscreen_flag = fullscreen;
	sd = &scaler_table[scaler_num];
	w = screen_w * sd->factor;
	h = screen_h * sd->factor;
	screen_surface = SDL_SetVideoMode(w, h, 16, fullscreen_flag ? (SDL_FULLSCREEN | SDL_HWSURFACE) : SDL_HWSURFACE);
	if (!screen_surface) {
		print_error("Unable to allocate screen_surface");
	}
	fmt = screen_surface->format;
	offscreen_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, screen_w + 2, screen_h + 2, 16, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (!offscreen_surface) {
		print_error("Unable to allocate offscreen_surface");
	}
	pause_flag = 0;
}

static void change_screen_surface(int fullscreen, int scaler) {
	if (scaler >= 0 && scaler < ARRAYSIZE(scaler_table)) {
		SDL_FreeSurface(offscreen_surface);
		SDL_FreeSurface(screen_surface);
		setup_screen_surface(fullscreen, scaler);
		force_screen_redraw_flag = 1;
	}
}

static uint32 adler32(uint32 adler, const uint16 *buf, int len) {
	uint32 s1 = adler & 0xFFFF;
	uint32 s2 = (adler >> 16) & 0xFFFF;
	assert(len < 5552);
	while (len--) {
		s1 += *buf++;
		s2 += s1;
	}
	s1 %= 65521;
	s2 %= 65521;
	return (s2 << 16) | s1;
}

void sys_init(int w, int h, const char *title) {
	int i;
	SDL_AudioSpec as;
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption(title, NULL);

	key_mask = 0;
	sys_exit_flag = 0;

	assert(w < TILES_SURFACE_W && h < TILES_SURFACE_H);
	
	screen_w = w;
	screen_h = h;
	current_mode = SSM_BITMAP;
	memset(screen_palette, 0, sizeof(screen_palette));
	
	/* initialize screen surface */
	fullscreen_flag = 0;
	setup_screen_surface(fullscreen_flag, DEFAULT_SCALER);

	/* initialize offscreen buffers */
	memset(offscreen_buffers_table, 0, sizeof(offscreen_buffers_table));
	offscreen_buffers_table[OSB_COPPER].size = screen_w * screen_h;
	offscreen_buffers_table[OSB_TILES].size = TILES_SURFACE_W * TILES_SURFACE_H;
	offscreen_buffers_table[OSB_SPRITES].size = screen_w * screen_h;
	for (i = 0; i < ARRAYSIZE(offscreen_buffers_table); ++i) {
		offscreen_buffer_t *ob = &offscreen_buffers_table[i];
		ob->ptr = (uint8 *)malloc(ob->size);
		if (!ob->ptr) {
			print_error("Unable to allocate %d bytes", ob->size);
		}
	}
	
	/* initialize auto_dirty_blocks */
	memset(&auto_dirty_blocks, 0, sizeof(auto_dirty_blocks));
	auto_dirty_blocks.table_size = (screen_w / AUTO_DIRTY_BLOCK_W) * (screen_h / AUTO_DIRTY_BLOCK_H) * sizeof(uint32);
	auto_dirty_blocks.table_ptr[0] = (uint32 *)malloc(auto_dirty_blocks.table_size);
	auto_dirty_blocks.table_ptr[1] = (uint32 *)malloc(auto_dirty_blocks.table_size);
	if (!auto_dirty_blocks.table_ptr[0] || !auto_dirty_blocks.table_ptr[1]) {
		print_error("Unable to allocate auto_dirty_blocks");
	}
	memset(auto_dirty_blocks.table_ptr[0], 0, auto_dirty_blocks.table_size);
	memset(auto_dirty_blocks.table_ptr[1], 0, auto_dirty_blocks.table_size);
	
	/* initialize sound */
	memset(&as, 0, sizeof(as));
	as.freq = SOUND_SAMPLE_RATE;
	as.format = AUDIO_S8;
	as.channels = 1;
	as.samples = SOUND_BUFFER_SIZE;
	as.callback = sound_callback;
	if (SDL_OpenAudio(&as, NULL) == 0) {
		mixer_init(SOUND_SAMPLE_RATE);
		SDL_PauseAudio(0);
	} else {
		print_warning("Unable to open sound device");
	}	
	
	last_frame_time_stamp = SDL_GetTicks();
}

void sys_destroy() {
	int i;

	SDL_CloseAudio();
	free(auto_dirty_blocks.table_ptr[0]);
	free(auto_dirty_blocks.table_ptr[1]);
	memset(&auto_dirty_blocks, 0, sizeof(auto_dirty_blocks));
	for (i = 0; i < ARRAYSIZE(offscreen_buffers_table); ++i) {
		free(offscreen_buffers_table[i].ptr);
	}
	SDL_FreeSurface(offscreen_surface);
	SDL_FreeSurface(screen_surface);
	SDL_Quit();
}

void sys_set_screen_mode(sys_screen_mode_e mode) {
	current_mode = mode;
}

void sys_set_palette_bmp(const uint8 *pal_data, int num_colors) {
	int i;
	for (i = 0; i < num_colors; ++i) {
		uint8 r, g, b;
		b = pal_data[0];
		g = pal_data[1];
		r = pal_data[2];
		pal_data += 4;
		screen_palette[i] = SDL_MapRGB(screen_surface->format, r, g, b);
	}
}

static uint16 convert_amiga_color(SDL_PixelFormat *fmt, uint16 color) {
	uint8 r, g, b;
	r = (color >> 8) & 0xF;
	g = (color >> 4) & 0xF;
	b = (color >> 0) & 0xF;
	r |= r << 4;
	g |= g << 4;
	b |= b << 4;
	return SDL_MapRGB(fmt, r, g, b);
}

void sys_set_palette_spr(const uint8 *pal_data, int num_colors, int pal_num) {
	int i;
	for (i = 0; i < num_colors; ++i) {
		uint16 color = read_uint16BE(pal_data); pal_data += 2;
		screen_palette[pal_num * 16 + i] = convert_amiga_color(screen_surface->format, color);
	}
}

void sys_fade_in_palette() {
	int step, i;
	uint16 fade_palette[256];
	memcpy(fade_palette, screen_palette, 256 * sizeof(uint16));
	for (step = 0; step <= 16; ++step) {
		for (i = 0; i < 256; ++i) {
			uint8 r, g, b;
			SDL_GetRGB(fade_palette[i], screen_surface->format, &r, &g, &b);
			r = r * step >> 4;
			g = g * step >> 4;
			b = b * step >> 4;
			screen_palette[i] = SDL_MapRGB(screen_surface->format, r, g, b);
		}
		sys_update_screen();
		SDL_Delay(50);
	}
}

void sys_fade_out_palette() {
	int step, i;
	for (step = 16; step >= 0; --step) {
		for (i = 0; i < 256; ++i) {
			uint8 r, g, b;
			SDL_GetRGB(screen_palette[i], screen_surface->format, &r, &g, &b);
			r = r * step >> 4;
			g = g * step >> 4;
			b = b * step >> 4;
			screen_palette[i] = SDL_MapRGB(screen_surface->format, r, g, b);
		}
		sys_update_screen();
		SDL_Delay(50);
	}
}

void sys_blit_bitmap(const uint8 *bmp_data, int w, int h) {
	uint8 *dst = offscreen_buffers_table[OSB_COPPER].ptr;
	bmp_data += (h - 1) * w;
	assert(w <= screen_w && h <= screen_h);
	while (h--) {
		memcpy(dst, bmp_data, w);
		dst += screen_w;
		bmp_data -= w;
	}
}

static void draw_sprite(const uint8 *spr_data, int x, int y, int w, int h, int flags, int pal_num) {
	int j, i, b, p;
	uint8 *dst = offscreen_buffers_table[OSB_SPRITES].ptr + y * screen_w;
	for (j = 0; j < h; ++j) {
		int cy = y + j;
		for (i = 0; i < (w + 7) / 8; ++i) {
			uint8 data[4];
			data[0] = *spr_data++;
			data[1] = *spr_data++;
			data[2] = *spr_data++;
			data[3] = *spr_data++;
			for (b = 0; b < 8; ++b) {
				int cx = x + i * 8 + b;
				if (cx >= 0 && cx < screen_w && cy >= 0 && cy < screen_h) {
					uint8 color = 0;
					for (p = 0; p < 4; ++p) {
						if (data[p] & rev_bitmask_table[b]) {
							color |= 1 << p;
						}
					}
					if (color != 0) {
						if (flags & SSF_HITMODE) {
							color = 15;
						} else {
							color += pal_num * 16;
						}
						dst[cx] = color;
					}
				}
			}
		}
		dst += screen_w;
	}
}

static void display_sprite_list() {
	sprite_t *spr = sprite_list;
	while (spr->flags != 0xFFFF) {
		const uint8 *spr_data = spr->data + read_uint16LE(spr->data + spr->num * 2);
		const int spr_w = read_uint16LE(spr_data + 0);
		const int spr_h = read_uint16LE(spr_data + 2);
		if (spr->x + spr_w >= 0 && spr->x < screen_w && spr->y + spr_h >= 0 && spr->y < screen_h) {
			draw_sprite(spr_data + 4, spr->x, spr->y, spr_w, spr_h, spr->flags, spr->pal_num);
		}
		++spr;
	}
}

static void draw_background() {
	int j, i;
	uint8 *dst = offscreen_buffers_table[OSB_SPRITES].ptr;
	const uint8 *cop = offscreen_buffers_table[OSB_COPPER].ptr;
	const uint8 *src = offscreen_buffers_table[OSB_TILES].ptr + tilemap_origin_y * TILES_SURFACE_W + tilemap_origin_x;
	for (j = 0; j < screen_h; ++j) {
		for (i = 0; i < screen_w; ++i) {
			if (src[i] != 7) {
				dst[i] = src[i];
			} else {
				dst[i] = cop[i];
			}
		}
		dst += screen_w;
		cop += screen_w;
		src += TILES_SURFACE_W;
	}
}

static void scale_surface_rect(SDL_Surface *src_surface, SDL_Surface *dst_surface, int x, int y, int w, int h) {
	uint16 *dst, *src;
	int dst_pitch, src_pitch;
	const scaler_desc_t *sd;
	
	sd = &scaler_table[scaler_num];
	SDL_LockSurface(dst_surface);
	SDL_LockSurface(src_surface);
	dst_pitch = dst_surface->pitch >> 1;
	src_pitch = src_surface->pitch >> 1;
	dst = (uint16 *)dst_surface->pixels + y * sd->factor * dst_pitch + x * sd->factor;
	src = (uint16 *)src_surface->pixels + (y + 1) * src_pitch + (x + 1);
	(*sd->proc)(dst, dst_pitch, src, src_pitch, w, h);
	SDL_UnlockSurface(src_surface);
	SDL_UnlockSurface(dst_surface);
}

static void init_rect(SDL_Rect *r, int x, int y, int w, int h, int scale) {
	r->x = x * scale;
	r->y = y * scale;
	r->w = w * scale;
	r->h = h * scale;
}

static void update_screen_from_surface(const uint8 *src) {
	int y, x, i, j;
	uint16 *dst;
	uint32 *adb_new_ptr, *adb_old_ptr;
	
	/* initialize screen blocks checksums */
	adb_new_ptr = auto_dirty_blocks.table_ptr[0];
	for (i = 0; i < (screen_w / AUTO_DIRTY_BLOCK_W) * (screen_h / AUTO_DIRTY_BLOCK_H); ++i) {
		adb_new_ptr[i] = 1;
	}
	
	/* 'unpalettize' the game screen buffer and compute screen blocks checksums */
	SDL_LockSurface(offscreen_surface);
	dst = (uint16 *)offscreen_surface->pixels + (offscreen_surface->pitch >> 1) + 1;
	j = 0;
	for (y = 0; y < screen_h; ++y) {
		for (x = 0; x < screen_w; ++x) {
			dst[x] = screen_palette[src[x]];
		}
		for (i = 0; i < screen_w / AUTO_DIRTY_BLOCK_W; ++i) {
			adb_new_ptr[i] = adler32(adb_new_ptr[i], &dst[i * AUTO_DIRTY_BLOCK_W], AUTO_DIRTY_BLOCK_W);
		}
		src += screen_w;
		dst += offscreen_surface->pitch >> 1;
		++j;
		if (j == AUTO_DIRTY_BLOCK_H) {
			j = 0;
			adb_new_ptr += screen_w / AUTO_DIRTY_BLOCK_W;
		}
	}
	SDL_UnlockSurface(offscreen_surface);
	
	if (force_screen_redraw_flag || (prev_tilemap_origin_x != tilemap_origin_x || prev_tilemap_origin_y != tilemap_origin_y)) {
		/* if scrolling occurred, force redraw */
		scale_surface_rect(offscreen_surface, screen_surface, 0, 0, screen_w, screen_h);
		SDL_UpdateRect(screen_surface, 0, 0, 0, 0);
		force_screen_redraw_flag = 0;
	} else {
		int rects_count = 0;
		SDL_Rect rects_table[MAX_AUTO_DIRTY_BLOCKS];
		const int scale = scaler_table[scaler_num].factor;
		
		adb_new_ptr = auto_dirty_blocks.table_ptr[0];
		adb_old_ptr = auto_dirty_blocks.table_ptr[1];
		for (j = 0, y = 0; j < screen_h / AUTO_DIRTY_BLOCK_H; ++j, y += AUTO_DIRTY_BLOCK_H) {
			int hcount = 0;
			for (i = 0; i < screen_w / AUTO_DIRTY_BLOCK_W; ++i) {
				if (adb_new_ptr[i] != adb_old_ptr[i]) {
					++hcount;
				} else if (hcount != 0) {
					x = (i - hcount) * AUTO_DIRTY_BLOCK_W;
					assert(rects_count < MAX_AUTO_DIRTY_BLOCKS);
					init_rect(&rects_table[rects_count], x, y, hcount * AUTO_DIRTY_BLOCK_W, AUTO_DIRTY_BLOCK_H, scale);
					++rects_count;
					scale_surface_rect(offscreen_surface, screen_surface, x, y, hcount * AUTO_DIRTY_BLOCK_W, AUTO_DIRTY_BLOCK_H);
					hcount = 0;
				}
			}
			if (hcount != 0) {
				x = (i - hcount) * AUTO_DIRTY_BLOCK_W;
				assert(rects_count < MAX_AUTO_DIRTY_BLOCKS);
				init_rect(&rects_table[rects_count], x, y, hcount * AUTO_DIRTY_BLOCK_W, AUTO_DIRTY_BLOCK_H, scale);
				++rects_count;
				scale_surface_rect(offscreen_surface, screen_surface, x, y, hcount * AUTO_DIRTY_BLOCK_W, AUTO_DIRTY_BLOCK_H);
			}
			adb_new_ptr += screen_w / AUTO_DIRTY_BLOCK_W;
			adb_old_ptr += screen_w / AUTO_DIRTY_BLOCK_W;
		}
		SDL_UpdateRects(screen_surface, rects_count, rects_table);
		print_debug(DBG_SYSTEM, "auto_dirty_rects_count=%d", rects_count);
	}
	
	/* copy current blocks checksums for comparison with next screen blit */
	memcpy(auto_dirty_blocks.table_ptr[1], auto_dirty_blocks.table_ptr[0], auto_dirty_blocks.table_size);	
}

void sys_update_screen() {
	switch (current_mode) {
	case SSM_BITMAP:
		update_screen_from_surface(offscreen_buffers_table[OSB_COPPER].ptr);
		break;
	case SSM_TILED:
		draw_background();
		display_sprite_list();
		update_screen_from_surface(offscreen_buffers_table[OSB_SPRITES].ptr);
		break;
	}
}

static void draw_tile(uint8 *dst, int pitch, uint8 tile_num) {
	int y, d, b, p;
	const uint8 *src = tile_data + 16 * 8 * tile_num;
	for (y = 0; y < 16; ++y) {
		uint8 data[2][4];
		data[0][0] = *src++;
		data[1][0] = *src++;
		data[0][1] = *src++;
		data[1][1] = *src++;
		data[0][2] = *src++;
		data[1][2] = *src++;
		data[0][3] = *src++;
		data[1][3] = *src++;
		for (d = 0; d < 2; ++d) {
			for (b = 0; b < 8; ++b) {
				*dst = 0;
				for (p = 0; p < 4; ++p) {
					if (data[d][p] & rev_bitmask_table[b]) {
						*dst |= 1 << p;
					}
				}
				++dst;
			}
		}
		dst += pitch - 16;
	}
}

void sys_set_tile_data(const uint8 *data) {
	tile_data = data;
}

static void redraw_tilemap(const uint8 *tilemap, int x, int y) {
	int i, j;
	uint8 *dst = offscreen_buffers_table[OSB_TILES].ptr;
	for (j = 0; j < TILES_SURFACE_H / 16; ++j) {
		uint8 *p = dst;
		for (i = 0; i < TILES_SURFACE_W / 16; ++i) {
			uint8 tile = tilemap[(x + i) * 50 + (y + j)];
			draw_tile(p, TILES_SURFACE_W, tilemap_lut[tile]);
			p += 16;
		}
		dst += TILES_SURFACE_W * 16;
	}
}

void sys_set_tilemap_data(const uint8 *data, const uint8 *anim_lut) {
	tilemap_data = data;
	tilemap_lut = anim_lut;
	tilemap_origin_x = 0;
	tilemap_origin_y = 0;
	force_screen_redraw_flag = 1;
}

void sys_set_tilemap_origin(int x, int y) {
	prev_tilemap_origin_x = tilemap_origin_x;
	prev_tilemap_origin_y = tilemap_origin_y;
	tilemap_origin_x = x & 0xF;
	tilemap_origin_y = y & 0xF;
	redraw_tilemap(tilemap_data, x >> 4, y >> 4);
}

void sys_set_copper_pal(const uint8 *pal_data, int num_lines) {
	int i;
	const int offset = 256 - num_lines;
	uint8 *copper_buf = offscreen_buffers_table[OSB_COPPER].ptr;
	for (i = 0; i < num_lines; ++i) {
		uint16 color = read_uint16BE(pal_data); pal_data += 2;
		screen_palette[offset + i] = convert_amiga_color(screen_surface->format, color);
	}
	for (i = 0; i < num_lines; ++i) {
		memset(copper_buf, offset + i, screen_w);
		copper_buf += screen_w;
	}
}

void sys_clear_sprite_list() {
	sprite_list_tail = sprite_list;
	sprite_list_tail->flags = 0xFFFF;
}

void sys_add_to_sprite_list(const uint8 *data, int num, int x, int y, int flags, int pal_num) {
	if (sprite_list_tail < sprite_list + MAX_SPRITES - 1) {
		sprite_list_tail->data = data;
		sprite_list_tail->num = num;
		sprite_list_tail->x = x;
		sprite_list_tail->y = y;
		sprite_list_tail->flags = flags;
		sprite_list_tail->pal_num = pal_num;
		++sprite_list_tail;
		sprite_list_tail->flags = 0xFFFF;
	} else {
		print_warning("sys_add_to_sprite_list() sprite_list overflow");
	}
}

void sys_process_events() {
	SDL_Event ev;
	while (1) {
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				sys_exit_flag = 1;
				break;
			case SDL_ACTIVEEVENT:
				if (ev.active.state & SDL_APPINPUTFOCUS) {
					pause_flag = !ev.active.gain;
				}
				break;
			case SDL_KEYUP:
				print_debug(DBG_SYSTEM, "SDL_KEYUP %d", ev.key.keysym.sym);
				switch (ev.key.keysym.sym) {
				case SDLK_LEFT:
					key_mask &= ~SKM_LEFT;
					break;
				case SDLK_RIGHT:
					key_mask &= ~SKM_RIGHT;
					break;
				case SDLK_UP:
					key_mask &= ~SKM_UP;
					break;
				case SDLK_DOWN:
					key_mask &= ~SKM_DOWN;
					break;
				case SDLK_i:
					key_mask &= ~SKM_INVENTORY;
					break;
				case SDLK_p:
					key_mask &= ~SKM_PAUSE;
					break;
				case SDLK_RCTRL:
				case SDLK_SPACE:
					key_mask &= ~SKM_ACTION;
					break;
				case SDLK_F1:
					key_mask &= ~SKM_F1;
					break;
				case SDLK_F2:
					key_mask &= ~SKM_F2;
					break;
				case SDLK_ESCAPE:
					key_mask &= ~SKM_QUIT;
					break;
				default:
					break;
				}
				break;
			case SDL_KEYDOWN:
				print_debug(DBG_SYSTEM, "SDL_KEYDOWN %d", ev.key.keysym.sym);
				if (ev.key.keysym.mod & KMOD_ALT) {
					switch (ev.key.keysym.sym) {
					case SDLK_RETURN:
						change_screen_surface(!fullscreen_flag, scaler_num);
						break;
					case SDLK_KP_PLUS:
						change_screen_surface(fullscreen_flag, scaler_num + 1);
						break;
					case SDLK_KP_MINUS:
						change_screen_surface(fullscreen_flag, scaler_num - 1);
						break;
					default:
						break;
					}
					break;
				}
				switch (ev.key.keysym.sym) {
				case SDLK_LEFT:
					key_mask |= SKM_LEFT;
					break;
				case SDLK_RIGHT:
					key_mask |= SKM_RIGHT;
					break;
				case SDLK_UP:
					key_mask |= SKM_UP;
					break;
				case SDLK_DOWN:
					key_mask |= SKM_DOWN;
					break;
				case SDLK_i:
					key_mask |= SKM_INVENTORY;
					break;
				case SDLK_p:
					key_mask |= SKM_PAUSE;
					break;
				case SDLK_RCTRL:
				case SDLK_SPACE:
					key_mask |= SKM_ACTION;
					break;
				case SDLK_F1:
					key_mask |= SKM_F1;
					break;
				case SDLK_F2:
					key_mask |= SKM_F2;
					break;
				case SDLK_ESCAPE:
					key_mask |= SKM_QUIT;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		if (!pause_flag && SDL_GetTicks() - last_frame_time_stamp >= 2000 / 60) {
			last_frame_time_stamp = SDL_GetTicks();
			break;
		}
		SDL_Delay(10);
	}
}

int sys_wait_for_keys(int timeout, int mask) {
	const int timestamp_end = SDL_GetTicks() + timeout;
	do {
		sys_process_events();
		if ((key_mask & mask) != 0 || sys_exit_flag != 0) {
			break;
		}
		SDL_Delay(20);
	} while (SDL_GetTicks() < timestamp_end);
	return key_mask;
}

int *sys_get_key_mask() {
	return &key_mask;
}

void sys_play_sfx(const int8 *sample_data, uint32 sample_size, int sample_period) {
	SDL_LockAudio();
	mixer_play_sfx(sample_data, sample_size, sample_period);
	SDL_UnlockAudio();
}

void sys_play_module(const uint8 *module_data, const uint8 *instrument_data) {
	SDL_LockAudio();
	mpl_load(module_data, instrument_data);
	SDL_UnlockAudio();
}

void sys_stop_module() {
	SDL_LockAudio();
	mpl_stop();
	SDL_UnlockAudio();
}

void sys_print_string(const char *str) {
	fprintf(stdout, str);
}
