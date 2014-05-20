#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "Document.h"
#include <string>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LENGTH	2048
Document document;

int main() {
	WSADATA wsaData;
	
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	struct addrinfo	*result = NULL,hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;		// Internet address family is unspecified so that either an IPv6 or IPv4 address can be returned
	hints.ai_socktype = SOCK_STREAM;	// Requests the socket type to be a stream socket for the TCP protocol
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	// Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed: %d", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	// To listen on a socket
	if ( listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket;


	do
	{
	ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	char recvbuf[DEFAULT_BUFFER_LENGTH];
	int iSendResult;

	// reveice until the client shutdown the connection
	do {
		iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFFER_LENGTH, 0);
		if (iResult > 0)
		{
			char msg[DEFAULT_BUFFER_LENGTH];
			memset(&msg, 0, sizeof(msg));
			strncpy(msg, recvbuf, iResult);

			printf("Received: %s\n", msg);
			std::string q(msg);

			if (q.substr(0,4)=="quit")
			{
				printf("I am leaving\n");
				return 0;
			}

			//std::cout<<q.substr(0,6)<<std::endl;
			if (q.substr(0,3)=="upd")
			{
				document.entry(q.substr(0,q.size()-3),q.substr(q.size()-3,3));
			}
			else if (q.substr(0,3)=="del")
			{
				document.entry(q);
			}
			else if (q.substr(0,6)=="search")
			{
				std::cout<<"Begin to entry"<<std::endl;
				std::string res=document.entry(q.substr(0,q.size()-3),q.substr(q.size()-3,3));				
				iSendResult = send(ClientSocket, res.c_str(), res.size(), 0);
				printf("Bytes sent: %ld\n", iSendResult);
			}
			/*
			if (iSendResult == SOCKET_ERROR)
			{
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}			
			*/
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
		{
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();	
			//return 1;
		}
	} while (iResult>0);
	}
	while (1);

	// Free the resouces
	closesocket(ListenSocket);
	WSACleanup();

	getchar();
	return 0;
}