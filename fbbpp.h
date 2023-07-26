/*
 * UCON -
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

#ifndef ucon_fbbpp_h
#define ucon_fbbpp_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

typedef char c_idx_t;			// color index [0, 15], -1:invalid
typedef u_short rgb15_t;		// 0rrrrrgggggbbbbb (5/5/5)
typedef u_short rgb16_t;		// rrrrrggggggbbbbb (5/6/5)
typedef u_int32 argb32_t;		// 00000000 rrrrrrrr gggggggg bbbbbbbb

struct fb_driver_;		// forward declaration
typedef struct fb_driver_ fb_driver_t;

struct fb_driver_ {
	int bitsPerPixel;
	int fbVisual;		// unused
	void (*clear_all)(c_idx_t color_idx, rgb15_t rgb);
	void (*fill_char_box)(int left_x, int top_y, int fnt_sx, int fnt_sy,
	 c_idx_t color_idx, rgb15_t rgb);
	void (*reverse_char_box)(int left_x, int top_y, int fnt_sx, int fnt_sy,
	 c_idx_t color_idx, rgb15_t rgb);
	void (*paint_char_box)(int left_x, int top_y,
	 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
	 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb);
#ifdef ENABLE_SCREEN_SHOT
///	int (*put_pixel_argb32)(int xx, int yy, argb32_t argb32);
	argb32_t (*get_pixel_argb32)(int xx, int yy);
	void (*reverse_all)(void);
#endif // ENABLE_SCREEN_SHOT
};

extern fb_driver_t fb_drivers[];

void fb_set_metrics(void);

#if defined(__cplusplus)
}
#endif // __cplusplus

#ifdef ENABLE_DEBUG
void test_fill_top_of_fb(void);
#endif // ENABLE_DEBUG

#endif // ucon_fbbpp_h

// End of fbbpp.h
