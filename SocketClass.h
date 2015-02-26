/*
Seneca College - School of Information and Communications Technology
Filename:  SocketClass.h
Author:    Dr. Elliott Coleshill
Purpose:   This header contains the definition for a SocketConnection
			class that will be used to create a distributed axiom
			connection between nodes.  Allowing for a distributed
			artificial network connection
*/

#ifndef __SOCKET_CLASS__
#define __SOCKET_CLASS__

//#pragma comment(lib, "Ws2_32.lib")	//Connect the windows WINSOCK library

const enum SOCKTYPE { TCP, UDP };
const enum CONNECTIONTYPE {SERVER, CLIENT};

#include <iostream>
#include <windows.networking.sockets.h>
using namespace std;

class SocketConnection
{
	//WinSock Information
	WORD wVersionRequested;
	WSADATA wsaData;

	//Connection information
	string IPAddr;						//IP Address for the given connection
	int Port;							//Socket port number
	struct sockaddr_in SvrAddr;				//Server socket address information
	struct sockaddr_in CltAddr;				//Client socket address information
	SOCKTYPE sType;						//Socket type TCP or UDP
	CONNECTIONTYPE cType;				//Conenction type SERVER or CLIENT
	SOCKET Welcome;						//Welcome socket used for listening for incoming connections (if TCP)
	SOCKET ClientConnection;			//Vector space holding all currrently active socket connections
	bool ConnectionState;				//Flag to determine if a client connection has been made or not (SERVER)

	//General Information
	string SocketName;					//SocketName

public:
	//Configuration Functions
	SocketConnection(CONNECTIONTYPE cTypeToUse, SOCKTYPE sTypeToUse, string IPToUse, int PortToUse, string NameToUse);
	~SocketConnection();
	bool InitSocket();					//Initialize socket variables

	//TCP Support Functions
	bool TCP_StartListeningSocket();	//SERVER only
	void TCP_CloseListeningSocket();	//SERVER only
	bool TCP_WaitForAccept();			//SERVER only

	bool TCP_ConnectClientSocket();		//CLIENT only

	//UDP Support Functions
	bool UDP_BindClientSocket();		//Binds a UDP client socket
	void setIP(string IPToUse) { IPAddr = IPToUse; };			//Resets the Objects IP Address
	void setPort(int PortToUse) { Port = PortToUse; };			//Resets the Objects Port Number
	string getIP() { return IPAddr; };							//Returns the currently set IP address
	int getPort() { return Port; };								//Returns the currently set Port number

	//TCP & UDP Support Functions
	void CloseClientSocket();			//Closes the Client Connectios
	
	//Data Communication Functions
	int TxData(char *Data, int DataSize);	//Sends data out the socket connection
	int RxData(char *Data, int DataSize);	//Gets data from the internal Rx buffer
};


#endif