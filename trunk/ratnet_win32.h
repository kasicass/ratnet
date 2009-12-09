#ifndef KCODE_RATNET_WIN32_H
#define KCODE_RATNET_WIN32_H
#ifdef RATNET_WIN32


#ifdef __cplusplus
extern "C" {
#endif

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")


typedef SOCKET RNET_socket;

#define	RNET_errno		(WSAGetLastError())

// error code
#define	RNET_EAGAIN		WSAEWOULDBLOCK
#define	RNET_EINTR		WSAEINTR


#ifdef __cplusplus
}
#endif


#endif
#endif // CODEMATE_RATNET_WIN32_H

