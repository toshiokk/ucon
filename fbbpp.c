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

#include "ucon.h"

// global variables for speed reasons =========================================

u_char *fb_start;
size_t fb_view_size;
u_char *fb_end;

u_char fb_bytes_per_pixel;
u_char *fb_top_left; 		// FB address of top_left  (Ex. 0, 0)
u_char *fb_top_right;		// FB address of top_right (Ex. 639, 0)
u_char *fb_bottom_left;		// FB address of bottom_left  (Ex. 479, 0)
u_char *fb_bottom_right;	// FB address of bottom_right (Ex. 479, 639)
ssize_t fb_bytes_inc_px;
ssize_t fb_bytes_inc_py;

ushort fbr_screen_size_hx;		// 640/480...3840/1920
ushort fbr_screen_size_hy;		// 480/640...1920/3840

// font and font expansion dependent parameters
// Example when FontX:6, FontX:12, Multi-X: 2, Multi-Y: 3
u_char fb_font_size_hx;			// 6
u_char fb_font_size_hy;			// 12
u_char fb_font_mul_hx;			// 2
u_char fb_font_mul_hy;			// 3
u_char fb_font_exp_size_hx;		// 12 (6 x 2)
u_char fb_font_exp_size_hy;		// 36 (12 x 3)

// Example when Depth: 32bits, X:640, Y:480, FontX:6, FontX:12, Multi-X: 2, Multi-Y: 3
u_short fbr_chars_hx;			// 80 / 60
u_short fbr_chars_hy;			// 25 / 40

u_char *fbr_origin;				// FB address of view origin
ssize_t fbr_bytes_inc_hx;		// 4 | 2560
ssize_t fbr_bytes_inc_hy;		// 2560 | 4
// font size and rotation dependent parameters (updated on change of rotation)
ssize_t fbr_bytes_inc_dot_hx;	// 4 | 8 | 12 | 2560|5120|10240
ssize_t fbr_bytes_inc_dot_hy;	// 2560|5120|10240 | 4|8|12
ssize_t fbr_bytes_inc_font_hx;	// 4 | 0 | 2560 | 0
ssize_t fbr_bytes_inc_font_hy;	// 0 | 4 | 0 | 2560

//-----------------------------------------------------------------------------
#ifdef ENABLE_8BPP
PRIVATE void fb_8bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_8bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_8bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_8bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_8bpp_get_pixel_argb32(u_short xx, u_short yy);
PRIVATE void fb_8bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_8BPP

#ifdef ENABLE_15BPP
PRIVATE void fb_15bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_15bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_15bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_15bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_15bpp_get_pixel_argb32(u_short xx, u_short yy);
PRIVATE void fb_15bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_15BPP

#ifdef ENABLE_16BPP
PRIVATE void fb_16bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_16bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_16bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_16bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_16bpp_get_pixel_argb32(u_short xx, u_short yy);
PRIVATE void fb_16bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_16BPP

#if defined(ENABLE_15BPP) || defined(ENABLE_16BPP)
PRIVATE void fb_15_16bpp_clear_all(u_short rgb);
PRIVATE void fb_15_16bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 rgb16_t rgb16);
PRIVATE void fb_15_16bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 rgb16_t rgb16);
PRIVATE void fb_15_16bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE rgb15_t fb_15_16bpp_get_pixel_rgb15(int xx, int yy);
PRIVATE void fb_15_16bpp_reverse_all(rgb15_t rgb15);
#endif // ENABLE_SCREEN_SHOT
#endif /* defined(ENABLE_15BPP) || defined(ENABLE_16BPP) */

#ifdef ENABLE_24BPP
PRIVATE void fb_24bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_24bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_24bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_24bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_24bpp_get_pixel_argb32(u_short xx, u_short yy);
PRIVATE void fb_24bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_24BPP

#ifdef ENABLE_32BPP
PRIVATE void fb_32bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_32bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_32bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15);
PRIVATE void fb_32bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);
PRIVATE void fb_32bpp_paint_char(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15);

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_32bpp_get_pixel_argb32(u_short xx, u_short yy);
PRIVATE void fb_32bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_32BPP

//-----------------------------------------------------------------------------

fb_driver_t fb_drivers[] = {
#if defined(ENABLE_8BPP)
	{
		8, FB_VISUAL_PSEUDOCOLOR,
		fb_8bpp_clear_all,
		fb_8bpp_fill_char_box,
		fb_8bpp_reverse_char_box,
		fb_8bpp_paint_char_box,
#ifdef ENABLE_SCREEN_SHOT
		fb_8bpp_get_pixel_argb32,
		fb_8bpp_reverse_all,
#endif // ENABLE_SCREEN_SHOT
	},
#endif // ENABLE_8BPP
#if defined(ENABLE_15BPP)
	{
		15, FB_VISUAL_TRUECOLOR,
		fb_15bpp_clear_all,
		fb_15bpp_fill_char_box,
		fb_15bpp_reverse_char_box,
		fb_15bpp_paint_char_box,
#ifdef ENABLE_SCREEN_SHOT
		fb_15bpp_get_pixel_argb32,
		fb_15bpp_reverse_all,
#endif // ENABLE_SCREEN_SHOT
	},
#endif // ENABLE_15BPP
#if defined(ENABLE_16BPP)
	{
		16, FB_VISUAL_DIRECTCOLOR,
		fb_16bpp_clear_all,
		fb_16bpp_fill_char_box,
		fb_16bpp_reverse_char_box,
		fb_16bpp_paint_char_box,
#ifdef ENABLE_SCREEN_SHOT
		fb_16bpp_get_pixel_argb32,
		fb_16bpp_reverse_all,
#endif // ENABLE_SCREEN_SHOT
	},
#endif // ENABLE_16BPP
#if defined(ENABLE_24BPP)
	{
		24, FB_VISUAL_TRUECOLOR,
		fb_24bpp_clear_all,
		fb_24bpp_fill_char_box,
		fb_24bpp_reverse_char_box,
		fb_24bpp_paint_char_box,
#ifdef ENABLE_SCREEN_SHOT
		fb_24bpp_get_pixel_argb32,
		fb_24bpp_reverse_all,
#endif // ENABLE_SCREEN_SHOT
	},
#endif // ENABLE_24BPP
#if defined(ENABLE_32BPP)
	{
		32, FB_VISUAL_TRUECOLOR,
		fb_32bpp_clear_all,
		fb_32bpp_fill_char_box,
		fb_32bpp_reverse_char_box,
		fb_32bpp_paint_char_box,
#ifdef ENABLE_SCREEN_SHOT
		fb_32bpp_get_pixel_argb32,
		fb_32bpp_reverse_all,
#endif // ENABLE_SCREEN_SHOT
	},
#endif // ENABLE_32BPP
	{
		0, 0,		// end mark
		NULL,
		NULL,
		NULL,
		NULL,
	}
};

// Workaround for i810/815/845/855 Framebuffer bitmap corruption
///#define I810_FB_DUMMY_ACCESS

#ifdef ENABLE_8BPP
#warning "ENABLE_8BPP"
/* 8 bpp */
PRIVATE cpal8_t pallete_idx_from_color_idx(u_char clr_idx)
{
	cpal8_t cpal8;

	// swap bit 2 and 0
	cpal8 = (clr_idx & 0x0a)
	 | ((clr_idx & 0x01) << 2)	// bit 0 ==> bit 2
	 | ((clr_idx & 0x04) >> 2);	// bit 2 ==> bit 0
	return cpal8;
}
#ifdef ENABLE_SCREEN_SHOT
PRIVATE u_char color_idx_from_pallete_idx(cpal8_t cpal8)
{
	u_char clr_idx;

	// swap bit 2 and 0
	clr_idx = (cpal8 & 0x0a)
	 | ((cpal8 & 0x01) << 2)	// bit 0 ==> bit 2
	 | ((cpal8 & 0x04) >> 2);	// bit 2 ==> bit 0
	return clr_idx;
}
#endif // ENABLE_SCREEN_SHOT
/* 8 bpp */
PRIVATE void fb_8bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15)
{
	cpal8_t cpal8 = pallete_idx_from_color_idx(clr_idx);	//=

	memset(fb_start, cpal8, fb_view_size);					//=bpp_clear_all

}
PRIVATE void fb_8bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	cpal8_t cpal8 = pallete_idx_from_color_idx(clr_idx);	//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {


		memset(fb_left, cpal8, fb_font_exp_size_hx);		//=bpp_fill_char_box


		fb_left += fbr_bytes_inc_hy;
	}
}
PRIVATE void fb_8bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	cpal8_t cpal8 = pallete_idx_from_color_idx(clr_idx);	//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		u_char *fb = fb_left;
		for (u_char fx = 0; fx < glyph_exp_width; fx++) {
			*(cpal8_t*)fb ^= cpal8;										//=bpp_reverse_char_box
			fb += fbr_bytes_inc_hx;
		}
		fb_left += fbr_bytes_inc_hy;
	}
#ifdef I810_FB_DUMMY_ACCESS
	fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		*(volatile cpal8_t*)fb_left = *(volatile cpal8_t*)fb_left;		//=
		fb_left += fbr_bytes_inc_hy;
	}
#endif // I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_8bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15)
{
	cpal8_t bcpal8 = pallete_idx_from_color_idx(bc_idx);	//=
	cpal8_t fcpal8 = pallete_idx_from_color_idx(fc_idx);	//=
	u_char glyph_width = fb_font_size_hx * (1 + wide);
	const u_short *glyph = font_set_glyph(ucs21, 0);
	u_char *fb_chr_y = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_size_hy; fy++) {
		u_char *fb_chr_x = fb_chr_y;
		u_short bitmap = *glyph;
		u_short bit_mask = 0x8000;
		for (u_char fx = 0; fx < glyph_width; fx++) {
			u_char *fb_dot_y = fb_chr_x;
			cpal8_t cpal8 = (bitmap & bit_mask) ? fcpal8 : bcpal8;								//=
			for (u_char mul_y = 0; mul_y < fb_font_mul_hy; mul_y++) {
				u_char *fb_dot_x = fb_dot_y;
				for (u_char mul_x = 0; mul_x < fb_font_mul_hx; mul_x++) {
					*(cpal8_t *)fb_dot_x = cpal8;				//=bpp_paint_char_box
					fb_dot_x += fbr_bytes_inc_hx;
				}
				fb_dot_y += fbr_bytes_inc_hy;
			}
			bit_mask >>= 1;
			fb_chr_x += fbr_bytes_inc_dot_hx;
		}
		glyph++;
		fb_chr_y += fbr_bytes_inc_dot_hy;
	}
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_8bpp_get_pixel_argb32(u_short xx, u_short yy)
{
	u_char *fb = fbr_origin + fbr_bytes_inc_hy * yy + fbr_bytes_inc_hx * xx;
	argb32_t argb32 = argb32_from_color_idx(color_idx_from_pallete_idx(*fb));	//=bpp_get_pixel
	return argb32;
}
PRIVATE void fb_8bpp_reverse_all(void)
{
	for (u_char *fb = fb_start; fb < fb_end; fb += fb_bytes_per_pixel) {
		*fb = *fb ^ 0x0f;									//=_reverse_all
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_8BPP

#ifdef ENABLE_15BPP
#warning "ENABLE_15BPP"
/* 15 bpp */
PRIVATE void fb_15bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15)
{
	fb_15_16bpp_clear_all(rgb15);
}
PRIVATE void fb_15bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	fb_15_16bpp_fill_char_box(xx, yy, wide, rgb15);
}
PRIVATE void fb_15bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	fb_15_16bpp_reverse_char_box(xx, yy, wide, rgb15);
}
PRIVATE void fb_15bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15)
{
	fb_15_16bpp_paint_char_box(xx, yy, ucs21, wide, bc_idx, fc_idx, bc_rgb15, fc_rgb15);
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_15bpp_get_pixel_argb32(u_short xx, u_short yy)
{
	rgb15_t rgb15 = fb_15_16bpp_get_pixel_rgb15(xx, yy);
	return argb32_from_rgb15(rgb15);
}
PRIVATE void fb_15bpp_reverse_all(void)
{
	fb_15_16bpp_reverse_all(0x7fff);	// 0rrrrrgggggbbbbb
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_15BPP

#ifdef ENABLE_16BPP
#warning "ENABLE_16BPP"
/* 16 bpp */
PRIVATE void fb_16bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15)
{
	fb_15_16bpp_clear_all(rgb16_from_rgb15(rgb15));
}
PRIVATE void fb_16bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	fb_15_16bpp_fill_char_box(xx, yy, wide, rgb16_from_rgb15(rgb15));
}
PRIVATE void fb_16bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	fb_15_16bpp_reverse_char_box(xx, yy, wide, rgb16_from_rgb15(rgb15));
}
PRIVATE void fb_16bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15)
{
	rgb16_t bc_rgb16 = rgb16_from_rgb15(bc_rgb15);
	rgb16_t fc_rgb16 = rgb16_from_rgb15(fc_rgb15);
	fb_15_16bpp_paint_char_box(xx, yy, ucs21, wide, bc_idx, fc_idx, bc_rgb15, fc_rgb15);
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_16bpp_get_pixel_argb32(u_short xx, u_short yy)
{
	rgb16_t rgb16 = fb_15_16bpp_get_pixel_rgb15(xx, yy);
	return argb32_from_rgb16(rgb16);
}
PRIVATE void fb_16bpp_reverse_all(void)
{
	fb_15_16bpp_reverse_all(0xffff);	// rrrrrggggggbbbbb
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_16BPP

#if defined(ENABLE_15BPP) || defined(ENABLE_16BPP)
/* 15 bpp / 16 bpp */
PRIVATE void fb_15_16bpp_clear_all(rgb15_t rgb15)
{
	rgb15 = rgb15;											//=
	for (u_char *fb = fb_start; fb < fb_end; fb += fb_bytes_per_pixel) {
		*(rgb15_t*)fb = rgb15;								//=bpp_clear_all
	}
}
PRIVATE void fb_15_16bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 rgb15_t rgb15)
{
	rgb15 = rgb15;											//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		u_char *fb = fb_left;
		for (u_char fx = 0; fx < glyph_exp_width; fx++) {
			*(rgb15_t*)fb = rgb15;							//=bpp_fill_char_box
			fb += fbr_bytes_inc_hx;
		}
		fb_left += fbr_bytes_inc_hy;
	}
}
PRIVATE void fb_15_16bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 rgb15_t rgb15)
{
	rgb15 = rgb15;											//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		u_char *fb = fb_left;
		for (u_char fx = 0; fx < glyph_exp_width; fx++) {
			*(rgb15_t*)fb ^= rgb15;										//=bpp_reverse_char_box
			fb += fbr_bytes_inc_hx;
		}
		fb_left += fbr_bytes_inc_hy;
	}
#ifdef I810_FB_DUMMY_ACCESS
	fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		*(volatile rgb15_t*)fb_left = *(volatile rgb15_t*)fb_left;		//=
		fb_left += fbr_bytes_inc_hy;
	}
#endif // I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_15_16bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15)
{
	bc_rgb15 = bc_rgb15;									//=
	fc_rgb15 = fc_rgb15;									//=
	u_char glyph_width = fb_font_size_hx * (1 + wide);
	const u_short *glyph = font_set_glyph(ucs21, 0);
	u_char *fb_chr_y = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_size_hy; fy++) {
		u_char *fb_chr_x = fb_chr_y;
		u_short bitmap = *glyph;
		u_short bit_mask = 0x8000;
		for (u_char fx = 0; fx < glyph_width; fx++) {
			u_char *fb_dot_y = fb_chr_x;
			rgb15_t rgb15 = (bitmap & bit_mask) ? fc_rgb15 : bc_rgb15;							//=
			for (u_char mul_y = 0; mul_y < fb_font_mul_hy; mul_y++) {
				u_char *fb_dot_x = fb_dot_y;
				for (u_char mul_x = 0; mul_x < fb_font_mul_hx; mul_x++) {
					*(rgb15_t*)fb_dot_x = rgb15;				//=bpp_paint_char_box
					fb_dot_x += fbr_bytes_inc_hx;
				}
				fb_dot_y += fbr_bytes_inc_hy;
			}
			bit_mask >>= 1;
			fb_chr_x += fbr_bytes_inc_dot_hx;
		}
		glyph++;
		fb_chr_y += fbr_bytes_inc_dot_hy;
	}
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE rgb15_t fb_15_16bpp_get_pixel_rgb15(int xx, int yy)
{
	u_char *fb = fbr_origin + fbr_bytes_inc_hy * yy + fbr_bytes_inc_hx * xx;
	rgb15_t rgb15 = *(rgb15_t*)fb;												//=bpp_get_pixel
	return rgb15;
}
PRIVATE void fb_15_16bpp_reverse_all(rgb15_t rgb15)
{
	for (u_char *fb = fb_start; fb < fb_end; fb += fb_bytes_per_pixel) {
		*(rgb15_t*)fb ^= rgb15;								//=_reverse_all
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif /* defined(ENABLE_15BPP) || defined(ENABLE_16BPP) */

#ifdef ENABLE_24BPP
#warning "ENABLE_24BPP"
/* 24 bpp */
PRIVATE void fb_24bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15)
{
	rgb24_t rgb24;  rgb24_from_rgb15(rgb15, &rgb24);		//=
	for (u_char *fb = fb_start; fb < fb_end; fb += fb_bytes_per_pixel) {
		memcpy(fb, &rgb24, sizeof(rgb24_t));				//=bpp_clear_all
	}
}
PRIVATE void fb_24bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	rgb24_t rgb24;  rgb24_from_rgb15(rgb15, &rgb24);		//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		u_char *fb = fb_left;
		for (u_char fx = 0; fx < glyph_exp_width; fx++) {
			memcpy(fb, &rgb24, sizeof(rgb24_t));			//=bpp_fill_char_box
			fb += fbr_bytes_inc_hx;
		}
		fb_left += fbr_bytes_inc_hy;
	}
}
PRIVATE void fb_24bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	rgb24_t rgb24;  rgb24_from_rgb15(rgb15, &rgb24);		//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		u_char *fb = fb_left;
		for (u_char fx = 0; fx < glyph_exp_width; fx++) {
			fb[0] ^= rgb24[0];  fb[1] ^= rgb24[1];  fb[2] ^= rgb24[2];	//=bpp_reverse_char_box
			fb += fbr_bytes_inc_hx;
		}
		fb_left += fbr_bytes_inc_hy;
	}
#ifdef I810_FB_DUMMY_ACCESS
	fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		memcpy(fb_left, fb_left, sizeof(rgb24_t));						//=
		fb_left += fbr_bytes_inc_hy;
	}
#endif // I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_24bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15)
{
	rgb24_t bc24;  rgb24_from_rgb15(bc_rgb15, &bc24);		//=
	rgb24_t fc24;  rgb24_from_rgb15(fc_rgb15, &fc24);		//=
	u_char glyph_width = fb_font_size_hx * (1 + wide);
	const u_short *glyph = font_set_glyph(ucs21, 0);
	u_char *fb_chr_y = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_size_hy; fy++) {
		u_char *fb_chr_x = fb_chr_y;
		u_short bitmap = *glyph;
		u_short bit_mask = 0x8000;
		for (u_char fx = 0; fx < glyph_width; fx++) {
			u_char *fb_dot_y = fb_chr_x;
			rgb24_t c24; memcpy(&c24, (bitmap & bit_mask) ? &fc24 : &bc24, sizeof(rgb24_t));	//=
			for (u_char mul_y = 0; mul_y < fb_font_mul_hy; mul_y++) {
				u_char *fb_dot_x = fb_dot_y;
				for (u_char mul_x = 0; mul_x < fb_font_mul_hx; mul_x++) {
					memcpy(fb_dot_x, &c24, sizeof(rgb24_t));	//=bpp_paint_char_box
					fb_dot_x += fbr_bytes_inc_hx;
				}
				fb_dot_y += fbr_bytes_inc_hy;
			}
			bit_mask >>= 1;
			fb_chr_x += fbr_bytes_inc_dot_hx;
		}
		glyph++;
		fb_chr_y += fbr_bytes_inc_dot_hy;
	}
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_24bpp_get_pixel_argb32(u_short xx, u_short yy)
{
	u_char *fb = fbr_origin + fbr_bytes_inc_hy * yy + fbr_bytes_inc_hx * xx;
	argb32_t argb32 = ((argb32_t)fb[2] << 16) + ((argb32_t)fb[1] << 8) + fb[0];	//=bpp_get_pixel
	return argb32;		// rrrrrrrrggggggggbbbbbbbb
}
PRIVATE void fb_24bpp_reverse_all(void)
{
	for (u_char *fb = fb_start; fb < fb_end; fb += fb_bytes_per_pixel) {
		fb[0] = ~fb[0];  fb[1] = ~fb[1];  fb[2] = ~fb[2];	//=_reverse_all
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_24BPP

#ifdef ENABLE_32BPP
/* 32 bpp */
PRIVATE void fb_32bpp_clear_all(c_idx_t clr_idx, rgb15_t rgb15)
{
	argb32_t argb32 = argb32_from_rgb15(rgb15);				//=
	for (u_char *fb = fb_start; fb < fb_end; fb += fb_bytes_per_pixel) {
		*(argb32_t*)fb = argb32;							//=bpp_clear_all
	}
}
PRIVATE void fb_32bpp_fill_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	argb32_t argb32 = argb32_from_rgb15(rgb15);				//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		u_char *fb = fb_left;
		for (u_char fx = 0; fx < glyph_exp_width; fx++) {
			*(argb32_t*)fb = argb32;						//=bpp_fill_char_box
			fb += fbr_bytes_inc_hx;
		}
		fb_left += fbr_bytes_inc_hy;
	}
}
PRIVATE void fb_32bpp_reverse_char_box(u_short xx, u_short yy, bool wide,
 c_idx_t clr_idx, rgb15_t rgb15)
{
	argb32_t argb32 = argb32_from_rgb15(rgb15);				//=
	u_char *fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	u_char glyph_exp_width = fb_font_exp_size_hx * (1 + wide);
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		u_char *fb = fb_left;
		for (u_char fx = 0; fx < glyph_exp_width; fx++) {
			*(argb32_t*)fb ^= argb32;									//=bpp_reverse_char_box
			fb += fbr_bytes_inc_hx;
		}
		fb_left += fbr_bytes_inc_hy;
	}
#ifdef I810_FB_DUMMY_ACCESS
	fb_left = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_exp_size_hy; fy++) {
		*(volatile argb32_t*)fb_left = *(volatile argb32_t*)fb_left;	//=
		fb_left += fbr_bytes_inc_hy;
	}
#endif // I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_32bpp_paint_char_box(u_short xx, u_short yy, wchar_t ucs21, bool wide,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb15, rgb15_t fc_rgb15)
{
	argb32_t bc_argb32 = argb32_from_rgb15(bc_rgb15);		//=
	argb32_t fc_argb32 = argb32_from_rgb15(fc_rgb15);		//=
	u_char glyph_width = fb_font_size_hx * (1 + wide);
	const u_short *glyph = font_set_glyph(ucs21, 0);
	u_char *fb_chr_y = fbr_origin + fbr_bytes_inc_font_hy * yy + fbr_bytes_inc_font_hx * xx;
	for (u_char fy = 0; fy < fb_font_size_hy; fy++) {
		u_char *fb_chr_x = fb_chr_y;
		u_short bitmap = *glyph;
		u_short bit_mask = 0x8000;
		for (u_char fx = 0; fx < glyph_width; fx++) {
			u_char *fb_dot_y = fb_chr_x;
			argb32_t argb32 = (bitmap & bit_mask) ? fc_argb32 : bc_argb32;						//=
			for (u_char mul_y = 0; mul_y < fb_font_mul_hy; mul_y++) {
				u_char *fb_dot_x = fb_dot_y;
				for (u_char mul_x = 0; mul_x < fb_font_mul_hx; mul_x++) {
					*(argb32_t*)fb_dot_x = argb32;				//=bpp_paint_char_box
					fb_dot_x += fbr_bytes_inc_hx;
				}
				fb_dot_y += fbr_bytes_inc_hy;
			}
			bit_mask >>= 1;
			fb_chr_x += fbr_bytes_inc_dot_hx;
		}
		glyph++;
		fb_chr_y += fbr_bytes_inc_dot_hy;
	}
}

// ...px: physical x (in physical view)
// ...py: physical y
// ...hx: human x (in human view)
// ...hy: human y
// chx: character human x (in human view)
// chy: character human y
// fpx: font physical x (in physical view)
// fpy: font physical y
// ppx: pixel physical x (in physical view)
// ppy: pixel physical y

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_32bpp_get_pixel_argb32(u_short xx, u_short yy)
{
	u_char *fb = fbr_origin + fbr_bytes_inc_hy * yy + fbr_bytes_inc_hx * xx;
////flf_d_printf("(%d, %d):(%d, %d)\n", xx, yy,
//// ((fb - fb_start) / fb_bytes_per_pixel) % fb__.screen_size_x,
//// ((fb - fb_start) / fb_bytes_per_pixel) / fb__.screen_size_x);
	argb32_t argb32 = *(argb32_t*)fb;											//=bpp_get_pixel
	return argb32;		// 00000000rrrrrrrrggggggggbbbbbbbb
}
PRIVATE void fb_32bpp_reverse_all(void)
{
	for (u_char *fb = fb_start; fb < fb_end; fb += fb_bytes_per_pixel) {
		*(argb32_t*)fb ^= 0x00ffffff;						//=bpp_reverse_all
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_32BPP

// chx: Charactor x in Human view
// chy: Charactor y in Human view
// px: x in Physical view
// py: y in Physical view
// hx: x in Human view
// hy: y in Human view

// glyph_rotation: 0      1        2      3
//           *---+  *-----+  *---+  *-----+
//           | # |  | ####|  |# #|  |#### |
//           |# #|  |# #  |  |# #|  |  # #|
//           |###|  | ####|  |###|  |#### |
//           |# #|  +-----+  |# #|  +-----+
//           |# #|           | # |         
//           +---+           +---+         

// End of fbbpp.c
