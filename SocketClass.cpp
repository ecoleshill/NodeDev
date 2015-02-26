/*
Seneca College - School of Information and Communications Technology
Filename:  SocketClass.cpp
Author:    Dr. Elliott Coleshill
Purpose:   This is the implementation file for the SocketClass object
			defined in SocketClass.h.  It provides the detailed 
			implementation of the Node axiom distributed connections
*/

#include "SocketClass.h"

SocketConnection::SocketConnection(CONNECTIONTYPE cTypeToUse, SOCKTYPE sTypeToUse, string IPToUse, int PortToUse, string NameToUse)
{
	//Store object information
	cType = cTypeToUse;
	sType = sTypeToUse;
	IPAddr = IPToUse;
	Port = PortToUse;
	SocketName = NameToUse;

	ConnectionState = false;

	//Create the WinSock 2.0 connection
	wVersionRequested = MAKEWORD(2, 3);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		cerr << "SocketConnection:  Failed to startup WinSock 2.0" << endl;
}

bool SocketConnection::InitSocket()
{
	//Create Sockets
	if ((sType == TCP) && (cType == SERVER))
	{
		Welcome = socket(AF_INET, SOCK_STREAM, 0);
		if (Welcome == SOCKET_ERROR)
		{
			cerr << "SocketConnection:  Welcome Socket INVALID" << endl;
			return false;
		}

		ClientConnection = 0;			//Not used
	}
	else if ((sType == TCP) && (cType == CLIENT))
	{
		Welcome = 0;		//Not used

		ClientConnection = socket(AF_INET, SOCK_STREAM, 0);
		if(ClientConnection == INVALID_SOCKET)
		{
			cerr << "SocketConnection:  ClientConnection INVALID" << endl;
			return false;
		}
	}
	else if (sType == UDP)    //Don't need to check the cType as Server/Client does not matter in UDP
	{
		Welcome = 0;		//Not used
		ClientConnection = socket(AF_INET, SOCK_DGRAM, 0);
		if(ClientConnection == INVALID_SOCKET)
		{
			cerr << "SocketConnection:  ClientConnection INVALID" << endl;
			return false;
		}
	}
	else
	{
		cerr << "SocketConnection:  Invalid Socket Type Provided" << endl;		//should never hit this line
		return false;
	}

	return true;
}

SocketConnection::~SocketConnection()
{
	//Make sure all socket connections are closed
	if (Welcome != 0)
		closesocket(Welcome);

	if (ClientConnection != 0)
		closesocket(ClientConnection);

	//Kill the WinSock 2.0 connection
	WSACleanup();
}

bool SocketConnection::TCP_StartListeningSocket()
{
	//Only setup a listening port if the object is a SERVER type
	if ((cType == SERVER) && (sType == TCP))
	{
		//Populate sockaddr_in structure
		SvrAddr.sin_family = AF_INET;
		SvrAddr.sin_addr.s_addr = INADDR_ANY;		//allow any IP address connection
		SvrAddr.sin_port = htons(Port);

		//Bind the listening socket
		if (bind(Welcome, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
		{
			cerr << "TCP_StartListeningSocket:  bind = SOCKET_ERROR" << endl;
			return false;
		}

		//Configure for listening
		if (listen(Welcome, 1) == SOCKET_ERROR)
		{
			cerr << "TCP_StartListeningSocket:  listen = SOCKET_ERROR" << endl;
			return false;
		}

		return true;
	}
	else
	{
		cerr << "TCP_StartListeningSocket:  Trying to start a listening socket on CLIENT" << endl;
		return false;
	}
}

void SocketConnection::TCP_CloseListeningSocket()
{
	if (Welcome != 0)
	{
		closesocket(Welcome);
		Welcome = 0;
	}
}

bool SocketConnection::TCP_WaitForAccept()
{
	//only perform an accept call is the object is configured correctly
	if ((cType == SERVER) && (sType == TCP) && (Welcome != 0))
	{
		if ((ClientConnection = accept(Welcome, NULL, NULL)) == SOCKET_ERROR)
			cerr << "TCP_WaitForAccept:  Accept = SOCKET_ERROR" << endl;
		else
		{
			cout << "Client Connection Made" << endl;
			ConnectionState = true;
		}
	}

	return ConnectionState;
}
bool SocketConnection::TCP_ConnectClientSocket()
{
	//Only allow a TCP connection is the object is a Client and set to TCP
	if ((cType == CLIENT) && (sType == TCP))
	{
		//Configure the sockaddr_in structure
		CltAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
		CltAddr.sin_family = AF_INET;
		CltAddr.sin_port = htons(Port);

		if (connect(ClientConnection, (struct sockaddr *)&CltAddr, sizeof(CltAddr)) == SOCKET_ERROR)
		{
			cerr << "TCP_ConnectClientSocket:  Connect = SOCKET_ERROR" << endl;
			return false;
		}
	}
	else
	{
		cerr << "TCP_ConnectClientSocket: Cannot connect TCP Client Socket if SERVER" << endl;
		return false;
	}

	ConnectionState = true;
	return true;
}

bool SocketConnection::UDP_BindClientSocket()
{
	//only perform this task if the sType is UDP
	if (sType == UDP)
	{
		//Configure the sockaddr_in structure
		SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
		SvrAddr.sin_family = AF_INET;
		SvrAddr.sin_port = htons(Port);

		if (bind(ClientConnection, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
		{
			cerr << "UDP_BindClientSocket:  bind = SOCKET_ERROR" << endl;
			return false;
		}
		else
			return true;
	}
	else
	{
		cerr << "UDP_BindClientSocket:  bind = SOCKET_ERROR" << endl;
		return false;
	}
}

void SocketConnection::CloseClientSocket()
{
	if (ClientConnection != 0)
	{
		closesocket(ClientConnection);
		ClientConnection = 0;
		ConnectionState = false;
	}
}

int SocketConnection::TxData(char *Data, int DataSize)
{
	int TxBytes = 0;			//Number of bytes transmitted

	if (sType == TCP)
		TxBytes = send(ClientConnection, Data, DataSize, 0);
	else if ((cType == CLIENT) && (sType == UDP))
	{
		//Configure the sockaddr_in structure
		CltAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
		CltAddr.sin_family = AF_INET;
		CltAddr.sin_port = htons(Port);

		int len = sizeof(CltAddr);
		TxBytes = sendto(ClientConnection, Data, DataSize, 0, (struct sockaddr *)&CltAddr, len);
	}
	else if ((cType == SERVER) && (sType == UDP))
	{
		int len = sizeof(CltAddr);
		TxBytes = sendto(ClientConnection, Data, DataSize, 0, (struct sockaddr *)&CltAddr, len);
	}
	else
		TxBytes = 0;		//No data transmitted

	return TxBytes;
}

int SocketConnection::RxData(char *Data, int DataSize)
{
	int RxBytes = 0;			//Number of bytes received

	if (sType == TCP)
	{
		RxBytes = recv(ClientConnection, Data, DataSize, 0);
	}
	else if (sType == UDP)
	{
		int len = sizeof(CltAddr);
		RxBytes = recvfrom(ClientConnection, Data, DataSize, 0, (struct sockaddr *)&CltAddr, &len);
	}
	else
		cerr << "RxData:  Invalid sType" << endl;    //Should never hit this.

	return RxBytes;
}