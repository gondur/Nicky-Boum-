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

#ifndef __INPUT_H__
#define __INPUT_H__

#include "util.h"

typedef enum {
	IRS_NONE = 0,
	IRS_REPLAY,
	IRS_RECORD
} input_recording_state_e;

extern uint8 inp_direction_mask, inp_fire_button, inp_fire_button_num_cycles_pressed;

extern void inp_init(input_recording_state_e rec_state);
extern void inp_start_level(int level_num);
extern void inp_end_level();
extern void inp_copy_state();

#endif /* __INPUT_H__ */
