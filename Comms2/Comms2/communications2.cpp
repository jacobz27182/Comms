#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
constexpr int PORT = 55555;
constexpr int MAX_BUFFER_SIZE = 200;
constexpr char DEST_IP_ADDRESS[] = "127.0.0.1"; // For now lets just use the local host address

int main() {

	using namespace std;

	WSADATA wsaData;

	int wsaerr; //return value 

	WORD wVersionRequested = MAKEWORD(2, 2);

	wsaerr = WSAStartup(wVersionRequested, &wsaData);


	if (wsaerr != 0) {
		cout << "The Winsock dll not found!" << std::endl;
		return 1;
	}
	else {
		cout << "The Winsock dll found" << std::endl;
	}

	SOCKET clientSocket;
	clientSocket = INVALID_SOCKET;

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for socket creation success
	if (clientSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Socket is OK!" << std::endl;
	}

	// Connect to the server
	sockaddr_in clientService;
	//protocol family, AF_INET is the address family for IPv4 addresses.
	clientService.sin_family = AF_INET;

	// The InetPton function converts an IP address in string format to a binary format. It takes three parameters: the address family (AF_INET for IPv4), the string representation of the IP address, and a pointer to a variable that will receive the binary representation of the IP address.
	int res = InetPtonA(AF_INET, DEST_IP_ADDRESS, &clientService.sin_addr.s_addr);

	if (res == 0) {
		std::cout << "Invalid IP address format!" << std::endl;
		WSACleanup();
		return 0;
	}
	else if (res == -1) {
		std::cout << "InetPton failed: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "IP address is OK!" << std::endl;
	}

	clientService.sin_port = htons(PORT);  // Choose a port number

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService)) == SOCKET_ERROR) { 
		std::cout << "Client: connect() - Failed to connect: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Client: Connect() is OK!" << std::endl;
		std::cout << "Client: Can start sending and receiving data..." << std::endl;
	}

	// Sending data to the server
	char buffer[MAX_BUFFER_SIZE];

	std::cout << "Enter the message: ";
	std::cin.getline(buffer, MAX_BUFFER_SIZE);

	int sbyteCount = send(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
	if (sbyteCount == SOCKET_ERROR) {
		std::cout << "Client send error: " << WSAGetLastError() << std::endl;
		return -1;
	}
	else {
		std::cout << "Client: Sent " << sbyteCount << " bytes" << std::endl;
	}

	// Receiving data from the server
	char receiveBuffer[MAX_BUFFER_SIZE];
	int rbyteCount = recv(clientSocket, receiveBuffer, MAX_BUFFER_SIZE, 0);
	if (rbyteCount < 0) {
		std::cout << "Client recv error: " << WSAGetLastError() << std::endl;
		return 0;
	}
	else {
		std::cout << "Client: Received data: " << receiveBuffer << std::endl;
	}

}