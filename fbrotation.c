/*
 * UCON -
 * Copyright (C) 2013 Toshio Koike
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
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
 */

#include "ucon.h"

view_rotation_t contents_rotation;

// fill in rotation independent part
void fbr_setup_constant_parameters()
{
	fb_start = fb__.fb_start;
	fb_size = fb__.fb_size;
	fb_end = fb__.fb_start + fb__.fb_size;
	fb_bytes_per_pixel = fb__.bytes_per_pixel;
	fb_bytes_inc_px = fb__.bytes_per_pixel;
	///fb_bytes_inc_py = fb__.bytes_per_pixel * fb__.screen_size_x;
	fb_bytes_inc_py = fb__.bytes_per_line;

	fb_top_left = fb__.fb_start
	 + fb_bytes_inc_px * (fb__.screen_size_x - 1) * 0
	 + fb_bytes_inc_py * (fb__.screen_size_y - 1) * 0;
	fb_top_right = fb__.fb_start
	 + fb_bytes_inc_px * (fb__.screen_size_x - 1) * 1
	 + fb_bytes_inc_py * (fb__.screen_size_y - 1) * 0;
	fb_bottom_left = fb__.fb_start
	 + fb_bytes_inc_px * (fb__.screen_size_x - 1) * 0
	 + fb_bytes_inc_py * (fb__.screen_size_y - 1) * 1;
	fb_bottom_right = fb__.fb_start
	 + fb_bytes_inc_px * (fb__.screen_size_x - 1) * 1
	 + fb_bytes_inc_py * (fb__.screen_size_y - 1) * 1;
flf_d_printf("fb_start/fb_end: %p/%p\n", fb_start, fb_end);
flf_d_printf("fb_size: %d\n", fb_size);
flf_d_printf("fb_bytes_per_pixel: %d\n", fb_bytes_per_pixel);
flf_d_printf("fb_bytes_inc_px: %d\n", fb_bytes_inc_px);
flf_d_printf("fb_bytes_inc_py: %d\n", fb_bytes_inc_py);

flf_d_printf("%p, top_left:     %d\n", fb_top_left, fb_top_left - fb__.fb_start);
flf_d_printf("%p, top_right:    %d\n", fb_top_right, fb_top_right - fb__.fb_start);
flf_d_printf("%p, bottom_left:  %d\n", fb_bottom_left, fb_bottom_left - fb__.fb_start);
flf_d_printf("%p, bottom_right: %d\n", fb_bottom_right, fb_bottom_right - fb__.fb_start);
	u_char *fba;
	fba = fb_top_left;
	flf_d_printf("fb_top_left     [%d, %d]\n",
	 ((fba - fb_start) % fb_bytes_inc_py) / fb_bytes_inc_px,
	 (fba - fb_start) / fb_bytes_inc_py);
	fba = fb_top_right;
	flf_d_printf("fb_top_right    [%d, %d]\n",
	 ((fba - fb_start) % fb_bytes_inc_py) / fb_bytes_inc_px,
	 (fba - fb_start) / fb_bytes_inc_py);
	fba = fb_bottom_left;
	flf_d_printf("fb_bottom_left  [%d, %d]\n",
	 ((fba - fb_start) % fb_bytes_inc_py) / fb_bytes_inc_px,
	 (fba - fb_start) / fb_bytes_inc_py);
	fba = fb_bottom_right;
	flf_d_printf("fb_bottom_right [%d, %d]\n",
	 ((fba - fb_start) % fb_bytes_inc_py) / fb_bytes_inc_px,
	 (fba - fb_start) / fb_bytes_inc_py);
}

void fbr_copy_font_size_into_driver()
{
_FLF_
	fb_font_size_hx = cur_font->width;
	fb_font_size_hy = cur_font->height;
	fb_font_mul_hx = cur_font_mul->mul_x;
	fb_font_mul_hy = cur_font_mul->mul_y;
	fb_font_exp_size_hx = cur_font_exp->width;
	fb_font_exp_size_hy = cur_font_exp->height;
flf_d_printf("fb_font_size_hx: %d\n", fb_font_size_hx);
flf_d_printf("fb_font_size_hy: %d\n", fb_font_size_hy);
flf_d_printf("fb_font_mul_hx: %d\n", fb_font_mul_hx);
flf_d_printf("fb_font_mul_hy: %d\n", fb_font_mul_hy);
flf_d_printf("fb_font_exp_size_hx: %d\n", fb_font_exp_size_hx);
flf_d_printf("fb_font_exp_size_hy: %d\n", fb_font_exp_size_hy);
flf_d_printf("bytes_per_char: %d\n", fb_font_exp_size_hx * fb_font_exp_size_hy * fb_bytes_per_pixel);
}
void fbr_set_rotation(view_rotation_t rotation)
{
	rotation = (view_rotation_t)(rotation % ROT360);
	contents_rotation = rotation;
}
view_rotation_t fbr_get_rotation()
{
	return contents_rotation;
}
void fbr_setup_rotation_dependent_parameters()
{
_FLF_
	switch (contents_rotation) {
	case ROT000:
	case ROT180:
		fbr_screen_size_hx = fb__.screen_size_x;
		fbr_screen_size_hy = fb__.screen_size_y;
		break;
	case ROT090:
	case ROT270:
		fbr_screen_size_hx = fb__.screen_size_y;
		fbr_screen_size_hy = fb__.screen_size_x;
		break;
	}
_FLF_
	switch (contents_rotation) {
	case ROT000:
		fbr_origin = fb_top_left;
		fbr_bytes_inc_hx = + fb_bytes_inc_px;
		fbr_bytes_inc_hy = + fb_bytes_inc_py;
		break;
	case ROT090:
		fbr_origin = fb_bottom_left;
		fbr_bytes_inc_hx = - fb_bytes_inc_py;
		fbr_bytes_inc_hy = + fb_bytes_inc_px;
		break;
	case ROT180:
		fbr_origin = fb_bottom_right;
		fbr_bytes_inc_hx = - fb_bytes_inc_px;
		fbr_bytes_inc_hy = - fb_bytes_inc_py;
		break;
	case ROT270:
		fbr_origin = fb_top_right;
		fbr_bytes_inc_hx = + fb_bytes_inc_py;
		fbr_bytes_inc_hy = - fb_bytes_inc_px;
		break;
	}
	fbr_bytes_inc_dot_hx = fbr_bytes_inc_hx * cur_font_mul->mul_x;
	fbr_bytes_inc_dot_hy = fbr_bytes_inc_hy * cur_font_mul->mul_y;
	fbr_bytes_inc_font_hx = fbr_bytes_inc_dot_hx * cur_font->width;
	fbr_bytes_inc_font_hy = fbr_bytes_inc_dot_hy * cur_font->height;
flf_d_printf("fbr_origin: %p\n", fbr_origin);
flf_d_printf("fbr_bytes_inc_hx: %d\n", fbr_bytes_inc_hx);
flf_d_printf("fbr_bytes_inc_hy: %d\n", fbr_bytes_inc_hy);
flf_d_printf("fbr_bytes_inc_dot_hx: %d\n", fbr_bytes_inc_dot_hx);
flf_d_printf("fbr_bytes_inc_dot_hy: %d\n", fbr_bytes_inc_dot_hy);
flf_d_printf("fbr_bytes_inc_font_hx: %d\n", fbr_bytes_inc_font_hx);
flf_d_printf("fbr_bytes_inc_font_hy: %d\n", fbr_bytes_inc_font_hy);

flf_d_printf("start %p, origin %p\n", fb__.fb_start, fbr_origin);
flf_d_printf("fbr_origin: (%d, %d)\n",
 ((fbr_origin - fb_start) / fb_bytes_per_pixel) % fb__.screen_size_x,
 ((fbr_origin - fb_start) / fb_bytes_per_pixel) / fb__.screen_size_x);
}

void fbr_set_text_metrics()
{
flf_d_printf("contents_rotation = %d\n", contents_rotation);
flf_d_printf("cur_font_mul_idx: %d\n", cur_font_mul_idx);
flf_d_printf("cur_font_idx: %d\n", cur_font_idx);
	fbr_chars_hx = fbr_screen_size_hx / cur_font_exp->width;
	fbr_chars_hy = fbr_screen_size_hy / cur_font_exp->height;
flf_d_printf("fbr_chars_hx: %d\n", fbr_chars_hx);
flf_d_printf("fbr_chars_hy: %d\n", fbr_chars_hy);
}

// End of fbcommon.c
