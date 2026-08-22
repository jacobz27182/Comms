#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string.h>

constexpr int PORT = 55555;
constexpr char DEST_IP_ADDRESS[] = "127.0.0.1"; // For now lets just use the local host address
constexpr int MAX_BUFFER_SIZE = 200;
constexpr bool DEBUG = true;


//const = once it is assigned a value, it cannot be changed. It is a way to create a constant variable that cannot be modified after initialization.
//constexpr = the compiler has to know the value of the variable at compile time. It is a way to create a constant variable that can be used in compile-time expressions.

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
		if (DEBUG) {
			cout << "The Winsock dll not found!" << endl;
		}
		return 1;
	} else {
		if (DEBUG) {
			cout << "The Winsock dll found!" << endl;
		}
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
		if (DEBUG) {
			cout << "Error at socket(): " << WSAGetLastError() << endl;
		}
		WSACleanup();
		return 0;
	} else {
		if (DEBUG) {
			cout << "socket() is OK!" << endl;
		}
	}

	// Bind the socket to an IP address and port number

	//sockaddr_in is a structure that contains an IPV4 address. It is used to specify the address and port number for a socket.
	sockaddr_in service;

	//protocol family, AF_INET is the address family for IPv4 addresses.
	service.sin_family = AF_INET;

	// The InetPton function converts an IP address in string format to a binary format. It takes three parameters: the address family (AF_INET for IPv4), the string representation of the IP address, and a pointer to a variable that will receive the binary representation of the IP address.
	int res = InetPtonA(AF_INET, DEST_IP_ADDRESS, &service.sin_addr.s_addr);

	if (res == 0) {
		if (DEBUG) {
			cout << "Invalid IP address format!" << endl;
		}
		WSACleanup();
		return 0;
	} else if (res == -1) {
		if (DEBUG) {
			cout << "InetPton failed: " << WSAGetLastError() << endl;
		}
		WSACleanup();
		return 0;
	} else {
		if (DEBUG) {
			cout << "IP address is OK!" << endl;
		}
	}

	//htons is a function that converts a 16-bit number from host byte order to network byte order.
	service.sin_port = htons(PORT);  // Choose a port number

	// Use the bind function
	if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
		if (DEBUG) {
			cout << "bind() failed: " << WSAGetLastError() << endl;
		}
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	} else {
		if (DEBUG) {
			cout << "bind() is OK!" << endl;
		}
	}

	// Listen allows the socket to accept incoming connections (TCP handshakes). The second parameter specifies the maximum length of the queue of pending connections.
	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		if (DEBUG) {
			cout << "listen(): Error listening on socket: " << WSAGetLastError() << endl;
		}
	} else {
		cout << "Awaiting new connection..." << endl;
	}


	// Accept incoming connections. 
	//accept() is a blocking function that waits for an incoming connection request. When a client connects, it returns a new socket descriptor for the accepted connection.
	//Think of it analagously to a serial read() function that blocks until data is available to read. In this case, the accept() function blocks until a client connects to the server socket.
	//Each accepted connection is assigned a new socket descriptor, which can be used to communicate with the client. The original server socket remains open and can continue to accept new connections.
	SOCKET acceptSocket;
	acceptSocket = accept(serverSocket, nullptr, nullptr);

	// Check for successful connection
	if (acceptSocket == INVALID_SOCKET) {
		if (DEBUG) {
			cout << "accept failed: " << WSAGetLastError() << endl;
		}
		WSACleanup();
		return -1;
	} else {
		if (DEBUG) {
			cout << "accept() is OK!" << endl;
		}
	}


	while (1) {
		// Receive data from the client
		char receiveBuffer[MAX_BUFFER_SIZE];
		int rbyteCount = recv(acceptSocket, receiveBuffer, MAX_BUFFER_SIZE, 0);

		if (rbyteCount < 0) {
			int errorCode = WSAGetLastError();
			if (errorCode == WSAECONNABORTED) { //This is our backup option
				if (DEBUG) {
					cout << "Client disconnected." << endl;
				}
				break; // Exit the loop if the client disconnected
			}
			else
			if (DEBUG) {
				cout << "Server recv error: " << errorCode << endl;
			}
			return 0;
		} else {
			if (DEBUG) {
				cout << "Server: Received " << rbyteCount << " bytes" << endl;
			}
			cout << "Received data: " << receiveBuffer << endl;
		}

		if (strcmp(receiveBuffer, "\\exit") == 0) {
			cout << "Client requested to exit." << endl;
			break; // Exit the loop if the client sent the exit command
		}

		// Send a response to the client
		char buffer[MAX_BUFFER_SIZE];
		cout << "Enter the message: ";
		cin.getline(buffer, MAX_BUFFER_SIZE);

		//exit command
		if (strcmp(buffer, "\\exit") == 0) {
			break;
		}

		int sbyteCount = send(acceptSocket, buffer, MAX_BUFFER_SIZE, 0);

		if (sbyteCount == SOCKET_ERROR) {
			if (DEBUG) {
				cout << "Server send error: " << WSAGetLastError() << endl;
			}
			return -1;
		} else {
			if (DEBUG) {
				cout << "Server: Sent " << sbyteCount << " bytes" << endl;
			}
		}
	}
	

}	
