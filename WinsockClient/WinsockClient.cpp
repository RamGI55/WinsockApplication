
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2def.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

struct addrinfo
*result = NULL,
*ptr = NULL,
hints; 

int main(int argc, char* argv[])
{
    //FOR CLIENT
    std::string ip = "127.0.0.1";   // IP Address of the server
    int port = 54000;               // Listening port # of the server. 
    // INITALISE WINSOCK
    WSADATA wsaData;
    WORD ver = MAKEWORD (2,2);
    int wsResult = WSAStartup(ver, &wsaData);

    if (wsResult != 0)
    {
        std::cerr << "WSAStartup failed with error: " << wsResult << std::endl;
        return 0; 
    }
    // CREATE THE SOCKET
    SOCKET sock = socket(AF_INET, SOCK_STREAM , 0); // using IvIP6
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << WSAGetLastError() << std::endl;
        return 0; 
    }

    // Fill in a hint structure 
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, ip.c_str(), &hint.sin_addr); 
    // CONNECT TO THE SERVER
    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        std::cerr << "Cannot connect to server, " << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 0; 
    }

    // SEND AND RECEIVE THE DATA (DO WHILE Loop)
    char buffer[4096];
    std::string userinput;

    do
    {
        // prompt the user for some text
        std::cout << " > ";
        getline(std::cin, userinput);

        if (userinput.size() > 0)
        {
            // send the text
            int sendResult = send(sock, userinput.c_str(), userinput.size() + 1, 0);
            if (sendResult != SOCKET_ERROR)
            {
                // wait for response
                int byteRecevived = recv(sock, buffer, sizeof(buffer), 0);
                if (byteRecevived > 0)
                {
                    // echo response to console.
                    std::cout << "SERVER> " << std::string(buffer, 0, byteRecevived) << std::endl;
                }
                
            }
        }
    }while (userinput.size() > 0);
    // DISCONNECT - Closing down

    closesocket(sock);
    WSACleanup(); 
    
    return 0;
}
