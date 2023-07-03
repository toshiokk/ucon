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
 * THIS SOFTWARE IS PROVIDED BY TAKASHI MANABE ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
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

#include "ucon.h"

void pen_init(pen_t *pen)
{
	pen->xx = 0;
	pen->yy = 0;
	pen_off_all_attr(pen);
}
void pen_destroy(pen_t *pen)
{
}
void pen_copy(pen_t *dest, pen_t *src)
{
	dest->xx = src->xx;
	dest->yy = src->yy;
	dest->bc_idx = src->bc_idx;
	dest->fc_idx = src->fc_idx;
	dest->bc_rgb = src->bc_rgb;
	dest->fc_rgb = src->fc_rgb;
	dest->attr = src->attr;
}

void pen_move_yx(pen_t *pen, int yy, int xx)
{
	pen->xx += xx;
	pen->yy += yy;
}
void pen_set_yx(pen_t *pen, int yy, int xx)
{
	if (xx >= 0) {
		pen->xx = xx;
	}
	if (yy >= 0) {
		pen->yy = yy;
	}
}
void pen_get_yxy(pen_t *pen, int *yy, int *xx)
{
	*xx = pen->xx;
	*yy = pen->yy;
}
int pen_get_x(pen_t *pen)
{
	return pen->xx;
}
int pen_get_y(pen_t *pen)
{
	return pen->yy;
}

void pen_off_all_attr(pen_t *pen)
{
	pen->attr = PEN_ATTR_NONE;
	pen_set_bgc_fgc_idx(pen, COLOR_DEF_BC, COLOR_DEF_FC);
}
void pen_highlight(pen_t *pen)
{
	pen->attr |= PEN_ATTR_HIGH;
	pen_conv_bgc_fgc_idx_to_rgb(pen);
}
void pen_unhighlight(pen_t *pen)
{
	pen->attr &= ~PEN_ATTR_HIGH;
	pen_conv_bgc_fgc_idx_to_rgb(pen);
}
void pen_italic(pen_t *pen)
{
	pen->attr |= PEN_ATTR_ITALIC;
}
void pen_unitalic(pen_t *pen)
{
	pen->attr &= ~PEN_ATTR_ITALIC;
}
void pen_reverse(pen_t *pen)
{
	pen->attr |= PEN_ATTR_REVERSE;
	pen_conv_bgc_fgc_idx_to_rgb(pen);
}
void pen_no_reverse(pen_t *pen)
{
	pen->attr &= ~PEN_ATTR_REVERSE;
	pen_conv_bgc_fgc_idx_to_rgb(pen);
}

void pen_set_bgc_fgc_idx(pen_t *pen, c_idx_t bc_idx, c_idx_t fc_idx)
{
	pen_set_bgc_idx(pen, bc_idx);
	pen_set_fgc_idx(pen, fc_idx);
}
void pen_set_bgc_idx(pen_t *pen, c_idx_t color_idx)
{
	if (color_idx >= 0) {
		pen->bc_idx = color_idx;
		pen_conv_bgc_idx_to_rgb(pen);
	}
}
void pen_set_fgc_idx(pen_t *pen, c_idx_t color_idx)
{
	if (color_idx >= 0) {
		pen->fc_idx = color_idx;
		pen_conv_fgc_idx_to_rgb(pen);
	}
}

c_idx_t pen_get_bgc_idx(pen_t *pen)
{
	c_idx_t color_idx;

	if ((pen->attr & PEN_ATTR_REVERSE) == 0) {
		color_idx = pen->bc_idx;
	} else {
		color_idx = pen->fc_idx;
	}
	if (color_idx < 0) {
		color_idx = COLOR_DEF_BC;
	}
	return color_idx;
}
c_idx_t pen_get_fgc_idx(pen_t *pen)
{
	c_idx_t color_idx;

	if ((pen->attr & PEN_ATTR_REVERSE) == 0) {
		color_idx = pen->fc_idx;
	} else {
		color_idx = pen->bc_idx;
	}
	if (color_idx < 0) {
		color_idx = COLOR_DEF_FC;
	}
	// apply highlight only to foreground color
	if (pen->attr & PEN_ATTR_HIGH) {
		if (color_idx >= COLOR_BLACK) {
			color_idx |= COLOR_HIGHLIGHT;
		}
	}
	return color_idx;
}

void pen_conv_bgc_fgc_idx_to_rgb(pen_t *pen)
{
	pen_conv_bgc_idx_to_rgb(pen);
	pen_conv_fgc_idx_to_rgb(pen);
}
// convert color_idx to rgb15_t
void pen_conv_bgc_idx_to_rgb(pen_t *pen)
{
	c_idx_t color_idx;

	color_idx = pen->bc_idx;
	if (pen->attr & PEN_ATTR_REVERSE) {
		// reverse attribute is set, backgound color will become a foreground color
		// apply highlight only to foreground color
		if (pen->attr & PEN_ATTR_HIGH) {
			if (color_idx >= COLOR_BLACK) {
				color_idx |= COLOR_HIGHLIGHT;
			}
		}
	}
	if (color_idx >= 0) {		// valid ?
		pen_set_bc_rgb(pen, rgb15_from_color_idx(color_idx));
	}
}
// convert color_idx to rgb15_t
void pen_conv_fgc_idx_to_rgb(pen_t *pen)
{
	c_idx_t color_idx;

	color_idx = pen->fc_idx;
	if ((pen->attr & PEN_ATTR_REVERSE) == 0) {
		// reverse attribute is not set
		// apply highlight only to foreground color
		if (pen->attr & PEN_ATTR_HIGH) {
			if (color_idx >= COLOR_BLACK) {
				color_idx |= COLOR_HIGHLIGHT;
			}
		}
	}
	if (color_idx >= 0) {		// valid ?
		pen_set_fc_rgb(pen, rgb15_from_color_idx(color_idx));
	}
}

void pen_set_bc_rgb_direct(pen_t *pen, rgb15_t rgb)
{
	pen->bc_idx = -1;		// invalidate color_idx
	pen_set_bc_rgb(pen, rgb);
}
void pen_set_fc_rgb_direct(pen_t *pen, rgb15_t rgb)
{
	pen->fc_idx = -1;		// invalidate color_idx
	pen_set_fc_rgb(pen, rgb);
}
void pen_set_bc_rgb(pen_t *pen, rgb15_t rgb)
{
	pen->bc_rgb = rgb;
}
void pen_set_fc_rgb(pen_t *pen, rgb15_t rgb)
{
	pen->fc_rgb = rgb;
}
rgb15_t pen_get_bc_rgb(pen_t *pen)
{
	if ((pen->attr & PEN_ATTR_REVERSE) == 0) {
		return pen->bc_rgb;
	} else {
		// reverse attribute is set, backgound color will become a foreground color
		return pen->fc_rgb;
	}
}
rgb15_t pen_get_fc_rgb(pen_t *pen)
{
	if ((pen->attr & PEN_ATTR_REVERSE) == 0) {
		return pen->fc_rgb;
	} else {
		// reverse attribute is set, foregound color will become a background color
		return pen->bc_rgb;
	}
}

// End of pen.c
