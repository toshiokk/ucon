/*****************************************************************************
	myselect.c
 *****************************************************************************/

#include "myselect.h"

#ifndef MAX
#define MAX(aa, bb)		(aa >= bb ? aa : bb)
#endif

int my_select_msec(int fd_read1, int fd_read2, int fd_write1, int fd_write2, int fd_status1, int fd_status2, int msec)
{
	return my_select(fd_read1, fd_read2, fd_write1, fd_write2, fd_status1, fd_status2, msec * 1000);
}
int my_select(int fd_read1, int fd_read2, int fd_write1, int fd_write2, int fd_status1, int fd_status2, int usec)
{
	fd_set fds_read;
	fd_set fds_write;
	fd_set fds_status;
	fd_set *fds_r = NULL;
	fd_set *fds_w = NULL;
	fd_set *fds_s = NULL;
	int max_fd = 0;
	struct timeval tv;
	int ret;

	if (fd_read1 >= 0 || fd_read2 >= 0) {
		fds_r = &fds_read;
		FD_ZERO(&fds_read);
		if (fd_read1 >= 0) {
			FD_SET(fd_read1, &fds_read);
			max_fd = MAX(max_fd, fd_read1);
		}
		if (fd_read2 >= 0) {
			FD_SET(fd_read2, &fds_read);
			max_fd = MAX(max_fd, fd_read2);
		}
	}
	if (fd_write1 >= 0 || fd_write2 >= 0) {
		fds_w = &fds_write;
		FD_ZERO(&fds_write);
		if (fd_write1 >= 0) {
			FD_SET(fd_write1, &fds_write);
			max_fd = MAX(max_fd, fd_write1);
		}
		if (fd_write2 >= 0) {
			FD_SET(fd_write2, &fds_write);
			max_fd = MAX(max_fd, fd_write2);
		}
	}
	if (fd_status1 >= 0 || fd_status2 >= 0) {
		fds_s = &fds_status;
		FD_ZERO(&fds_status);
		if (fd_status1 >= 0) {
			FD_SET(fd_status1, &fds_status);
			max_fd = MAX(max_fd, fd_status1);
		}
		if (fd_status2 >= 0) {
			FD_SET(fd_status2, &fds_status);
			max_fd = MAX(max_fd, fd_status2);
		}
	}
	tv.tv_sec = usec / 1000000;
	tv.tv_usec = usec % 1000000;

	ret = select(max_fd+1, fds_r, fds_w, fds_s, &tv);

	if (ret < 0 && errno != EINTR) {
		return ret;
	}
	if (ret == 0 || (ret < 0 && errno == EINTR)) {
		return MYSELECT_BIT_TIMEOUT;
	}
	ret = MYSELECT_BIT_TIMEOUT;
	if (fd_read1 >= 0) {
		if (FD_ISSET(fd_read1, &fds_read)) {
			ret |= MYSELECT_BIT_READ1;
		}
	}
	if (fd_read2 >= 0) {
		if (FD_ISSET(fd_read2, &fds_read)) {
			ret |= MYSELECT_BIT_READ2;
		}
	}
	if (fd_write1 >= 0) {
		if (FD_ISSET(fd_write1, &fds_write)) {
			ret |= MYSELECT_BIT_WRITE1;
		}
	}
	if (fd_write2 >= 0) {
		if (FD_ISSET(fd_write2, &fds_write)) {
			ret |= MYSELECT_BIT_WRITE2;
		}
	}
	if (fd_status1 >= 0) {
		if (FD_ISSET(fd_status1, &fds_status)) {
			ret |= MYSELECT_BIT_STATUS1;
		}
	}
	if (fd_status2 >= 0) {
		if (FD_ISSET(fd_status2, &fds_status)) {
			ret |= MYSELECT_BIT_STATUS2;
		}
	}
	return ret;
}

// End of myselect.c
