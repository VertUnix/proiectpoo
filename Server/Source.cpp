#include <iostream>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main()
{

	//initializez Winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Nu pot initializa Winsock! Inchid program..." << "\n";
		return;
	}

	//creez un socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Nu pot crea un socket! Inchidere program..." << "/n";
		return;
	}

	//leg (Bind) IP-ul si portul de un socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //poate folosi de asemenea inet_pton...

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	
	//spun Winsock ca socket-ul e pt ascultare (listening)
	listen(listening, SOMAXCONN);

	//asteapta comexiunea
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	///TODO: if (clientSocket == INVALID_SOCKET) //trateaza eroarea

	char host[NI_MAXHOST]; //numele clientului
	char service[NI_MAXHOST]; //serviciul (i.e. portul) pa care clientul e conectat

	ZeroMemory(host, NI_MAXHOST); //acelasi lucru cu memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);
	
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		cout << host << "conectat la portul " << service << "\n";

	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " conectat la portul " <<
			ntohs(client.sin_port) << "\n";
	}


	//inchid socket-ul de ascultare
	closesocket(listening);


	//while: accepta conexiunea si trimite mesaj inapoi clientului
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);
		//astept clientul sa trimita date
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << "\n";
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected" << "\n";
			break;
		}

		send(clientSocket, buf, bytesReceived + 1, 0);
		//trimite mesaj inapoi clientului
	}

	//inchid socket-ul
	closesocket(clientSocket);


	//inchid winsock
	WSACleanup();
}