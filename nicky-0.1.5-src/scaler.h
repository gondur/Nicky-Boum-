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

#ifndef __SCALER_H__
#define __SCALER_H__
 
#include "util.h"

typedef void (*scaler_pf)(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);

extern void point1x(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);
extern void point2x(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);
extern void point3x(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);
extern void point4x(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);
extern void scale2x(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);
extern void scale3x(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);
extern void scale4x(uint16 *dst, int dst_pitch, const uint16 *src, int src_pitch, int w, int h);

#endif /* __SCALER_H__ */
