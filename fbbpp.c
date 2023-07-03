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

// global variablas for speed reasons
PRIVATE u_char *fb_fb_start;
PRIVATE u_int32 fb_fb_mem_size;
PRIVATE u_char *fb_fb_end;
PRIVATE int fb_fb_bytes_per_line;
PRIVATE int fb_dots_per_pixel_x;
PRIVATE int fb_dots_per_pixel_y;

//-----------------------------------------------------------------------------
#ifdef ENABLE_8BPP
PRIVATE void fb_8bpp_clear_all(c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_8bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_8bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_8bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_8bpp_get_pixel_argb32(int xx, int yy);
PRIVATE void fb_8bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_8BPP

#ifdef ENABLE_15BPP
PRIVATE void fb_15bpp_clear_all(c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_15bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_15bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_15bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_15bpp_get_pixel_argb32(int xx, int yy);
PRIVATE void fb_15bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_15BPP

#ifdef ENABLE_16BPP
PRIVATE void fb_16bpp_clear_all(c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_16bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_16bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_16bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_16bpp_get_pixel_argb32(int xx, int yy);
PRIVATE void fb_16bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_16BPP

#if defined(ENABLE_15BPP) || defined(ENABLE_16BPP)
PRIVATE void fb_15_16bpp_clear_all(u_short rgb);
PRIVATE void fb_15_16bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, u_short rgb);
PRIVATE void fb_15_16bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, u_short rgb);
PRIVATE void fb_15_16bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 u_short bc_rgb, u_short fc_rgb);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE u_short fb_15_16bpp_get_pixel_rgb16(int xx, int yy);
PRIVATE void fb_15_16bpp_reverse_all(u_short reverse_bits);
#endif // ENABLE_SCREEN_SHOT
#endif /* defined(ENABLE_15BPP) || defined(ENABLE_16BPP) */

#ifdef ENABLE_24BPP
PRIVATE void fb_24bpp_clear_all(c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_24bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_24bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_24bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_24bpp_get_pixel_argb32(int xx, int yy);
PRIVATE void fb_24bpp_reverse_all(void);
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_24BPP

#ifdef ENABLE_32BPP
PRIVATE void fb_32bpp_clear_all(c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_32bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_32bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb);
PRIVATE void fb_32bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb);
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_32bpp_get_pixel_argb32(int xx, int yy);
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
///TTT#define I810_FB_DUMMY_ACCESS

void fb_set_metrics(void)
{
	// copy values into local variables for speed
	fb_fb_start = frame_buffer__.fb_start;
	fb_fb_mem_size = frame_buffer__.fb_mem_size;
	fb_fb_end = fb_fb_start + fb_fb_mem_size;
	fb_fb_bytes_per_line = frame_buffer__.bytes_per_line;
	fb_dots_per_pixel_x = cur_font_expand_x;
	fb_dots_per_pixel_y = cur_font_expand_y;
flf_d_printf("dots_per_pixel_x:%d, dots_per_pixel_y:%d\n", fb_dots_per_pixel_x, fb_dots_per_pixel_y);
}

#ifdef ENABLE_8BPP
PRIVATE u_char pallete_idx_from_color_idx(u_char color_idx)
{
	u_char pal_idx;

	// swap bit 2 and 0
	pal_idx = (color_idx & 0x0a)
	 | ((color_idx & 0x01) << 2)	// bit 0 ==> bit 2
	 | ((color_idx & 0x04) >> 2);	// bit 2 ==> bit 0
	return pal_idx;
}
#ifdef ENABLE_SCREEN_SHOT
PRIVATE u_char color_idx_from_pallete_idx(u_char pal_idx)
{
	u_char color_idx;

	// swap bit 2 and 0
	color_idx = (pal_idx & 0x0a)
	 | ((pal_idx & 0x01) << 2)	// bit 0 ==> bit 2
	 | ((pal_idx & 0x04) >> 2);	// bit 2 ==> bit 0
	return color_idx;
}
#endif // ENABLE_SCREEN_SHOT
/* 8 bpp */
PRIVATE void fb_8bpp_clear_all(c_idx_t color_idx, rgb15_t rgb)
{
	u_char pal_idx;

	pal_idx = pallete_idx_from_color_idx(color_idx);
	memset(fb_fb_start, pal_idx, fb_fb_mem_size);
}
PRIVATE void fb_8bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	int bottom_y, char_sx;
	u_char pal_idx;
	int yy;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	pal_idx = pallete_idx_from_color_idx(color_idx);
	for (yy = top_y; yy < bottom_y; yy++) {
		memset(fb_fb_start + yy * fb_fb_bytes_per_line + left_x, pal_idx, char_sx);
	}
}
PRIVATE void fb_8bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	int bottom_y, char_sx, dots_x;
	u_char pal_idx;
	int yy;
	volatile u_char *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	pal_idx = pallete_idx_from_color_idx(color_idx);
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_char);
		for (dots_x = 0; dots_x < char_sx; dots_x++) {
			*fb ^= pal_idx;
			fb++;
		}
	}
#ifdef I810_FB_DUMMY_ACCESS
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_char);
		*fb = (volatile u_char)*fb;
	}
#endif	// I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_8bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb)
{
	int bottom_y, dots_y, dots_x;
	u_char bc_pal_idx;
	u_char fc_pal_idx;
	int yy, xx8, xx;
	const u_char *fnt;
	u_char fnt_byte;
	u_char bit;
	volatile u_char *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;

	bc_pal_idx = pallete_idx_from_color_idx(bc_idx);
	fc_pal_idx = pallete_idx_from_color_idx(fc_idx);
	for (yy = top_y; yy < bottom_y; ) {
		for (dots_y = 0; dots_y < fb_dots_per_pixel_y; dots_y++) {
			fnt = font;
			fb = fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_char);
			for (xx8 = 0; xx8 < fnt_sx; xx8 += 8) {
				fnt_byte = *fnt++;
				bit = 0x80;
				for (xx = xx8; xx < xx8 + 8 && xx < fnt_sx; xx++) {
					for (dots_x = 0; dots_x < fb_dots_per_pixel_x; dots_x++) {
						*fb++ = (fnt_byte & bit) ? fc_pal_idx : bc_pal_idx;
					}
					bit >>= 1;
				}
			}
			yy++;
		}
		font += font_bytes_per_line;
	}
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_8bpp_get_pixel_argb32(int xx, int yy)
{
	volatile u_char *fb;

	fb = fb_fb_start + yy * fb_fb_bytes_per_line + xx * sizeof(u_char);
	return argb32_from_color_idx(color_idx_from_pallete_idx(*fb));
}
PRIVATE void fb_8bpp_reverse_all(void)
{
	volatile u_char *fb;

	for (fb = fb_fb_start; fb < fb_fb_end; ) {
		*fb = *fb ^ 0x0f; fb++;
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_8BPP

#if defined(ENABLE_15BPP)
PRIVATE void fb_15bpp_clear_all(c_idx_t color_idx, rgb15_t rgb)
{
	rgb15_t rgb15;

	rgb15 = rgb15_from_color_idx(color_idx);
	fb_15_16bpp_clear_all(rgb15);
}
PRIVATE void fb_15bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	rgb15_t rgb15;

	rgb15 = rgb15_from_color_idx(color_idx);
	fb_15_16bpp_fill_char_box(left_x, top_y, fnt_sx, fnt_sy, rgb15);
}
PRIVATE void fb_15bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	rgb15_t rgb15;

	rgb15 = rgb15_from_color_idx(color_idx);
	fb_15_16bpp_reverse_char_box(left_x, top_y, fnt_sx, fnt_sy, rgb15);
}
PRIVATE void fb_15bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb)
{
	rgb15_t bc_rgb15;
	rgb15_t fc_rgb15;

	bc_rgb15 = rgb15_from_color_idx(bc_idx);
	fc_rgb15 = rgb15_from_color_idx(fc_idx);
	fb_15_16bpp_paint_char_box(left_x, top_y,
	 font, fnt_sx, fnt_sy, font_bytes_per_line,
	 bc_rgb15, fc_rgb15);
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_15bpp_get_pixel_argb32(int xx, int yy)
{
	rgb15_t rgb15;

	rgb15 = fb_15_16bpp_get_pixel_rgb16(xx, yy);
	return argb32_from_rgb15(rgb15);
}
PRIVATE void fb_15bpp_reverse_all(void)
{
	fb_15_16bpp_reverse_all(0x7fff);	// 0rrrrrgggggbbbbb
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_15BPP

#if defined(ENABLE_16BPP)
PRIVATE void fb_16bpp_clear_all(c_idx_t color_idx, rgb15_t rgb)
{
	rgb16_t rgb16;

	rgb16 = rgb16_from_color_idx(color_idx);
	fb_15_16bpp_clear_all(rgb16);
}
PRIVATE void fb_16bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	rgb16_t rgb16;

	rgb16 = rgb16_from_color_idx(color_idx);
	fb_15_16bpp_fill_char_box(left_x, top_y, fnt_sx, fnt_sy, rgb16);
}
PRIVATE void fb_16bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	rgb16_t rgb16;

	rgb16 = rgb16_from_color_idx(color_idx);
	fb_15_16bpp_reverse_char_box(left_x, top_y, fnt_sx, fnt_sy, rgb16);
}
PRIVATE void fb_16bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb)
{
	rgb16_t bc_rgb16;
	rgb16_t fc_rgb16;

	bc_rgb16 = rgb16_from_color_idx(bc_idx);
	fc_rgb16 = rgb16_from_color_idx(fc_idx);
	fb_15_16bpp_paint_char_box(left_x, top_y,
	 font, fnt_sx, fnt_sy, font_bytes_per_line,
	 bc_rgb16, fc_rgb16);
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_16bpp_get_pixel_argb32(int xx, int yy)
{
	rgb16_t rgb16;

	rgb16 = fb_15_16bpp_get_pixel_rgb16(xx, yy);
	return argb32_from_rgb16(rgb16);
}
PRIVATE void fb_16bpp_reverse_all(void)
{
	fb_15_16bpp_reverse_all(0xffff);	// rrrrrggggggbbbbb
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_16BPP

#if defined(ENABLE_15BPP) || defined(ENABLE_16BPP)
/* 15 bpp */
/* 16 bpp */
PRIVATE void fb_15_16bpp_clear_all(u_short rgb)
{
	u_short *fb;

	for (fb = (u_short*)fb_fb_start; fb < (u_short*)fb_fb_end; ) {
		*fb++ = rgb;
	}
}
PRIVATE void fb_15_16bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, u_short rgb)
{
	int bottom_y, char_sx, dots_x;
	int yy;
	u_short *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	for (yy = top_y; yy < bottom_y; yy++) {
		fb = (u_short*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_short));
		for (dots_x = 0; dots_x < char_sx; dots_x++) {
			*fb++ = rgb;
		}
	}
}
PRIVATE void fb_15_16bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, u_short rgb)
{
	int bottom_y, char_sx, dots_x;
	int yy;
	volatile u_short *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	for (yy = top_y; yy < bottom_y; yy++) {
		fb = (u_short*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_short));
		for (dots_x = 0; dots_x < char_sx; dots_x++) {
			*fb++ ^= rgb;
		}
	}
#ifdef I810_FB_DUMMY_ACCESS
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = (u_short*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_short));
		*fb = (volatile u_short)*fb;
	}
#endif	// I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_15_16bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 u_short bc_rgb, u_short fc_rgb)
{
	int bottom_y, dots_y, dots_x;
	int yy, xx8, xx;
	const u_char *fnt;
	u_char fnt_byte;
	u_char bit;
	volatile u_short *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;

	for (yy = top_y; yy < bottom_y; ) {
		for (dots_y = 0; dots_y < fb_dots_per_pixel_y; dots_y++) {
			fnt = font;
			fb = (u_short*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_short));
			for (xx8 = 0; xx8 < fnt_sx; xx8 += 8) {
				fnt_byte = *fnt++;
				bit = 0x80;
				for (xx = xx8; xx < xx8 + 8 && xx < fnt_sx; xx++) {
					for (dots_x = 0; dots_x < fb_dots_per_pixel_x; dots_x++) {
						*fb++ = (fnt_byte & bit) ? fc_rgb : bc_rgb;
					}
					bit >>= 1;
				}
			}
			yy++;
		}
		font += font_bytes_per_line;
	}
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE u_short fb_15_16bpp_get_pixel_rgb16(int xx, int yy)
{
	volatile u_short *fb;
	u_short rgb16;

	fb = (u_short *)(fb_fb_start + yy * fb_fb_bytes_per_line + xx * sizeof(u_short));
	rgb16 = *fb;
	return (((rgb16 >> 10) & 0x1f) << 16) + (((rgb16 >> 5) & 0x1f) << 8) + rgb16 & 0x1f;
}
PRIVATE void fb_15_16bpp_reverse_all(u_short reverse_bits)
{
	volatile u_char *fb;

	for (fb = fb_fb_start; fb < fb_fb_end; ) {
		*(u_short *)fb = (*(u_short *)fb) ^ reverse_bits;
		fb += sizeof(u_short);
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif /* defined(ENABLE_15BPP) || defined(ENABLE_16BPP) */

#ifdef ENABLE_24BPP
/* 24 bpp */
#define SIZEOF24BPP		3
PRIVATE void fb_24bpp_clear_all(c_idx_t color_idx, rgb15_t rgb)
{
	u_char b0, b1, b2;
	u_char *fb;

	r_g_b_24_from_rgb15(rgb, &b0, &b1, &b2);
	for (fb = fb_fb_start; fb < fb_fb_end; ) {
		fb[0] = b0;
		fb[1] = b1;
		fb[2] = b2;
		fb += SIZEOF24BPP;
	}
}
PRIVATE void fb_24bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	int bottom_y, char_sx, dots_x;
	u_char b0, b1, b2;
	int yy;
	u_char *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	r_g_b_24_from_rgb15(rgb, &b0, &b1, &b2);
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = fb_fb_start + yy * fb_fb_bytes_per_line + left_x * SIZEOF24BPP;
		for (dots_x = 0; dots_x < char_sx; dots_x++) {
			fb[0] = b0;
			fb[1] = b1;
			fb[2] = b2;
			fb += SIZEOF24BPP;
		}
	}
}
PRIVATE void fb_24bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	int bottom_y, char_sx, dots_x;
	u_char b0, b1, b2;
	int yy;
	volatile u_char *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	r_g_b_24_from_rgb15(rgb, &b0, &b1, &b2);
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = fb_fb_start + yy * fb_fb_bytes_per_line + left_x * SIZEOF24BPP;
		for (dots_x = 0; dots_x < char_sx; dots_x++) {
			fb[0] ^= b0;
			fb[1] ^= b1;
			fb[2] ^= b2;
			fb += SIZEOF24BPP;
		}
	}
#ifdef I810_FB_DUMMY_ACCESS
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = fb_fb_start + yy * fb_fb_bytes_per_line + left_x * SIZEOF24BPP;
		fb[0] = (volatile u_char)fb[0];
		fb[1] = (volatile u_char)fb[1];
		fb[2] = (volatile u_char)fb[2];
	}
#endif	// I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_24bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb)
{
	int bottom_y, dots_y, dots_x;
	u_char bc_b0, bc_b1, bc_b2;
	u_char fc_b0, fc_b1, fc_b2;
	int yy, xx8, xx;
	const u_char *fnt;
	u_char fnt_byte;
	u_char bit;
	volatile u_char *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;

	r_g_b_24_from_rgb15(bc_rgb, &bc_b0, &bc_b1, &bc_b2);
	r_g_b_24_from_rgb15(fc_rgb, &fc_b0, &fc_b1, &fc_b2);
	for (yy = top_y; yy < bottom_y; ) {
		for (dots_y = 0; dots_y < fb_dots_per_pixel_y; dots_y++) {
			fnt = font;
			fb = fb_fb_start + yy * fb_fb_bytes_per_line + left_x * SIZEOF24BPP;
			for (xx8 = 0; xx8 < fnt_sx; xx8 += 8) {
				fnt_byte = *fnt++;
				bit = 0x80;
				for (xx = xx8; xx < xx8 + 8 && xx < fnt_sx; xx++) {
					for (dots_x = 0; dots_x < fb_dots_per_pixel_x; dots_x++) {
						if (fnt_byte & bit) {
							fb[0] = fc_b0;
							fb[1] = fc_b1;
							fb[2] = fc_b2;
						} else {
							fb[0] = bc_b0;
							fb[1] = bc_b1;
							fb[2] = bc_b2;
						}
						fb += SIZEOF24BPP;
					}
					bit >>= 1;
				}
			}
			yy++;
		}
		font += font_bytes_per_line;
	}
}

#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_24bpp_get_pixel_argb32(int xx, int yy)
{
	volatile u_char *fb;
	u_int32 argb32;

	fb = fb_fb_start + yy * fb_fb_bytes_per_line + xx * SIZEOF24BPP;
	argb32 = ((u_int32)fb[2] << 16) + ((u_int32)fb[1] << 8) + fb[0];
	return argb32;		// rrrrrrrrggggggggbbbbbbbb
}
PRIVATE void fb_24bpp_reverse_all(void)
{
	volatile u_char *fb;

	for (fb = fb_fb_start; fb < fb_fb_end; ) {
		*fb = ~ *fb; fb++;	// bb
		*fb = ~ *fb; fb++;	// gg
		*fb = ~ *fb; fb++;	// rr
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_24BPP

#ifdef ENABLE_32BPP
/* 32 bpp */
PRIVATE void fb_32bpp_clear_all(c_idx_t color_idx, rgb15_t rgb)
{
	u_int32 argb32;
	u_int32 *fb;

	argb32 = argb32_from_rgb15(rgb);
	for (fb = (u_int32 *)fb_fb_start; fb < (u_int32 *)fb_fb_end; ) {
		*fb++ = argb32;
	}
}
PRIVATE void fb_32bpp_fill_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	int bottom_y, char_sx, dots_x;
	u_int32 argb32;
	int yy;
	u_int32 *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	argb32 = argb32_from_rgb15(rgb);
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = (u_int32*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_int32));
		for (dots_x = 0; dots_x < char_sx; dots_x++) {
			*fb++ = argb32;
		}
	}
}
PRIVATE void fb_32bpp_reverse_char_box(int left_x, int top_y, int fnt_sx, int fnt_sy, c_idx_t color_idx, rgb15_t rgb)
{
	int bottom_y, char_sx, dots_x;
	u_int32 argb32;
	int yy;
	volatile u_int32 *fb;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;
	char_sx = fnt_sx * fb_dots_per_pixel_x;

	argb32 = argb32_from_rgb15(rgb);
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = (u_int32*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_int32));
		for (dots_x = 0; dots_x < char_sx; dots_x++) {
			*fb ^= argb32;
			fb++;
		}
	}
#ifdef I810_FB_DUMMY_ACCESS
	for (yy = top_y; yy < bottom_y; yy++) {
		fb = (u_int32*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(u_int32));
		*fb = (volatile u_int32)*fb;
	}
#endif	// I810_FB_DUMMY_ACCESS
}
PRIVATE void fb_32bpp_paint_char_box(int left_x, int top_y,
 const u_char *font, int fnt_sx, int fnt_sy, int font_bytes_per_line,
 c_idx_t bc_idx, c_idx_t fc_idx, rgb15_t bc_rgb, rgb15_t fc_rgb)
{
	int bottom_y;
	argb32_t bc_argb32;
	argb32_t fc_argb32;
	const u_char *fnt;
	volatile argb32_t *fb;
	int dots_y, dots_x;
	int yy, xx8, xx;
	u_char fnt_byte;
	u_char bit;

	bottom_y = (top_y + fnt_sy) * fb_dots_per_pixel_y;
	top_y *= fb_dots_per_pixel_y;
	left_x *= fb_dots_per_pixel_x;

	bc_argb32 = argb32_from_rgb15(bc_rgb);
	fc_argb32 = argb32_from_rgb15(fc_rgb);
	for (yy = top_y; yy < bottom_y; ) {
		for (dots_y = 0; dots_y < fb_dots_per_pixel_y; dots_y++) {
			fnt = font;
			fb = (argb32_t*)(fb_fb_start + yy * fb_fb_bytes_per_line + left_x * sizeof(argb32_t));
			for (xx8 = 0; xx8 < fnt_sx; xx8 += 8) {
				fnt_byte = *fnt++;
				bit = 0x80;
				for (xx = xx8; xx < xx8 + 8 && xx < fnt_sx; xx++) {
					for (dots_x = 0; dots_x < fb_dots_per_pixel_x; dots_x++) {
						*fb++ = (fnt_byte & bit) ? fc_argb32 : bc_argb32;
					}
					bit >>= 1;
				}
			}
			yy++;
		}
		font += font_bytes_per_line;
	}
}
#ifdef ENABLE_SCREEN_SHOT
PRIVATE argb32_t fb_32bpp_get_pixel_argb32(int xx, int yy)
{
	argb32_t argb32;

	argb32 = *(argb32_t*)(fb_fb_start + yy * fb_fb_bytes_per_line + xx * sizeof(argb32_t));
	return argb32;		// 00000000rrrrrrrrggggggggbbbbbbbb
}
PRIVATE void fb_32bpp_reverse_all(void)
{
	volatile argb32_t *fb;

	for (fb = (argb32_t *)fb_fb_start; fb < (argb32_t *)fb_fb_end; ) {
		*fb = *fb ^ 0x00ffffff;		// aarrggbb
		fb += sizeof(argb32_t) * 4;
	}
}
#endif // ENABLE_SCREEN_SHOT
#endif // ENABLE_32BPP

#ifdef ENABLE_DEBUG
void test_fill_top_of_fb(void)
{
#define TEST_FILL_SIZE	(80 * 25 * 4 * 5)
	static u_char base = 0x00;
	volatile u_char *fb;
_MFLF_
	fb = fb_fb_start;
	for (size_t bytes = 0; bytes < TEST_FILL_SIZE; bytes++) {
		fb[bytes] = base + bytes;
	}
	base += 0x10;
}
#endif // ENABLE_DEBUG

// End of fbbpp.c
