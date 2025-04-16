
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <ostream>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")


struct addrinfo
*result = NULL,
*ptr = NULL,
hints;



int main(int argc, char* argv[])
{
// FOR SERVER 
    // initialise winsock
    WSAData wsData;

    WORD ver = MAKEWORD(2,2) ;

    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0 )
    {
        std::cerr << " Cannot intialise the windsock!" << std::endl;
        return 0; 
    }
    // create the socket
    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        std::cerr << " Cannot create the socket!" << std::endl;
        return 0; 
    }
    // bind the socket to an ip address and port to the socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.s_addr = INADDR_ANY; // Could also use inet_pton...

    bind(listening, (sockaddr*)&hint, sizeof(hint));
    

    // Tell winsock the socket is for lisening.
    listen(listening, SOMAXCONN);

    // wait for connection
    sockaddr_in client;
    int clientSize = sizeof(client);

    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];  // client's remote name 
    char service[NI_MAXHOST]; // service (i.e port) the client is connect on 

    ZeroMemory(host, NI_MAXHOST);   // same as memset (host, 0 ,NI_MAXHOST)
    ZeroMemory(service, NI_MAXHOST);

    // try and look up the host name.
    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) ==0 )
    {
        std::cout << host << "Connected on port " << service << std::endl; 
    }
    else
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << "Connected on port " << ntohs(client.sin_port)<< std::endl; 
    }
    
    // close the list of the socket
    closesocket (listening); 
    // while loop : accept and echo message back to client.
    char buff [4096];
    while (true)
    {
        ZeroMemory(buff, 4096);
        // wait for client to send data
        int bytesReceived = recv (clientSocket, buff, 4096, 0 );
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << " Error in recv(). Quitting" << std::endl;
            break; 
        }
        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break; 
        }

        std::cout << "CLIENT> " << std::string(buff, 0, bytesReceived) << std::endl; 
        // echo message back to client 
        send(clientSocket, buff, bytesReceived + 1, 0); // prevent to get the zero buffer >> +1
        
    }
    // close the sock
    closesocket (clientSocket); 
    // shutdown the winsock.
    WSACleanup(); 
    
    return 0;
}
