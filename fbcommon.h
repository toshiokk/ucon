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

#ifndef ucon_fbcommon_h
#define ucon_fbcommon_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus
struct frame_buffer_;			// forward declaration
typedef struct frame_buffer_ frame_buffer_t;

struct frame_buffer_ {
#define FB_DEV_NAME_LEN		16		// /dev/fb0
	char fb_dev_name[FB_DEV_NAME_LEN+1];	// FB device name
	int fd_tty0;
	int fd_fb;
	// FB screen size
	int width;
	int height;
	int bytes_per_line;
	int bits_per_pixel;
	int bytes_per_pixel;
	// FB image mmaped to process local
	u_char *fb_mmapped;
	u_int32 fb_offset;
	u_char *fb_start;
	u_int32 fb_view_size;
	u_int32 fb_mem_size;
	fb_driver_t *driver;	// selected FB driver
#ifdef ENABLE_DIMMER
	int my_vt_num;
#endif
};

extern frame_buffer_t frame_buffer__;

int fb_get_fb_dev_name(frame_buffer_t *fb);
int fb_open(frame_buffer_t *fb);
int fb_close(frame_buffer_t *fb);

rgb15_t rgb15_from_color_idx(c_idx_t color_idx);
rgb16_t rgb16_from_color_idx(c_idx_t color_idx);
argb32_t argb32_from_color_idx(c_idx_t color_idx);

void r_g_b_24_from_color_idx(c_idx_t color_idx, u_char *b0, u_char *b1, u_char *b2);
void r_g_b_24_from_rgb15(rgb15_t rgb15, u_char *b0, u_char *b1, u_char *b2);
void r_g_b_24_from_argb32(argb32_t argb32, u_char *b0, u_char *b1, u_char *b2);

rgb15_t rgb15_from_r_g_b_24(u_char rr, u_char gg, u_char bb);
rgb16_t rgb16_from_r_g_b_24(u_char rr, u_char gg, u_char bb);
argb32_t argb32_from_r_g_b_24(u_char rr, u_char gg, u_char bb);
rgb16_t rgb16_from_rgb15(rgb15_t rgb15);
argb32_t argb32_from_rgb15(rgb15_t rgb15);
rgb15_t rgb15_from_rgb16(rgb16_t rgb16);
argb32_t argb32_from_rgb16(rgb15_t rgb16);
rgb15_t rgb15_from_argb32(argb32_t argb32);
rgb16_t rgb16_from_argb32(argb32_t argb32);

#if defined(ENABLE_DIMMER) && defined(FBIOBLANK)
int fb_set_blank(int fd_fb, int vt_num, int blank);
#endif

const char *dump_rgb15(rgb15_t rgb15);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // ucon_fbcommon_h

// End of fbcommon.h
