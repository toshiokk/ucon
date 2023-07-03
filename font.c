/*
 * UCON -
 * Copyright (C) 2003 Fumitoshi Ukai <ukai@debian.or.jp>
 * Copyright (C) 1999 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
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

font_t fonts__[MAX_FONTS];
int cur_font_multi_idx = 0;
font_t *cur_font;
int cur_font_idx;
int cur_font_expand_x;
int cur_font_expand_y;

#define	SQUARE_SHAPE
#define	TALL_SHAPE
///#define	HUGE_SHAPE

font_multi_t font_multi_table[] = {
	{ 0, 10, 1, 1, },	//  0:  5x10=  50
#ifdef	SQUARE_SHAPE
	{ 0, 10, 2, 1, },	//  1: 10x10= 100
#endif
#ifdef	TALL_SHAPE
	{ 0, 10, 1, 2, },	//  2:  5x20= 100
#endif
	{ 0, 10, 2, 2, },	//  3: 10x20= 200
	{ 0, 10, 2, 3, },	//  4: 10x30= 300
#ifdef	SQUARE_SHAPE
	{ 0, 10, 3, 2, },	//  5: 15x20= 300
	{ 0, 10, 4, 2, },	//  6: 20x20= 400
#endif
	{ 0, 10, 3, 3, },	//  7: 15x30= 450
	{ 0, 10, 3, 4, },	//  8: 15x40= 600
	{ 0, 10, 4, 4, },	//  9: 20x40= 800
	{ 1, 12, 1, 1, },	// 10:  6x12=  72
#ifdef	TALL_SHAPE
	{ 1, 12, 1, 2, },	// 11:  6x24= 144
#endif
	{ 1, 12, 2, 2, },	// 12: 12x24= 288
	{ 1, 12, 2, 3, },	// 13: 12x36= 432
	{ 1, 12, 3, 3, },	// 14: 18x36= 648
	{ 1, 12, 3, 4, },	// 15: 18x48= 864
#ifdef	HUGE_SHAPE
	{ 1, 12, 4, 4, },	// 16: 24x48=1152
#endif
	{ 2, 14, 1, 1, },	// 17:  7x14=  82
#ifdef	TALL_SHAPE
	{ 2, 14, 1, 2, },	// 18:  7x28= 164
#endif
	{ 2, 14, 2, 2, },	// 19: 14x28= 392
	{ 2, 14, 2, 3, },	// 10: 14x42= 588
	{ 2, 14, 3, 3, },	// 11: 21x42= 882
#ifdef	HUGE_SHAPE
	{ 2, 14, 3, 4, },	// 12: 21x56=1176
	{ 2, 14, 4, 4, },	// 23: 28x56=1568
#endif
	{ 3, 16, 1, 1, },	// 24:  8x16= 128
#ifdef	TALL_SHAPE
	{ 3, 16, 1, 2, },	// 25:  8x32= 256
#endif
	{ 3, 16, 2, 2, },	// 26: 16x32= 192
	{ 3, 16, 2, 3, },	// 27: 16x48= 288
#ifdef	HUGE_SHAPE
	{ 3, 16, 3, 3, },	// 28: 24x48=1152
	{ 3, 16, 3, 4, },	// 29: 24x64=1536
	{ 3, 16, 4, 4, },	// 30: 32x64=2048
#endif
};
#define	FONT_MULTI_TABLE_ENTRIES	(sizeof(font_multi_table) / sizeof(font_multi_table[0]))

PRIVATE int comp_font_multi_t(const void *aa, const void *bb);
PRIVATE int calc_font_pixel_size(font_multi_t *font_multi);
PRIVATE int font_select__(int font_size, int multi_x, int multi_y);
int font_select(int font_size, int multi_x, int multi_y)
{
	int font_multi_idx;

flf_d_printf("font_multi_table entries: %d\n", FONT_MULTI_TABLE_ENTRIES);
///
#define	SORT_BY_SHAPE_SIZE
#ifdef	SORT_BY_SHAPE_SIZE
	qsort(font_multi_table, FONT_MULTI_TABLE_ENTRIES, sizeof(font_multi_t), comp_font_multi_t);
#endif
	if ((font_multi_idx = font_select__(font_size, multi_x, multi_y)) >= 0) {
		return font_multi_idx;
	}
	multi_y = 0;
	if ((font_multi_idx = font_select__(font_size, multi_x, multi_y)) >= 0) {
		return font_multi_idx;
	}
	multi_x = 0;
	if ((font_multi_idx = font_select__(font_size, multi_x, multi_y)) >= 0) {
		return font_multi_idx;
	}
	font_size = 0;
	if ((font_multi_idx = font_select__(font_size, multi_x, multi_y)) >= 0) {
		return font_multi_idx;
	}
	return -1;	// no font selected
}
PRIVATE int comp_font_multi_t(const void *aa, const void *bb)
{
	return calc_font_pixel_size((font_multi_t *)aa) - calc_font_pixel_size((font_multi_t *)bb);
}
PRIVATE int calc_font_pixel_size(font_multi_t *font_multi)
{
	font_t *font;

	font = &fonts__[font_multi->font_idx];
	return font->font_width * font_multi->multi_x
	 * font->font_height * font_multi->multi_y;
}
PRIVATE int font_select__(int font_size, int multi_x, int multi_y)
{
	int font_multi_idx;

flf_d_printf("font_size: %d, multi_x: %d, multi_y: %d\n", font_size, multi_x, multi_y);
	for (font_multi_idx = 0; font_multi_idx < FONT_MULTI_TABLE_ENTRIES;
	 font_multi_idx++) {
		if (fonts__[font_multi_table[font_multi_idx].font_idx].glyphs > 0
		 && (font_size == 0 || font_multi_table[font_multi_idx].font_size == font_size)
		 && (multi_x == 0 || font_multi_table[font_multi_idx].multi_x == multi_x)
		 && (multi_y == 0 || font_multi_table[font_multi_idx].multi_y == multi_y)) {
			return font_multi_idx;
		}
	}
	return -1;
}
int font_correct_selection(int font_multi_idx)
{
	int next_font_multi_idx;

	if (font_check_selection(font_multi_idx)) {
		return font_multi_idx;
	}
	next_font_multi_idx = font_select_next(font_multi_idx, -1);
	if (font_check_selection(next_font_multi_idx)) {
		return next_font_multi_idx;
	}
	next_font_multi_idx = font_select_next(font_multi_idx, +1);
	if (font_check_selection(next_font_multi_idx)) {
		return next_font_multi_idx;
	}
	return 0;
}
int font_select_next(int font_multi_idx, int shift)
{
	int prev_font_multi_idx = font_multi_idx;

	for (font_multi_idx += shift;
	 IS_IN_RANGE(0, font_multi_idx, FONT_MULTI_TABLE_ENTRIES);
	 font_multi_idx += shift) {
		if (fonts__[font_multi_table[font_multi_idx].font_idx].glyphs) {
			return font_multi_idx;
		}
	}
	return prev_font_multi_idx;
}
int font_check_selection(int font_multi_idx)
{
	return IS_IN_RANGE(0, font_multi_idx, FONT_MULTI_TABLE_ENTRIES)
	 && fonts__[font_multi_table[font_multi_idx].font_idx].glyphs;
}
font_t *font_get_font(int font_multi_idx, int *font_idx, int *multi_x, int *multi_y)
{
	*font_idx = font_multi_table[font_multi_idx].font_idx;
	*multi_x = font_multi_table[font_multi_idx].multi_x;
	*multi_y = font_multi_table[font_multi_idx].multi_y;
flf_d_printf("font_multi_idx: %d, font_idx: %d, multi_x: %d, multi_y: %d\n",
 font_multi_idx, *font_idx, *multi_x, *multi_y);
	return &fonts__[font_multi_table[font_multi_idx].font_idx];
}

void font_init_all(void)
{
	int font_idx;

	for (font_idx = 0; font_idx < MAX_FONTS; font_idx++) {
		font_init(&fonts__[font_idx]);
	}
}
int font_load_all(void)
{
	int fonts = 0;

	font_init_all();
#ifdef ENABLE_FONT10
	if (font_load(&fonts__[0], "ucon10.hex.gz") == 0)
		fonts++;
#endif // ENABLE_FONT10
#ifdef ENABLE_FONT12
	if (font_load(&fonts__[1], "ucon12.hex.gz") == 0)
		fonts++;
#endif // ENABLE_FONT12
#ifdef ENABLE_FONT14
	if (font_load(&fonts__[2], "ucon14.hex.gz") == 0)
		fonts++;
#endif // ENABLE_FONT14
#ifdef ENABLE_FONT16
	if (font_load(&fonts__[3], "ucon16.hex.gz") == 0)
		fonts++;
#endif // ENABLE_FONT16
flf_d_printf("%d fonts loaded\n", fonts);
	return fonts;
}
int font_destroy_all(void)
{
	font_destroy(&fonts__[0]);
	font_destroy(&fonts__[1]);
	font_destroy(&fonts__[2]);
	font_destroy(&fonts__[3]);
	return 0;
}

void font_init(font_t *font)
{
	memset(font, 0x00, sizeof(*font));
}
int font_load(font_t *font, char *file_name)
{
#define MAX_PATH_LEN	256
	char file_path[MAX_PATH_LEN+1];

///printf_stderr("trying to load font [%s]\n", file_name);
	hex_init(font);

	// $HOME/bin/fontfile
	snprintf(file_path, MAX_PATH_LEN+1, "%s/bin/%s", 
	 getenv("HOME"), file_name);
	if (hex_load(font, file_path) == 0)
		goto font_loaded;

	// /usr/local/bin/fontfile
	snprintf(file_path, MAX_PATH_LEN+1, "/usr/local/bin/%s", file_name);
///printf_stderr("trying to load font [%s]\n", file_path);
	if (hex_load(font, file_path) == 0)
		goto font_loaded;

	printf_stderr("!!!! ERROR: CAN NOT load font [%s] !!!!\n", file_name);
	return -1;
font_loaded:;
	font_count_glyphs(font);
flf_d_printf("font_name: %s, glyphs: %d, width: %d, height: %d\n",
 file_name, font->glyphs, font->font_width, font->font_height);
	return 0;
}
int font_is_loaded(font_t *font)
{
	return font_count_glyphs(font);
}
int font_count_glyphs(font_t *font)
{
	long glyphs = 0;
	int glyph_idx;

	if (font->glyphs < 0) {
		for (glyph_idx = 0; glyph_idx < MAX_GLYPHS; glyph_idx++) {
			if (font->glyph_width[glyph_idx])
				glyphs++;
		}
		font->glyphs = glyphs;
	}
	return font->glyphs;
}
int font_destroy(font_t *font)
{
	return hex_destroy(font);
}

const u_char *font_get_glyph_bitmap(font_t *font, wchar_t ucs21,
 int *width_in_pixels, int *unknown)
{
	int width;
	const u_char *glyph;

	width = font->glyph_width[ucs21];
	*width_in_pixels = font_get_glyph_width(font, ucs21);
	if (width) {
		glyph = font->glyph_bitmap[ucs21];
		*unknown = 0;
	} else {
		glyph = font_get_undefined_glyph_bitmap(font, ucs21);
		*unknown = 1;
	}
	return glyph;
}
const int font_get_glyph_width(font_t *font, wchar_t ucs21)
{
	int width_in_pixels;

	width_in_pixels = font->glyph_width[ucs21];
	if (width_in_pixels == 0) {
		width_in_pixels = font_get_undefined_glyph_width(font, ucs21);
	}
	return width_in_pixels;
}

// End of font.c
