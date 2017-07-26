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

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "util.h"
#include "common.h"

extern uint8 res_decor_cdg[20000];
extern anim_data_t res_ref_ref[282];
extern const uint8 *res_digits_spr, *res_lifebar_spr, *res_nicky_spr, *res_level_spr, *res_monster_spr, *res_decor_blk, *res_decor_ref, *res_posit_ref, *res_decor_pal, *res_flash_pal;

extern void res_load_level_data(int level);
extern void res_unload_level_data();

#endif /* __RESOURCE_H__ */
