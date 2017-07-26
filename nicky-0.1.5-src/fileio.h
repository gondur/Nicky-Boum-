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

#ifndef __FILEIO_H__
#define __FILEIO_H__

#include "util.h"

typedef enum {
	FIO_READ = 0,
	FIO_WRITE
} fio_open_mode_e;

extern void fio_init(const char *data_path);
extern int fio_open(const char *filename, fio_open_mode_e mode, int error_flag);
extern void fio_close(int slot);
extern int fio_size(int slot);
extern const uint8 *fio_fetch(int slot);
extern void fio_write(int slot, const uint8 *data, int len);

#endif /* __FILEIO_H__ */
