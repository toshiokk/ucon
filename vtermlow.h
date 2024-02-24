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

#ifndef ucon_vtermlow_h
#define ucon_vtermlow_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define COLOR_BLACK			0
#define COLOR_RED			1
#define COLOR_GREEN			2
#define COLOR_BROWN			3
#define COLOR_BLUE			4
#define COLOR_MAGENTA		5
#define COLOR_CYAN			6
#define COLOR_LIGHTGRAY		7
#define COLOR_DARKGRAY		8
#define COLOR_LIGHTRED		9
#define COLOR_LIGHTGREEN	10
#define COLOR_YELLOW		11
#define COLOR_LIGHTBLUE		12
#define COLOR_LIGHTMAGENTA	13
#define COLOR_LIGHTCYAN		14
#define COLOR_WHITE			15

#define COLOR_DEF_BC		COLOR_BLACK			// default background color
#define COLOR_DEF_FC		COLOR_LIGHTGRAY		// default foreground color

#define COLOR_HIGHLIGHT		8
#define COLORS_8			8
#define COLORS_8_MASK		(COLORS_8-1)	// 7
#define COLORS_16			16
#define COLORS_16_MASK		(COLORS_16-1)	// 15

#define INV_C_IDX(c_idx)		((c_idx) ^ COLORS_8_MASK)

#define BFC_FROM_BC_FC(bc_idx, fc_idx)		(((bc_idx) << 4) | (fc_idx))
#define BC_FROM_BFC(bfc_idx)				((bfc_idx) >> 4)
#define FC_FROM_BFC(bfc_idx)				((bfc_idx) & COLORS_16_MASK)

// cursor blink parameters
#define CURSOR_BLINK_PERIOD_NORMAL	10
#define CURSOR_BLINK_PERIOD_FAST	2

typedef struct cursor_ {
	int yy;
	int xx;
	bool_t on;			// 0: cursor off, 1: cursor on
	int mode;			// 0: normal - white/black, slow blinking
						// 1: IM-on  - red/cyan,    fast blinking
	int blink_counter;	// blink counter
	char rev_color;		// cursor inversion color
	bool_t shown;		// 0: hidden, 1: shown(inverted)
	bool_t wide;		// 0: half width, 1: double width
} cursor_t;

//             bfc_idx:
//    ++++-------Back-ground-color
//    ||||++++---Fore-ground-color
//    ||||||||          flags:
//    |||||||| ++---------00:FLAG_NARROW_CHAR
//    |||||||| ++---------01:FLAG_WIDE_CHAR_1
//    |||||||| ++---------10:FLAG_WIDE_CHAR_2
//    |||||||| ++---------11:FLAG_TO_PAINT
//    |||||||| ||+-------------------------Unused
//    |||||||| |||+++++-++++++++-++++++++--UCS21
//    |||||||| |||||||| |||||||| ||||||||
//    76543210 76543210 76543210 76543210
#define FLAG_NARROW_CHAR	0x00	/* narrow char */
#define FLAG_WIDE_CHAR_1	0x40	/* 1st byte of wide char */
#define FLAG_WIDE_CHAR_2	0x80	/* 2nd byte of wide char */
#define FLAG_TO_PAINT		0xc0	/* to be repainted */
#define IS_FLAG_NARROW(flags)	((flags) == FLAG_NARROW_CHAR)
#define IS_FLAG_WIDE1(flags)	((flags) == FLAG_WIDE_CHAR_1)
#define IS_FLAG_WIDE2(flags)	((flags) == FLAG_WIDE_CHAR_2)

#define	TEXT_CHAR_T_BGC			0xf0000000UL
#define	TEXT_CHAR_T_FGC			0x0f000000UL
#define	TEXT_CHAR_T_BFC			(TEXT_CHAR_T_BGC | TEXT_CHAR_T_FGC)
#define	TEXT_CHAR_T_FLAGS		0x00c00000UL
#define	TEXT_CHAR_T_UCS21		0x001fffffUL
#define	UCS21_SPACE				' '

// 8 bytes/char
typedef struct /*text_char_*/ {
	u_int32 bfc_flags_ucs21;	// BGC/FGC/flags/UCS21
	rgb15_t bc_rgb;				// background color 15 bit RGB
	rgb15_t fc_rgb;				// foreground color 15 bit RGB
} text_char_t;

// Maximum screen size
//  Standard Lap top
//   1366 / 5 = 273(274)
//   768 / 10 = 76(77)
//  Full Hi-vision
//   1920 / 5 = 384
//   1080 / 10 = 108
//  Full Hi-vision portrait
//   1080 / 5 = 216
//   1920 / 10 = 192
//  WQXGA
//   2560 / 5 = 512
//   1600 / 10 = 160
//  4K landscape
//   3840 / 5 = 768 (current target)
//   2160 / 10 = 216
//  4K portrait
//   2160 / 5 = 432
//   3840 / 10 = 384 (current target)
///  8K
///   7680 / 5 = 1536
///   4320 / 10 = 432
///  8K portrait
///   4320 / 5 = 864
///   7680 / 10 = 768
#ifdef ENABLE_255COLUMNS

// NOTE: If your terminal interface supports columns > 254
////# define MAX_TERM_COLS	274	// Lap top
////# define MAX_TERM_COLS	384	// FHD
////# define MAX_TERM_COLS	512	//  WQXGA
///# define MAX_TERM_COLS	768	// 4K landscape
////
# define MAX_TERM_COLS	1536	// 8K landscape

#else // ENABLE_255COLUMNS

# define MAX_TERM_COLS	254		// WORKAROUND: ncurses doesn't support columns > 254

#endif // ENABLE_255COLUMNS
////#define MAX_TERM_LINES	77	// Lap top
////#define MAX_TERM_LINES	108	// FHD
////#define MAX_TERM_LINES	160	//  WQXGA
///#define MAX_TERM_LINES	384	// 4K portrait
////
#define MAX_TERM_LINES	768	// 8K portrait

#define OVERLAY_TEXT_LEN		100
#define OVERLAY_TEXT_Y			0
#define OVERLAY_TEXT_X			0
#define OVERLAY_TEXT_SECS		3	// 3[Secs]

struct overlay_line {
	int yy;
	time_t timet;
	int secs;
	// overlay line
	text_char_t text_overlay[MAX_TERM_COLS];
	text_char_t text_save[MAX_TERM_COLS];
};

#define MAX_ROTATION		4	// 0 -- 3

typedef struct vterm_ {
	int text_columns;		/* 1行あたり文字数 */
	int text_lines;			/* 行数 */
	int text_characters;	/* == text_columns * text_lines */

	int top_y;
	int bottom_y;
	pen_t pen;
	cursor_t cursor;		/* カーソル */
	int scroll;				/* スクロール行数 x>0: scroll up, x<0: scroll down */
	bool_t clear_fb_before_repaint;

#define PEN_STACK_DEPTH		10
	int pen_stack_ptr;						// [0, PEN_STACK_DEPTH]
	pen_t pen_stack[PEN_STACK_DEPTH];
	int pen_sl_stack_ptr;					// [0, PEN_STACK_DEPTH]
	pen_t pen_sl_stack[PEN_STACK_DEPTH];	/* ステータスライン用 */

///#define ENABLE_STATUS_LINE
#ifdef ENABLE_STATUS_LINE
	enum { SL_NONE, SL_ENTER, SL_LEAVE } status_line;
#endif // ENABLE_STATUS_LINE

	char tab_size;			/* TAB サイズ */
	bool_t insert;			/* 挿入モード */
	bool_t dec_sp_gr;		/* DEC Special Graphics mode */
	enum {
		ESC_ST_0,
		ESC_ST_1_ESC,
		ESC_ST_2_CSI,
		ESC_ST_3_OSC,
		ESC_ST_4_LPAR,
#ifdef ENABLE_STATUS_LINE
		ESC_ST_5_SL
#endif // ENABLE_STATUS_LINE
	} esc_seq_parse_state;

	char utf8_state;		// [0, 5]
	wchar_t ucs21;

	/* virtual text buffer */
	text_char_t text_buf_to_paint[MAX_TERM_LINES][MAX_TERM_COLS];
	text_char_t text_buf_painted[MAX_TERM_LINES][MAX_TERM_COLS];

#define	OVERLAY_LINES		3
#define	OVERLAY_IDX_0		0		// screen and font size
#define	OVERLAY_IDX_1		1		// IMJ line-1
#define	OVERLAY_IDX_2		2		// IMJ line-2
	struct overlay_line overlay_lines[OVERLAY_LINES];
} vterm_t;

#ifdef ENABLE_DEBUG
#define	LIMIT_TEXT_Y(yy)	yy = MIN_MAX(0, (yy), vterm->text_lines - 1)
#define	LIMIT_TEXT_X(xx)	xx = MIN_MAX(0, (xx), vterm->text_columns - 1)
#else // ENABLE_DEBUG
#define	LIMIT_TEXT_Y(yy)
#define	LIMIT_TEXT_X(xx)
#endif // ENABLE_DEBUG

void vterm_start_fb(vterm_t *vterm);
void vterm_stop_fb(void);

void vterm_register_vt_signals(vterm_t *vterm);
void vterm_unregister_vt_signals(void);

int vterm_reply_sig_release(void);
int vterm_reply_sig_acquire(void);

void vterm_send_sigwinch(vterm_t *vterm);

/*---------------------------------------------------------------------------*/

void vterm_delete_n_columns(vterm_t *vterm, int del_len);
void vterm_insert_n_columns(vterm_t *vterm, int ins_len);

void vterm_save_pen_cursor(vterm_t *vterm, pen_t *pen, cursor_t *cursor);
void vterm_restore_pen_cursor(vterm_t *vterm, pen_t *pen, cursor_t *cursor);

void vterm_interval_timer_process(vterm_t *vterm);

void vterm_set_cursor_on_off(vterm_t *vterm, bool_t on);
void vterm_set_cursor_mode(vterm_t *vterm, int mode);
void vterm_set_cursor_color(vterm_t *vterm, int color);
void vterm_set_cursor_pos(vterm_t *vterm, int yy, int xx);
void vterm_clear_cursor_blink_counter(vterm_t *vterm);
void vterm_blink_cursor(vterm_t *vterm);
void vterm_invert_cursor_yxwc(vterm_t *vterm, int yy, int xx, int wide, int color_idx);

void vterm_repaint_all(vterm_t *vterm);

void vterm_paint_screen__cursor(vterm_t *vterm);

void vterm_set_overlay(vterm_t *vterm, int overlay_idx, int yy, int xx,
 c_idx_t bc_idx, c_idx_t fc_idx, const char *text, int bytes, int secs);
void vterm_check_timeout_of_overlay(vterm_t *vterm);

void vterm_paint_screen(vterm_t *vterm);

void vterm_request_repaint_all(vterm_t *vterm);
void vterm_request_repaint_line(vterm_t *vterm, int yy);

void vterm_text_clear_all(vterm_t *vterm);
void vterm_text_clear_columns(vterm_t *vterm, u_char columns);
void vterm_clear_in_line_mode(vterm_t *vterm, char mode);
void vterm_clear_in_screen_mode(vterm_t *vterm, char mode);
void vterm_put_to_buf_narrow(vterm_t *vterm, wchar_t ucs21);
void vterm_put_to_buf_wide(vterm_t *vterm, wchar_t ucs21);

void vterm_text_scroll_up(vterm_t *vterm, int lines);
void vterm_text_scroll_up_region(vterm_t *vterm, int yy1, int yy2, int lines);
void vterm_text_scroll_down(vterm_t *vterm, int lines);
void vterm_text_scroll_down_region(vterm_t *vterm, int yy1, int yy2, int lines);
int is_column1_of_wide_char(vterm_t *vterm, int yy, int xx);
int is_column2_of_wide_char(vterm_t *vterm, int yy, int xx);

void vterm_clear_outside_of_view(vterm_t *vterm);

int vterm_adjust_pen_pos(vterm_t *vterm);
void vterm_adjust_cursor_pos(vterm_t *vterm);
int vterm_limit_screen_pos(vterm_t *vterm, int *yy, int *xx);
int vterm_check_screen_pos(vterm_t *vterm, int *yy, int *xx);

void vterm_save_text_buf_to_paint(vterm_t *vterm, int yy, text_char_t *text_buf);
void vterm_restore_text_buf_to_paint(vterm_t *vterm, int yy, text_char_t *text_buf);

void set_text_char_t(text_char_t *text_char, wchar_t ucs21, pen_t *pen, u_char flags);
void set_flags_to_text_char_t(text_char_t *text_char, u_char flags);
void set_flags_char_to_text_char_t(text_char_t *text_char, u_char flags, wchar_t ucs21);

wchar_t get_ucs21_from_text_char_t(text_char_t *text_char);
u_char get_flags_from_text_char_t(text_char_t *text_char);
u_char get_bfc_from_text_char_t(text_char_t *text_char);
const char *dump_text_char_t(text_char_t *text_char);

void vterm_pen_move_yx(vterm_t *vterm, int yy, int xx);
void vterm_pen_set_y(vterm_t *vterm, int yy);
void vterm_pen_set_x(vterm_t *vterm, int xx);
void vterm_pen_set_yx(vterm_t *vterm, int yy, int xx);
int vterm_pen_get_y(vterm_t *vterm);
int vterm_pen_get_x(vterm_t *vterm);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // ucon_vtermlow_h

// End of vtermlow.h
