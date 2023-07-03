/*
 * UCON -
 * Copyright (C) 2010 Toshio Koike
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef ucon_hex_h
#define ucon_hex_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

void hex_init(font_t *font);
int hex_load(font_t *font, char *file_path);
int hex_destroy(font_t *font);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // ucon_hex_h

// End of hex.h
