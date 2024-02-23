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

#include "stdincs.h"
#include "util.h"

long get_page_size()
{
	long page_size = sysconf(_SC_PAGESIZE);
	return page_size;
}

int util_privilege_open(const char *pathname, int flags)
{
	int fd;

	util_privilege_raise();
	fd = open(pathname, flags);
	util_privilege_lower();
	return fd;
}
int util_privilege_ioctl(int fd, int request, void *ptr)
{
	int ret;

	util_privilege_raise();
	ret = ioctl(fd, request, ptr);
	util_privilege_lower();
	return ret;
}

static uid_t runner_uid;
static uid_t owner_uid;

void util_privilege_init()
{
	runner_uid = getuid();
	owner_uid = geteuid();	/* if setuid, owner_uid is root */
///flf_d_printf("real:%d effective:%d\n", runner_uid, owner_uid);
	/* swap real and effective */
	util_privilege_lower();
}

uid_t util_getuid()
{
	return runner_uid;
}

void util_privilege_raise()
{
	if (setreuid(runner_uid, owner_uid) < 0) {
		perror("setreuid()");
	}
}

void util_privilege_lower()
{
	if (setreuid(owner_uid, runner_uid) < 0) {
		perror("setreuid()");
	}
}

void util_privilege_drop()
{
	if (setreuid(runner_uid, runner_uid) < 0) {
		perror("setreuid()");
	}
}

//-----------------------------------------------------------------------------

#ifdef __x86_64__
int32_t util_time32(int32_t *time32)
#else
long util_time32(long *time32)
#endif
{
	time_t time_;
	time_t time_ret;

	time_ret = time(&time_);
	*time32 = time_;
	return time_ret;
}

// End of util.c
