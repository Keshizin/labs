/*
	Notes for building the winsock experiment!

		Linker:
			- Ws2_32.lib (#pragma comment(lib, "Ws2_32.lib"))
*/

/*
	Including Winsock 2 header files

	- The "WinSock2.h" header is different from "Winsock.h" header included by "Windows.h"

	The Winsock2.h header file internally includes core elements from the Windows.h header file,
	so there is not usually an #include line for the Windows.h header file in Winsock applications.

	- The Ws2tcpip.h header file contains definitions introduced in the WinSock 2 Protocol-Specific Annex
	  document for TCP/IP that includes newer functions and structures used to retrieve IP addresses.
*/


#include <iostream>

//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif
//#include <windows.h>

#include <WinSock2.h>
#include <ws2tcpip.h>

// #pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

int main()
{
	std::cout << "> STARTING WIN32 Server" << std::endl;

	/*
		INITIALIZING WINDOWS SOCKET DLL

		All processes (applications or DLLs) that call Winsock functions must initialize
		the use of the Windows Sockets DLL before making other Winsock functions calls.
		This also makes certain that Winsock is supported on the system.
	*/

	// The WSADATA structure contains information about the Windows Sockets implementation.
	WSADATA wsaData;
	
	// The WSAStartup function initiates use of the Winsock DLL by a process.
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (ret != 0) {
		std::cout << "> (!) WSAStartup failed: " << ret << std::endl;
		return 1;
	}

	std::cout << "> Windows socket information:"
		<< "\n\t - version: " << static_cast<int>(LOBYTE(wsaData.wVersion)) << "." << static_cast<int>(HIBYTE(wsaData.wVersion))
		<< "\n\t - highest version: " << static_cast<int>(LOBYTE(wsaData.wHighVersion)) << "." << static_cast<int>(HIBYTE(wsaData.wHighVersion))
		<< "\n\t - " << wsaData.szDescription
		<< "\n\t - " << wsaData.szSystemStatus
		<< "\n" << std::endl;


	/*
		Setting up the network structure

		addrinfo reference:
		https://learn.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa

		socktype:
			- SOCK_STREAM (TCP)
			- SOCK_DGRAM (UDP)
			- SOCK_RAW
			- SOCK_RDM
			- SOCK_SEQPACKET
	*/

	

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET; // IPV4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_protocol = IPPROTO_TCP; // TCP
	hints.ai_flags = AI_PASSIVE;

	ret = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (ret != 0) {
		std::cout << "> (!) getaddrinfo failed: " << ret << std::endl;
		WSACleanup();
		return 1;
	}


	/*
		Creating an SOCKET
	*/

	SOCKET listenSocket = INVALID_SOCKET;

	// The socket function creates a socket that is bound to a specific transport service provider.
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		std::cout << "> (!) Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	/*
		Binding the SOCKET
	*/

	// The bind function associates a local address with a socket.
	ret = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (ret == SOCKET_ERROR) {
		std::cout << "> (!) Bind failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);


	/*
		Listening on a socket
	*/

	// the listen function places a socket in a state in which it is listening for an incoming connection
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


	/*
		Accepting a connection	
	*/
	SOCKET clientSocket = INVALID_SOCKET;

	// Accept a client socket
	clientSocket = accept(listenSocket, NULL, NULL);

	if (clientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// pass this client socket to a worker thread or an I/O completion port and continue accepting additional connections

	/*
		Cleaning and Finishing
	*/

	closesocket(listenSocket);

	// The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll)
	ret = WSACleanup();

	if (ret != 0) {
		std::cout << "> (!) WSACleanup failed: " << ret << std::endl;
		return 1;
	}

	std::cout << "> ENDING WIN32 Server" << std::endl;
	return 0;
}