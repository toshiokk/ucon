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

int cur_font_idx = 0;
font_t *cur_font;

int cur_font_mul_idx = 0;
font_mul_t *cur_font_mul;

font_exp_t cur_font_exp_, *cur_font_exp = &cur_font_exp_;

///
#define SQUARE_SHAPE
#define TALL_SHAPE
///
#define HUGE_SHAPE

font_mul_t font_mul_table[] = {
	{ 0, /*10,*/ 1, 1, },	//  5x10=  50
#ifdef TALL_SHAPE
	{ 0, /*10,*/ 1, 2, },	//  5x20= 100
#endif // TALL_SHAPE
#ifdef SQUARE_SHAPE
	{ 0, /*10,*/ 2, 1, },	// 10x10= 100
#endif // SQUARE_SHAPE
	{ 0, /*10,*/ 2, 2, },	// 10x20= 200
	{ 0, /*10,*/ 2, 3, },	// 10x30= 300
#ifdef TALL_SHAPE
	{ 0, /*10,*/ 2, 4, },	// 10x40= 400
#endif // TALL_SHAPE
	{ 0, /*10,*/ 3, 2, },	// 15x20= 300
	{ 0, /*10,*/ 3, 3, },	// 15x30= 450
	{ 0, /*10,*/ 3, 4, },	// 15x40= 600
	{ 0, /*10,*/ 3, 5, },	// 15x50= 750
#ifdef TALL_SHAPE
	{ 0, /*10,*/ 3, 6, },	// 15x60= 900
#endif // TALL_SHAPE
#ifdef SQUARE_SHAPE
	{ 0, /*10,*/ 4, 2, },	// 20x20= 400
#endif // SQUARE_SHAPE
	{ 0, /*10,*/ 4, 3, },	// 20x30= 600
	{ 0, /*10,*/ 4, 4, },	// 20x40= 800
#ifdef HUGE_SHAPE
	{ 0, /*10,*/ 4, 6, },	// 20x60=1200
#ifdef TALL_SHAPE
	{ 0, /*10,*/ 4, 8, },	// 20x80=1600
#endif // TALL_SHAPE
#endif // HUGE_SHAPE
	{ 0, /*10,*/ 5, 3, },	// 25x30= 750
#ifdef HUGE_SHAPE
	{ 0, /*10,*/ 5, 5, },	// 25x50=1250
	{ 0, /*10,*/ 5, 7, },	// 25x70=1750
#ifdef TALL_SHAPE
	{ 0, /*10,*/ 5, 9, },	// 25x90=2250
#endif // TALL_SHAPE
	{ 0, /*10,*/ 6, 4, },	// 30x 40=1200
	{ 0, /*10,*/ 6, 6, },	// 30x 60=1800
	{ 0, /*10,*/ 6, 9, },	// 30x 90=2700
#ifdef TALL_SHAPE
	{ 0, /*10,*/ 6,12, },	// 30x120=3600
#endif // TALL_SHAPE
#endif // HUGE_SHAPE
//-----------------------------------------------------------------------------
	{ 1, /*12,*/ 1, 1, },	//  6x12=  72
#ifdef TALL_SHAPE
	{ 1, /*12,*/ 1, 2, },	//  6x24= 144
#endif // TALL_SHAPE
#ifdef SQUARE_SHAPE
	{ 1, /*12,*/ 2, 1, },	// 12x12= 144
#endif // SQUARE_SHAPE
	{ 1, /*12,*/ 2, 2, },	// 12x24= 288
	{ 1, /*12,*/ 2, 3, },	// 12x36= 432
#ifdef TALL_SHAPE
	{ 1, /*12,*/ 2, 4, },	// 12x48= 576
#endif // TALL_SHAPE
	{ 1, /*12,*/ 3, 2, },	// 18x24= 432
	{ 1, /*12,*/ 3, 3, },	// 18x36= 648
	{ 1, /*12,*/ 3, 4, },	// 18x48= 864
#ifdef HUGE_SHAPE
	{ 1, /*12,*/ 3, 5, },	// 18x60=1080
#ifdef TALL_SHAPE
	{ 1, /*12,*/ 3, 6, },	// 18x72=1296
#endif // TALL_SHAPE
#endif // HUGE_SHAPE
	{ 1, /*12,*/ 4, 3, },	// 24x36= 864
#ifdef HUGE_SHAPE
	{ 1, /*12,*/ 4, 4, },	// 24x48=1152
	{ 1, /*12,*/ 4, 6, },	// 24x72=1728
#ifdef TALL_SHAPE
	{ 1, /*12,*/ 4, 8, },	// 24x96=2304
#endif // TALL_SHAPE
	{ 1, /*12,*/ 5, 3, },	// 30x 36=1080
	{ 1, /*12,*/ 5, 5, },	// 30x 60=1800
	{ 1, /*12,*/ 5, 7, },	// 30x 72=2160
#ifdef TALL_SHAPE
	{ 1, /*12,*/ 5, 9, },	// 30x108=3240
#endif // TALL_SHAPE
	{ 1, /*12,*/ 6, 4, },	// 36x 48=1728
	{ 1, /*12,*/ 6, 6, },	// 36x 72=2592
	{ 1, /*12,*/ 6, 9, },	// 36x108=3888
#ifdef TALL_SHAPE
	{ 1, /*12,*/ 6,12, },	// 36x144=5184
#endif // TALL_SHAPE
#endif // HUGE_SHAPE
//-----------------------------------------------------------------------------
	{ 2, /*14,*/ 1, 1, },	//  7x14=  98
#ifdef	TALL_SHAPE
	{ 2, /*14,*/ 1, 2, },	//  7x28= 196
#endif // TALL_SHAPE
#ifdef	SQUARE_SHAPE
	{ 2, /*14,*/ 2, 1, },	// 14x14= 196
#endif // SQUARE_SHAPE
	{ 2, /*14,*/ 2, 2, },	// 14x28= 392
	{ 2, /*14,*/ 2, 3, },	// 14x42= 588
	{ 2, /*14,*/ 3, 2, },	// 21x28= 588
	{ 2, /*14,*/ 3, 3, },	// 21x42= 882
#ifdef	HUGE_SHAPE
	{ 2, /*14,*/ 3, 4, },	// 21x 56=1176
	{ 2, /*14,*/ 4, 3, },	// 28x 42=1176
	{ 2, /*14,*/ 4, 4, },	// 28x 56=1568
	{ 2, /*14,*/ 4, 6, },	// 28x 84=2352
#ifdef	TALL_SHAPE
	{ 2, /*14,*/ 4, 8, },	// 28x112=3136
#endif // TALL_SHAPE
	{ 2, /*14,*/ 5, 3, },	// 35x 42=1470
	{ 2, /*14,*/ 5, 4, },	// 35x 56=1960
	{ 2, /*14,*/ 5, 5, },	// 35x 70=2450
	{ 2, /*14,*/ 5, 7, },	// 35x 98=3430
#ifdef	TALL_SHAPE
	{ 2, /*14,*/ 5,10, },	// 35x140=4900
#endif // TALL_SHAPE
#endif // HUGE_SHAPE
//-----------------------------------------------------------------------------
	{ 3, /*16,*/ 1, 1, },	//  8x16= 128
#ifdef TALL_SHAPE
	{ 3, /*16,*/ 1, 2, },	//  8x32= 256
#endif // TALL_SHAPE
#ifdef SQUARE_SHAPE
	{ 3, /*16,*/ 2, 1, },	// 16x16= 256
#endif // SQUARE_SHAPE
	{ 3, /*16,*/ 2, 2, },	// 16x32= 512
	{ 3, /*16,*/ 2, 3, },	// 16x48= 768
#ifdef HUGE_SHAPE
#ifdef TALL_SHAPE
	{ 3, /*16,*/ 2, 4, },	// 16x64=1024
#endif // TALL_SHAPE
#endif // HUGE_SHAPE
	{ 3, /*16,*/ 3, 2, },	// 24x32= 768
#ifdef HUGE_SHAPE
	{ 3, /*16,*/ 3, 3, },	// 24x48=1152
	{ 3, /*16,*/ 3, 4, },	// 24x64=1536
#ifdef TALL_SHAPE
	{ 3, /*16,*/ 3, 6, },	// 24x96=2304
#endif // TALL_SHAPE
	{ 3, /*16,*/ 4, 3, },	// 32x48=1536
	{ 3, /*16,*/ 4, 4, },	// 32x64=2048
	{ 3, /*16,*/ 4, 6, },	// 32x96=3072
#ifdef TALL_SHAPE
	{ 3, /*16,*/ 4, 8, },	// 32x128=4096
#endif // TALL_SHAPE
	{ 3, /*16,*/ 5, 3, },	// 40x 48=1920
	{ 3, /*16,*/ 5, 4, },	// 40x 64=2560
	{ 3, /*16,*/ 5, 5, },	// 40x 80=3200
	{ 3, /*16,*/ 5, 7, },	// 40x118=4720
#ifdef TALL_SHAPE
	{ 3, /*16,*/ 5,10, },	// 40x160=6400
#endif // TALL_SHAPE
#endif // HUGE_SHAPE
};
#define	FONT_MULTI_TABLE_ENTRIES	(sizeof(font_mul_table) / sizeof(font_mul_table[0]))

PRIVATE int comp_font_area(const void *aa, const void *bb);
PRIVATE int comp_font_width(const void *aa, const void *bb);
PRIVATE int calc_font_area(font_mul_t *font_multi);
PRIVATE int calc_font_width(font_mul_t *font_multi);
PRIVATE int calc_font_height(font_mul_t *font_multi);
PRIVATE int font_search_by_height_mul_xy(int font_size, int mul_x, int mul_y);
int font_select_by_height_mul_xy(int font_size, int mul_x, int mul_y)
{
	int font_mul_idx;

	flf_d_printf("font_mul_table entries: %d\n", FONT_MULTI_TABLE_ENTRIES);
///
#define	SORT_BY_SHAPE_SIZE
#ifdef	SORT_BY_SHAPE_SIZE
	qsort(font_mul_table, FONT_MULTI_TABLE_ENTRIES, sizeof(font_mul_t), comp_font_area);
	qsort(font_mul_table, FONT_MULTI_TABLE_ENTRIES, sizeof(font_mul_t), comp_font_width);
#endif
	if ((font_mul_idx = font_search_by_height_mul_xy(font_size, mul_x, mul_y)) >= 0) {
		return font_mul_idx;
	}
	mul_y = 0;
	if ((font_mul_idx = font_search_by_height_mul_xy(font_size, mul_x, mul_y)) >= 0) {
		return font_mul_idx;
	}
	mul_x = 0;
	if ((font_mul_idx = font_search_by_height_mul_xy(font_size, mul_x, mul_y)) >= 0) {
		return font_mul_idx;
	}
	font_size = 0;
	if ((font_mul_idx = font_search_by_height_mul_xy(font_size, mul_x, mul_y)) >= 0) {
		return font_mul_idx;
	}
	return -1;	// no font selected
}

PRIVATE int comp_font_area(const void *aa, const void *bb)
{
	return calc_font_area((font_mul_t *)aa) - calc_font_area((font_mul_t *)bb);
}
PRIVATE int comp_font_width(const void *aa, const void *bb)
{
	int diff = calc_font_width((font_mul_t *)aa) - calc_font_width((font_mul_t *)bb);
	if (diff) {
		return diff;
	}
	return calc_font_height((font_mul_t *)aa) - calc_font_height((font_mul_t *)bb);
}
PRIVATE int calc_font_area(font_mul_t *font_multi)
{
	font_t *font;

	font = &fonts__[font_multi->font_idx];
	return font->width * font_multi->mul_x * font->height * font_multi->mul_y;
}
PRIVATE int calc_font_width(font_mul_t *font_multi)
{
	font_t *font;

	font = &fonts__[font_multi->font_idx];
	return font->width * font_multi->mul_x;
}
PRIVATE int calc_font_height(font_mul_t *font_multi)
{
	font_t *font;

	font = &fonts__[font_multi->font_idx];
	return font->height * font_multi->mul_y;
}

PRIVATE int font_search_by_height_mul_xy(int font_size, int mul_x, int mul_y)
{
	int font_mul_idx;

	flf_d_printf("font_size: %d, mul_x: %d, mul_y: %d\n", font_size, mul_x, mul_y);
	for (font_mul_idx = 0; font_mul_idx < FONT_MULTI_TABLE_ENTRIES;
	 font_mul_idx++) {
		if (fonts__[font_mul_table[font_mul_idx].font_idx].glyphs > 0
		 && (font_size == 0
		  || fonts__[font_mul_table[font_mul_idx].font_idx].height == font_size)
		 && (mul_x == 0 || font_mul_table[font_mul_idx].mul_x == mul_x)
		 && (mul_y == 0 || font_mul_table[font_mul_idx].mul_y == mul_y)) {
			return font_mul_idx;
		}
	}
	return -1;
}
/////int font_correct_selection(int font_mul_idx)
/////{
/////	int next_font_mul_idx;
/////
/////	if (font_check_selection_valid(font_mul_idx)) {
/////		return font_mul_idx;
/////	}
/////	next_font_mul_idx = font_select_next(font_mul_idx, -1);
/////	if (font_check_selection_valid(next_font_mul_idx)) {
/////		return next_font_mul_idx;
/////	}
/////	next_font_mul_idx = font_select_next(font_mul_idx, +1);
/////	if (font_check_selection_valid(next_font_mul_idx)) {
/////		return next_font_mul_idx;
/////	}
/////	return 0;
/////}
int font_select_next(int font_mul_idx, int shift)
{
	int prev_font_mul_idx = font_mul_idx;

	for (font_mul_idx += shift;
	 IS_IN_RANGE(0, font_mul_idx, FONT_MULTI_TABLE_ENTRIES);
	 font_mul_idx += shift) {
		if (fonts__[font_mul_table[font_mul_idx].font_idx].glyphs > 0) {
			return font_mul_idx;
		}
	}
	return prev_font_mul_idx;
}
/////int font_check_selection_valid(int font_mul_idx)
/////{
/////	return IS_IN_RANGE(0, font_mul_idx, FONT_MULTI_TABLE_ENTRIES)
/////	 && fonts__[font_mul_table[font_mul_idx].font_idx].glyphs > 0;
/////}
// setup font metrics
void font_setup_metrics()
{
	cur_font_mul = &(font_mul_table[cur_font_mul_idx]);
	cur_font_idx = font_mul_table[cur_font_mul_idx].font_idx;
	cur_font = &(fonts__[cur_font_idx]);

	cur_font_exp->width = cur_font->width * cur_font_mul->mul_x;
	cur_font_exp->height = cur_font->height * cur_font_mul->mul_y;

	flf_d_printf("cur_font_mul_idx: %d\n", cur_font_mul_idx);
	flf_d_printf("cur_font_idx: %d\n", cur_font_idx);
	flf_d_printf("cur_font->width: %d, cur_font->height: %d\n", cur_font->width, cur_font->height);
	flf_d_printf("font_mul_x: %d, font_mul_y: %d\n", cur_font_mul->mul_x, cur_font_mul->mul_y);
	flf_d_printf("font_exp_width: %d, font_exp_height: %d\n",
	 cur_font_exp->width, cur_font_exp->height);
}

//-----------------------------------------------------------------------------
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

	///er_printf("trying to load font [%s]\n", file_name);
	hex_init(font);

	// $HOME/bin/fontfile
	snprintf(file_path, MAX_PATH_LEN+1, "%s/bin/%s", 
	 getenv("HOME"), file_name);
	if (hex_load(font, file_path) == 0)
		goto font_loaded;

	// /usr/local/bin/fontfile
	snprintf(file_path, MAX_PATH_LEN+1, "/usr/local/bin/%s", file_name);
	///er_printf("trying to load font [%s]\n", file_path);
	if (hex_load(font, file_path) == 0)
		goto font_loaded;

	er_printf("!!!! ERROR: CAN NOT load font [%s] !!!!\n", file_name);
	return -1;
font_loaded:;
	font_count_glyphs(font);
	flf_d_printf("font_name: %s, glyphs: %d, width: %d, height: %d\n",
	 file_name, font->glyphs, font->width, font->height);
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

	if (font->glyphs == 0) {
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

const u_short *font_get_glyph_bitmap(font_t *font, wchar_t ucs21,
 int *width_in_pixels, int *found)
{
	ucs21 = LIM_MAX_(MAX_GLYPHS-1, ucs21);	// [0 -- MAX_GLYPHS-1]
	const u_short *glyph;
	int width = font_get_glyph_width_in_pixels(font, ucs21);
	if (width) {
		glyph = font->glyph_bitmap[ucs21];
	} else {
		width = font_get_undefined_glyph_width(font, ucs21);
		glyph = font_get_undefined_glyph_bitmap(font, ucs21);
	}
	if (width_in_pixels) {
		*width_in_pixels = width;
	}
	if (found) {
		*found = width;
	}
	return glyph;
}
const int font_get_glyph_width_in_pixels(font_t *font, wchar_t ucs21)
{
	int width_in_pixels;

	ucs21 = LIM_MAX_(MAX_GLYPHS-1, ucs21);	// [0 -- MAX_GLYPHS-1]
	width_in_pixels = font->glyph_width[ucs21];
	/////if (width_in_pixels == 0) {
	/////	width_in_pixels = font_get_undefined_glyph_width(font, ucs21);
	/////}
	return width_in_pixels;
}

//-----------------------------------------------------------------------------
static const u_short *glyph_bitmap;
static u_char glyph_offset_x = 0;
const u_short *font_set_glyph(wchar_t ucs21, u_char offset_x)
{
//flf_d_printf("ucs21: %04x, offset_x: %d\n", ucs21, offset_x);
	glyph_bitmap = font_get_glyph_bitmap(cur_font, ucs21, NULL, NULL);
	glyph_offset_x = offset_x;
	return glyph_bitmap;
}
bool font_get_glyph_pixel(u_char fx, u_char fy)
{
	return (glyph_bitmap[fy] & (0x8000 >> (glyph_offset_x + fx))) != 0x0000;
}

// End of font.c
