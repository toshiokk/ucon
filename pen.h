/*
 * UCON -
 * Copyright (C) 1999 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992-1996 Takashi Manabe (manabe@papilio.tutics.tut.ac.jp)
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
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICH AND TAKASHI MANABE ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R. LAMBERT BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#ifndef ucon_pen_h
#define ucon_pen_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

// NOTE: highlight must be applied to only foreground of ANSI 16 colors
#define PEN_ATTR_NONE		0x00	/* no attribute */
#define PEN_ATTR_HIGH		0x01	/* highlight */
#define PEN_ATTR_DIM		0x02	/* dim (unsupported) */
#define PEN_ATTR_ITALIC		0x04	/* italic (unsupported) */
#define PEN_ATTR_ULINE		0x08	/* under line (unsupported) */
#define PEN_ATTR_BLINK		0x10	/* blink (unsupported) */
#define PEN_ATTR_BLINK2		0x20	/* fast blink (unsupported) */
// NOTE: reverse will be applied to ANSI 16/216 colors and RGB colors
#define PEN_ATTR_REVERSE	0x40	/* reverse */
#define PEN_ATTR_HIDDEN		0x80	/* hidden (unsupported) */

typedef struct /*pen_*/ {
	int yy;
	int xx;
	c_idx_t bc_idx;		// background color index, -1: invalid
	c_idx_t fc_idx;		// foreground color index, -1: invalid
	rgb15_t bc_rgb;		// background color 15 bit RGB
	rgb15_t fc_rgb;		// foreground color 15 bit RGB
	u_char attr;
} pen_t;

void pen_init(pen_t *pen);
void pen_destroy(pen_t *pen);
void pen_copy(pen_t *dest, pen_t *src);

void pen_move_yx(pen_t *pen, int yy, int xx);
void pen_set_yx(pen_t *pen, int yy, int xx);
void pen_get_yxy(pen_t *pen, int *yy, int *xx);
int pen_get_x(pen_t *pen);
int pen_get_y(pen_t *pen);

void pen_off_all_attr(pen_t *pen);
void pen_highlight(pen_t *pen);
void pen_unhighlight(pen_t *pen);
void pen_reverse(pen_t *pen);
void pen_no_reverse(pen_t *pen);

void pen_set_bgc_fgc_idx(pen_t *pen, c_idx_t bc_idx, c_idx_t fc_idx);
void pen_set_bgc_idx(pen_t *pen, c_idx_t color_idx);
void pen_set_fgc_idx(pen_t *pen, c_idx_t color_idx);
c_idx_t pen_get_bgc_idx(pen_t *pen);
c_idx_t pen_get_fgc_idx(pen_t *pen);

void pen_conv_bgc_fgc_idx_to_rgb(pen_t *pen);
void pen_conv_bgc_idx_to_rgb(pen_t *pen);
void pen_conv_fgc_idx_to_rgb(pen_t *pen);
void pen_set_bc_rgb_direct(pen_t *pen, rgb15_t rgb);
void pen_set_fc_rgb_direct(pen_t *pen, rgb15_t rgb);
void pen_set_bc_rgb(pen_t *pen, rgb15_t rgb);
void pen_set_fc_rgb(pen_t *pen, rgb15_t rgb);
rgb15_t pen_get_bc_rgb(pen_t *pen);
rgb15_t pen_get_fc_rgb(pen_t *pen);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // ucon_pen_h

// End of pen.h
