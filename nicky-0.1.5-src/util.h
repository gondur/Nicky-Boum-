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
 
#ifndef __UTIL_H__
#define __UTIL_H__

#ifdef NICKY_SDL_VERSION
#define GCC_PACK __attribute__((packed))
#define GCC_EXT
#define GCC_INT_CALL
#define GCC_ALIGN(x) __attribute__ ((__aligned__ (x)))
#endif

#ifdef NICKY_GBA_VERSION
#define GCC_PACK __attribute__((packed))
#define GCC_EXT __attribute__((section(".ewram")))
#define GCC_INT_CALL __attribute__ ((section(".iwram"),long_call))
#define GCC_ALIGN(x) __attribute__ ((__aligned__ (x)))
#endif

#define DBG_GAME       (1 << 0)
#define DBG_FILEIO     (1 << 1)
#define DBG_RESOURCE   (1 << 2)
#define DBG_SOUND      (1 << 3)
#define DBG_SYSTEM     (1 << 4)
#define DBG_MODPLAYER  (1 << 5)
#define DBG_INPUT      (1 << 6)

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned long uint32;
typedef signed long int32;

extern int util_debug_mask;

extern uint16 read_uint16LE(const void *ptr);
extern uint32 read_uint32LE(const void *ptr);
extern uint16 read_uint16BE(const void *ptr);
extern uint32 read_uint32BE(const void *ptr);
extern void write_uint16BE(void *ptr, uint16 n);
extern void write_uint32BE(void *ptr, uint32 n);
extern void string_lower(char *p);
extern void string_upper(char *p);
extern void print_debug(int debug_channel, const char *msg, ...);
extern void print_warning(const char *msg, ...);
extern void print_error(const char *msg, ...);

#endif /* __UTIL_H__ */
