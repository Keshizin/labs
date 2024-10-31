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
#define DEFAULT_BUFFFER_SIZE 512

int main()
{
	std::cout << "> STARTING WIN32 Client" << std::endl;

	/*
		INITIALIZING WINDOWS SOCKET DLL

		All processes (applications or DLLs) that call Winsock functions must initialize the use of the Windows Sockets DLL before making other Winsock functions calls. This also makes certain that Winsock is supported on the system.
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

	std::string serverIP = "localhost";

	ret = getaddrinfo(serverIP.c_str(), DEFAULT_PORT, &hints, &result);

	if (ret != 0) {
		std::cout << "> (!) getaddrinfo failed: " << ret << std::endl;
		WSACleanup();
		return 1;
	}

	/*
		Creating an SOCKET
	*/

	SOCKET connectSocket = INVALID_SOCKET;

	// Creates a socket that is bound to a specific transport service provider.
	connectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (connectSocket == INVALID_SOCKET) {
		std::cout << "> (!) Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	/*
		Connecting to server
	*/

	std::cout << "> Connecting to server..." << std::endl;

	// establishes a connection to a specified socket.
	ret = connect(connectSocket, result->ai_addr, (int)result->ai_addrlen);

	if (ret == SOCKET_ERROR) {
		std::cout << "> (!) Error to connect to server: " << WSAGetLastError() << std::endl;
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	std::wcout << "> Server connected!" << std::endl;

	freeaddrinfo(result);

	/*
		Sending and Receiving packets
	*/
	std::string message = "hello im client";

	ret = send(connectSocket, message.c_str(), message.length(), 0);

	if (ret == SOCKET_ERROR) {
		std::cout << "> (!) send failed: " << WSAGetLastError() << std::endl;
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "> bytes sent: " << ret << std::endl;

	ret = shutdown(connectSocket, SD_SEND);

	if (ret == SOCKET_ERROR) {
		std::cout << "> shutdown failed: " << WSAGetLastError() << std::endl;
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	/*
		Receive data until the server closes the connection
	*/

	char recvBuffer[DEFAULT_BUFFFER_SIZE];

	do {
		ret = recv(connectSocket, recvBuffer, DEFAULT_BUFFFER_SIZE, 0);

		if (ret > 0) {
			std::cout << "> bytes received: " << ret << std::endl;
			std::string recvMsg(recvBuffer, ret);
			std::cout << "> bytes received: " << recvMsg << std::endl;
		}

		else if (ret == 0)
			std::cout << "> The connection was closed: " << ret << std::endl;

		else {
			std::cout << "> recv failed: " << WSAGetLastError() << std::endl;
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}
			
	} while (ret > 0);




	/*
		Cleaning and Finishing
	*/

	closesocket(connectSocket);

	// The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll)
	ret = WSACleanup();

	if (ret != 0) {
		std::cout << "> (!) WSACleanup failed: " << ret << std::endl;
		return 1;
	}

	std::cout << "> ENDING WIN32 Server" << std::endl;
	return 0;
}