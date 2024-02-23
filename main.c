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

/*
  module hierarchy:
	app_t - term_t - vterm_t - frame_buffer_t
 */

app_t app__;

PRIVATE void app_get_options(app_t *app, int argc, char *argv[]);
PRIVATE void app_init(app_t *app);
PRIVATE void app_destroy(app_t *app);
PRIVATE void app_show_help(void);

int main(int argc, char *argv[])
{
	const char start_message[] = 
"+---------------------------------------------------------------\n"
"| %s - Framebuffer UTF-8 Console%s\n"
"|   Version %s (compiled at %s %s)\n"
"|   Copyright (C) 2010 PCR\n"
"| %s is based on JFBTERM\n"
"|   Copyright (C) 2003 Fumitoshi UKAI\n"
"|   Copyright (C) 1999-2000 Noritoshi Masuichi\n"
"| JFBTERM is based on KON2\n"
"|   Copyright (C) 1992-1996 Takashi MANABE\n"
"+---------------------------------------------------------------\n";
	int arg_idx;

#if 1
																// i386	amd64
	flf_d_printf("sizeof(char): %d\n", sizeof(char));			//	1	1
	flf_d_printf("sizeof(short): %d\n", sizeof(short));			//	2	2
	flf_d_printf("sizeof(int): %d\n", sizeof(int));				//	4	4
	flf_d_printf("sizeof(long): %d\n", sizeof(long));			//	4	8
	flf_d_printf("sizeof(void *): %d\n", sizeof(void *));		//	4	8
	flf_d_printf("sizeof(long long): %d\n", sizeof(long long));	//	4	8
	flf_d_printf("sizeof(wchar_t): %d\n", sizeof(wchar_t));		//	4	4
#endif

///	v_printf("path:%s\n", argv[0]);
	setenv("LANG", "ja_JP.UTF-8", 1);
	setlocale(LC_ALL, "ja_JP.UTF-8");
///	setlocale(LC_CTYPE, "ja_JP.UTF-8");
	setenv("UCON", "1", 1);

	util_privilege_init();

#ifdef ENABLE_IMJ
	flf_d_printf(start_message, PACKAGE, " with Japanese Input Method",
	 VERSION, __DATE__, __TIME__, PACKAGE);
#else // ENABLE_IMJ
	flf_d_printf(start_message, PACKAGE, "",
	 VERSION, __DATE__, __TIME__, PACKAGE);
#endif // ENABLE_IMJ

	app_init(&app__);
	app_get_options(&app__, argc, argv);

	if (app__.show_help) {
		app_show_help();
		goto main_exit;
	}

	v_printf("exec_cmd: [%s]\n", app__.exec_cmd);
	for (arg_idx = 0; arg_idx < MAX_SHELL_ARGS+1 && app__.exec_argv[arg_idx]; arg_idx++) {
		v_printf("exec_argv[%d]: [%s]\n", arg_idx, app__.exec_argv[arg_idx]);
	}

	if (term_start_child_and_serve(&term__) < 0) {
		_ERR_
	}

main_exit:;
	app_destroy(&app__);
	flf_d_printf("Exit %s\n", PACKAGE);
	exit(EXIT_SUCCESS);
}

PRIVATE void app_get_options(app_t *app, int argc, char *argv[])
{
	char *short_opt = "br:f:x:y:wc:hlvd";
	static struct option long_opt[] = {
		{ "bell",			0, NULL, 'b' },
		{ "rotation",		1, NULL, 'r' },		// -r 1
		{ "font",			1, NULL, 'f' },		// -f 12
		{ "expand",			1, NULL, 'x' },
		{ "expand_y",		1, NULL, 'y' },
		{ "use_whole_buf",	0, NULL, 'w' },
		{ "exec",			1, NULL, 'c' },		// -c "commands ..."
		{ "help",			0, NULL, 'h' },
		{ "highlight",		0, NULL, 'l' },
		///{ "verbose",		0, NULL, 'v' },
		{ "debug",			0, NULL, 'd' },
		{ NULL,				0, NULL, 0   },
	};
	int chr;
	int arg_idx;

#ifdef ENABLE_DEBUG
_FLF_
	for (arg_idx = 0; arg_idx < argc; arg_idx++) {
		v_printf("%d: [%s]\n", arg_idx, argv[arg_idx]);
	}
#endif // ENABLE_DEBUG

	while (1) {
		chr = getopt_long(argc, argv, short_opt, long_opt, &arg_idx);
		if (chr == EOF) {
			break;
		}
		switch (chr) {
		case 'b':
			app->no_bell = TRUE;
			break;
		case 'r':
			app->contents_rotation = MIN_MAX(0, atoi(optarg), 3);
			break;
		case 'f':
			app->font_size = MIN_MAX(10, atoi(optarg), 16) / 2 * 2;
			break;
		case 'x':
			app->expand_x = MIN_MAX(1, atoi(optarg), 4);
			app->expand_y = app->expand_x;
			break;
		case 'y':
			app->expand_y = MIN_MAX(1, atoi(optarg), 4);
			break;
		case 'w':
			app->use_whole_buf = TRUE;
			break;
		case 'c':
			app->exec_cmd = optarg;
			app->exec_argv[0] = optarg;
			app->exec_argv[1] = NULL;
			break;
		case 'h':
			app->show_help = TRUE;
			break;
		case 'l':
			app->highlight = TRUE;
			break;
		case 'v':
			///app->verbose = TRUE;
			break;
		case 'd':
			app->debug = TRUE;
			///app->verbose = TRUE;	// "-d" option enables "-v" too
			break;
		default:
			break;
		}
	}
}

PRIVATE void app_init(app_t *app)
{
	static char shell[128+1];

	///
	app->contents_rotation = ROT000;
	///	app->contents_rotation = ROT090;
	///	app->contents_rotation = ROT180;
	///	app->contents_rotation = ROT270;
	app->font_size = 12;
	app->expand_x = 1;		// 1 dots per pixels
	app->expand_y = 1;		// 1 dots per pixels
	app->no_bell = FALSE;
	app->use_whole_buf = FALSE;
	if (getenv("SHELL")) {
		/* This cause a buffer overflow. */
		memset(shell, '\0', sizeof(shell));
		strncpy(shell, getenv("SHELL"), sizeof(shell) - 1);
		app->exec_cmd = shell;
	} else {
		app->exec_cmd = "/bin/bash";
	}
///app->exec_cmd = "/bin/echo";
	app->highlight = FALSE;
	app->show_help = FALSE;
	///app->verbose = FALSE;
	app->debug = FALSE;
#ifdef FORCE_DEBUG
	///app->verbose = TRUE;
	app->debug = TRUE;
#endif // FORCE_DEBUG
}

PRIVATE void app_destroy(app_t *app)
{
}

PRIVATE void app_show_help(void)
{
	fprintf(stdout, "Usage: %s [options]\n", PACKAGE);
	fprintf(stdout,
	 "  -b --bell\n"
	 "  -r --rotation 0/1/2/3\n"
	 "  -f --font fontsize[10/12/14/16]\n"
	 "  -x --expand N\n"
	 "  -y --expand_y N\n"
	 "  -w --use_whole_buf\n"
	 "  -e --exec program-name\n"
	 "  -h --help\n"
	 "  -l --highlight\n"
	/// "  -v --verbose\n"
	 "  -d --debug\n"
	);
	fprintf(stdout,
	 "  Supported FB depth are:"
#ifdef ENABLE_8BPP
			" 8bpp"
#endif
#ifdef ENABLE_15BPP
			" 15bpp"
#endif
#ifdef ENABLE_16BPP
			" 16bpp"
#endif
#ifdef ENABLE_24BPP
			" 24bpp"
#endif
#ifdef ENABLE_32BPP
			" 32bpp"
#endif
		"\n"
	);
}

// End of main.c
