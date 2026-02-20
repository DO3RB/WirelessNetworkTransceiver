#include <sys/types.h>
#include <sys/stat.h>
#include <reent.h>
#include <errno.h>

#include "sam.h"
#include "fiber.h"

#include "tinyusb/device/usbd.h"

extern intptr_t __heap_base;
extern intptr_t __heap_limit;

#ifdef __cplusplus
extern "C" {
#endif

void* _sbrk_r (struct _reent* impure, ptrdiff_t incr)
{
	static caddr_t _break = NULL;
	if (_break == NULL) _break = (caddr_t) &__heap_base;
	caddr_t prev_break = _break;
	if (_break+incr < (caddr_t) &__heap_limit) {
		_break += incr;
		return prev_break;
	} else {
		impure->_errno = ENOMEM;
		return (caddr_t) -1;
	}
}

int _link_r (struct _reent* impure, const char* old, const char* new)
{
	(void) impure;
	(void) old;
	(void) new;
	return -1;
}

int _open_r (struct _reent* impure, const char* buf, int flags, int mode)
{
	(void) impure;
	(void) buf;
	(void) flags;
	(void) mode;
	errno = EIO;
	return -1;
}

int _close_r (struct _reent* impure, int fd)
{
	(void) impure;
	(void) fd;
	return 0;
}

int _fstat_r [[gnu::used]] (struct _reent* impure, int fd, struct stat* st)
{
	(void) impure;
	(void) fd;
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty_r [[gnu::used]] (struct _reent* impure, int fd)
{
	(void) impure;
	(void) fd;
	return 0; // newlib uses linebuffer if true
}

_off_t _lseek_r (struct _reent* impure, int fd, _off_t ptr, int dir)
{
	(void) impure;
	(void) fd;
	(void) ptr;
	(void) dir;
	errno = ESPIPE;
	return -1;
}

int _kill_r (struct _reent* impure, int pid, int sig)
{
	(void) impure;
	(void) pid;
	(void) sig;
	return 0;
}

int _getpid_r (struct _reent* impure)
{
	(void) impure;
	return -1;
}

void _exit (int status)
{
	extern void (*__fini_array []) (void);
	extern size_t __fini_count [];
	// execute active destructors in reverse order
	for (signed n = (signed) __fini_count-1; n >= 0; n--) {
		void(*fn)(void) = __fini_array[n]; if (fn) fn();
	}

	if (status == 0) { asm("BKPT #0"); while(1); }
	if (status  > 0) *((volatile uint32_t *)(HMCRAMC0_ADDR+HMCRAMC0_SIZE-4)) = 0xF01669EF;
	tud_disconnect();
	NVIC_SystemReset();
	__builtin_unreachable();
}

#include <limits.h>

int usleep (useconds_t usec)
{
	unsigned diff, time, mark;
	mark = REG_TC4_COUNT32_COUNT;
	usec = usec * 48;
	while (1) {
		fiber_yield();
		time = REG_TC4_COUNT32_COUNT;
		diff = time - mark;
		if (diff == 0) diff = UINT_MAX;
		if (diff < usec) {
			usec = usec - diff;
			mark = time;
		}
		else return 0;
	}
}

#ifdef __cplusplus
}
#endif
