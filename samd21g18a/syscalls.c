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

__attribute__((used))
int _fstat_r (struct _reent* impure, int fd, struct stat* st)
{
	(void) impure;
	(void) fd;
	st->st_mode = S_IFCHR;
	return 0;
}

__attribute__((used))
int _isatty_r (struct _reent* impure, int fd)
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
	if (status == 0) { asm("BKPT #0"); while(1); }
	if (status  > 0) *((volatile uint32_t *)(HMCRAMC0_ADDR+HMCRAMC0_SIZE-4)) = 0xF01669EF;
	tud_disconnect();
	NVIC_SystemReset();
	__builtin_unreachable();
}

int usleep (useconds_t usec)
{
	unsigned diff, time, mark;
	mark = SysTick->VAL;
	usec = usec * 48;
	while (1) {
		fiber_yield();
		time = SysTick->VAL; // systick counts downward
		diff = (mark - time) & SysTick_VAL_CURRENT_Msk;
		if (diff == 0) diff  = SysTick_LOAD_RELOAD_Msk;
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
