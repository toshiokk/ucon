#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(ucon, 23.0809, www.sourceforge.net)
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lutil':
AC_CHECK_LIB([util], [main])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h langinfo.h locale.h malloc.h stdlib.h string.h sys/ioctl.h sys/time.h termios.h unistd.h utmp.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_HEADER_TIME
AC_TYPE_UID_T

# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_HEADER_MAJOR
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit bzero dup2 memmove memset munmap pow select setenv setlocale strcasecmp strchr strdup strerror strtol])

AC_ARG_ENABLE(debug,
 [  --enable-debug            turn on debugging [default no]],,enable_debug=no)
AC_MSG_CHECKING(whether to enable debugging)
if test x$enable_debug = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_DEBUG, 1, [Define this to enable debug])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(8bpp,
 [  --enable-8bpp             support 8bpp [default yes]],,enable_8bpp=yes)
AC_MSG_CHECKING(whether to enable 8bpp)
if test x$enable_8bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_8BPP, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(15bpp,
 [  --enable-15bpp            support 15bpp [default yes]],,enable_15bpp=yes)
AC_MSG_CHECKING(whether to enable 15bpp)
if test x$enable_15bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_15BPP, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(16bpp,
 [  --enable-16bpp            support 16bpp [default yes]],,enable_16bpp=yes)
AC_MSG_CHECKING(whether to enable 16bpp)
if test x$enable_16bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_16BPP, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(24bpp,
 [  --enable-24bpp            support 24bpp [default yes]],,enable_24bpp=yes)
AC_MSG_CHECKING(whether to enable 24bpp)
if test x$enable_24bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_24BPP, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(32bpp,
 [  --enable-32bpp            support 32bpp [default yes]],,enable_32bpp=yes)
AC_MSG_CHECKING(whether to enable 32bpp)
if test x$enable_32bpp = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_32BPP, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(font10,
 [  --enable-font10            support 10 pixel font [default yes]],,enable_font10=yes)
AC_MSG_CHECKING(whether to enable 10 pixel font)
if test x$enable_font10 = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_FONT10, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(font12,
 [  --enable-font12            support 12 pixel font [default yes]],,enable_font12=yes)
AC_MSG_CHECKING(whether to enable 12 pixel font)
if test x$enable_font12 = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_FONT12, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(font14,
 [  --enable-font14            support 14 pixel font [default yes]],,enable_font14=yes)
AC_MSG_CHECKING(whether to enable 14 pixel font)
if test x$enable_font14 = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_FONT14, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(font16,
 [  --enable-font16            support 16 pixel font [default yes]],,enable_font16=yes)
AC_MSG_CHECKING(whether to enable 16 pixel font)
if test x$enable_font16 = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_FONT16, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(imj,
 [  --enable-imj              Enable Japanese-Input-Method],,enable_imj=yes)
AC_MSG_CHECKING(whether to enable Japanese-IM)
if test x$enable_imj = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_IMJ, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(screenshot,
 [  --enable-screenshot              Enable Screen shot],,enable_screenshot=no)
AC_MSG_CHECKING(whether to enable Screen shot)
if test x$enable_screenshot = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_SCREEN_SHOT, 1, [description])
	LIBS="$LIBS -lpng"
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(255columns,
 [  --enable-255columns              Enable more than 255 columns of screen size],,enable_255columns=no)
AC_MSG_CHECKING(whether to enable more than 255 columns)
if test x$enable_255columns = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_255COLUMNS, 1, [description])
else
	AC_MSG_RESULT(no)
fi

AC_ARG_ENABLE(dimmer,
 [  --enable-dimmer           Enable auto dimmer (condole blank) [default no]],,enable_dimmer=no)
AC_MSG_CHECKING(whether to enable dimmer)
if test x$enable_dimmer = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(ENABLE_DIMMER, 0, [description])
else
	AC_MSG_RESULT(no)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
