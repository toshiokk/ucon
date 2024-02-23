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

PRIVATE const u_short *font_get_undefined_glyph_bitmap__(
 font_t *font, wchar_t ucs21, u_short *pico_font, u_short *gly,
 int top_space, int bytes_per_digit, int digit_space);

u_short pico_font_3x6[16*3] = {
	/* 0 */
	0x3c00,		/* ..####.. */
	0x4200,		/* .#....#. */
	0x3c00,		/* ..####.. */
	/* 1 */
	0x4400,		/* .#...#.. */
	0x7e00,		/* .######. */
	0x4000,		/* .#...... */
	/* 2 */
	0x6200,		/* .##...#. */
	0x5200,		/* .#.#. #. */
	0x4c00,		/* .#..##.. */
	/* 3 */
	0x4a00,		/* .#..#.#. */
	0x4a00,		/* .#..#.#. */
	0x3400,		/* ..##.#.. */
	/* 4 */
	0x1800,		/* ...##... */
	0x1400,		/* ...#.#.. */
	0x7e00,		/* .######. */
	/* 5 */
	0x4e00,		/* .#..###. */
	0x4a00,		/* .#..#.#. */
	0x3200,		/* ..##..#. */
	/* 6 */
	0x3c00,		/* ..####.. */
	0x4a00,		/* .#..#.#. */
	0x3200,		/* ..##..#. */
	/* 7 */
	0x0600,		/* .....##. */
	0x7200,		/* .###..#. */
	0x0e00,		/* ....###. */
	/* 8 */
	0x3400,		/* ..##.#.. */
	0x4a00,		/* .#..#.#. */
	0x3400,		/* ..##.#.. */
	/* 9 */
	0x4c00,		/* .#..##.. */
	0x5200,		/* .#.#..#. */
	0x3c00,		/* ..####.. */
	/* A */
	0x7c00,		/* .#####.. */
	0x1200,		/* ...#..#. */
	0x7c00,		/* .#####.. */
	/* B */
	0x7e00,		/* .######. */
	0x4a00,		/* .#..#.#. */
	0x3400,		/* ..##.#.. */
	/* C */
	0x3c00,		/* ..####.. */
	0x4200,		/* .#....#. */
	0x4200,		/* .#....#. */
	/* D */
	0x7e00,		/* .######. */
	0x4200,		/* .#....#. */
	0x3c00,		/* ..####.. */
	/* E */
	0x7e00,		/* .######. */
	0x4a00,		/* .#..#.#. */
	0x4a00,		/* .#..#.#. */
	/* F */
	0x7e00,		/* .######. */
	0x0a00,		/* ....#.#. */
	0x0a00,		/* ....#.#. */
};
u_short pico_font_2x4[16*2] = {
//Pico-font:
// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
//     # ## ##  # ## ## ## ## ## #  #      # ## ##
// ##  #  #  # ## #  #   # ## ##  # #  ##  # #  # 
// ##  # #   # ##  # ##  # ##  # ## ## #  ## #  ##
// ##  # ## ##  # ## ##  # ##  # ## ## ## ## ## # 
	/* 0 */
	0x7000,		/* .###.... */
	0x7000,		/* .###.... */
	/* 1 */
	0x0000,		/* ........ */
	0x7800,		/* .####... */
	/* 2 */
	0x6800,		/* .##.#... */
	0x5800,		/* .#.##... */
	/* 3 */
	0x4800,		/* .#..#... */
	0x7800,		/* .####... */
	/* 4 */
	0x3000,		/* ..##.... */
	0x7800,		/* .####... */
	/* 5 */
	0x5800,		/* .#.##... */
	0x6800,		/* .##.#... */
	/* 6 */
	0x7800,		/* .####... */
	0x6800,		/* .##.#... */
	/* 7 */
	0x0800,		/* ....#... */
	0x7800,		/* .####... */
	/* 8 */
	0x7800,		/* .####... */
	0x7800,		/* .####... */
	/* 9 */
	0x5800,		/* .#.##... */
	0x7800,		/* .####... */
	/* A */
	0x6800,		/* .##.#... */
	0x7000,		/* .###.... */
	/* B */
	0x7800,		/* .####... */
	0x6000,		/* .##..... */
	/* C */
	0x7000,		/* .###.... */
	0x5000,		/* .#.#.... */
	/* D */
	0x6000,		/* .##..... */
	0x7800,		/* .####... */
	/* E */
	0x7800,		/* .####... */
	0x5800,		/* .#.##... */
	/* F */
	0x7800,		/* .####... */
	0x2800,		/* ..#.#... */
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

const u_short *font_get_undefined_glyph_bitmap(font_t *font, wchar_t ucs21)
{
	static u_short gly[PICOFONT_HEIGHT] = {
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
		0x0000,
	};

	switch(font_get_undefined_glyph_width(font, ucs21)) {
	case 16:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 1, 3, 1);
		break;
	case 14:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 1, 3, 0);
		break;
	case 12:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 1, 2, 1);
		break;
	case 10:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 1, 2, 0);
		break;
	case 8:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 1, 3, 1);
		break;
	case 7:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_3x6, gly, 1, 3, 0);
		break;
	case 6:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 1, 2, 1);
		break;
	case 5:
		font_get_undefined_glyph_bitmap__(font, ucs21, pico_font_2x4, gly, 1, 2, 0);
		break;
	}
	return gly;
}
PRIVATE const u_short *font_get_undefined_glyph_bitmap__(
 font_t *font, wchar_t ucs21, u_short *pico_font, u_short *gly,
 int top_space, int bytes_per_digit, int digit_space)
{
	u_short *gp;
	int bit;
	int hex;
	int off;

	gp = gly;
	if (top_space) {
		*gp++ = 0x0000;
	}
	for (bit = 12; bit >= 0; bit -= 4) {	// 12, 8, 4, 0
		hex = (ucs21 >> bit) & 0xf;		// 0xf000, 0x0f00, 0x00f0, 0x000f
		for (off = 0; off < bytes_per_digit; off++) {
			*gp++ = pico_font[hex * bytes_per_digit + off];
		}
		if (bit > 0 && digit_space) {
			*gp++ = 0x0000;
		}
	}
	return gly;
}

int font_get_undefined_glyph_width(font_t *font, wchar_t ucs21)
{
	int columns;

#ifdef UNDEFINED_FONT_WIDTH
	columns = UNDEFINED_FONT_WIDTH;
#else
	columns = wchar_columns(ucs21);
#endif
	return font->width * columns;
}

// End of picofont.c
