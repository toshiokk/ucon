/*
 * Copyright (C) 2010 Toshio Koike
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>
#include <fcntl.h>
#include <pwd.h>
#include <pty.h>
#include <utmp.h>
#include <grp.h>
#include <math.h>
#include <termios.h>
#include <malloc.h>
#include <endian.h>
#include <getopt.h>
#include <locale.h>
#include <langinfo.h>
#include <iconv.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/vt.h>
#include <sys/kd.h>
#include <linux/vt.h>
#include <linux/fb.h>
//#include <asm/page.h>
#ifdef HAVE_IOPERM
#include <sys/io.h>
#endif
#ifndef __USE_XOPEN
#define __USE_XOPEN		// for wcwidth() in wchar.h
#endif
#include <wchar.h>

// End of stdincs.h
