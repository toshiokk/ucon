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

#include "ucon.h"

PRIVATE int get_hex_n_digits(char *buffer, int digits);

/*---------------------------------------------------------------------------*/
void hex_init(font_t *font)
{
	font->font_width = 8;
	font->font_height = 16;
	memset(font->glyph_bitmap, 0xff, sizeof(font->glyph_bitmap));
	memset(font->glyph_width, 0x00, sizeof(font->glyph_width));
	font->glyphs = -1;
}
int hex_destroy(font_t *font)
{
	hex_init(font);
}
//            0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 
//            1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 
// 5x10 "0000:0000A010801080500000"
//10x10 "3000:0000000000000000000000000000000000000000"
// 6x12 "0000:0000A0108010805000000000"
//12x12 "3000:000000000000000000000000000000000000000000000000"
// 7x14 "0000:0000A01080108050000000000000"
//14x14 "3000:00000000000000000000000000000000000000000000000000000000"
// 8x16 "0000:0000A010801080500000000000000000"
//16x16 "3000:0000000000000000000000000000000000000000000000000000000000000000"
#define HEX_CODE_LEN	5		// "3000:"
int hex_load(font_t *font, char *file_path)
{
#define MAX_LINE_LEN	100
	char filter[MAX_LINE_LEN+1];
	FILE *fp;
	char buffer[MAX_LINE_LEN+1];
	int char_code;
	char *bitmap;
	int len;
	int idx;
	int byte;
	int lines;
	int height;
	int width;

	verbose_printf("Loading font file: %s ... ", file_path);
	filter[0] = '\0';
	if (strlen(file_path) > 3
	 && strcmp(".gz", &file_path[strlen(file_path)-3]) == 0) {
		snprintf(filter, MAX_LINE_LEN+1, "zcat \"%s\" 2>/dev/null", file_path);
		if ((fp = popen(filter, "r")) <= 0) {
			return 1;
		}
	} else {
		if ((fp = fopen(file_path, "r")) == NULL) {
			return 1;
		}
	}
	lines = 0;
	for ( ; ; ) {
		if (fgets(buffer, MAX_LINE_LEN, fp) == NULL) {
			break;
		}
//flf_d_printf("<%s", buffer);
		char_code = get_hex_n_digits(buffer, 4);
		if (char_code >= 0) {
			lines++;
			bitmap = font->glyph_bitmap[char_code];
			len = strlen(buffer)-1;
			len = MIN_(len-HEX_CODE_LEN, MAX_FONT_HEIGHT * 2 * 2);
			for (idx = 0; idx < len; idx += 2) {
				byte = get_hex_n_digits(&buffer[HEX_CODE_LEN+idx], 2);
				if (byte < 0)
					break;
				*bitmap++ = byte;
			}
			height = len / 2;
//flf_d_printf("len:%d, height:%d\n", len, height);
			if (height <= MAX_FONT_HEIGHT) {
				// narrow
				width = height / 2;
				font->font_width = width;
				font->font_height = height;
			} else {
				// wide character
				height /= 2;
				width = height;
			}
//flf_d_printf("%04x:width:%d, height:%d\n", char_code, width, height);
			font->glyph_width[char_code] = width;
		}
	}
	if (filter[0]) {
		if (pclose(fp) < 0) {
			_FL_
			return 2;
		}
	} else {
		if (fclose(fp) < 0) {
			_FL_
			return 2;
		}
	}
	if (lines == 0) {
		return 3;
	}
	verbose_printf("OK\n");
///flf_d_printf("width:%d, height:%d\n", font->font_width, font->font_height);
	return 0;
}

PRIVATE int get_hex_n_digits(char *buffer, int digits)
{
	int hex;
	char chr;

#if 0
	// sscanf( , "%0?x", ) is too slow
	if (digits == 4) {
		if (sscanf(buffer, "%04x", &hex) > 0)
			return hex;
	} else if (digits == 2) {
		if (sscanf(buffer, "%02x", &hex) > 0)
			return hex;
	}
	return -1;
#else
	// fast replacement of sscanf(str, "%0?x", &hex)
	hex = 0;
	for ( ; digits > 0; digits--) {
		hex <<= 4;
		chr = *buffer++;
		if ('0' <= chr && chr <= '9')
			hex += chr - '0';
		else if ('A' <= chr && chr <= 'F')
			hex += chr - 'A' + 10;
		else if ('a' <= chr && chr <= 'f')
			hex += chr - 'a' + 10;
		else
			return -1;
	}
	return hex;
#endif
}

// End of hex.c
