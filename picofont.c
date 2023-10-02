/*
 * UCON -
 * Copyright (C) 2000 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#include "ucon.h"

PRIVATE const u_char *font_get_undefined_glyph_bitmap__(font_t *font, wchar_t ucs21,
 u_char *pico_font, u_char *gly,
 int bytes_per_line, int top_space, int bytes_per_digit, int digit_space);

u_char pico_font_3x6[16*3] = {
	/* 0 */
	0x3c,		/* ..####.. */
	0x42,		/* .#....#. */
	0x3c,		/* ..####.. */
	/* 1 */
	0x44,		/* .#...#.. */
	0x7e,		/* .######. */
	0x40,		/* .#...... */
	/* 2 */
	0x62,		/* .##...#. */
	0x52,		/* .#.#. #. */
	0x4c,		/* .#..##.. */
	/* 3 */
	0x4a,		/* .#..#.#. */
	0x4a,		/* .#..#.#. */
	0x34,		/* ..##.#.. */
	/* 4 */
	0x18,		/* ...##... */
	0x14,		/* ...#.#.. */
	0x7e,		/* .######. */
	/* 5 */
	0x4e,		/* .#..###. */
	0x4a,		/* .#..#.#. */
	0x32,		/* ..##..#. */
	/* 6 */
	0x3c,		/* ..####.. */
	0x4a,		/* .#..#.#. */
	0x32,		/* ..##..#. */
	/* 7 */
	0x06,		/* .....##. */
	0x72,		/* .###..#. */
	0x0e,		/* ....###. */
	/* 8 */
	0x34,		/* ..##.#.. */
	0x4a,		/* .#..#.#. */
	0x34,		/* ..##.#.. */
	/* 9 */
	0x4c,		/* .#..##.. */
	0x52,		/* .#.#..#. */
	0x3c,		/* ..####.. */
	/* A */
	0x7c,		/* .#####.. */
	0x12,		/* ...#..#. */
	0x7c,		/* .#####.. */
	/* B */
	0x7e,		/* .######. */
	0x4a,		/* .#..#.#. */
	0x34,		/* ..##.#.. */
	/* C */
	0x3c,		/* ..####.. */
	0x42,		/* .#....#. */
	0x42,		/* .#....#. */
	/* D */
	0x7e,		/* .######. */
	0x42,		/* .#....#. */
	0x3c,		/* ..####.. */
	/* E */
	0x7e,		/* .######. */
	0x4a,		/* .#..#.#. */
	0x4a,		/* .#..#.#. */
	/* F */
	0x7e,		/* .######. */
	0x0a,		/* ....#.#. */
	0x0a,		/* ....#.#. */
};
u_char pico_font_2x4[16*2] = {
//Pico-font:
// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
//     # ## ##  # ## ## ## ## ## #  #      # ## ##
// ##  #  #  # ## #  #   # ## ##  # #  ##  # #  # 
// ##  # #   # ##  # ##  # ##  # ## ## #  ## #  ##
// ##  # ## ##  # ## ##  # ##  # ## ## ## ## ## # 
	/* 0 */
	0x70,		/* .###.... */
	0x70,		/* .###.... */
	/* 1 */
	0x00,		/* ........ */
	0x78,		/* .####... */
	/* 2 */
	0x68,		/* .##.#... */
	0x58,		/* .#.##... */
	/* 3 */
	0x48,		/* .#..#... */
	0x78,		/* .####... */
	/* 4 */
	0x30,		/* ..##.... */
	0x78,		/* .####... */
	/* 5 */
	0x58,		/* .#.##... */
	0x68,		/* .##.#... */
	/* 6 */
	0x78,		/* .####... */
	0x68,		/* .##.#... */
	/* 7 */
	0x08,		/* ....#... */
	0x78,		/* .####... */
	/* 8 */
	0x78,		/* .####... */
	0x78,		/* .####... */
	/* 9 */
	0x58,		/* .#.##... */
	0x78,		/* .####... */
	/* A */
	0x68,		/* .##.#... */
	0x70,		/* .###.... */
	/* B */
	0x78,		/* .####... */
	0x60,		/* .##..... */
	/* C */
	0x70,		/* .###.... */
	0x50,		/* .#.#.... */
	/* D */
	0x60,		/* .##..... */
	0x78,		/* .####... */
	/* E */
	0x78,		/* .####... */
	0x58,		/* .#.##... */
	/* F */
	0x78,		/* .####... */
	0x28,		/* ..#.#... */
};

// 0123    CDEF    
// +------++------+
// | #### || #### |
// |#    #||#    #|
// | #### ||#    #|
// |      ||      |
// |#   # ||######|
// |######||#    #|
// |#     || #### |
// |      ||      |
// |##   #||######|
// |# #  #||#  # #|
// |#  ## ||#  # #|
// |      ||      |
// |#  # #||######|
// |#  # #||   # #|
// +- ###-++---#-#+

// 0123 CDEF 
// +---++---+
// |### .###.
// |### .#.#.
// |... .##..
// |####.####
// |##.#.####
// |#.##.#.##
// |#..#.####
// |####..#.#
// +---++---+

const u_char *font_get_undefined_glyph_bitmap(font_t *font, wchar_t ucs21)
{
	static u_char gly[PICOFONT_HEIGHT*2] = {
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
	};

	switch(font_get_undefined_glyph_width(font, ucs21)) {
	case 16:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 2, 1, 3, 1);
		break;
	case 14:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 2, 1, 3, 0);
		break;
	case 12:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 2, 1, 2, 1);
		break;
	case 10:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 2, 1, 2, 0);
		break;
	case 8:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 1, 1, 3, 1);
		break;
	case 7:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 1, 1, 3, 0);
		break;
	case 6:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 1, 1, 2, 1);
		break;
	case 5:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 1, 1, 2, 0);
		break;
	}
	return gly;
}
PRIVATE const u_char *font_get_undefined_glyph_bitmap__(font_t *font, wchar_t ucs21,
 u_char *pico_font, u_char *gly,
 int bytes_per_line, int top_space, int bytes_per_digit, int digit_space)
{
	u_char *gp;
	int bit;
	int hex;
	int off;

	gp = gly;
	if (top_space) {
		*gp++ = 0x00;
		if (bytes_per_line >= 2) {
			*gp++ = 0x00;
		}
	}
	for (bit = 12; bit >= 0; bit -= 4) {	// 12, 8, 4, 0
		hex = (ucs21 >> bit) & 0xf;
		for (off = 0; off < bytes_per_digit; off++) {
			*gp++ = pico_font[hex * bytes_per_digit + off];
			if (bytes_per_line >= 2) {
				*gp++ = 0x00;
			}
		}
		if (bit > 0 && digit_space) {
			*gp++ = 0x00;
			if (bytes_per_line >= 2) {
				*gp++ = 0x00;
			}
		}
	}
	return gly;
}

int font_get_undefined_glyph_width(font_t *font, wchar_t ucs21)
{
	int columns;

//#define UNDEFINED_FONT_WIDTH	1
///#define UNDEFINED_FONT_WIDTH	2
#ifdef UNDEFINED_FONT_WIDTH
	columns = UNDEFINED_FONT_WIDTH;
#else
	columns = wchar_columns(ucs21);
#endif
	return font->font_width * columns;
}

// End of picofont.c
