/*****************************************************************************
	myselect.h
 *****************************************************************************/

#ifndef myselect_h
#define myselect_h

#include <stdlib.h>
#include <sys/select.h>

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define MYSELECT_BIT_ERROR		-1
#define MYSELECT_BIT_TIMEOUT	0x0000
#define MYSELECT_BIT_READ1		0x0001
#define MYSELECT_BIT_READ2		0x0002
#define MYSELECT_BIT_WRITE1		0x0010
#define MYSELECT_BIT_WRITE2		0x0020
#define MYSELECT_BIT_STATUS1	0x0100
#define MYSELECT_BIT_STATUS2	0x0200

int my_select_msec(int fd_read1, int fd_read2, int fd_write1, int fd_write2, int fd_status1, int fd_status2, int msec);
int my_select(int fd_read1, int fd_read2, int fd_write1, int fd_write2, int fd_status1, int fd_status2, int usec);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // myselect_h

// End of myselect.h
