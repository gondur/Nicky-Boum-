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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "systemstub.h"
#include "util.h"


int util_debug_mask = 0;


uint16 read_uint16LE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[1] << 8) | b[0];
}

uint32 read_uint32LE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
}

uint16 read_uint16BE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 8) | b[1];
}

uint32 read_uint32BE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

void write_uint16BE(void *ptr, uint16 n) {
	uint8 *b = (uint8 *)ptr;
	b[0] = n >> 8;
	b[1] = n & 0xFF;
}

void write_uint32BE(void *ptr, uint32 n) {
	uint8 *b = (uint8 *)ptr;
	b[0] = n >> 24;
	b[1] = (n >> 16) & 0xFF;
	b[2] = (n >> 8) & 0xFF;
	b[3] = n & 0xFF;
}

void string_lower(char *p) {
	for (; *p; ++p) {
		if (*p >= 'A' && *p <= 'Z') {
			*p += 'a' - 'A';
		}
	}
}

void string_upper(char *p) {
	for (; *p; ++p) {
		if (*p >= 'a' && *p <= 'z') {
			*p += 'A' - 'a';
		}
	}
}

void print_debug(int debug_channel, const char *msg, ...) {
	if (util_debug_mask & debug_channel) {
		char buf[256];
		va_list va;
		va_start(va, msg);
		vsprintf(buf, msg, va);
		va_end(va);
		sys_print_string(buf);
		sys_print_string("\n");
	}
}

void print_warning(const char *msg, ...) {
	char buf[256];
	va_list va;
	va_start(va, msg);
	vsprintf(buf, msg, va);
	va_end(va);
	sys_print_string("WARNING: ");
	sys_print_string(buf);
	sys_print_string("\n");
}

void print_error(const char *msg, ...) {
	char buf[256];
	va_list va;
	va_start(va, msg);
	vsprintf(buf, msg, va);
	va_end(va);
	sys_print_string("ERROR: ");
	sys_print_string(buf);
	sys_print_string("!\n");
	exit(-1);
}
