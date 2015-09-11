#ifndef _NOLIBC_H_
#define _NOLIBC_H_

#include <bmk-core/types.h>
#include <bmk-core/sched.h>

/* fake some POSIX types used in the compat system calls */
#define RUMP_HOST_NOT_POSIX

typedef int		clockid_t;
typedef unsigned int	socklen_t;
typedef int		timer_t;

struct timespec;
struct itimerspec;
struct sigevent;
struct sockaddr;

typedef void fd_set;
typedef void sigset_t;

struct pollfd {
	int	fd;		/* file descriptor */
	short	events;		/* events to look for */
	short	revents;	/* events returned */
};

/*
 * Testable events (may be specified in events field).
 */
#define	POLLIN		0x0001
#define	POLLPRI		0x0002
#define	POLLOUT		0x0004
#define	POLLRDNORM	0x0040
#define	POLLWRNORM	POLLOUT
#define	POLLRDBAND	0x0080
#define	POLLWRBAND	0x0100

/*
 * Non-testable events (may not be specified in events field).
 */
#define	POLLERR		0x0008
#define	POLLHUP		0x0010
#define	POLLNVAL	0x0020

#include <rump/rump_syscalls.h>

#endif /* _NOLIBC_H_ */
