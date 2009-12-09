#ifndef KCODE_RATNET_PLATFORM_DETECT_H
#define KCODE_RATNET_PLATFORM_DETECT_H

// auto detect platform
// http://gcc.gnu.org/onlinedocs/cpp/System_002dspecific-Predefined-Macros.html
#if WIN32
#define RATNET_WIN32
#elif defined(__LINUX__)
#define RATNET_LINUX
#elif defined(__FreeBSD__)	// http://www.freebsd.org/doc/en/books/porters-handbook/porting-versions.html
#define	RATNET_FREEBSD
#elif defined(__WHAT_COULD_BE_HERE__)
#define RATNET_OPENBSD
#endif

// platform-specific implementation
#if defined(RATNET_WIN32)
	#include "ratnet_win32.h"
#elif defined(RATNET_LINUX)
	#include "ratnet_linux.h"
#elif defined(RATNET_FREEBSD)
	#include "ratnet_freebsd.h"
#endif

#endif
