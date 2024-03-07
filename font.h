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

#ifndef ucon_font_h
#define ucon_font_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define MAX_FONT_HEIGHT		16
////#define MAX_GLYPHS			65535
#define MAX_GLYPHS			65536

typedef struct /*font_*/ {
	u_char width;		// 5, 6, 7, 8
	u_char height;		// 10, 12, 14, 16
	// glyph bitmap
	u_short glyph_bitmap[MAX_GLYPHS][MAX_FONT_HEIGHT];
	// glyph width in pixels
	u_char glyph_width[MAX_GLYPHS];	// 0: no glyph
	u_short glyphs;		// 0 -- 65535
} font_t;

#define MAX_FONTS		4	// 5x10, 6x12, 7x14, 8x16
extern font_t fonts__[MAX_FONTS];
extern int cur_font_idx;
extern font_t *cur_font;

typedef struct /*font_mul_*/ {
	int font_idx;
	u_char mul_x;
	u_char mul_y;
} font_mul_t;
extern int cur_font_mul_idx;
extern font_mul_t *cur_font_mul;

typedef struct /*font_exp_*/ {
	u_char width;
	u_char height;
} font_exp_t;
extern font_exp_t cur_font_exp_, *cur_font_exp;

//-----------------------------------------------------------------------------
int font_select(int font_size, int mul_x, int mul_y);
int font_correct_selection(int font_mul_idx);
int font_select_next(int font_mul_idx, int shift);
int font_check_selection_valid(int font_mul_idx);
void font_setup_metrics();

void font_init_all(void);
int font_load_all(void);
int font_destroy_all(void);

void font_init(font_t *font);
int font_load(font_t *font, char *file_name);
int font_is_loaded(font_t *font);
int font_count_glyphs(font_t *font);
int font_destroy(font_t *font);

const u_short *font_get_glyph_bitmap(font_t *font, wchar_t ucs21,
 int *width_in_pixels, int *found);
const int font_get_glyph_width(font_t *font, wchar_t ucs21);

const u_short *font_set_glyph(wchar_t ucs21, u_char offset_x);
bool font_get_glyph_pixel(u_char fx, u_char fy);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // ucon_font_h

// End of font.h
