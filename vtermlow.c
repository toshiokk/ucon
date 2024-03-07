/*
 * UCON -
 * Copyright (C) 2003 Fumitoshi Ukai <ukai@debian.or.jp>
 * Copyright (C) 1999 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * KON2 - Kanji ON Console -
 * Copyright (C) 1992, 1993 MAEDA Atusi (mad@math.keio.ac.jp)
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
 * THIS SOFTWARE IS PROVIDED BY MASUICHI NORITOSHI AND MAEDA ATUSI AND
 * TAKASHI MANABE ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE TERRENCE R.
 * LAMBERT BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "ucon.h"

#ifdef ENABLE_255COLUMNS
#warning "**** More than 254 columns of terminal screen width is enabled. ****"
#warning "**** ncurses may cause screen disruption. **************************"
#endif // ENABLE_255COLUMNS

bool_t vt_state_active;				// selected

PRIVATE void sig_release_handler(int signum);
PRIVATE void sig_acquire_handler(int signum);

PRIVATE void vterm_paint_blink_cursor(vterm_t *vterm);
PRIVATE int vterm_get_cursor_blink_period(vterm_t *vterm);
PRIVATE void vterm_hide_cursor_before_paint(vterm_t *vterm, int yy, int xx, bool_t wide);
PRIVATE void vterm_paint_cursor(vterm_t *vterm);
PRIVATE void vterm_erase_cursor(vterm_t *vterm);
PRIVATE void vterm_invert_cursor(vterm_t *vterm);

PRIVATE void vterm_paint_char(vterm_t *vterm, int yy, int xx);
PRIVATE void vterm_clear_lines(vterm_t *vterm, int yy1, int yy2);

PRIVATE void vterm_move_line(vterm_t *vterm, int yy_dst, int yy_src);
PRIVATE void vterm_move_in_line(vterm_t *vterm, int yy_dst, int xx_dst,
 int yy_src, int xx_src, int chars);
PRIVATE void vterm_clear_line(vterm_t *vterm, int yy);
PRIVATE void vterm_clear_in_line(vterm_t *vterm, int yy, int xx1, int xx2);
PRIVATE void vterm_clear_line_in_text_buf(vterm_t *vterm, text_char_t *text_buf);
PRIVATE void vterm_clear_in_text_buf_line(vterm_t *vterm, text_char_t *text_buf, int xx1, int xx2);

/*---------------------------------------------------------------------------*/
void vterm_start_fb(vterm_t *vterm)
{
	vterm_send_sigwinch(vterm);
	vterm_register_vt_signals(vterm);
	vterm_text_clear_all(vterm);
	sig_acquire_handler(SIGUSR2);
}
void vterm_stop_fb(void)
{
	vterm_unregister_vt_signals();
}
/*---------------------------------------------------------------------------*/
void vterm_register_vt_signals(vterm_t *vterm)
{
	struct vt_mode vtm;

	// initialize VT state
	vt_state_active = 0;

	signal(SIGUSR1, sig_release_handler);
	signal(SIGUSR2, sig_acquire_handler);

	vtm.mode = VT_PROCESS;
	vtm.waitv = 0;
	vtm.relsig = SIGUSR1;
	vtm.acqsig = SIGUSR2;
	if (util_privilege_ioctl(0, VT_SETMODE, (void *)&vtm) < 0) {
		_ERR_
		se_printf("ioctl VT_SETMODE");
	}
}
void vterm_unregister_vt_signals(void)
{
	struct vt_mode vtm;

	signal(SIGUSR1, SIG_DFL);
	signal(SIGUSR2, SIG_DFL);

	vtm.mode = VT_AUTO;
	vtm.waitv = 0;
	vtm.relsig = 0;
	vtm.acqsig = 0;
	if (util_privilege_ioctl(0, VT_SETMODE, (void *)&vtm) < 0) {
		_ERR_
		se_printf("ioctl VT_SETMODE");
	}
}

PRIVATE int sig_release_signaled = 0;
PRIVATE int sig_release_replyed = 0;
PRIVATE int sig_acquire_signaled = 0;
PRIVATE int sig_acquire_replyed = 0;
PRIVATE void sig_release_handler(int signum)
{
	sig_release_signaled++;
	signal(SIGUSR1, sig_release_handler);
}
PRIVATE void sig_acquire_handler(int signum)
{
	sig_acquire_signaled++;
	signal(SIGUSR2, sig_acquire_handler);
}
int vterm_reply_sig_release(void)
{
	if (sig_release_signaled != sig_release_replyed) {
		sig_release_replyed++;
		vt_state_active = 0;
		if (ioctl(0, VT_RELDISP, 1) < 0) {	// accept vt releasing
			_ERR_
			se_printf("ioctl VT_RELDISP");
		}
		return 1;
	}
	return 0;
}
int vterm_reply_sig_acquire(void)
{
	if (sig_acquire_signaled != sig_acquire_replyed) {
		sig_acquire_replyed++;
		if (ioctl(0, VT_RELDISP, VT_ACKACQ) < 0) {	// accept vt acquisition
			_ERR_
			se_printf("ioctl VT_RELDISP");
		}
		vt_state_active = 1;
		return 1;
	}
	return 0;
}
/*---------------------------------------------------------------------------*/
void vterm_send_sigwinch(vterm_t *vterm)
{
	struct winsize winsz;

	winsz.ws_row = vterm->text_lines;
	winsz.ws_col = vterm->text_columns;
	winsz.ws_xpixel = 0;
	winsz.ws_ypixel = 0;
flf_d_printf("row:%2d, col:%2d\n", winsz.ws_row, winsz.ws_col);
	// DNU: SIGWINCH is not sent to the client process
	if (ioctl(term__.fd_console, TIOCSWINSZ, &winsz) < 0) {
		_ERR_
		se_printf("ioctl TIOCSWINSZ");
	}
	// DNU: SIGWINCH is not sent to the client process
	if (ioctl(term__.fd_pty_slave, TIOCSWINSZ, &winsz) < 0) {
		_ERR_
		se_printf("ioctl TIOCSWINSZ");
	}
	// SIGWINCH will be sent to the client process
	if (ioctl(term__.fd_pty_master, TIOCSWINSZ, &winsz) < 0) {
		_ERR_
		se_printf("ioctl TIOCSWINSZ");
	}
	// SIGWINCH will be sent to the client process
}

/*---------------------------------------------------------------------------*/
//         xx  xx2
//         |   |
// aaaaaaaaccccbbbbbbbb
// aaaaaaaabbbbbbbb____
void vterm_delete_n_columns(vterm_t *vterm, int del_len)
{
	int xx2;
	int move_len;

	del_len = MIN_(vterm->text_columns - vterm_pen_get_x(vterm), del_len);
	xx2 = vterm_pen_get_x(vterm) + del_len;
	move_len = vterm->text_columns - xx2;
	vterm_move_in_line(vterm, vterm_pen_get_y(vterm), vterm_pen_get_x(vterm),
							 vterm_pen_get_y(vterm), xx2, move_len);
	vterm_clear_in_line(vterm, vterm_pen_get_y(vterm),
	 vterm->text_columns - del_len, vterm->text_columns);
}
//         xx  xx2
//         |   |
// aaaaaaaabbbbbbbbcccc
// aaaaaaaa____bbbbbbbb
void vterm_insert_n_columns(vterm_t *vterm, int ins_len)
{
	int xx2;
	int move_len;

	ins_len = MIN_(vterm->text_columns - vterm_pen_get_x(vterm), ins_len);
	xx2 = vterm_pen_get_x(vterm) + ins_len;
	move_len = vterm->text_columns - xx2;
	vterm_move_in_line(vterm, vterm_pen_get_y(vterm), xx2,
							 vterm_pen_get_y(vterm), vterm_pen_get_x(vterm), move_len);
	vterm_clear_in_line(vterm, vterm_pen_get_y(vterm),
	 vterm_pen_get_x(vterm), vterm_pen_get_x(vterm) + ins_len);
}
/*---------------------------------------------------------------------------*/
// カーソル設定を保存する。
void vterm_save_pen_cursor(vterm_t *vterm, pen_t *pen, cursor_t *cursor)
{
	memcpy_ol(pen, &vterm->pen, sizeof(*pen));
	memcpy_ol(cursor, &vterm->cursor, sizeof(*cursor));
}
// カーソル設定を復帰する。
void vterm_restore_pen_cursor(vterm_t *vterm, pen_t *pen, cursor_t *cursor)
{
	memcpy_ol(&vterm->pen, pen, sizeof(*pen));
	memcpy_ol(&vterm->cursor, cursor, sizeof(*cursor));
}
//-----------------------------------------------------------------------------
// call this every 100 mSec
void vterm_interval_timer_process(vterm_t *vterm)
{
	vterm_blink_cursor(vterm);
	vterm_check_timeout_of_overlay(vterm);
}
//-----------------------------------------------------------------------------
// カーソルをON/OFFする。
// カーソルをOFFする時:
//   カーソルが表示中だった場合、カーソルを消す。
// カーソルをONする時:
//   カーソルが非表示中だった場合、カーソルを表示する。
void vterm_set_cursor_on_off(vterm_t *vterm, bool_t on)
{
///mflf_d_printf("on ==> %d\n", on);
	if (on == 0) {
		vterm_erase_cursor(vterm);			// hide cursor
		vterm->cursor.on = on;
	} else {
		vterm->cursor.on = on;
		vterm_paint_blink_cursor(vterm);	// show cursor
	}
}
// mode = 0 : white and slow blinking
// mode = 1 : light magenta and fast blinking (Input Method ON)
void vterm_set_cursor_mode(vterm_t *vterm, int mode)
{
	if (vterm->cursor.mode != mode) {
_MFLF_
		vterm->cursor.mode = mode;
	}
	vterm_set_cursor_color(vterm, mode == 0 ? COLOR_WHITE : COLOR_LIGHTMAGENTA);
}
void vterm_set_cursor_color(vterm_t *vterm, int color)
{
	if (vterm->cursor.rev_color != color) {
		if (vterm->cursor.shown) {
			vterm_erase_cursor(vterm);	// hide cursor with prev. color
			vterm->cursor.rev_color = color;	// cursor reverse color
			vterm_paint_blink_cursor(vterm);	// show with new color
		} else {
			vterm->cursor.rev_color = color;	// cursor reverse color
		}
	}
}
void vterm_set_cursor_pos(vterm_t *vterm, int yy, int xx)
{
///mflf_d_printf("%d, %d ==>? %d, %d\n", vterm->cursor.yy, vterm->cursor.xx, yy, xx);
	if ((vterm->cursor.yy != yy) || (vterm->cursor.xx != xx)) {
		vterm_erase_cursor(vterm);	// ensure that the cursor is hidden
///mflf_d_printf("       ==> %d, %d\n", yy, xx);
		vterm->cursor.yy = yy;
		vterm->cursor.xx = xx;
		if (vterm->cursor.on) {
			// to paint cursor soon after changing cursor pos
			vterm_clear_cursor_blink_counter(vterm);
			vterm_paint_blink_cursor(vterm);
		}
	}
}
void vterm_clear_cursor_blink_counter(vterm_t *vterm)
{
///_MFLF_
	vterm->cursor.blink_counter = vterm_get_cursor_blink_period(vterm);	// reset blink counter
}
void vterm_blink_cursor(vterm_t *vterm)
{
	vterm->cursor.blink_counter++;
///mflf_d_printf("blink_counter: %d\n", vterm->cursor.blink_counter);
	vterm_paint_blink_cursor(vterm);
}
PRIVATE void vterm_paint_blink_cursor(vterm_t *vterm)
{
	if (vterm->cursor.blink_counter >= vterm_get_cursor_blink_period(vterm)) {
		vterm->cursor.blink_counter = 0;
	}
///mflf_d_printf("blink_counter: %d\n", vterm->cursor.blink_counter);
	if (vterm->cursor.blink_counter < vterm_get_cursor_blink_period(vterm) / 2) {
		// [0, blink_count / 2) ==> paint
		vterm_paint_cursor(vterm);
	} else {
		// [blink_count / 2, blink_count) ==> erase
		vterm_erase_cursor(vterm);
	}
}
PRIVATE int vterm_get_cursor_blink_period(vterm_t *vterm)
{
	return (vterm->cursor.mode == 0) ? CURSOR_BLINK_PERIOD_NORMAL : CURSOR_BLINK_PERIOD_FAST;
}
// hide cursor before repainting character
PRIVATE void vterm_hide_cursor_before_paint(vterm_t *vterm, int yy, int xx, bool_t wide)
{
	int xx2 = xx + ((wide == 0) ? 1 : 2);
	int cursor_xx2 = vterm->cursor.xx + ((vterm->cursor.wide == 0) ? 1 : 2);

	if (vterm->cursor.shown
	 && (yy == vterm->cursor.yy)
	 && IS_OVERLAP(xx, xx2, vterm->cursor.xx, cursor_xx2)) {
///_MFLF_
		vterm_erase_cursor(vterm);
	}
}
PRIVATE void vterm_paint_cursor(vterm_t *vterm)
{
	if (vterm->cursor.on && vterm->cursor.shown == FALSE) {
		vterm->cursor.wide = is_column1_of_wide_char(vterm, vterm->cursor.yy, vterm->cursor.xx);
		vterm_invert_cursor(vterm);
		vterm->cursor.shown = TRUE;
	}
}
PRIVATE void vterm_erase_cursor(vterm_t *vterm)
{
	if (vterm->cursor.shown) {
		vterm_invert_cursor(vterm);
		vterm->cursor.shown = FALSE;
	}
}
// カーソルを描画する。
// 設定済みのカーソル位置に表示する。
PRIVATE void vterm_invert_cursor(vterm_t *vterm)
{
///mflf_d_printf("(%d, %d), wide: %d\n", vterm->cursor.yy, vterm->cursor.xx, vterm->cursor.wide);
	vterm_adjust_cursor_pos(vterm);
	vterm_invert_cursor_yxwc(vterm,
	 vterm->cursor.yy, vterm->cursor.xx, vterm->cursor.wide, vterm->cursor.rev_color);
}
// カーソルを描画する。
// 実際には表示イメージを反転する。
void vterm_invert_cursor_yxwc(vterm_t *vterm, int yy, int xx, int wide, int color_idx)
{
mflf_d_printf("vt_state_active: %d\n", vt_state_active);
	if (vt_state_active == 0) {
mflf_d_printf("vt_state_active == 0\n");
		return;
	}

///flf_d_printf("(%d, %d)\n", yy, xx);
	vterm_limit_screen_pos(vterm, &yy, &xx);
///flf_d_printf("yy:%d, xx:%d\n", yy, xx);
///flf_d_printf("font_w:%d, font_h:%d\n", cur_font->width, cur_font->height);
	fb__.driver->bpp_reverse_char_box(xx, yy, wide, color_idx, rgb15_from_color_idx(color_idx));
}
/*---------------------------------------------------------------------------*/

void vterm_repaint_all(vterm_t *vterm)
{
	vterm_request_repaint_all(vterm);
	vterm_paint_screen__cursor(vterm);
}

void vterm_paint_screen__cursor(vterm_t *vterm)
{
	vterm_paint_screen(vterm);
	vterm_set_cursor_pos(vterm, vterm_pen_get_y(vterm), vterm_pen_get_x(vterm));
}

PRIVATE void vterm_paint_and_set_painted(vterm_t *vterm);
#define COLOR_CLEAR				COLOR_DARKGRAY
#define COLOR_UNKNOWN_GLYPH		COLOR_BLUE

void vterm_set_overlay(vterm_t *vterm, int overlay_idx, int yy, int xx,
 c_idx_t bc_idx, c_idx_t fc_idx, const char *text, int bytes, int secs)
{
	struct overlay_line *overlay_line;
	pen_t pen;
	cursor_t cursor;

	if (overlay_idx < OVERLAY_IDX_0 || OVERLAY_LINES <= overlay_idx)
		return;		// error !!

	overlay_line = &(vterm->overlay_lines[overlay_idx]);
	vterm_save_pen_cursor(vterm, &pen, &cursor);
	if (yy < 0) {
		// clear overlay
		overlay_line->yy = yy;
		overlay_line->timet = 0;
		overlay_line->secs = 0;
		pen_off_all_attr(&(vterm->pen));
		pen_set_bgc_fgc_idx(&(vterm->pen), bc_idx, fc_idx);
		vterm_clear_line_in_text_buf(vterm, overlay_line->text_overlay);
	} else {
		overlay_line->yy = yy;
		overlay_line->timet = time(NULL);
		overlay_line->secs = secs;

		// save original text
		vterm_save_text_buf_to_paint(vterm, yy, overlay_line->text_save);
		// replace with overlay text
		vterm_restore_text_buf_to_paint(vterm, yy, overlay_line->text_overlay);

		// replace overlay line with overlay text
		vterm_emulate_str_yx_bc_fc(vterm, yy, xx, bc_idx, fc_idx, text, bytes);

		// save overlay text
		vterm_save_text_buf_to_paint(vterm, yy, overlay_line->text_overlay);
		// restore original text
		vterm_restore_text_buf_to_paint(vterm, yy, overlay_line->text_save);
	}
	vterm_restore_pen_cursor(vterm, &pen, &cursor);
}
void vterm_check_timeout_of_overlay(vterm_t *vterm)
{
	int overlay_idx;
	struct overlay_line *overlay_line;

	for (overlay_idx = 0; overlay_idx < OVERLAY_LINES; overlay_idx++) {
		overlay_line = &(vterm->overlay_lines[overlay_idx]);
		if ((overlay_line->yy >= 0)
		 && overlay_line->secs
		 && (time(NULL) >= overlay_line->timet + overlay_line->secs)) {
			// time out
			// clear overlay
			vterm_set_overlay(vterm, overlay_idx, -1, 0, COLOR_DEF_BC, COLOR_DEF_FC, "", 0, 0);
			vterm_paint_screen(vterm);		// erase overlay
		}
	}
}

void vterm_paint_screen(vterm_t *vterm)
{
	int overlay_idx;
	struct overlay_line *overlay_line;

mflf_d_printf("vt_state_active: %d\n", vt_state_active);
	if (vt_state_active == 0) {
mflf_d_printf("vt_state_active == 0\n");
		return;
	}

	if (vterm->clear_fb_before_repaint) {
		// clear framebuffer before redrawing all contents
mflf_d_printf("\n");
		fb__.driver->bpp_clear_all(COLOR_CLEAR, rgb15_from_color_idx(COLOR_CLEAR));
mflf_d_printf("\n");
		vterm->clear_fb_before_repaint = 0;
	}
mflf_d_printf("\n");
	// save original text and overwrite overlay text
	for (overlay_idx = 0; overlay_idx < OVERLAY_LINES; overlay_idx++) {
		overlay_line = &(vterm->overlay_lines[overlay_idx]);
		 if (overlay_line->yy >= 0) {
			// save original text
			vterm_save_text_buf_to_paint(vterm, overlay_line->yy,
			 overlay_line->text_save);
			// replace overlay text
			vterm_restore_text_buf_to_paint(vterm, overlay_line->yy,
			 overlay_line->text_overlay);
		}
	}

_FLF_
	// paint screen
	vterm_paint_and_set_painted(vterm);
_FLF_

	// restore saved original text
	for (overlay_idx = 0; overlay_idx < OVERLAY_LINES; overlay_idx++) {
		overlay_line = &(vterm->overlay_lines[overlay_idx]);
		 if (overlay_line->yy >= 0) {
			// restore original text
			vterm_restore_text_buf_to_paint(vterm, overlay_line->yy,
			 overlay_line->text_save);
		}
	}
_FLF_
}

void vterm_request_repaint_all(vterm_t *vterm)
{
	int yy;

	vterm->clear_fb_before_repaint = 1;
	for (yy = 0; yy < vterm->text_lines; yy++) {
		vterm_request_repaint_line(vterm, yy);
	}
}
inline void vterm_request_repaint_char(vterm_t *vterm, int yy, int xx);
void vterm_request_repaint_line(vterm_t *vterm, int yy)
{
	int xx;

	for (xx = 0; xx < vterm->text_columns; xx++) {
		vterm_request_repaint_char(vterm, yy, xx);
	}
}
inline void vterm_request_repaint_char(vterm_t *vterm, int yy, int xx)
{
	set_flags_to_text_char_t(&(vterm->text_buf_painted[yy][xx]), FLAG_TO_PAINT);
}

// paint framebuffer and set updated
PRIVATE void vterm_paint_and_set_painted(vterm_t *vterm)
{
	int xx, yy;

	for (yy = 0; yy < vterm->text_lines; yy++) {
		for (xx = 0; xx < vterm->text_columns; xx++) {
			if (*(u_int64*)&vterm->text_buf_painted[yy][xx]
			 != *(u_int64*)&vterm->text_buf_to_paint[yy][xx]) {
				vterm_hide_cursor_before_paint(vterm, yy, xx,
				 is_column1_of_wide_char(vterm, yy, xx));

				vterm_paint_char(vterm, yy, xx);

				vterm->text_buf_painted[yy][xx]
				 = vterm->text_buf_to_paint[yy][xx];
			}
		}
	}
}

PRIVATE void vterm_paint_char(vterm_t *vterm, int yy, int xx)
{
	text_char_t *tc_ptr;
	wchar_t ucs21;
	u_char bfc_idx;
	u_char flags;
	rgb15_t bc_rgb;
	rgb15_t fc_rgb;
	const u_short *glyph;
	bool wide = 0;
	int glyph_width_in_pixels;
	int found;

	tc_ptr = &(vterm->text_buf_to_paint[yy][xx]);
	ucs21 = get_ucs21_from_text_char_t(tc_ptr);
	flags = get_flags_from_text_char_t(tc_ptr);
	bfc_idx = get_bfc_from_text_char_t(tc_ptr);
	bc_rgb = tc_ptr->bc_rgb;
	fc_rgb = tc_ptr->fc_rgb;
	if (IS_FLAG_NARROW(flags)) {
		// narrow character
		wide = 0;
	} else if (IS_FLAG_WIDE1(flags)) {
		// 1st byte of wide character
		wide = 1;
	} else {
		// 2nd byte of wide character
		return;		// already painted in 1st byte
	}
mflf_d_printf("yy: %d, xx: %d, ucs21: %04x\n", yy, xx, ucs21);
	font_get_glyph_bitmap(cur_font, ucs21, NULL, &found);
	if (found == 0) {
		bfc_idx ^= COLOR_UNKNOWN_GLYPH;
	}
	fb__.driver->bpp_paint_char_box(xx, yy, ucs21, wide,
	 BC_FROM_BFC(bfc_idx), FC_FROM_BFC(bfc_idx), bc_rgb, fc_rgb);
}

/*---------------------------------------------------------------------------*/

// 1 column (narrow) character
void vterm_put_to_buf_narrow(vterm_t *vterm, wchar_t ucs21)
{
	int yy = vterm_pen_get_y(vterm);
	int xx = vterm_pen_get_x(vterm);

	LIMIT_TEXT_Y(yy);
	LIMIT_TEXT_X(xx);
///
#define CHECK_AND_CORRECT_INCONSISTENCY
#ifdef CHECK_AND_CORRECT_INCONSISTENCY
	if (is_column2_of_wide_char(vterm, yy, xx)) {
		// putting narrow char. to the 2nd byte of wide char.
		//                         clear 1st byte of wide char.
		//  v      v
		// [] ==> _A
		set_flags_char_to_text_char_t(&(vterm->text_buf_to_paint[yy][xx-1]),
		 FLAG_NARROW_CHAR, UCS21_SPACE);
	}
	if (is_column1_of_wide_char(vterm, yy, xx)) {
		// putting narrow char. to the 1st byte of wide char.
		//                         clear 2nd byte of wide char.
		// v      v
		// [] ==> A_
		set_flags_char_to_text_char_t(&(vterm->text_buf_to_paint[yy][xx+1]),
		 FLAG_NARROW_CHAR, UCS21_SPACE);
	}
#endif // CHECK_AND_CORRECT_INCONSISTENCY
	set_text_char_t(&(vterm->text_buf_to_paint[yy][xx]), ucs21, &(vterm->pen), FLAG_NARROW_CHAR);
}

// 2 columns (wide) character
void vterm_put_to_buf_wide(vterm_t *vterm, wchar_t ucs21)
{
	int yy = vterm_pen_get_y(vterm);
	int xx = vterm_pen_get_x(vterm);

	LIMIT_TEXT_Y(yy);
	LIMIT_TEXT_X(xx);
#ifdef CHECK_AND_CORRECT_INCONSISTENCY
	if ((0 <= (xx-1)) && is_column1_of_wide_char(vterm, yy, xx-1)) {
		// putting wide char. to the 2nd byte of prev. placed wide char.
		//                       clear 1st byte of prev. placed wide char.
		//  v      v
		// [] ==> _[]
		set_flags_char_to_text_char_t(&(vterm->text_buf_to_paint[yy][xx-1]),
		 FLAG_NARROW_CHAR, UCS21_SPACE);
	}
	if (((xx+2) < vterm->text_columns) && is_column2_of_wide_char(vterm, yy, xx+2)) {
		// putting wide char. to the prev. byte of prev. placed wide char.
		//                       clear 2nd byte of prev. placed wide char.
		//  v       v
		//   [] ==> []_
		set_flags_char_to_text_char_t(&(vterm->text_buf_to_paint[yy][xx+2]),
		 FLAG_NARROW_CHAR, UCS21_SPACE);
	}
#endif // CHECK_AND_CORRECT_INCONSISTENCY
	set_text_char_t(&(vterm->text_buf_to_paint[yy][xx+0]), ucs21, &(vterm->pen), FLAG_WIDE_CHAR_1);
	set_text_char_t(&(vterm->text_buf_to_paint[yy][xx+1]), ucs21, &(vterm->pen), FLAG_WIDE_CHAR_2);
}

/**
	行区間 [0, vterm_t::text_lines) をクリアする。
**/
void vterm_text_clear_all(vterm_t *vterm)
{
	vterm_clear_lines(vterm, 0, vterm->text_lines);
}

void vterm_text_clear_columns(vterm_t *vterm, u_char columns)
{
	int xx2;

	xx2 = MIN_(vterm->text_columns, vterm_pen_get_x(vterm) + columns);
	vterm_clear_in_line(vterm, vterm_pen_get_y(vterm), vterm_pen_get_x(vterm), xx2);
}

/**
	スクリーン内の指定部分をクリアする。
	mode
	0	行 yy のカラム[xx, text_columns) と、
		行 [yy+1, text_lines) をクリアする。
	1	行 yy のカラム[0, xx) と、
		行 [0, yy) をクリアする。
	2	スクリーン全体をクリアする。
**/
void vterm_clear_in_screen_mode(vterm_t *vterm, char mode)
{
	switch(mode) {
	case 0:
	default:
		vterm_clear_in_line_mode(vterm, 0);
		vterm_clear_lines(vterm, vterm_pen_get_y(vterm) + 1, vterm->text_lines);
		break;
	case 1:
		vterm_clear_in_line_mode(vterm, 1);
		vterm_clear_lines(vterm, 0, vterm_pen_get_y(vterm));
		break;
	case 2:
		vterm_text_clear_all(vterm);
		break;
	}
}

/**
	行内の指定部分をクリアする。
	mode
	0	行 yy のカラム[xx, text_columns) をクリアする。
	1	行 yy のカラム[0, xx] をクリアする。
	2	行 yy のカラム[0, text_columns) をクリアする。
**/
void vterm_clear_in_line_mode(vterm_t *vterm, char mode)
{
	int yy = vterm_pen_get_y(vterm);
	int xx = vterm_pen_get_x(vterm);
	int xx1, xx2;

	switch(mode) {
	case 0:
	default:
		xx1 = is_column2_of_wide_char(vterm, yy, xx) ? (xx - 1) : xx;
		xx2 = vterm->text_columns;
		break;
	case 1:
		xx1 = 0;
		xx2 = is_column1_of_wide_char(vterm, yy, xx) ? (xx + 2) : (xx + 1);
		break;
	case 2:
		xx1 = 0;
		xx2 = vterm->text_columns;
		break;
	}
	vterm_clear_in_line(vterm, vterm_pen_get_y(vterm), xx1, xx2);
}

/**
	行区間 [yy1, yy2) をクリアする。
**/
PRIVATE void vterm_clear_lines(vterm_t *vterm, int yy1, int yy2)
{
	int yy;

	for (yy = yy1; yy < yy2; yy++) {
		vterm_clear_line(vterm, yy);
	}
}

#define	SCROLL_FB
void vterm_text_scroll_up(vterm_t *vterm, int lines)
{
	vterm_text_scroll_up_region(vterm, vterm->top_y, vterm->bottom_y, lines);
}
//	+-------------------------------+
//	|aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|yy1
//	|bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb|
//	|ccccccccccccccccccccccccccccccc|yy1+lines
//	|ddddddddddddddddddddddddddddddd|
//	|eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee|
//	|fffffffffffffffffffffffffffffff|
//	|ggggggggggggggggggggggggggggggg|
//	+-------------------------------+yy2
//
//	+-------------------------------+
//	|ccccccccccccccccccccccccccccccc|
//	|ddddddddddddddddddddddddddddddd|
//	|eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee|
//	|fffffffffffffffffffffffffffffff|
//	|ggggggggggggggggggggggggggggggg|
//	|                               |
//	|                               |
//	+-------------------------------+
void vterm_text_scroll_up_region(vterm_t *vterm, int yy1, int yy2, int lines)
{
	int yy;
	int yy_dst;
	int yy_src;

	if (yy2 <= yy1 + lines) {
		vterm_clear_lines(vterm, yy1, yy2);
	} else {
		for (yy = yy1; yy + lines < yy2; yy++) {
			yy_dst = yy;
			yy_src = yy + lines;
			vterm_move_line(vterm, yy_dst, yy_src);
		}
		vterm_clear_lines(vterm, yy2 - lines, yy2);
	}
}

void vterm_text_scroll_down(vterm_t *vterm, int lines)
{
	vterm_text_scroll_down_region(vterm, vterm->top_y, vterm->bottom_y, lines);
}
//	+-------------------------------+
//	|aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|yy1
//	|bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb|
//	|ccccccccccccccccccccccccccccccc|
//	|ddddddddddddddddddddddddddddddd|
//	|eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee|yy2-1-lines
//	|fffffffffffffffffffffffffffffff|
//	|ggggggggggggggggggggggggggggggg|yy2-1
//	+-------------------------------+yy2
//
//	+-------------------------------+
//	|                               |
//	|                               |
//	|aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|
//	|bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb|
//	|ccccccccccccccccccccccccccccccc|
//	|ddddddddddddddddddddddddddddddd|
//	|eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee|
//	+-------------------------------+
void vterm_text_scroll_down_region(vterm_t *vterm, int yy1, int yy2, int lines)
{
	int yy;
	int yy_dst;
	int yy_src;

	if (yy2 <= yy1 + lines) {
		vterm_clear_lines(vterm, yy1, yy2);
	} else {
		for (yy = yy2 - 1; yy - lines >= yy1; yy--) {
			yy_dst = yy;
			yy_src = yy - lines;
			vterm_move_line(vterm, yy_dst, yy_src);
		}
		vterm_clear_lines(vterm, yy1, yy1 + lines);
	}
}

/*---------------------------------------------------------------------------*/

int is_column1_of_wide_char(vterm_t *vterm, int yy, int xx)
{
	if (vterm_limit_screen_pos(vterm, &yy, &xx)) {
_FLF_
	}
///mflf_d_printf("%016llx\n", *(u_int64*)&vterm->text_buf_painted[yy][xx]);
	return IS_FLAG_WIDE1(get_flags_from_text_char_t(&(vterm->text_buf_to_paint[yy][xx])))
	 && ((xx + 1) < vterm->text_columns);	// 2nd byte exists
}
int is_column2_of_wide_char(vterm_t *vterm, int yy, int xx)
{
	if (vterm_limit_screen_pos(vterm, &yy, &xx)) {
_FLF_
	}
///mflf_d_printf("%016llx\n", *(u_int64*)&vterm->text_buf_painted[yy][xx]);
	return (0 <= (xx - 1))					// 1st byte exists
	 &&	IS_FLAG_WIDE2(get_flags_from_text_char_t(&(vterm->text_buf_to_paint[yy][xx])));
}

/*---------------------------------------------------------------------------*/

PRIVATE void vterm_move_line(vterm_t *vterm, int yy_dst, int yy_src)
{
	vterm_move_in_line(vterm, yy_dst, 0, yy_src, 0, vterm->text_columns);
}
PRIVATE void vterm_move_in_line(vterm_t *vterm, int yy_dst, int xx_dst,
 int yy_src, int xx_src, int cols)
{
	vterm_limit_screen_pos(vterm, &yy_dst, &xx_dst);
	vterm_limit_screen_pos(vterm, &yy_src, &xx_src);
	memmove(&vterm->text_buf_to_paint[yy_dst][xx_dst], &vterm->text_buf_to_paint[yy_src][xx_src],
	 cols * sizeof(vterm->text_buf_to_paint[0][0]));
}

/*---------------------------------------------------------------------------*/

PRIVATE void vterm_clear_line(vterm_t *vterm, int yy)
{
	vterm_clear_in_line(vterm, yy, 0, vterm->text_columns);
}
PRIVATE void vterm_clear_in_line(vterm_t *vterm, int yy, int xx1, int xx2)
{
	LIMIT_TEXT_Y(yy);
	vterm_clear_in_text_buf_line(vterm, vterm->text_buf_to_paint[yy], xx1, xx2);
}
PRIVATE void vterm_clear_line_in_text_buf(vterm_t *vterm, text_char_t *text_buf)
{
	vterm_clear_in_text_buf_line(vterm, text_buf, 0, vterm->text_columns);
}
PRIVATE void vterm_clear_in_text_buf_line(vterm_t *vterm, text_char_t *text_buf, int xx1, int xx2)
{
	text_char_t tx_ch;
	u_int64 tc;
	int xx;

	set_text_char_t(&tx_ch, UCS21_SPACE, &(vterm->pen), FLAG_NARROW_CHAR);
	tc = *(u_int64 *)&tx_ch;
	for (xx = xx1; xx < xx2; xx++) {
		*(u_int64 *)&text_buf[xx] = tc;
	}
}

// +--------------------------+-----+
// |                          |     |
// |                          |     |
// |        View area         |     |
// |                          |     |
// |                          |     |
// +--------------------------+     |
// |      outside of view area      |
// +--------------------------------+
void vterm_clear_outside_of_view(vterm_t *vterm)
{
	text_char_t tx_ch;
	u_int64 tc;
	int yy;
	int xx;

#ifndef ENABLE_DEBUG
	set_text_char_t(&tx_ch, UCS21_SPACE, &(vterm->pen), FLAG_NARROW_CHAR);
#else // ENABLE_DEBUG
	set_text_char_t(&tx_ch, '#', &(vterm->pen), FLAG_NARROW_CHAR);
#endif // ENABLE_DEBUG
	tc = *(u_int64 *)&tx_ch;
	for (yy = 0; yy < MAX_TERM_LINES; yy++) {
		for (xx = (yy < vterm->text_lines ? vterm->text_columns : 0);
		 xx < MAX_TERM_COLS; xx++) {
			*(u_int64 *)&vterm->text_buf_to_paint[yy][xx] = tc;
///			*(u_int64 *)&vterm->text_buf_painted[yy][xx] = tc;
		}
	}
}

/*---------------------------------------------------------------------------*/

int vterm_adjust_pen_pos(vterm_t *vterm)
{
///flf_d_printf("(%d, %d)\n", vterm->pen.yy, vterm->pen.xx);
	/* WARN: pen position go out of screen by resize(1) for example */
	if (vterm_limit_screen_pos(vterm, &(vterm->pen.yy), &(vterm->pen.xx))) {
_FLF_
		return 1;
	}
	return 0;
}
void vterm_adjust_cursor_pos(vterm_t *vterm)
{
///flf_d_printf("(%d, %d)\n", vterm->cursor.yy, vterm->cursor.xx);
	if (vterm_limit_screen_pos(vterm, &(vterm->cursor.yy), &(vterm->cursor.xx))) {
_FLF_
	}
}
int vterm_limit_screen_pos(vterm_t *vterm, int *yy, int *xx)
{
	int ret;

	ret = vterm_check_screen_pos(vterm, yy, xx);
	*yy = MIN_MAX(0, *yy, vterm->text_lines - 1);
	*xx = MIN_MAX(0, *xx, vterm->text_columns - 1);
	return ret;
}
int vterm_check_screen_pos(vterm_t *vterm, int *yy, int *xx)
{
	if (IS_IN_RANGE(0, *yy, vterm->text_lines) == 0
	 || IS_IN_RANGE(0, *xx, vterm->text_columns) == 0) {
		warn_printf("LIMIT (y%d, x%d) to (y%d, x%d)\n",
		 *yy, *xx, vterm->text_lines - 1, vterm->text_columns - 1);
		return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/

PRIVATE void vterm_copy_text_buf_line(text_char_t *dest, text_char_t *src);
void vterm_save_text_buf_to_paint(vterm_t *vterm, int yy, text_char_t *text_buf)
{
	LIMIT_TEXT_Y(yy);
	vterm_copy_text_buf_line(text_buf, vterm->text_buf_to_paint[yy]);
}
void vterm_restore_text_buf_to_paint(vterm_t *vterm, int yy, text_char_t *text_buf)
{
	LIMIT_TEXT_Y(yy);
	vterm_copy_text_buf_line(vterm->text_buf_to_paint[yy], text_buf);
}
PRIVATE void vterm_copy_text_buf_line(text_char_t *dest, text_char_t *src)
{
	memmove(dest, src, sizeof(text_char_t) * MAX_TERM_COLS);
}
//-----------------------------------------------------------------------------
void set_text_char_t(text_char_t *text_char, wchar_t ucs21, pen_t *pen, u_char flags)
{
	text_char->bfc_flags_ucs21 =
	 ((BFC_FROM_BC_FC(pen_get_bgc_idx(pen), pen_get_fgc_idx(pen)) << 24)  & TEXT_CHAR_T_BFC)
	  | ((flags << 16) & TEXT_CHAR_T_FLAGS) | (ucs21 & TEXT_CHAR_T_UCS21);
	text_char->bc_rgb = pen_get_bc_rgb(pen);
	text_char->fc_rgb = pen_get_fc_rgb(pen);
}
void set_flags_to_text_char_t(text_char_t *text_char, u_char flags)
{
	text_char->bfc_flags_ucs21 =
	 (text_char->bfc_flags_ucs21 & (TEXT_CHAR_T_BFC | TEXT_CHAR_T_UCS21))
	  | (flags << 16);
}
void set_flags_char_to_text_char_t(text_char_t *text_char, u_char flags, wchar_t ucs21)
{
	text_char->bfc_flags_ucs21 =
	 (text_char->bfc_flags_ucs21 & TEXT_CHAR_T_BFC) | (flags << 16) | ucs21;
}

wchar_t get_ucs21_from_text_char_t(text_char_t *text_char)
{
	// 00000000 000111111 11111111 11111111
	return text_char->bfc_flags_ucs21 & TEXT_CHAR_T_UCS21;
}
u_char get_flags_from_text_char_t(text_char_t *text_char)
{
	// 00000000 1100000 00000000 00000000 ==> 00000000 00000000 00000000 11000000
	return (text_char->bfc_flags_ucs21 & TEXT_CHAR_T_FLAGS) >> 16;
}
u_char get_bfc_from_text_char_t(text_char_t *text_char)
{
	// 11111111 00000000 00000000 00000000 ==> 00000000 00000000 00000000 11111111
	return (text_char->bfc_flags_ucs21 & TEXT_CHAR_T_BFC) >> 24;
}

const char *dump_text_char_t(text_char_t *text_char)
{
	static char buf[15+1];

	//                  012345678901234
	//                 "15:15:ff:00ffff"
	snprintf(buf, 15+1, "%2d:%2d:%02x:%06x", 
	 BC_FROM_BFC(get_bfc_from_text_char_t(text_char)),
	 FC_FROM_BFC(get_bfc_from_text_char_t(text_char)),
	 get_flags_from_text_char_t(text_char),
	 get_ucs21_from_text_char_t(text_char));
	return buf;
}

//-----------------------------------------------------------------------------

void vterm_pen_move_yx(vterm_t *vterm, int yy, int xx)
{
	pen_move_yx(&vterm->pen, yy, xx);
}
void vterm_pen_set_x(vterm_t *vterm, int xx)
{
	vterm_pen_set_yx(vterm, -1, xx);
}
void vterm_pen_set_y(vterm_t *vterm, int yy)
{
	vterm_pen_set_yx(vterm, yy, -1);
}
void vterm_pen_set_yx(vterm_t *vterm, int yy, int xx)
{
	pen_set_yx(&vterm->pen, yy, xx);
	if (vterm_adjust_pen_pos(vterm)) {
_FLF_
	}
}

int vterm_pen_get_y(vterm_t *vterm)
{
	return pen_get_y(&vterm->pen);
}
int vterm_pen_get_x(vterm_t *vterm)
{
	return pen_get_x(&vterm->pen);
}

// End of vtermlow.c
