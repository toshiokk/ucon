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

extern u_char *fb_start;
extern size_t fb_size;
extern u_char *fb_end;

extern u_char fb_bytes_per_pixel;
extern ssize_t fb_bytes_inc_px;
extern ssize_t fb_bytes_inc_py;
extern u_char *fb_top_left;
extern u_char *fb_top_right;
extern u_char *fb_bottom_left;
extern u_char *fb_bottom_right;
// rotation dependent parameters (updated on change of rotation)
extern ushort fbr_screen_size_hx;
extern ushort fbr_screen_size_hy;
// font expansion and rotation dependent parameters
// Example when FontX:6, FontX:12, Multi-X: 2, Multi-Y: 3
extern u_char fb_font_size_hx;			// 6
extern u_char fb_font_size_hy;			// 12
extern u_char fb_font_mul_hx;			// 2
extern u_char fb_font_mul_hy;			// 3
extern u_char fb_font_exp_size_hx;		// 12 (6 x 2)
extern u_char fb_font_exp_size_hy;		// 36 (12 x 3)
// Example when Depth: 32bits, X:640, Y:480, FontX:6, FontX:12, Multi-X: 2, Multi-Y: 3
extern u_short fbr_chars_hx;			// 80 / 60
extern u_short fbr_chars_hy;			// 25 / 40

extern u_char *fbr_origin;				// FB address of view origin
extern ssize_t fbr_bytes_inc_hx;		// 4 | 2560
extern ssize_t fbr_bytes_inc_hy;		// 2560 | 4
extern ssize_t fbr_bytes_inc_dot_hx;	// 4 | 8 | 12 | 2560|5120|10240
extern ssize_t fbr_bytes_inc_dot_hy;	// 2560|5120|10240 | 4|8|12
// font size and rotation dependent parameters
extern ssize_t fbr_bytes_inc_font_hx;	// 4 | 0 | 2560 | 0
extern ssize_t fbr_bytes_inc_font_hy;	// 0 | 4 | 0 | 2560
// FB address calculation parameters

//-----------------------------------------------------------------------------

typedef char c_idx_t;			// color index [0, 15], -1:invalid
typedef u_char cpal8_t;			// color palette [0, 15]
typedef u_short rgb15_t;		// 0rrrrrgggggbbbbb (5/5/5)
typedef u_short rgb16_t;		// rrrrrggggggbbbbb (5/6/5)
typedef u_char rgb24_t[3];		// rrrrrrrr gggggggg bbbbbbbb
typedef u_int32 argb32_t;		// 00000000 rrrrrrrr gggggggg bbbbbbbb

struct fb_driver_;		// forward declaration
typedef struct fb_driver_ fb_driver_t;

// framebuffer driver with rotation support
struct fb_driver_ {
	int bitsPerPixel;
	int fbVisual;		// unused
	void (*bpp_clear_all)(c_idx_t clr_idx, rgb15_t rgb15);
	void (*bpp_fill_char_box)(u_short xx, u_short yy, bool wide,
	 c_idx_t clr_idx, rgb15_t rgb15);
	void (*bpp_reverse_char_box)(u_short xx, u_short yy, bool wide,
	 c_idx_t clr_idx, rgb15_t rgb15);
	void (*bpp_paint_char_box)(u_short cx, u_short cy, wchar_t ucs21, bool wide,
	 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);
#ifdef ENABLE_SCREEN_SHOT
///	int (*put_pixel_argb32)(u_short xx, u_short yy, argb32_t argb32);
	argb32_t (*get_pixel_argb32)(u_short xx, u_short yy);
	void (*reverse_all)(void);
#endif // ENABLE_SCREEN_SHOT
};

extern fb_driver_t fb_drivers[];

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // ucon_fbbpp_h

// End of fbbpp.h
