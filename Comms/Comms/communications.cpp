#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

constexpr int PORT = 55555;


int main() {
	using namespace std;

	// Initialize WSA variables
	// WSAdata structure contains information about the Windows Sockets implementation (it is an output)
	WSADATA wsaData;

	int wsaerr; //return value 

	//WORD is a 16-bit unsigned integer type used to represent a word in Windows programming. 
	//The MAKEWORD macro is used to create a WORD value from two 8-bit values (the low-order byte and the high-order byte). 
	//In this case, it is used to specify the version of Winsock that the application wants to use (version 2.2).

	WORD wVersionRequested = MAKEWORD(2, 2);

	//LP = long pointer
	wsaerr = WSAStartup(wVersionRequested, &wsaData);


	if (wsaerr != 0) {
		cout << "The Winsock dll not found!" << std::endl;
		return 1;
	} else {
		cout << "The Winsock dll found" << std::endl;
		//cout << "The status: " << wsaData.szSystemStatus << std::endl;
	}

	// Create a socket
	//SOCKET is a typedef for an unsigned integer type that represents a socket descriptor in the Windows Sockets API.
	SOCKET serverSocket;
	serverSocket = INVALID_SOCKET;

	//af = address family, AF_INET is the address family for IPv4 addresses.
	//use SOCK_STREAM for TCP sockets and SOCK_DGRAM for UDP sockets.
	//IPPROTO_TCP is the protocol to be used with the socket, in this case TCP.
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//TCP = Transmission Control Protocol 


	// Check for socket creation success
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Socket is OK!" << std::endl;
	}
}	
