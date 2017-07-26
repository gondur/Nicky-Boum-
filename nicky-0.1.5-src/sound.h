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

#ifndef __SOUND_H__
#define __SOUND_H__

#include "util.h"

#define PAULA_FREQ 3546897

#define SND_MUS_SONGDEAD  0
#define SND_MUS_SONGMON   1
#define SND_MUS_SONGPRES  2
#define SND_MUS_SONGWIN   3
#define SND_MUS_SONGLEVEL 4

#define MAX_SND_MODULES 8

#define SND_SAMPLE_0    0
#define SND_SAMPLE_1    1 /* explosion */
#define SND_SAMPLE_2    2 /* silence */
#define SND_SAMPLE_3    3 /* bubble */
#define SND_SAMPLE_4    4 /* chest open */
#define SND_SAMPLE_5    5 /* bonus */
#define SND_SAMPLE_6    6 /* 'hop la' */
#define SND_SAMPLE_7    7 /* broken glass */
#define SND_SAMPLE_8    8
#define SND_SAMPLE_9    9
#define SND_SAMPLE_10  10 /* door */
#define SND_SAMPLE_11  11 /* monster explosion */
#define SND_SAMPLE_12  12 /* teleport */
#define SND_SAMPLE_13  13
#define SND_SAMPLE_14  14 /* 'yeah' */
#define SND_SAMPLE_15  15 /* 'youpie' */
#define SND_SAMPLE_16  16
#define SND_SAMPLE_19  19

#define MAX_SND_SAMPLES 20

typedef struct {
	uint16 offs;
	uint16 size;
	uint16 period;
	uint8 volume;
	uint16 unk;
	uint16 priority;
} sample_data_t;

extern const char *snd_module_table[MAX_SND_MODULES];
extern const sample_data_t snd_sample_data_table[MAX_SND_SAMPLES];

extern void snd_init();
extern void snd_stop();
extern void snd_play_song(int song);
extern void snd_play_sfx(int sfx);

#endif /* __SOUND_H__ */
