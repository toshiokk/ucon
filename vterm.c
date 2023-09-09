/*
 * UCON -
 * Copyright (C) 2003 Fumitoshi Ukai <ukai@debian.or.jp>
 * Copyright (C) 1999 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992-1996 Takashi MANABE (manabe@papilio.tutics.tut.ac.jp)
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

PRIVATE int vterm_iso_C0_set(vterm_t *vterm, u_char chr);
PRIVATE void vterm_reset_utf8_state(vterm_t *vterm);
PRIVATE int vterm_parse_utf8_seq(vterm_t *vterm, u_char chr);
PRIVATE void vterm_put_ucs21_char(vterm_t *vterm, wchar_t ucs21);
PRIVATE void vterm_wrap_pen_pos(vterm_t *vterm, int width);

PRIVATE void vterm_parse_func_esc(vterm_t *vterm, u_char chr);
PRIVATE void vterm_parse_func_csi(vterm_t *vterm, u_char chr);
PRIVATE void vterm_parse_func_osc(vterm_t *vterm, u_char chr);

#ifdef ENABLE_STATUS_LINE
PRIVATE void vterm_init_status_line(vterm_t *vterm);
PRIVATE void vterm_parse_func_status_line(vterm_t *vterm, u_char chr);
#endif // ENABLE_STATUS_LINE

PRIVATE void vterm_set_mode(vterm_t *vterm, u_short mode, bool_t on_off);
PRIVATE int vterm_esc_set_attr(vterm_t *vterm, int num, int arg_idx);
PRIVATE rgb15_t rgb15_from_color256_idx(u_char color256_idx);

PRIVATE void vterm_init_pen_stack(vterm_t *vterm);
PRIVATE void vterm_push_pen(vterm_t *vterm, bool_t bb);
PRIVATE void vterm_pop_pen(vterm_t *vterm, bool_t bb);

PRIVATE void vterm_set_scroll_region__adjust(vterm_t *vterm, int top_y, int bottom_y);
PRIVATE void vterm_init_scroll_region(vterm_t *vterm);
PRIVATE void vterm_set_scroll_region(vterm_t *vterm, int top_y, int bottom_y);
PRIVATE void vterm_esc_report(vterm_t *vterm, u_char mode, u_short arg);

// ESC [ arg1 ; arg2 ; arg3 ; arg4 ; arg5 ; arg6 ; arg7 ; arg8
#define MAX_CSI_ARGS	8
#define DEFAULT_CSI_ARG_TO_1(arg)	((arg) ? (arg) : 1)
#define DEF_TO_1(arg)				DEFAULT_CSI_ARG_TO_1(arg)
#define DEFAULT_CSI_ARG_TO_0(arg)	((arg) ? (arg)-1 : 0)
#define DEF_TO_0(arg)				DEFAULT_CSI_ARG_TO_0(arg)
PRIVATE void clear_csi_args(void);
PRIVATE int get_csi_arg_idx(void);
PRIVATE int parse_csi_args(vterm_t *vterm, u_char chr);
PRIVATE int get_csi_arg(int arg_idx);

PRIVATE wchar_t subst_dec_sp_gr_char(wchar_t ucs21);

#ifdef ENABLE_DEBUG
PRIVATE void dump_csi_args(char chr);
#endif // ENABLE_DEBUG

PRIVATE void clear_parsed_str(void);
PRIVATE void put_parsed_str(u_char chr);
PRIVATE void dump_parsed_str(void);

void vterm_init(vterm_t *vterm, int columns, int lines)
{
	int overlay_idx;

	vterm->top_y = 0;
	vterm->bottom_y = MAX_TERM_LINES;
	pen_init(&(vterm->pen));
	vterm->cursor.on = TRUE;
	vterm->cursor.xx = 0;
	vterm->cursor.yy = 0;
	vterm->cursor.mode = 0;
	vterm->cursor.blink_counter = 0;
	vterm->cursor.rev_color = COLOR_WHITE;
	vterm->cursor.shown = FALSE;
	vterm->cursor.wide = FALSE;
	vterm->scroll = 0;
	vterm->clear_fb_before_repaint = 0;

	vterm_init_pen_stack(vterm);
#ifdef ENABLE_STATUS_LINE
	vterm_init_status_line(vterm);
#endif // ENABLE_STATUS_LINE

	vterm->tab_size = 8;
	vterm->insert = FALSE;
	vterm->dec_sp_gr = FALSE;
	vterm->esc_seq_parse_state = ESC_ST_0;

	vterm_reset_utf8_state(vterm);

	vterm_reinit(vterm, columns, lines);

	for (overlay_idx = 0; overlay_idx < OVERLAY_LINES; overlay_idx++) {
		vterm_set_overlay(vterm, overlay_idx, -1, 0, COLOR_DEF_BC, COLOR_DEF_FC, "", 0, 0);
	}
}
void vterm_reinit(vterm_t *vterm, int columns, int lines)
{
	columns = MIN_(MAX_TERM_COLS, columns);
	lines = MIN_(MAX_TERM_LINES, lines);
	vterm->text_columns = columns;
	vterm->text_lines = lines;
flf_d_printf("vterm->text_lines:%2d, vterm->text_columns:%2d\n", vterm->text_lines, vterm->text_columns);
	vterm->text_characters = vterm->text_columns * vterm->text_lines;

	verbose_printf("columns:%d, lines:%d, characters:%d\n",
	 vterm->text_columns, vterm->text_lines, vterm->text_characters);

	vterm_init_scroll_region(vterm);

	fb_set_metrics();
_FLF_
}
void vterm_destroy(vterm_t *vterm)
{
}

/*---------------------------------------------------------------------------*/

void vterm_emulate_str__update_screen(vterm_t *vterm, const char *string, int bytes)
{
	vterm_emulate_str(vterm, string, bytes);
	vterm_paint_screen__cursor(vterm);
}

void vterm_emulate_str_yx_bc_fc(vterm_t *vterm, int yy, int xx, c_idx_t bc_idx, c_idx_t fc_idx,
 const char *string, int bytes)
{
	vterm_pen_set_yx(vterm, yy, xx);
	pen_set_bgc_fgc_idx(&(vterm->pen), bc_idx, fc_idx);
	vterm_emulate_str(vterm, string, bytes);
}

// emulate VT-100 terminal
void vterm_emulate_str(vterm_t *vterm, const char *string, int bytes)
{
	u_char chr;

	if (bytes < 0) {
		bytes = strlen(string);
	}
///d_printf("ZZZ");
///dump_string(string, bytes);
///mflf_d_printf("%d: %s\n", bytes, dump_string_to_static_buf(string, bytes));
	while (bytes-- > 0) {
		chr = *(string++);
///mflf_d_printf("%02x[%c]\n", chr, isgraph(chr) ? chr : '.');
		vterm_emulate_char(vterm, chr);
	}
}

int vterm_emulate_char(vterm_t *vterm, u_char chr)
{
	put_parsed_str(chr);
	if (vterm->esc_seq_parse_state > ESC_ST_0) {
		vterm_reset_utf8_state(vterm);
	}
	switch(vterm->esc_seq_parse_state) {
	case ESC_ST_0:
		if (vterm_iso_C0_set(vterm, chr)) {
			vterm_reset_utf8_state(vterm);
		} else {
			if (vterm_parse_utf8_seq(vterm, chr) == 0) {
				vterm_put_ucs21_char(vterm, vterm->ucs21);
			}
		}
		break;
	case ESC_ST_1_ESC:	// ESC
		vterm_parse_func_esc(vterm, chr);
		break;
	case ESC_ST_2_CSI:	// ESC [
		vterm_parse_func_csi(vterm, chr);
		break;
	case ESC_ST_3_OSC:	// ESC ]
		vterm_parse_func_osc(vterm, chr);
		break;
	case ESC_ST_4_LPAR:	// ESC (
		switch (chr) {
		case '0':		// ESC '(' '0' : DEC Special Graphics
			vterm->dec_sp_gr = TRUE;
			break;
		case 'B':		// ESC '(' 'B' : ASCII code set
			// FALLTHROUGH
		default:
			vterm->dec_sp_gr = FALSE;
			break;
		}
		vterm->esc_seq_parse_state = ESC_ST_0;
		break;
#ifdef ENABLE_STATUS_LINE
	case ESC_ST_5_SL:
		vterm_parse_func_status_line(vterm, chr);
		break;
#endif // ENABLE_STATUS_LINE
	}
	if (vterm->scroll > 0) {
		vterm_text_scroll_up(vterm, vterm->scroll);
	} else if (vterm->scroll < 0) {
		vterm_text_scroll_down(vterm, -(vterm->scroll));
	}
	vterm->scroll = 0;
	return 0;
}

// process ISO control chars [0x00, 0x1f]
PRIVATE int vterm_iso_C0_set(vterm_t *vterm, u_char chr)
{
	char buf[1];

	switch(chr) {
	case ISO_BEL:
		if (app__.no_bell == 0) {
			buf[0] = chr;
			if (write(STDOUT_FILENO, buf, 1) <= 0) {	// redirect to original stdout
				_ERR_
			}
		}
		break;
	case ISO_BS:
		if (vterm_pen_get_x(vterm) > 0) {
			vterm_pen_move_yx(vterm, 0, -1);
		}
		break;
	case ISO_HT:
		vterm_pen_move_yx(vterm, 0,
		 vterm->tab_size - (vterm_pen_get_x(vterm) % vterm->tab_size));
		if (vterm_pen_get_x(vterm) < vterm->text_columns) {
			break;
		}
		vterm_pen_move_yx(vterm, 0, - vterm->text_columns);
		/* fall into next case */
	case ISO_VT:
	case ISO_FF:
		/* fall into next case */
	case ISO_LF:
		if (vterm_pen_get_y(vterm) >= vterm->bottom_y - 1) {
			vterm->scroll++;
		} else {
			vterm_pen_move_yx(vterm, +1, 0);
		}
		break;
	case ISO_CR:
		vterm_pen_set_x(vterm, 0);
		break;
	case ISO_ESC:
		vterm->esc_seq_parse_state = ESC_ST_1_ESC;
		break;
	case ISO_LS1:
		// do nothing
		break;
	case ISO_LS0:
		// do nothing
		break;
	default:
		return 0;	// not processed
	}
	return 1;		// processed
}

PRIVATE void vterm_reset_utf8_state(vterm_t *vterm)
{
	vterm->utf8_state = 0;
	vterm->ucs21 = 0x0000;
}
PRIVATE int vterm_parse_utf8_seq(vterm_t *vterm, u_char chr)
{
	if (chr < 0x80) {					// 0x00--0x7f
		if (vterm->utf8_state) {
			warn_printf("illegal UTF-8 sequence\n");
			dump_parsed_str();
			vterm_reset_utf8_state(vterm);
		}
		vterm->ucs21 = chr;
		// ASCII char, send ucs21
	} else if ((chr & 0xc0) == 0x80) {	// 10xxxxxx
		if (vterm->utf8_state == 0) {
			warn_printf("illegal UTF-8 sequence\n");
			dump_parsed_str();
			vterm->ucs21 = chr;
		} else {
			vterm->ucs21 = (vterm->ucs21 << 6) | (chr & 0x3f);
			vterm->utf8_state--;
			// end of UTF-8 sequence, send ucs21
		}
	} else if ((chr & 0xe0) == 0xc0) {	// 110xxxxx 10xxxxxx
		vterm->utf8_state = 1;
		vterm->ucs21 = (chr & 0x1f);
	} else if ((chr & 0xf0) == 0xe0) {	// 1110xxxx 10xxxxxx 10xxxxxx
		vterm->utf8_state = 2;
		vterm->ucs21 = (chr & 0x0f);
	} else if ((chr & 0xf8) == 0xf0) {	// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		vterm->utf8_state = 3;
		vterm->ucs21 = (chr & 0x07);
	} else if ((chr & 0xfc) == 0xf8) {	// 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		vterm->utf8_state = 4;
		vterm->ucs21 = (chr & 0x03);
	} else if ((chr & 0xfe) == 0xfc) {	// 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		vterm->utf8_state = 5;
		vterm->ucs21 = (chr & 0x01);
	} else {							// 1111111x
		vterm_reset_utf8_state(vterm);
		vterm->ucs21 = chr;				// [0xfe, 0xff]
	}
	return vterm->utf8_state;
}

PRIVATE void vterm_put_ucs21_char(vterm_t *vterm, wchar_t ucs21)
{
	int width_in_pixels;
	int width;

	if (vterm->dec_sp_gr) {
		// substitute to graphic character
		ucs21 = subst_dec_sp_gr_char(ucs21);
	}
	// get font width from font
	width_in_pixels = font_get_glyph_width(cur_font, ucs21);
	if (width_in_pixels <= cur_font->font_width)
		width = 1;
	else
		width = 2;
///flf_d_printf("pen(%d, %d)\n", vterm->pen.xx, vterm->pen.yy);
	vterm_wrap_pen_pos(vterm, width);
///flf_d_printf("pen(%d, %d)\n", vterm->pen.xx, vterm->pen.yy);
	if (vterm_adjust_pen_pos(vterm)) {
_FLF_
	}
	if (vterm->insert) {
		vterm_insert_n_columns(vterm, width);
	}
	if (width == 1) {
		vterm_put_to_buf_narrow(vterm, ucs21);
	} else {
		vterm_put_to_buf_wide(vterm, ucs21);
	}
	vterm_pen_move_yx(vterm, 0, width);
///flf_d_printf("pen(%d, %d)\n", vterm->pen.xx, vterm->pen.yy);
}

// Before putting the charactor to Pen position:
// If current pen position is NOT on the last line.
//	+--------------------------+
//	|ABCDEFGHIJKLMNOPQRSTUVWXYZp
//	|                          |
//	|                          |
//	+--------------------------+
//	Pen position will be returned to the top of the next line.
//	+--------------------------+
//	|ABCDEFGHIJKLMNOPQRSTUVWXYZ|
//	|p                         |
//	|                          |
//	+--------------------------+
// If current pen position is on the last line.
//	+--------------------------+
//	|                          |
//	|                          |
//	|ABCDEFGHIJKLMNOPQRSTUVWXYZp
//	+--------------------------+
//	Screen will be scrolled
//	and pen position will be returned to the top of the line.
//	+--------------------------+
//	|                          |
//	|ABCDEFGHIJKLMNOPQRSTUVWXYZ|
//	|p                         |
//	+--------------------------+

// After putting a charactor to the last screen position:
// +--------------------------+
// |                          |
// |                          |
// |ABCDEFGHIJKLMNOPQRSTUVWXYZ|
// +--------------------------+
// Pen(p) and Cursor(c) is located as below.
// +--------------------------+
// |                          |
// |                          |
// |ABCDEFGHIJKLMNOPQRSTUVWXYcp
// +--------------------------+

PRIVATE void vterm_wrap_pen_pos(vterm_t *vterm, int width)
{
	if (vterm_pen_get_x(vterm) + width > vterm->text_columns) {
		vterm_pen_set_x(vterm, 0);							// return to top of the line
		if (vterm_pen_get_y(vterm) < vterm->bottom_y - 1) {
			// not bottom line of the screen
			vterm_pen_move_yx(vterm, +1, 0);				// goto the next line
		} else {
			// already bottom line of the screen
			vterm_text_scroll_up(vterm, 1);					// scroll up
		}
	}
}

//-----------------------------------------------------------------------------
// ANSI Escape Sequence parsing fuctions
//-----------------------------------------------------------------------------

// convert C1 code (0x80--0x9f) to second byte of two byte sequence (0x40--0x5f)
///#define Fe(x)	((x)-0x40)

// parse ESC ...
PRIVATE void vterm_parse_func_esc(vterm_t *vterm, u_char chr)
{
	vterm->esc_seq_parse_state = ESC_ST_0;
	switch(chr) {
	case '['/*Fe(ISO_CSI)*/:	/* ESC [ */
		vterm->esc_seq_parse_state = ESC_ST_2_CSI;
		clear_csi_args();
		break;
	case ']'/*Fe(ISO_OSC)*/:	/* ESC ] */
		vterm->esc_seq_parse_state = ESC_ST_3_OSC;
		clear_csi_args();
		break;
	case 'D'/*Fe(TERM_IND)*/: 	/* ESC D */
		if (vterm_pen_get_y(vterm) >= vterm->bottom_y - 1) {
			vterm->scroll++;
		} else {
			vterm_pen_move_yx(vterm, +1, 0);
		}
		break;
	case 'E'/*Fe(ISO_NEL)*/:	/* ESC E */
		vterm_pen_set_x(vterm, 0);
		/* fall into next case */
	case 'M'/*Fe(ISO_RI)*/:		/* ESC M */
		if (vterm_pen_get_y(vterm) <= vterm->top_y) {
			vterm->scroll--;
		} else {
			vterm_pen_move_yx(vterm, -1, 0);
		}
		break;
	case 'c'/*ISO_RIS*/:		/* ESC c (Reset to Initial State) */
		pen_init(&(vterm->pen));
		vterm_init_scroll_region(vterm);
		vterm_text_clear_all(vterm);
		vterm_repaint_all(vterm);
		break;
	case '7'/*DEC_SC*/:			/* ESC 7 */
		vterm_push_pen(vterm, TRUE);
		break;
	case '8'/*DEC_RC*/:			/* ESC 8 */
		vterm_pop_pen(vterm, TRUE);
		break;

	case '$'/*ISO__MBS*/:		/* $ */
		break;
	case '%'/*ISO_DOCS*/:		/* % */
		break;
	case '('/*ISO_GZD4*/:		/* ESC ( */
		vterm->esc_seq_parse_state = ESC_ST_4_LPAR;
		break;
	case ','/*MULE__GZD6*/:		/* , */
		break;
	case ')'/*ISO_G1D4*/:		/* ) */
		break;
	case '-'/*ISO_G1D6*/:		/* - */
		break;
	case '*'/*ISO_G2D4*/:		/* * */
		break;
	case '.'/*ISO_G2D6*/:		/* . */
		break;
	case '+'/*ISO_G3D4*/:		/* + */
		break;
	case '/'/*ISO_G3D6*/:		/* / */
		break;
	case 'H':				// ESC H
		// set horizontal tab position
		break;
	case 'N'/*Fe(ISO_SS2)*/:	/* N *//* 7bit single shift 2 */
		break;
	case 'O'/*Fe(ISO_SS3)*/:	/* O *//* 7bit single shift 3 */
		break;
	case 'n'/*ISO_LS2*/:		/* n */
		break;
	case 'o'/*ISO_LS3*/:		/* o */
		break;
	case '|'/*ISO_LS3R*/:		/* | */
		break;
	case '}'/*ISO_LS2R*/:		/* } */
		break;
	case '~'/*ISO_LS1R*/:		/* ~ */
		break;
	default:
		flf_d_printf("illegal ESC sequence: ESC %c\n", chr);
		dump_parsed_str();
		break;
	}
}

// parse ESC '[' ...
PRIVATE void vterm_parse_func_csi(vterm_t *vterm, u_char chr)
{
	int arg_idx;

	if (parse_csi_args(vterm, chr)) {
		return;
	}
	vterm->esc_seq_parse_state = ESC_ST_0;
#ifdef ENABLE_DEBUG
	dump_csi_args(chr);
#endif // ENABLE_DEBUG
	switch(chr) {
	case 'A'/*ISO_CS_NO_CUU*/:	// ESC [ {n} A
		vterm_pen_move_yx(vterm, -(DEF_TO_1(get_csi_arg(0))), 0);
		if (vterm_pen_get_y(vterm) < vterm->top_y) {
			vterm->scroll += vterm_pen_get_y(vterm) - vterm->top_y;
			vterm_pen_set_y(vterm, vterm->top_y);
		}
		break;
	case 'B'/*ISO_CS_NO_CUD*/:	// ESC [ {n} B
		vterm_pen_move_yx(vterm, +(DEF_TO_1(get_csi_arg(0))), 0);
		if (vterm_pen_get_y(vterm) >= vterm->bottom_y) {
			vterm->scroll += vterm_pen_get_y(vterm) - (vterm->bottom_y - 1);
			vterm_pen_set_y(vterm, vterm->bottom_y - 1);
		}
		break;
	case 'C'/*ISO_CS_NO_CUF*/:	// ESC [ {n} C
		vterm_pen_move_yx(vterm, 0, +(DEF_TO_1(get_csi_arg(0))));
		break;
	case 'D'/*ISO_CS_NO_CUB*/:	// ESC [ {n} D
		vterm_pen_move_yx(vterm, 0, -(DEF_TO_1(get_csi_arg(0))));
		break;
	case 'G':			// ESC [ {n} G
		vterm_pen_set_x(vterm, DEF_TO_0(get_csi_arg(0)));
		break;
	case 'P':			// ESC [ {n} P
		vterm_delete_n_columns(vterm, DEF_TO_1(get_csi_arg(0)));
		break;
	case '@':			// ESC [ {n} @
		vterm_insert_n_columns(vterm, DEF_TO_1(get_csi_arg(0)));
		break;
	case 'H':			// ESC [ {line} ; {col} H
	case 'f':			// ESC [ {line} ; {col} f
		vterm_pen_set_yx(vterm, DEF_TO_0(get_csi_arg(0)), DEF_TO_0(get_csi_arg(1)));
		break;
	case 'd':
		// ESC [ {n} d
		vterm_pen_set_y(vterm, DEF_TO_0(get_csi_arg(0)));
		break;
	case 'J':			// ESC [ {n} J
		vterm_clear_in_screen_mode(vterm, get_csi_arg(0));
		break;
	case 'K':			// ESC [ {n} K
		vterm_clear_in_line_mode(vterm, get_csi_arg(0));
		break;
	case 'L':			// ESC [ {n} L
		vterm_text_scroll_down_region(vterm, vterm_pen_get_y(vterm), vterm->bottom_y,
		 DEF_TO_1(get_csi_arg(0)));
		break;
	case 'M':			// ESC [ {n} M
		vterm_text_scroll_up_region(vterm, vterm_pen_get_y(vterm), vterm->bottom_y,
		 DEF_TO_1(get_csi_arg(0)));
		break;
	case 'S':			// ESC [ {n} S
		vterm_text_scroll_down_region(vterm, 0, vterm->bottom_y,
		 DEF_TO_1(get_csi_arg(0)));
		break;
	case 'T':			// ESC [ {n} T
		vterm_text_scroll_up_region(vterm, 0, vterm->bottom_y,
		 DEF_TO_1(get_csi_arg(0)));
		break;
	case 'X':			// ESC [ {n} X
		vterm_text_clear_columns(vterm, get_csi_arg(0));
		break;
	case 'g':			// ESC [ {arg1} g
		switch (get_csi_arg(0)) {
		case 0:	// clear horizontal tab position
		case 1:	// clear vertical tab position
		case 2:	// clear current line vertical tab position
		case 3:	// clear current line horizontal tab position
		case 4:	// clear vertical tab position
		case 5:	// clear horizontal and vertical tab position
			break;
		}
		break;
	case 'm':			// ESC [ {arg1} ; {arg2} ; ... ; {arg9} m
		for (arg_idx = 0; arg_idx <= get_csi_arg_idx(); arg_idx++) {
			arg_idx = vterm_esc_set_attr(vterm, get_csi_arg(arg_idx), arg_idx);
		}
		break;
	case 'r':			// ESC [ {top} ; {bottom} r (set scroll region)
		{
			int yy1;
			int yy2;

			yy1 = DEF_TO_0(get_csi_arg(0));
			yy2 = get_csi_arg(1) ? get_csi_arg(1) : vterm->text_lines;
#ifdef ENABLE_STATUS_LINE
			if (vterm->status_line != SL_NONE) {
				if (yy2 >= vterm->text_lines) {
					// reserve status line
					 yy2 = vterm->text_lines-1;
				}
			}
#endif // ENABLE_STATUS_LINE
			vterm_set_scroll_region__adjust(vterm, yy1, yy2);
		}
		break;
	case 'h':			// ESC [ {n} h
		for (arg_idx = 0; arg_idx <= get_csi_arg_idx(); arg_idx++) {
			vterm_set_mode(vterm, get_csi_arg(arg_idx), TRUE);
		}
		break;
	case 'l':			// ESC [ {n} l
		for (arg_idx = 0; arg_idx <= get_csi_arg_idx(); arg_idx++) {
			vterm_set_mode(vterm, get_csi_arg(arg_idx), FALSE);
		}
		break;
	case 's':			// ESC [ {n} s
		vterm_push_pen(vterm, TRUE);
		break;
	case 'u':			// ESC [ {n} u
		vterm_pop_pen(vterm, TRUE);
		break;
	case 'n':			// ESC [ {n} n
	case 'c':			// ESC [ {n} c
		vterm_esc_report(vterm, chr, get_csi_arg(0));
		break;
	case 'R':			// ESC [ {n} R
		break;
	case '?':			// ESC [ {n} ?
#ifdef ENABLE_STATUS_LINE
		vterm->esc_seq_parse_state = ESC_ST_5_SL;
#else
		vterm->esc_seq_parse_state = ESC_ST_2_CSI;
#endif // ENABLE_STATUS_LINE
		break;
	default:
		warn_printf("illegal ESC sequence: ESC [ n %c\n", chr);
		dump_parsed_str();
		break;
	}
	if (vterm->esc_seq_parse_state == ESC_ST_0) {
		clear_csi_args();
	}
}

// parse ESC ']' ...
PRIVATE void vterm_parse_func_osc(vterm_t *vterm, u_char chr)
{
	if (parse_csi_args(vterm, chr)) {
		return;
	}
	vterm->esc_seq_parse_state = ESC_ST_0;
#ifdef ENABLE_DEBUG
	dump_csi_args(chr);
#endif // ENABLE_DEBUG
	switch(chr) {
	case 'R':			// ESC ] {n} R
		// Linux reset command generate this (ESC ] R) sequence
		break;
	default:
		warn_printf("illegal ESC sequence: ESC ] n %c\n", chr);
		dump_parsed_str();
		break;
	}
	clear_csi_args();
}

#ifdef ENABLE_STATUS_LINE
PRIVATE void vterm_init_status_line(vterm_t *vterm)
{
	vterm->status_line = SL_NONE;
}
// parse ESC [ ? ...
PRIVATE void vterm_parse_func_status_line(vterm_t *vterm, u_char chr)
{
	vterm->esc_seq_parse_state = ESC_ST_0;
	switch(chr) {
	case 'T':	/* To */
		if (vterm->status_line == SL_ENTER)
			break;
		vterm_push_pen(vterm, FALSE);
		/* fall into next case */
	case 'S':	/* Show */
		if (vterm->status_line == SL_NONE) {
			vterm->bottom_y = vterm->text_lines - 1;
		}
		if (chr == 'T') {
			vterm->status_line = SL_ENTER;
			vterm_set_scroll_region__adjust(vterm, vterm->text_lines - 1, vterm->text_lines);
		}
		break;
	case 'F':	/* From */
		if (vterm->status_line == SL_ENTER) {
			vterm->status_line = SL_LEAVE;
			vterm_set_scroll_region__adjust(vterm, 0, vterm->text_lines - 1);
			vterm_pop_pen(vterm, FALSE);
		}
		break;
	case 'H':	/* Hide */
	case 'E':	/* Erase */
		if (vterm->status_line == SL_NONE) {
			break;
		}
		vterm_set_scroll_region__adjust(vterm, 0, vterm->text_lines);
		vterm->status_line = SL_NONE;
		break;
	default:
		vterm->esc_seq_parse_state = ESC_ST_2_CSI;
		vterm_parse_func_csi(vterm, chr);
		break;
	}
}
#endif // ENABLE_STATUS_LINE

PRIVATE void vterm_set_mode(vterm_t *vterm, u_short mode, bool_t on_off)
{
	switch(mode) {
	case 4:		// Enable / Disable insert mode
		vterm->insert = on_off;
		break;
	case 7:		// Enable / Disable line wrapping
		break;
	case 25:	// Enable / Disable cursor display
		vterm_set_cursor_on_off(vterm, on_off);
		break;
	case 1004:	// Enable / Disable reporting focus
	case 1049:	// Enable / Disable alternative screen buffer
	case 2004:	// Turn on / off bracketed paste mode
	}
}

// analyze p1, p2, p3, ... in ESC [ p1 ; p2 ; p3 ... m
PRIVATE int vterm_esc_set_attr(vterm_t *vterm, int num, int arg_idx)
{
	c_idx_t color_idx;

	switch(num) {
	case 0:
		pen_off_all_attr(&(vterm->pen));
		break;

	case 1:
		pen_highlight(&(vterm->pen));
		break;
	case 21:
		pen_unhighlight(&(vterm->pen));
		break;

	case 2:		// dim
		break;
	case 22:	// not dim
		break;

	case 3:		// italic
		break;
	case 23:	// not italic
		break;

	case 4:
		break;
	case 24:
		break;

	case 5:		// blink (slow)
		break;
	case 25:	// not blink
		break;

	case 6:		// blink (fast)
		break;
	case 26:	// not blink
		break;

	case 7:
		pen_reverse(&(vterm->pen));
		break;
	case 27:
		pen_no_reverse(&(vterm->pen));
		break;

	case 10:	/* primary font */
		break;
	case 11:	/* alternative font */
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
		break;

	case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
	case 39:
	case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
	case 49:
	case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97:
	case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107:
		if (num <= 37) {						// [30, 37]
			color_idx = num - 30;				// normal fg color [0, 7]
		} else if (num <= 39) {
			color_idx = COLOR_DEF_FC;
		} else if (num <= 47) {					// [40, 47]
			color_idx = num - 40;				// normal bg color [0, 7]
		} else if (num <= 49) {
			color_idx = COLOR_DEF_BC;
		} else if (90 <= num && num <= 97) {
			color_idx = (num - 90) | COLOR_HIGHLIGHT;	// bright fg color [8, 15]
		} else if (100 <= num && num <= 107) {
			color_idx = (num - 100) | COLOR_HIGHLIGHT;	// bright bg color [8, 15]
		}
		if ((num <= 39) || (90 <= num && num <= 97)) {
			pen_set_fgc_idx(&(vterm->pen), color_idx);
		} else if ((num <= 49) || (100 <= num && num <= 107)) {
			pen_set_bgc_idx(&(vterm->pen), color_idx);
		}
		break;
	case 38:
	case 48:
		if (MAX_CSI_ARGS - arg_idx >= 3) {
			if (get_csi_arg(arg_idx+1) == 5) {
				if (num == 38) {
					// ESC [ 38 ; 5 ; {color_idx} m
					pen_set_fc_rgb_direct(&(vterm->pen),
					 rgb15_from_color256_idx(get_csi_arg(arg_idx+2)));
				} else {
					// ESC [ 48 ; 5 ; {color_idx} m
					pen_set_bc_rgb_direct(&(vterm->pen),
					 rgb15_from_color256_idx(get_csi_arg(arg_idx+2)));
				}
				arg_idx += (3-1);	// eat 3 arguments
			}
		}
		if (MAX_CSI_ARGS - arg_idx >= 5) {
			if (get_csi_arg(arg_idx+1) == 2) {
				if (num == 38) {
					// ESC [ 38 ; 2 ; {red} ; {green} ; {blue} m
					pen_set_fc_rgb_direct(&(vterm->pen),
					 rgb15_from_r_g_b_24(get_csi_arg(arg_idx+2),
										 get_csi_arg(arg_idx+3),
										 get_csi_arg(arg_idx+4)));
				} else {
					// ESC [ 48 ; 2 ; {red} ; {green} ; {blue} m
					pen_set_bc_rgb_direct(&(vterm->pen),
					 rgb15_from_r_g_b_24(get_csi_arg(arg_idx+2),
										 get_csi_arg(arg_idx+3),
										 get_csi_arg(arg_idx+4)));
				}
				arg_idx += (5-1);	// eat 5 arguments
			}
		}
		break;
	default:
		break;
	}
	return arg_idx;
}

PRIVATE rgb15_t rgb15_from_color256_idx(u_char color256_idx)
{
	int color_idx;
	int rr, gg, bb;

	// 16 ANSI + 216 RGB + 24 Gray-scale = 256 colors
	if (color256_idx < 16) {
		// 16 ANSI colors
		color_idx = color256_idx;		// [0, 15]
		return rgb15_from_color_idx(color_idx);
	} else
	if (color256_idx < 16 + 216) {
		// 216 RGB colors (216 = 6 * 6 * 6)
		color_idx = color256_idx - 16;	// [16, 232) ==> [0, 216)
		bb = (color_idx % 6);		// [0, 5]
		gg = ((color_idx / 6) % 6);	// [0, 5]
		rr = (color_idx / 36);		// [0, 5]
		bb *= 51;	// 0, 51, 102, 153, 204, 255
		gg *= 51;	// 0, 51, 102, 153, 204, 255
		rr *= 51;	// 0, 51, 102, 153, 204, 255
		return rgb15_from_r_g_b_24(rr, gg, bb);
	} else {
		// 24 gray scale colors
		color_idx = color256_idx - (16 + 216);	// [232, 255] ==> [0, 23]
		color_idx *= 11;						// [0, 253]
		return rgb15_from_r_g_b_24(color_idx, color_idx, color_idx);
	}
}

PRIVATE void vterm_init_pen_stack(vterm_t *vterm)
{
	vterm->pen_stack_ptr = 0;
	vterm->pen_sl_stack_ptr = 0;
}
PRIVATE void vterm_push_pen(vterm_t *vterm, bool_t bb)
{
	if (bb) {
		if (vterm->pen_stack_ptr < PEN_STACK_DEPTH) {
			pen_copy(&vterm->pen_stack[vterm->pen_stack_ptr], &vterm->pen);
			vterm->pen_stack_ptr++;
		}
	} else {
		if (vterm->pen_sl_stack_ptr < PEN_STACK_DEPTH) {
			pen_copy(&vterm->pen_sl_stack[vterm->pen_sl_stack_ptr], &vterm->pen);
			vterm->pen_sl_stack_ptr++;
		}
	}
}
PRIVATE void vterm_pop_pen(vterm_t *vterm, bool_t bb)
{
	if (bb) {
		if (vterm->pen_stack_ptr > 0) {
			vterm->pen_stack_ptr--;
			pen_copy(&vterm->pen, &vterm->pen_stack[vterm->pen_stack_ptr]);
		}
	} else {
		if (vterm->pen_sl_stack_ptr > 0) {
			vterm->pen_sl_stack_ptr--;
			pen_copy(&vterm->pen, &vterm->pen_sl_stack[vterm->pen_sl_stack_ptr]);
		}
	}
}

PRIVATE void vterm_set_scroll_region__adjust(vterm_t *vterm, int top_y, int bottom_y)
{
	if (top_y < 0 || vterm->text_lines <= top_y
	 || bottom_y < 0 || vterm->text_lines < bottom_y
	 || top_y >= bottom_y) {
		msg_printf("illegal (top:%d, bottom:%d)\n", top_y, bottom_y);
		// illegal parameter
		return;
	}
	vterm_set_scroll_region(vterm, top_y, bottom_y);
	vterm_pen_set_x(vterm, 0);
	if (vterm_pen_get_y(vterm) < vterm->top_y) {
		// pen position is out of region, correct it
		vterm_pen_set_y(vterm, vterm->top_y);
	} else if (vterm->bottom_y <= vterm_pen_get_y(vterm)) {
		// pen position is out of region, correct it
		vterm_pen_set_y(vterm, vterm->bottom_y-1);
	}
}
PRIVATE void vterm_init_scroll_region(vterm_t *vterm)
{
	vterm_set_scroll_region(vterm, 0, vterm->text_lines);
}
PRIVATE void vterm_set_scroll_region(vterm_t *vterm, int top_y, int bottom_y)
{
	if (top_y >= 0) {
		vterm->top_y = top_y;
	}
	if (bottom_y >= 0) {
		vterm->bottom_y = bottom_y;
	}
}

PRIVATE void vterm_esc_report(vterm_t *vterm, u_char mode, u_short arg)
{
#define MAX_REPORT_LEN		100
	char report[MAX_REPORT_LEN+1];

	report[0] = '\0';
	switch(mode) {
	case 'n':
		switch(arg) {
		case 5:
			// send "device OK"
			strcpy(report, "\x1B[0n\0");
			break;
		case 6:
			// send "current cursor position"
///			int xx = (vterm->pen.xx < vterm->text_columns - 1)
///			 ? vterm->pen.xx : vterm->text_columns - 1;
///			int yy = (vterm->pen.yy < vterm->bottom_y - 1)
///			 ? vterm->pen.yy : vterm->bottom_y - 1;
			snprintf(report, MAX_REPORT_LEN+1, "\x1b[%d;%dR",
			 vterm_pen_get_y(vterm)+1, vterm_pen_get_x(vterm)+1);
			break;
		}
		break;
	case 'c':
#if 0
		if (arg == 0) {
			snprintf(report, MAX_REPORT_LEN+1, "\x1b[%s0c", PACKAGE);
		}
#endif
		break;
	}
	if (write(term__.fd_pty_master, report, strlen(report)) < 0) {
		_ERR_
	}
}

/*---------------------------------------------------------------------------*/
PRIVATE int csi_arg_idx = 0;
PRIVATE u_short csi_args[MAX_CSI_ARGS];

PRIVATE void clear_csi_args(void)
{
	for (csi_arg_idx = 0; csi_arg_idx < MAX_CSI_ARGS; csi_arg_idx++) {
		csi_args[csi_arg_idx] = 0;
	}
	csi_arg_idx = 0;
}
PRIVATE int get_csi_arg_idx(void)
{
	return csi_arg_idx;
}
PRIVATE int parse_csi_args(vterm_t *vterm, u_char chr)
{
	if ('0' <= chr && chr <= '9') {
		// ESC [ 9
		if (csi_arg_idx < MAX_CSI_ARGS) {
			csi_args[csi_arg_idx] = (csi_args[csi_arg_idx] * 10) + (chr - '0');
		}
		return 1;
	} else if (chr == ';') {
		// ESC [ 9 ;
		/* 引数は MAX_CSI_ARGS 個までサポート */
		if (csi_arg_idx < MAX_CSI_ARGS-1) {
			csi_arg_idx++;
		}
		return 1;
	}
	return 0;
}
PRIVATE int get_csi_arg(int arg_idx)
{
	if (0 <= arg_idx && arg_idx <= csi_arg_idx) {
		return csi_args[arg_idx];
	}
	return 0;
}

// DEC Special Graphics support -----------------------------------------------
// UTF8 codes for DEC Special Graphics
#define	DEC_SP_GR_CHAR_BEGIN	'`'
#define	DEC_SP_GR_CHAR_END		'~'
#define	DEC_SP_GR_CHARS			(DEC_SP_GR_CHAR_END - DEC_SP_GR_CHAR_BEGIN + 1)

char *dec_sp_gr_utf8[DEC_SP_GR_CHARS] = {
	"♦", // `
	"▒", // a
	"␉", // b
	"␌", // c
	"␍", // d
	"␊", // e
	"°", //  f
	"±", //  g
	"␤", // h
	"␋", // i
	"┘", // j
	"┐", // k
	"┌", // l
	"└", // m
	"┼", // n
	"─", // o
	"─", // p
	"─", // q
	"─", // r
	"─", // s
	"├", // t
	"┤", // u
	"┴", // v
	"┬", // w
	"│", // x
	"≤", // y
	"≥", // z
	"π", //  {
	"≠", // |
	"£", //  }
	"·", //  ~
};
wchar_t dec_sp_gr_ucs21[DEC_SP_GR_CHARS];

PRIVATE void make_ucs21_tbl_for_dec_sp_gr(void)
{
	static char converted = 0;
	wchar_t ucs21;

	if (converted == 0) {
		for (int idx = 0; idx < DEC_SP_GR_CHARS; idx++) {
#define MAX_UTF8C_BYTES			4
			mbtowc(&ucs21, dec_sp_gr_utf8[idx], MAX_UTF8C_BYTES);
			dec_sp_gr_ucs21[idx] = ucs21;
		}
	}
	converted = 1;
}
PRIVATE wchar_t subst_dec_sp_gr_char(wchar_t ucs21)
{
	make_ucs21_tbl_for_dec_sp_gr();
	if (DEC_SP_GR_CHAR_BEGIN <= ucs21 && ucs21 <= DEC_SP_GR_CHAR_END) {
		return dec_sp_gr_ucs21[ucs21 - DEC_SP_GR_CHAR_BEGIN];
	}
	return ucs21;	// No substitution
}
//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
PRIVATE void dump_csi_args(char chr)
{
	int arg_idx;

	flf_d_printf("{");
	for (arg_idx = 0; arg_idx <= csi_arg_idx; arg_idx++) {
		d_printf(" %d", csi_args[arg_idx]);
	}
	d_printf(" } %c\n", chr);
}
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------

#define MAX_SEQ		20
PRIVATE int parsed_str_idx = 0;
PRIVATE u_char parsed_str[MAX_SEQ+1];
PRIVATE void clear_parsed_str(void)
{
	parsed_str_idx = 0;
	parsed_str[parsed_str_idx] = '\0';
}
PRIVATE void put_parsed_str(u_char chr)
{
	if (parsed_str_idx < MAX_SEQ) {
		parsed_str[parsed_str_idx] = chr;
		parsed_str_idx++;
	} else {
		strcpy_ol(parsed_str, &parsed_str[1]);
		parsed_str[parsed_str_idx-1] = chr;
	}
	parsed_str[parsed_str_idx] = '\0';
}
PRIVATE void dump_parsed_str(void)
{
	flf_d_printf("");
	dump_string(parsed_str, parsed_str_idx);
	clear_parsed_str();
}

// End of vterm.c
