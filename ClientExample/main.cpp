#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <fstream>

// link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015" 
#define DEFAULT_BUFFER_LENGTH	2048


using namespace std;

class Client {
public:
	Client(char * servername)
	{
		szServerName = servername;
		ConnectSocket = INVALID_SOCKET;
	}

	bool Start() {
		WSADATA wsaData;

		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if(iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			return false;
		}

		struct addrinfo	*result = NULL,
						*ptr = NULL,
						hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;		
		hints.ai_socktype = SOCK_STREAM;	
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(szServerName, DEFAULT_PORT, &hints, &result);
		if (iResult != 0)
		{
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return false;
		}

		ptr = result;

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Error at socket(): %d\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return false;
		}

		// Connect to server
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Unable to connect to server!\n");
			WSACleanup();
			return false;
		}

		return true;
	};

	// Free the resouces
	void Stop() {
		int iResult = shutdown(ConnectSocket, SD_SEND);

		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
		}

		closesocket(ConnectSocket);
		WSACleanup();
	};

	// Send message to server
	bool Send(char* szMsg)
	{
		
		int iResult = send(ConnectSocket, szMsg, strlen(szMsg), 0);

		if (iResult == SOCKET_ERROR)
		{
			printf("send failed: %d\n", WSAGetLastError());
			Stop();
			return false;
		}

		return true;
	};

	// Receive message from server
	bool Recv()
	{
		char recvbuf[DEFAULT_BUFFER_LENGTH];
		int iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFFER_LENGTH, 0);

		if (iResult > 0)
		{
			char msg[DEFAULT_BUFFER_LENGTH];
			memset(&msg, 0, sizeof(msg));
			strncpy(msg, recvbuf, iResult);
			printf("Received: %s\n", msg);
			ofstream fout("output");
			string output(msg);
			fout<<msg;

			return true;
		}


		return false;
	}

private:
	char* szServerName;
	SOCKET ConnectSocket;
};


int main(int argc, CHAR* argv[]) 
{
	string p,l;
	p=std::string(argv[1]);
	if (argc>2)
	{
	  l=std::string(argv[2]);
	  p+=l;
	}

	string ip;
	ifstream fin("conf.txt");
	std::getline(fin,ip);
	Client client(const_cast<char*>(ip.c_str()));
	if (!client.Start())
		return 1;


	if (p.substr(0,6)=="search")
	{
		client.Send((char*)p.c_str());
		client.Recv();
	} 
	else if ((p.substr(0,3)=="del") || (p.substr(0,3)=="upd")|| (p.substr(0,4)=="quit"))
	{
		cout<<p<<endl;
		client.Send((char*)p.c_str());
	}
	client.Stop();
	//getchar();
	return 0;
}