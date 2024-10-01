/*
 * UCON -
 * Copyright (C) 2003 Fumitoshi Ukai <ukai@debian.or.jp>
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

#ifdef ENABLE_SCREEN_SHOT
#include "screenshot.h"
#endif // ENABLE_SCREEN_SHOT

#ifdef ENABLE_IMJ
#include "im/imj.h"
#include "im/imjcui.h"
#endif // ENABLE_IMJ

#ifdef ENABLE_SCREEN_SHOT
#include "screenshot.c"
#endif // ENABLE_SCREEN_SHOT

#ifdef ENABLE_IMJ

int imj_open(void);
void imj_close(void);

void imj_display(void);
PRIVATE void imj_show(void);
PRIVATE void imj_hide(void);

#include "im/imj.cpp"
#include "im/imjcui.cpp"

#endif // ENABLE_IMJ

int child_pid = 0;

#include "myselect.c"

term_t term__;

int sigchld_signaled = 0;
int sigterm_signaled = 0;

PRIVATE void term_child_exec_shell(term_t *term);
PRIVATE void term_parent_serve(term_t *term);

PRIVATE void call_interval_timer_process(vterm_t *vterm);
PRIVATE void check_hot_key(term_t *term, char *buf, int input_len);
PRIVATE void term_change_font_size(term_t *term, int shift);
PRIVATE void term_change_rotation(term_t *term, int shift);
PRIVATE void term_select_font_by_columns(int columns);
PRIVATE void term_setup_font_and_rotation_parameters(bool font);

PRIVATE void term_set_metrics_n_show(term_t *term);
PRIVATE void term_set_metrics(term_t *term);
PRIVATE void term_show_metrics(term_t *term);

PRIVATE int term_init(term_t *term);
PRIVATE void term_destroy(term_t *term, int err);

PRIVATE int term_open_dev_console(term_t *term);
PRIVATE int term_close_dev_console(term_t *term);

PRIVATE void term_save_term_attr(term_t *term);
PRIVATE void term_setup_term_attr(term_t *term);
PRIVATE void term_restore_term_attr(term_t *term);

PRIVATE int term_set_kd_graphics_mode(term_t *term);
PRIVATE void term_restore_kd_text_mode(term_t *term);
#if 0
PRIVATE int get_current_vt_num(term_t *term);
#endif
PRIVATE int term_get_kdmode(term_t *term);
PRIVATE void term_set_kdmode(int kdmode);

PRIVATE int term_open_ptytty(term_t *term);
PRIVATE void term_close_ptytty(term_t *term);
PRIVATE void term_close_pty_master(term_t *term);
PRIVATE void term_close_pty_slave(term_t *term);

PRIVATE int term_redirect_console_output(int fd);

PRIVATE void term_set_utmp(term_t *term);
PRIVATE void term_reset_utmp(term_t *term);

/****  Data flow of Framebuffer Console  ****
				+=======================+							+------------------+
				|		ucon			|		+-----------+		|    console based |
				|						|       |           |       |      application |
 /dev/console-->|fd:0----->[imj]------->|------>|    pty    |------>|fd:0  (ex. shell) |
				|						|       | interface |       |                  |
				|						|<------|           |<--+---|fd:1              |
				|						|		+-----------+	+---|fd:2              |
				+=======================+							+------------------+
						|||||
						vvvvv render characters onto Framebuffer
					+-----------+
					|			|
					|    FB		|
					|640x480etc.|
					|			|
					+-----------+
*/

int term_start_child_and_serve(term_t *term)
{
	int err = 0;

	if ((err = term_init(term)) < 0) {
		_ERR_
		goto term_start_child_and_serve_err;
	}
	fflush(stdout);
	fflush(stderr);
	signal_init();

flf_d_printf("starting child process...\n");
	if ((child_pid = fork()) < 0) {
		/* error */
		_ERR_
		se_printf("fork");
		err = -9;
		goto term_start_child_and_serve_err;
	} else if (child_pid == 0) {
		/* child */
///	setenv("TERM", term_name, 1);
///	setenv("TERM", "linux", 1);
		term_child_exec_shell(term);
		exit(1);
	} else {
		/* parent */
		term_parent_serve(term);
	}

term_start_child_and_serve_err:;
_FL_
	term_destroy(term, err);
_FL_
	return err;
}

// executed only by a child
PRIVATE void term_child_exec_shell(term_t *term)
{
	char buf[1];

	term_close_pty_master(term);
	term_close_dev_console(term);

	login_tty(term->fd_pty_slave);
	term_restore_term_attr(term);
	util_privilege_drop();
	if (setgid(getgid()) < 0) {
		_ERR_
	}
	if (setuid(getuid()) < 0) {
		_ERR_
	}
	if (read(STDIN_FILENO, buf, 1) <= 0) {		// wait for "server is READY" signal
		_ERR_
	}

	execvp(app__.exec_cmd, app__.exec_argv);	// Never returns on success
	exit(1);
}

// executed only by a parent
PRIVATE void term_parent_serve(term_t *term)
{
	int selected;
	int input_len;
#ifdef ENABLE_IMJ
	int filtered_len;
#endif // ENABLE_IMJ
	int output_len;
	int imcui_ret;
#define INPUT_BUF_LEN		100
	char input_buf[INPUT_BUF_LEN+1];

#ifndef ENABLE_DEBUG
#define OUTPUT_BUF_LEN		4096
#else // ENABLE_DEBUG
#define OUTPUT_BUF_LEN		100
#endif // ENABLE_DEBUG
	char output_buf[OUTPUT_BUF_LEN+1];

//#define DEBUG_NO_INPUT_EXIT
#if defined(DEBUG_NO_INPUT_EXIT)
	int no_input_timer = 0;
#endif

flf_d_printf("\nstart %s server...\n", PACKAGE);
	term_close_pty_slave(term);

	snprintf(output_buf, OUTPUT_BUF_LEN,
	 "(((( %s - Framebuffer UTF-8 Console, Ver %s )))) Screen size : %d x %d\r",
	 PACKAGE, VERSION, term->vterm.text_columns, term->vterm.text_lines);
	vterm_emulate_str(&(term->vterm), output_buf, -1);

	if (write(term->fd_pty_master, "\r", 1) <= 0) {	// send "server is READY" signal
		_ERR_
	}
	while (1) {
		input_len = 0;
		output_len = 0;
		if (sigchld_signaled) {
mflf_d_printf("Child process exited\n");
			break;
		}
		if (sigterm_signaled) {
mflf_d_printf("SIGTERM signaled\n");
			break;
		}
		if (vterm_reply_sig_release()) {
mflf_d_printf("SIGUSR1(release) signaled\n");
		}
		if (vterm_reply_sig_acquire()) {
mflf_d_printf("SIGUSR2(acquire) signaled\n");
#define WORKAROUND_KD_TEXT_KD_GRAPHICS
#ifdef WORKAROUND_KD_TEXT_KD_GRAPHICS
			// WORKAROUND: set KD_TEXT and then KD_GRAPHICS
			//             to redraw screen
			term_set_kdmode(KD_TEXT);
			term_set_kdmode(KD_GRAPHICS);
#endif // WORKAROUND_KD_TEXT_KD_GRAPHICS
			vterm_repaint_all(&(term->vterm));
		}
#if defined(DEBUG_NO_INPUT_EXIT)
		if (no_input_timer >= 0) {
			if (no_input_timer++ >= 30)
				break;
		}
#endif
#define SELECT_TIMEOUT_MSEC		100
		selected = my_select_msec(STDIN_FILENO, term->fd_pty_master, -1, -1, -1, -1,
		 SELECT_TIMEOUT_MSEC);
///mflf_d_printf("my_select(): %04x\n", selected);
		if (selected < 0) {
			_ERR_
			se_printf("select");
			break;
		}
		if (selected == MYSELECT_BIT_TIMEOUT) {
			// no event (time-out)
#ifdef ENABLE_DIMMER
			dimmer_set_blank_on();
#endif
			input_len = 0;	// to distinguish ESC(single ESC key) and ESC-X(ESC sequence)
		}
		if (selected & MYSELECT_BIT_READ1) {
			// input from console
#ifdef ENABLE_DIMMER
			dimmer_set_blank_off();
#endif
			input_len = read(STDIN_FILENO, input_buf, INPUT_BUF_LEN);
			check_hot_key(term, input_buf, input_len);
			vterm_clear_cursor_blink_counter(&(term->vterm));
		}
#ifdef ENABLE_IMJ
		// If input_len = 0, ESC(single ESC key) and ESC-X(ESC sequence) are distinguished.
		imcui_ret = imcui_filter(input_buf, input_len, &filtered_len, INPUT_BUF_LEN);
		input_len = filtered_len;
#endif // ENABLE_IMJ
		if (input_len > 0) {
			// send to application
			if (write(term->fd_pty_master, input_buf, input_len) <= 0) {
				_ERR_
			}
#if defined(DEBUG_NO_INPUT_EXIT)
			no_input_timer = -1;
#endif
		}
		if (selected & MYSELECT_BIT_READ2) {
			// output from application
			output_len = read(term->fd_pty_master, output_buf, OUTPUT_BUF_LEN);
			if (output_len > 0) {
				// render output onto framebuffer
				output_buf[output_len] = '\0';	// NUL terminate string
				vterm_emulate_str__update_screen(&(term->vterm), output_buf, output_len);
			}
		}
		if (input_len == 0 && output_len == 0) {
		}
#ifdef ENABLE_IMJ
		if (imcui_ret < 0) {	// key is eaten by IMJ
			imj_display();
		}
#endif // ENABLE_IMJ
		call_interval_timer_process(&(term->vterm));
	}
}

PRIVATE void call_interval_timer_process(vterm_t *vterm)
{
	static unsigned long prev_msec;
	unsigned long cur_msec;

	cur_msec = get_cur_abs_msec();
	if ((cur_msec - prev_msec) % (SELECT_TIMEOUT_MSEC * 2) >= SELECT_TIMEOUT_MSEC) {
///mflf_d_printf("zzz %03d\n", (cur_msec - prev_msec) % (SELECT_TIMEOUT_MSEC * 2));
		vterm_interval_timer_process(vterm);
		prev_msec = cur_msec;
	}
}

#ifdef ENABLE_SCREEN_SHOT
#define SCREEN_SHOT_KEY_STR		"\x1b\x21"	// Alt-'!'
#define SCREEN_SHOT_KEY_STR_LEN		2
#endif // ENABLE_SCREEN_SHOT

///
#define ON_THE_FLY_SCREEN_RESIZING
#ifdef ON_THE_FLY_SCREEN_RESIZING
#define DEC_FONT_SIZE_KEY1_STR	"\x1b\x2d"	// Alt-'-'
#define DEC_FONT_SIZE_KEY1_STR_LEN	2
#define INC_FONT_SIZE_KEY1_STR	"\x1b\x3d"	// Alt-'='
#define INC_FONT_SIZE_KEY1_STR_LEN	2
#define DEC_ROTATION_KEY1_STR	"\x1b\x5f"	// Alt-'_'(Shift)
#define DEC_ROTATION_KEY1_STR_LEN	2
#define INC_ROTATION_KEY1_STR	"\x1b\x2b"	// Alt-'+'(Shift)
#define INC_ROTATION_KEY1_STR_LEN	2
#endif // ON_THE_FLY_SCREEN_RESIZING

PRIVATE void check_hot_key(term_t *term, char *buf, int input_len)
{
#ifdef ENABLE_SCREEN_SHOT
	// check Special function key
///flf_d_printf("%d[%02x %02x]\n", input_len, buf[0], buf[1]);
	if (input_len == SCREEN_SHOT_KEY_STR_LEN
	 && strncmp(buf, SCREEN_SHOT_KEY_STR, SCREEN_SHOT_KEY_STR_LEN) == 0) {
		// Screen-shot key pushed
_FLF_
		screen_shot(fbr_screen_size_hx, fbr_screen_size_hy,
		 fb__.driver->get_pixel_argb32, fb__.driver->reverse_all,
		 NULL);
	}
#endif // ENABLE_SCREEN_SHOT
#ifdef ON_THE_FLY_SCREEN_RESIZING
	if ((input_len == DEC_FONT_SIZE_KEY1_STR_LEN)
	  && (strncmp(buf, DEC_FONT_SIZE_KEY1_STR, DEC_FONT_SIZE_KEY1_STR_LEN) == 0)) {
		// Change font size smaller
		term_change_font_size(term, -1);
	}
	if ((input_len == INC_FONT_SIZE_KEY1_STR_LEN)
	  && (strncmp(buf, INC_FONT_SIZE_KEY1_STR, INC_FONT_SIZE_KEY1_STR_LEN) == 0)) {
		// Change font size larger
		term_change_font_size(term, +1);
	}
	if ((input_len == DEC_ROTATION_KEY1_STR_LEN)
	  && (strncmp(buf, DEC_ROTATION_KEY1_STR, DEC_ROTATION_KEY1_STR_LEN) == 0)) {
		term_change_rotation(term, +1);
	}
	if ((input_len == INC_ROTATION_KEY1_STR_LEN)
	  && (strncmp(buf, INC_ROTATION_KEY1_STR, INC_ROTATION_KEY1_STR_LEN) == 0)) {
		term_change_rotation(term, -1);
	}
#endif // ON_THE_FLY_SCREEN_RESIZING
}

//-----------------------------------------------------------------------------

#define	INIT_ERR1	-1
#define	INIT_ERR2	-2
#define	INIT_ERR3	-3
#define	INIT_ERR4	-4
#define	INIT_ERR5	-5
#define	INIT_ERR6	-6
#define	INIT_ERR7	-7

PRIVATE int ret_val_eater;
PRIVATE int term_init(term_t *term)
{
	term->orig_kdmode = -1;
	term->fd_console = -1;
	term->fd_pty_master = -1;
	term->fd_pty_slave = -1;
	term->pty_slave_name[0] = '\0';

	ret_val_eater = write(1, "\x1B[?25l", 6);	// cursor off

	if (term_open_dev_console(term) < 0) {
		_ERR_
		return INIT_ERR1;
	}
	term_save_term_attr(term);
	if (fb_get_fb_dev_name(&fb__) < 0) {
		_ERR_
		return INIT_ERR2;
	}
	if (fb_open(&fb__) < 0) {				// open FB device and mmap
		_ERR_
		return INIT_ERR3;
	}

	if (term_set_kd_graphics_mode(term) < 0) {
		_ERR_
		return INIT_ERR4;
	}
	if (term_open_ptytty(term) < 0) {
		_ERR_
		return INIT_ERR5;
	}
	term_set_utmp(term);
	term_setup_term_attr(term);

	if (font_load_all() <= 0) {
		_ERR_
		return INIT_ERR6;
	}
	cur_font_mul_idx = font_select_by_height_mul_xy(app__.font_size,
	 app__.expand_x, app__.expand_y);
	if (cur_font_mul_idx < 0) {
		_ERR_
		return INIT_ERR7;
	}

	fbr_setup_constant_parameters();
	fbr_set_rotation(app__.contents_rotation);
	term_setup_font_and_rotation_parameters(1);
	if (app__.columns > 0) {
		term_select_font_by_columns(app__.columns);	// search font from small to larger one
		term_select_font_by_columns(app__.columns);	// search font from larger to smaller one
	}
	term_set_metrics_n_show(term);

	vterm_init(&(term->vterm));

#ifdef ENABLE_IMJ
	imj_open();
#endif // ENABLE_IMJ

	vterm_start_fb(&(term->vterm));
	// redirect console output to pty
	if (term_redirect_console_output(term->fd_pty_slave) < 0) {
///		_ERR_
///		se_printf("ioctl TIOCCONS");
	}
_FLF_
	return 0;
}

PRIVATE void term_destroy(term_t *term, int err)
{
	if (err == INIT_ERR1)
		goto term_destroy_INIT_ERR1;
	if (err == INIT_ERR2)
		goto term_destroy_INIT_ERR2;
	if (err == INIT_ERR3)
		goto term_destroy_INIT_ERR3;
	if (err == INIT_ERR4)
		goto term_destroy_INIT_ERR4;
	if (err == INIT_ERR5)
		goto term_destroy_INIT_ERR5;
	if (err == INIT_ERR6)
		goto term_destroy_INIT_ERR6;

	// stop redirection of console output to pty
	if (term_redirect_console_output(term->fd_console) < 0) {
		_ERR_
		se_printf("ioctl TIOCCONS");
	}
	vterm_stop_fb();

#ifdef ENABLE_IMJ
	imj_close();
#endif // ENABLE_IMJ

	vterm_destroy(&(term->vterm));
	font_destroy_all();

term_destroy_INIT_ERR6:;
	term_restore_term_attr(term);
	term_reset_utmp(term);
	term_close_ptytty(term);

term_destroy_INIT_ERR5:;
	term_restore_kd_text_mode(term);

term_destroy_INIT_ERR4:;
	ret_val_eater = fb_close(&fb__);

term_destroy_INIT_ERR3:;
	term_close_dev_console(term);
term_destroy_INIT_ERR2:;
term_destroy_INIT_ERR1:;

	ret_val_eater = write(1, "\x1B[?25h", 6);	// cursor on
}

//-----------------------------------------------------------------------------
// On the fly font-size and screen-rotation change

// User changed font glyph and size on the fly
PRIVATE void term_change_font_size(term_t *term, int shift)
{
flf_d_printf("shift: %d\n", shift);
	cur_font_mul_idx = font_select_next(cur_font_mul_idx, shift);

	term_setup_font_and_rotation_parameters(1);
	term_set_metrics_n_show(term);
}
// User changed screen rotation on the fly
PRIVATE void term_change_rotation(term_t *term, int shift)
{
flf_d_printf("shift: %d\n", shift);
	fbr_set_rotation(view_rotation_t((fbr_get_rotation() + shift) % ROT360));

	int columns = fbr_chars_hx;
	term_setup_font_and_rotation_parameters(0);
	term_select_font_by_columns(columns);
	term_set_metrics_n_show(term);
}

// TODO: automatically select font that can display
//  the nearest screen character columns to before changing rotation
PRIVATE void term_select_font_by_columns(int columns)
{
	if (fbr_chars_hx < columns) {
		// columns became smaller.
		//  select smaller(mul_x == mul_y) font and make columns larger
		int shift = -1;
		while (fbr_chars_hx < columns || (cur_font_mul->mul_x != cur_font_mul->mul_y)) {
			int prev_cur_font_mul_idx = cur_font_mul_idx;
			cur_font_mul_idx = font_select_next(cur_font_mul_idx, shift);
			if (cur_font_mul_idx == prev_cur_font_mul_idx) {
				break;
			}
			term_setup_font_and_rotation_parameters(1);
		}
	} else {
		// columns became bigger.
		//  select larger(mul_x == mul_y) font and make columns smaller
		int shift = +1;
		while (fbr_chars_hx > columns || (cur_font_mul->mul_x != cur_font_mul->mul_y)) {
			int prev_cur_font_mul_idx = cur_font_mul_idx;
			cur_font_mul_idx = font_select_next(cur_font_mul_idx, shift);
			if (cur_font_mul_idx == prev_cur_font_mul_idx) {
				break;
			}
			term_setup_font_and_rotation_parameters(1);
		}
	}
}

PRIVATE void term_setup_font_and_rotation_parameters(bool font)
{
	// setup font-size dependent parts
	if (font) {
		font_setup_metrics();
		fbr_copy_font_size_into_driver();
	}

	// setup screen-rotation dependent parts
	fbr_setup_rotation_dependent_parameters();

	// setup font-size and screen-rotation dependent parts
	fbr_set_text_metrics();
}

// Re-setup "terminal metrics" when font glyph or rotation is changed by user on the fly
PRIVATE void term_set_metrics_n_show(term_t *term)
{
	term_set_metrics(term);
	term_show_metrics(term);
}

PRIVATE void term_set_metrics(term_t *term)
{
_FLF_
	vterm_set_metrics(&(term->vterm));
}
PRIVATE void term_show_metrics(term_t *term)
{
_FLF_
	// hide overlay
	vterm_set_overlay(&(term->vterm), OVERLAY_IDX_0, -1, 0,
	 COLOR_LIGHTCYAN, COLOR_LIGHTRED, "", 0, 0);
	imj_hide();

	char overlay_text[OVERLAY_TEXT_LEN+1];
	// set overlay
	snprintf(overlay_text, OVERLAY_TEXT_LEN+1,
	//12345678901234567890123456789012345678901234567890123456789012345678901234567890
	 "[ Frame(%dx%d) : Font(%dx%d) : Console(%dx%d) ]",
	 fb__.screen_size_x, fb__.screen_size_y,
	 cur_font_exp->width, cur_font_exp->height,
	 term->vterm.text_columns, term->vterm.text_lines);
	vterm_set_overlay(&(term->vterm), OVERLAY_IDX_0, OVERLAY_TEXT_Y, OVERLAY_TEXT_X,
	 COLOR_LIGHTCYAN, COLOR_LIGHTRED, overlay_text, -1, OVERLAY_TEXT_SECS);

	// repaint screen
	vterm_clear_outside_of_view(&(term->vterm));
	vterm_request_repaint_all(&(term->vterm));

	// send SIGWINCH
	vterm_send_sigwinch(&(term->vterm));
flf_d_printf("vterm->text_lines:%2d, vterm->text_columns:%2d\n",
 term->vterm.text_lines, term->vterm.text_columns);
}

//-----------------------------------------------------------------------------

PRIVATE int term_open_dev_console(term_t *term)
{
	if ((term->fd_console = util_privilege_open("/dev/console", O_WRONLY)) < 0) {
		if ((term->fd_console = util_privilege_open("/dev/console", O_RDONLY)) < 0) {
			_ERR_
			se_printf("open /dev/console");
			return -1;
		}
	}
	return term->fd_console;
}
PRIVATE int term_close_dev_console(term_t *term)
{
	close(term->fd_console);
	term->fd_console = -1;
	return 0;
}
//-----------------------------------------------------------------------------
PRIVATE void term_save_term_attr(term_t *term)
{
	tcgetattr(0, &(term->tty_attr_saved));
}
PRIVATE void term_setup_term_attr(term_t *term)
{
	struct termios tio;

	tio = term->tty_attr_saved;
	tio.c_lflag &= ~(ECHO|ISIG|ICANON|XCASE);
	tio.c_iflag = 0;
	tcsetattr(0, TCSANOW, &tio);
}
PRIVATE void term_restore_term_attr(term_t *term)
{
	tcsetattr(0, TCSANOW, &(term->tty_attr_saved));
}
//-----------------------------------------------------------------------------
#define TTY_DEV_NAME_LEN	16		// /dev/tty99
PRIVATE int term_set_kd_graphics_mode(term_t *term)
{
	if ((term->orig_kdmode = term_get_kdmode(term)) != KD_TEXT) {
		er_printf("Already in KD_GRAPHICS mode. Stop starting %s.\n", PACKAGE);
		return -1;
	}
	term_set_kdmode(KD_GRAPHICS);
_FLF_
	return 0;
}
PRIVATE void term_restore_kd_text_mode(term_t *term)
{
	term_set_kdmode(term->orig_kdmode);
}
//-----------------------------------------------------------------------------
#if 0
PRIVATE int get_current_vt_num(term_t *term)
{
	struct vt_stat vt_st;

	// get current VT number
	if (ioctl(term->fd_console, VT_GETSTATE, &vt_st) < 0) {
		_ERR_
		se_printf("ioctl VT_GETSTATE");
		return -1;
	}
mflf_d_printf("current VT number: %d\n", vt_st.v_active);
	return vt_st.v_active;
}
#endif
PRIVATE int term_get_kdmode(term_t *term)
{
	int kdmode;

	if (ioctl(term->fd_console, KDGETMODE, &kdmode) < 0) {
		_ERR_
		se_printf("ioctl KDGETMODE");
		return -1;
	}
flf_d_printf("kdmode:%d (KD_TEXT:%d, KD_GRAPHICS:%d)\n", kdmode, KD_TEXT, KD_GRAPHICS);
	if (kdmode != KD_TEXT) {
flf_d_printf("ioctl(KDGETMODE) != KD_TEXT\n");
flf_d_printf("started on FB console or X-Window\n");
	} else {
flf_d_printf("ioctl(KDGETMODE) == KD_TEXT\n");
flf_d_printf("started on text console\n");
	}
	return kdmode;
}
PRIVATE void term_set_kdmode(int kdmode)
{
flf_d_printf("ioctl KDSETMODE, %s\n",
 kdmode == KD_TEXT ? "KD_TEXT" : (kdmode == KD_GRAPHICS ? "KD_GRAPHICS" : "KD_????"));
	if (util_privilege_ioctl(0, KDSETMODE, (void *)(long)kdmode) < 0) {
		_ERR_
		se_printf("ioctl KDSETMODE");
	}
}
//-----------------------------------------------------------------------------
PRIVATE int term_open_ptytty(term_t *term)
{
	int ret;

	util_privilege_raise();
	ret = openpty(&term->fd_pty_master, &term->fd_pty_slave,
	 term->pty_slave_name, NULL, NULL);
flf_d_printf("fd_pty_master: %d, fd_pty_slave: %d\n", term->fd_pty_master, term->fd_pty_slave);
	util_privilege_lower();
	if (ret < 0) {
		_ERR_
		se_printf("openpty");
		return -1;
	}
	return 0;
}
PRIVATE void term_close_ptytty(term_t *term)
{
	term_close_pty_master(term);
	term_close_pty_slave(term);
}
PRIVATE void term_close_pty_master(term_t *term)
{
	if (term->fd_pty_master >= 0) {
		close(term->fd_pty_master);
		term->fd_pty_master = -1;
	}
}
PRIVATE void term_close_pty_slave(term_t *term)
{
	if (term->fd_pty_slave >= 0) {
		close(term->fd_pty_slave);
		term->fd_pty_slave = -1;
	}
}
//-----------------------------------------------------------------------------
PRIVATE int term_redirect_console_output(int fd)
{
flf_d_printf("redirect console output to fd: %d\n", fd);
	return util_privilege_ioctl(fd, TIOCCONS, NULL);
}
//-----------------------------------------------------------------------------

PRIVATE void term_set_utmp(term_t *term)
{
	struct utmp utmp;
	struct passwd *pw;
	char *term_name;

_FLF_
	pw = getpwuid(util_getuid());
flf_d_printf("pw_name: [%s]\n", pw->pw_name);
	term_name = rindex(term->pty_slave_name, '/') + 1;
flf_d_printf("term->slave: [%s], basename: [%s]\n", term->pty_slave_name, term_name);
	memset((char *)&utmp, 0, sizeof(utmp));
	strncpy(utmp.ut_id, term_name, sizeof(utmp.ut_id));
	utmp.ut_type = DEAD_PROCESS;
flf_d_printf("utmp.ut_id: [%s]\n", utmp.ut_id);

	setutent();
	getutid(&utmp);
	utmp.ut_type = USER_PROCESS;
	utmp.ut_pid = getpid();
	if (strncmp("/dev/", term->pty_slave_name, 5) == 0) {
		term_name = term->pty_slave_name + 5;
	}
	strncpy(utmp.ut_line, term_name, sizeof(utmp.ut_line));
	strncpy(utmp.ut_user, pw->pw_name, sizeof(utmp.ut_user));
	util_time32(&(utmp.ut_time));
	pututline(&utmp);
	endutent();
}

PRIVATE void term_reset_utmp(term_t *term)
{
	struct utmp utmp, *utp;
	char *term_name;

	term_name = rindex(term->pty_slave_name, '/') + 1;
flf_d_printf("term->slave: [%s], basename: [%s]\n", term->pty_slave_name, term_name);
	memset((char *)&utmp, 0, sizeof(utmp));
	strncpy(utmp.ut_id, term_name, sizeof(utmp.ut_id));
	utmp.ut_type = USER_PROCESS;
flf_d_printf("utmp.ut_id: [%s]\n", utmp.ut_id);

	setutent();
	utp = getutid(&utmp);
	if (utp) {
		utp->ut_type = DEAD_PROCESS;
		memset(utp->ut_user, 0, sizeof(utmp.ut_user));
		utp->ut_type = DEAD_PROCESS;
		util_time32(&(utp->ut_time));
		pututline(utp);
	}
	endutent();
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_DIMMER
#define DIMMER_TIMEOUT (3 * 60 * 10)        /* 3 min */
PRIVATE int dimmer_idle_time = 0;
PRIVATE int dimmer_blank = 0;
PRIVATE void dimmer_set_blank_on(void)
{
	if (!dimmer_blank && ++dimmer_idle_time > DIMMER_TIMEOUT) {
		// Goto dimmer_blank
		dimmer_idle_time = 0;
		if (fb_set_blank(fb__.fd_fb, fb__.my_vt_num, 1))
			dimmer_blank = 1;
	}
}
PRIVATE void dimmer_set_blank_off(void)
{
	dimmer_idle_time = 0;
	if (dimmer_blank) {
		// Wakeup console
		dimmer_blank = 0;
		fb_set_blank(fb__.fd_fb, fb__.my_vt_num, 0);
	}
}
#endif // ENABLE_DIMMER

//-----------------------------------------------------------------------------

__sighandler_t sigchld_handler(int signum);
__sighandler_t sigterm_handler(int signum);

void signal_init(void)
{
	sigchld_signaled = 0;
	sigterm_signaled = 0;

	signal(SIGCHLD, (__sighandler_t)sigchld_handler);
	signal(SIGTERM,  (__sighandler_t)sigterm_handler);
}

__sighandler_t sigchld_handler(int signum)
{
	int st;
	int ret;

	ret = wait(&st);
	if (ret == child_pid || ret == ECHILD) {
		sigchld_signaled = 1;
	}
	return 0;
}

__sighandler_t sigterm_handler(int signum)
{
	sigterm_signaled = 1;
	return 0;
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_IMJ

// imjの描画管理
//   imj表示
//     読み入力中または変換中は、カーソルの上1〜2行または、カーソルの上1〜2行を
//     imjの表示領域として使用する。
//   imj描画を行うべき時
//     imj に対しキーイベントが送付された(表示内容が変化した)時。
//     ターミナルのカーソル位置が変更された時。

PRIVATE vterm_t *imj_vterm;
PRIVATE int imj_shown = 0;
PRIVATE int imj_line1_y = 0;
PRIVATE int imj_line2_y = 0;
PRIVATE char imj_blank_line[MAX_TERM_COLS+1];

int imj_open(void)
{
	int err = 0;

	imj_vterm = &(term__.vterm);
flf_d_printf("imj_vterm->text_lines:%2d, imj_vterm->text_columns:%2d\n",
 imj_vterm->text_lines, imj_vterm->text_columns);

	switch(imj__.im_open(getenv("HOME"))) {
	case 1:
		flf_d_printf("Can not load dictionary file\n");
		break;
	case 2:
		flf_d_printf("Can not load history file\n");
		break;
	default:
		break;
	}

	memset(imj_blank_line, ' ', MAX_TERM_COLS);
	imj_blank_line[MAX_TERM_COLS] = '\0';

	return err;
}
void imj_close(void)
{
	imj__.im_close();
}

void imj_display(void)
{
	static int prev_im_mode = 0;
	int im_mode;
	int to_show = 0;
	char buf[IM_INPUT_LINE_LEN+1];

	im_mode = imj__.im_get_mode();
mflf_d_printf("im_get_mode(): %d\n", im_mode);
	switch(im_mode) {
	case IM_MODE_OFF:
		// IM is off
		break;
	case IM_MODE_INPUT:
		if (strlen(imj__.im_get_pronun(buf, IM_INPUT_LINE_LEN))) {
			// inputting pronun
			to_show = 1;
		}
mflf_d_printf("pronun: %d:[%s]\n", strlen(buf), buf);
		break;
	case IM_MODE_CONVERT:
		// converting pronun to kanji
		to_show = 1;
		break;
	}

	if (prev_im_mode != im_mode) {
		// im_mode changed -> repaint screen
		vterm_request_repaint_all(imj_vterm);
		prev_im_mode = im_mode;
	}
	if (to_show == 0) {
		imj_hide();
	} else {
		imj_show();
	}
	vterm_set_cursor_mode(imj_vterm, im_mode != IM_MODE_OFF);
}
PRIVATE void imj_show(void)
{
	int xx, yy;
	char buf[IM_INPUT_LINE_LEN+1];
	char buffer[IM_INPUT_LINE_LEN+1];
	const char *prompt_input   = "I: ";
	const char *prompt_convert = "C: ";
	int byte_idx;
	int cursor_byte_idx;

///_MFLF_
///	if (vterm_adjust_pen_pos(imj_vterm)) {
///_FLF_
///	}
	pen_get_yxy(&(imj_vterm->pen), &yy, &xx);
	vterm_limit_screen_pos(imj_vterm, &yy, &xx);
#define	IM_LINES		2
#define	IM_BC			COLOR_LIGHTBLUE
#define	IM_FC			COLOR_WHITE
	if (yy < imj_vterm->text_lines-IM_LINES) {
		// application cursor line
		// [input line]
		// [conversion candidates]
		imj_line1_y = yy+1;
		imj_line2_y = yy+2;
	} else {
		// [conversion candidates]
		// [input line]
		// application cursor line
		imj_line1_y = yy-1;
		imj_line2_y = yy-2;
	}

	switch(imj__.im_get_mode()) {
	case IM_MODE_OFF:
		// IM is off
		break;
	case IM_MODE_INPUT:
		if (strlen(imj__.im_get_pronun(buf, IM_INPUT_LINE_LEN))) {
			vterm_set_overlay(imj_vterm, OVERLAY_IDX_1, -1, 0, IM_BC, IM_FC, "", 0, 0);
			vterm_set_overlay(imj_vterm, OVERLAY_IDX_2, -1, 0, IM_BC, IM_FC, "", 0, 0);
			// inputting pronun
			// [I:かんじ                                         ]
			strcpy(buffer, prompt_input);
			utf8strcat_blen(buffer, IM_INPUT_LINE_LEN, buf);
			// display input line
			vterm_set_overlay(imj_vterm, OVERLAY_IDX_1, imj_line1_y, 0, IM_BC, IM_FC, buffer, -1, 0);
			byte_idx = imj__.im_get_cursor_offset();
			xx = utf8s_columns(prompt_input, INT_MAX) + utf8s_columns(buf, byte_idx);
			xx = MIN_(xx, utf8s_columns(buffer, INT_MAX));
			cursor_byte_idx = strlen(prompt_input) + byte_idx;
			// display IM cursor
			vterm_set_overlay(imj_vterm, OVERLAY_IDX_1, imj_line1_y, xx,
			 INV_C_IDX(IM_BC), INV_C_IDX(IM_FC),
			 &buffer[cursor_byte_idx], utf8c_bytes(&buf[byte_idx]), 0);
			// no conversion candidates
			vterm_set_overlay(imj_vterm, OVERLAY_IDX_2, -1, 0, COLOR_DEF_BC, COLOR_DEF_FC,
			 "", 0, 0);
			vterm_paint_screen(imj_vterm);
		}
		break;
	case IM_MODE_CONVERT:
		vterm_set_overlay(imj_vterm, OVERLAY_IDX_1, -1, 0, IM_BC, IM_FC, "", 0, 0);
		vterm_set_overlay(imj_vterm, OVERLAY_IDX_2, -1, 0, IM_BC, IM_FC, "", 0, 0);
		// converting pronun to kanji
		// [C:感じ                                              ]
		// [<感じ> 幹事 漢字                                    ]
		imcui_get_converting_line_str(0, buf, IM_INPUT_LINE_LEN);
		strcpy(buffer, prompt_convert);
		utf8strcat_blen(buffer, MIN_(IM_INPUT_LINE_LEN, imj_vterm->text_columns), buf);
		// display input line
		vterm_set_overlay(imj_vterm, OVERLAY_IDX_1, imj_line1_y, 0, IM_BC, IM_FC,
		 buffer, -1, 0);
		imj__.im_c_get_cur_candid(buf, IM_INPUT_LINE_LEN);
		xx = utf8s_columns(prompt_input, INT_MAX) + utf8s_columns(buf, byte_idx);
		xx = MIN_(xx, utf8s_columns(buffer, INT_MAX));
		cursor_byte_idx = strlen(prompt_input) + byte_idx;
		// display IM cursor
		vterm_set_overlay(imj_vterm, OVERLAY_IDX_1, imj_line1_y, xx,
		 INV_C_IDX(IM_BC), INV_C_IDX(IM_FC),
		 &buffer[cursor_byte_idx], utf8c_bytes(&buf[byte_idx]), 0);

		imcui_get_converting_line_str(1, buf, IM_INPUT_LINE_LEN);
		utf8strcpy_blen(buffer, MIN_(IM_INPUT_LINE_LEN, imj_vterm->text_columns), buf);
		// display conversion candidates
		vterm_set_overlay(imj_vterm, OVERLAY_IDX_2, imj_line2_y, 0, IM_BC, IM_FC,
		 buffer, -1, 0);
		vterm_paint_screen(imj_vterm);
		break;
	}

	imj_shown = 1;
}
PRIVATE void imj_hide(void)
{
_MFLF_
	if (imj_shown) {
_MFLF_
		// hide IM by clearing overlay lines
		vterm_set_overlay(imj_vterm, OVERLAY_IDX_1, -1, 0, COLOR_DEF_BC, COLOR_DEF_FC,
		 "", 0, 0);
		vterm_set_overlay(imj_vterm, OVERLAY_IDX_2, -1, 0, COLOR_DEF_BC, COLOR_DEF_FC,
		 "", 0, 0);
		vterm_paint_screen(imj_vterm);
		imj_shown = 0;
	}
}

#endif // ENABLE_IMJ

// End of term.cpp
