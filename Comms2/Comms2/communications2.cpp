#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string.h>
constexpr int PORT = 55555;
constexpr int MAX_BUFFER_SIZE = 200;
constexpr char DEST_IP_ADDRESS[] = "127.0.0.1"; // For now lets just use the local host address'
constexpr bool DEBUG = true;

int main() {

	using namespace std;

	WSADATA wsaData;

	int wsaerr; //return value 

	WORD wVersionRequested = MAKEWORD(2, 2);

	wsaerr = WSAStartup(wVersionRequested, &wsaData);


	if (wsaerr != 0) {
		if (DEBUG) {
			cout << "The Winsock dll not found!" << endl;
		}
		return 1;
	} else {
		if (DEBUG) {
			cout << "The Winsock dll found" << endl;
		}
	}

	SOCKET clientSocket;
	clientSocket = INVALID_SOCKET;

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for socket creation success
	if (clientSocket == INVALID_SOCKET) {
		if (DEBUG) {
			cout << "Error at socket(): " << WSAGetLastError() << endl;
		}
		WSACleanup();
		return 0;
	} else {
		if (DEBUG) {
			cout << "Socket is OK!" << endl;
		}
	}

	// Connect to the server
	sockaddr_in clientService;
	//protocol family, AF_INET is the address family for IPv4 addresses.
	clientService.sin_family = AF_INET;

	// The InetPton function converts an IP address in string format to a binary format. It takes three parameters: the address family (AF_INET for IPv4), the string representation of the IP address, and a pointer to a variable that will receive the binary representation of the IP address.
	int res = InetPtonA(AF_INET, DEST_IP_ADDRESS, &clientService.sin_addr.s_addr);

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

	clientService.sin_port = htons(PORT);  // Choose a port number

	//Connect to server.
	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService)) == SOCKET_ERROR) { 
		if (DEBUG) {
			cout << "Client: connect() - Failed to connect: " << WSAGetLastError() << endl;
		}
		WSACleanup();
		return 0;
	} else {
		if (DEBUG) {
			cout << "Client: Connect() is OK!" << endl;
			cout << "Client: Can start sending and receiving data..." << endl;
		}
	}

	bool exit_flag = false;
	while (1) {
		// Sending data to the server
		char buffer[MAX_BUFFER_SIZE];

		cout << "Enter the message: ";
		cin.getline(buffer, MAX_BUFFER_SIZE);

		//exit command
		if (strcmp(buffer, "\\exit") == 0) {
			exit_flag = true;
		}

		int sbyteCount = send(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
		if (sbyteCount == SOCKET_ERROR) {
			if (DEBUG) {
				cout << "Client send error: " << WSAGetLastError() << endl;
			}
			return -1;
		} else {
			if (DEBUG) {
				cout << "Client: Sent " << sbyteCount << " bytes" << endl;
			}
		}
		
		if (exit_flag) {
			break;
		}

		// Receiving data from the server
		char receiveBuffer[MAX_BUFFER_SIZE];
		int rbyteCount = recv(clientSocket, receiveBuffer, MAX_BUFFER_SIZE, 0);
		if (rbyteCount < 0) {
			if (DEBUG) {
				cout << "Client recv error: " << WSAGetLastError() << endl;
			}
			return 0;
		} else {
			cout << "Client: Received data: " << receiveBuffer << endl;
		}
	}

	shutdown(clientSocket, SD_SEND);
	closesocket(clientSocket);
	WSACleanup();

}