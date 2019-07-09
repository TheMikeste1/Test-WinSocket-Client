/*****************************************************************************
* Author:
*    Michael Hegerhorst
* Summary:
*
*
******************************************************************************/
/*****************************************************************************
 * Help from
 * https://docs.microsoft.com/en-us/windows/desktop/winsock/about-clients-and-servers
 *
 * SERVER STEPS
 *  1. Initialize Winsock.
 *  2. Create a socket.
 *  3. Connect to the server.
 *  4. Send and receive data.
 *  5. Disconnect.
 *
 *****************************************************************************/
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

#define DEFAULT_PORT "6789"
#define DEFAULT_BUFLEN 512

/*****************************************************************************
 * main()
 * This function pilots the program.
 *****************************************************************************/
int main(int argc, char *argv[])
{
	//CLIENT SETUP
	// 1. Initialize Winsock.
	//https://docs.microsoft.com/en-us/windows/desktop/winsock/creating-a-basic-winsock-application
	WSADATA wsaData;

    int resultCode;
	if ((resultCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		cout << "WSAStartup failed: " << resultCode << endl;
		return 1;
	}

	// 2. Create a socket.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
	struct addrinfo *servinfo = NULL;
	struct addrinfo hints;

	PCSTR port = DEFAULT_PORT; //PCSTR = const char* (Pointer Constant STRing)

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	string servIP;

	if (argc == 1)
	{
		cout << "Please enter an IP: ";
		cin >> servIP;
	}
	else
		servIP = argv[1];


	if ((resultCode = getaddrinfo(servIP.c_str(), port, &hints, &servinfo) != 0))
	{
		printf("getaddrinfo failed: %d\n", resultCode);
	    WSACleanup();
	    return 1;
	}

	SOCKET ConnectSocket = socket(servinfo->ai_family, 
								servinfo->ai_socktype, 
									servinfo->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) 
	{
	    printf("Error at socket(): %ld\n", WSAGetLastError());
	    freeaddrinfo(servinfo);
	    WSACleanup();
	    return 1;
	}

	// 3. Connect to the server.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/connecting-to-a-socket
	if ((resultCode = connect(ConnectSocket, servinfo->ai_addr, (int)servinfo->ai_addrlen)) == SOCKET_ERROR)
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(servinfo);

	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Unable to connect to server!\n";
		WSACleanup();
		return 1;
	}

	// 4. Send and receive data.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/sending-and-receiving-data-on-the-client
	string input;
	cout << "Enter input:\n> ";
	cin.ignore(256, '\n');
	getline(cin, input);
	while (input != "q")
	{
		if ((resultCode = send(ConnectSocket, input.c_str(), input.size(), 0))
			== SOCKET_ERROR)
		{
			printf("send failed: %d\n", WSAGetLastError());
		    closesocket(ConnectSocket);
		    WSACleanup();
		    return 1;	
		}

		cout << "> ";
		getline(cin, input);
	} 

	// 5. Disconnect.
	//https://docs.microsoft.com/en-us/windows/win32/winsock/disconnecting-the-client
	if ((resultCode = shutdown(ConnectSocket, SD_SEND)) == SOCKET_ERROR) 
	{
	    printf("shutdown failed: %d\n", WSAGetLastError());
	    closesocket(ConnectSocket);
	    WSACleanup();
	    return 1;
	}

	//Cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}