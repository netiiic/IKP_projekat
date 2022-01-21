#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Functions.h"
#include "../GCS/HashMap.cpp"
#include "../GCS/HashMapPoruka.cpp"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"
//#define MAX_CLIENT 20

bool InitializeWindowsSockets();

int main(void)
{
    typedef struct FromClient
    {
        unsigned char group[MAX_GROUP_LEN];
        unsigned int port;
        unsigned char listen_address[MAX_ADDRESS];
        unsigned int listen_port;
    }FromClient;

    // Socket used for listening for new clients 
    SOCKET listenSocket = INVALID_SOCKET;
    // Socket used for communication with client
    SOCKET acceptedSocket[MAX_GROUPS];

    int trenutniBrojKonekcija = 0;

    for (int i = 0; i < MAX_GROUPS; i++)
    {
        acceptedSocket[i] = INVALID_SOCKET;
    }
    // variable used to store function return value
    int iResult;
    // Buffer used for storing incoming data
    char recvbuf[DEFAULT_BUFLEN];

    if (InitializeWindowsSockets() == false)
    {
        // we won't log anything since it will be logged
        // by InitializeWindowsSockets() function
        return 1;
    }

    // Prepare address information structures
    addrinfo* resultingAddress = NULL;
    addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    listenSocket = socket(AF_INET,      // IPv4 address famly
        SOCK_STREAM,  // stream socket
        IPPROTO_TCP); // TCP

    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket - bind port number and local address 
    // to socket
    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Since we don't need resultingAddress any more, free it
    freeaddrinfo(resultingAddress);

    // Set listenSocket in listening mode
    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    unsigned long mode = 1;
    iResult = ioctlsocket(listenSocket, FIONBIO, &mode);

    printf("Server initialized, waiting for clients.\n");

    fd_set readfds;


    // maksimalni period cekanja select funkcije
    timeval timeVal;
    timeVal.tv_sec = 1;
    timeVal.tv_usec = 0;

    while (true)
    {
        FD_ZERO(&readfds);

        if (trenutniBrojKonekcija < MAX_GROUPS)
        {
            FD_SET(listenSocket, &readfds);
        }

        for (int i = 0; i < trenutniBrojKonekcija; i++)
        {
            FD_SET(acceptedSocket[i], &readfds);
        }

        int result = select(0, &readfds, NULL, NULL, &timeVal);

        if (result == 0) {
            // vreme za cekanje je isteklo
        }
        else if (result == SOCKET_ERROR) {
            //desila se greska prilikom poziva funkcije
        }
        else {
            // rezultat je jednak broju soketa koji su zadovoljili uslov
            if (FD_ISSET(listenSocket, &readfds)) {
                // izvrsenje operacije
                sockaddr_in clientAddr;

                int clientAddrSize = sizeof(struct sockaddr_in);
                acceptedSocket[trenutniBrojKonekcija] = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

                printf("\nNew client request accepted. Client address: %s : %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

                if (acceptedSocket[trenutniBrojKonekcija] == INVALID_SOCKET)
                {
                    printf("accept failed with error: %d\n", WSAGetLastError());
                    closesocket(listenSocket);
                    WSACleanup();
                    return 1;
                }

                unsigned long mode = 1;
                iResult = ioctlsocket(acceptedSocket[trenutniBrojKonekcija], FIONBIO, &mode);
                trenutniBrojKonekcija++;
            }

            for (int i = 0; i < trenutniBrojKonekcija; i++)
            {
                if (FD_ISSET(acceptedSocket[i], &readfds))
                {
                    // Receive data until the client shuts down the connection
                    iResult = recv(acceptedSocket[i], recvbuf, DEFAULT_BUFLEN, 0);
                    if (iResult > 0)
                    {
                        FromClient* fromClient = (FromClient*)recvbuf;

                        ClientData* newClient = (ClientData*)malloc(sizeof(ClientData));

                        sockaddr_in socketAddress;
                        int socketAddress_len = sizeof(struct sockaddr_in);

                        // Ask getsockname to fill in this socket's local adress
                        if (getpeername(acceptedSocket[i], (sockaddr*)&socketAddress, &socketAddress_len) == -1)
                        {
                            printf("getsockname() failed.\n"); return -1;
                        }

                        char clientAddress[MAX_ADDRESS];
                        inet_ntop(AF_INET, &socketAddress.sin_addr, clientAddress, INET_ADDRSTRLEN);

                        strcpy((char*)newClient->group, (char*)fromClient->group);
                        strcpy((char*)newClient->listen_address, (char*)fromClient->listen_address);
                        newClient->port = (int)ntohs(socketAddress.sin_port);
                        newClient->listen_port = fromClient->listen_port;

                        HashMap_AddValue(newClient);
                        HashMap_Show();

                        //printf("Message received from client: %s.\n", recvbuf);

                        char* poruka = "Successfully connected! Would you like to send a message?(yes/no)\n";
                        iResult = send(acceptedSocket[i], poruka, (int)strlen(poruka) + 1, 0);
                        if (iResult == SOCKET_ERROR)
                        {
                            printf("send failed with error: %d\n", WSAGetLastError());
                            closesocket(acceptedSocket[i]);
                            for (int j = i; j < trenutniBrojKonekcija - 1; j++)
                            {
                                acceptedSocket[j] = acceptedSocket[j + 1];
                            }
                            acceptedSocket[trenutniBrojKonekcija - 1] = INVALID_SOCKET;
                            trenutniBrojKonekcija--;
                            i--;


                        }
                    }
                    else if (iResult == 0)
                    {
                        FromClient* fromClient = (FromClient*)recvbuf;
                        HashMap_DeleteValue(fromClient->group, fromClient->listen_port);
                        // connection was closed gracefully
                        printf("Connection with client closed.\n");
                        HashMap_Show();
                        closesocket(acceptedSocket[i]);
                    }
                    else
                    {
                        if (WSAGetLastError() == WSAEWOULDBLOCK) {

                            continue;
                        }
                        else
                        {
                            for (int j = 0; j < MAX_GROUP_LEN; j++)
                            {
                                struct Element* tempClientElement = HashMap[j];
                                while (tempClientElement)
                                {
                                    sockaddr_in socketAddress;
                                    int socketAddress_len = sizeof(struct sockaddr_in);
                                    if (getpeername(acceptedSocket[i], (sockaddr*)&socketAddress, &socketAddress_len) == -1)
                                    {
                                        break;
                                    }
                                    char tempClientAddress[30];
                                    inet_ntop(AF_INET, &socketAddress.sin_addr, tempClientAddress, INET_ADDRSTRLEN);

                                    if ((strcmp(tempClientAddress, (const char*)tempClientElement->client->listen_address) == 0) && ((unsigned int)ntohs(socketAddress.sin_port) == tempClientElement->client->port))
                                    {
                                        HashMap_DeleteValue(tempClientElement->client->group, tempClientElement->client->listen_port);
                                        //HashMap_DeleteGroup(tempClientElement->client->group);
                                        //RemoveValueFromHashMap(tempClientElement->clientData->name);
                                        //printf("Klijent %s se diskonektovao\n", tempClientElement->clientData->name);
                                        HashMap_Show();
                                        break;
                                    }
                                    tempClientElement = tempClientElement->nextElement;
                                }
                            }
                            // there was an error during recv
                            printf("recv failed with error: %d\n", WSAGetLastError());
                            //HashMap_Show();
                            closesocket(acceptedSocket[i]);
                        }

                        
                    }
                }
            }
        }
    }

        /*do
        {
            // Wait for clients and accept client connections.
            // Returning value is acceptedSocket used for further
            // Client<->Server communication. This version of
            // server will handle only one client.
            acceptedSocket = accept(listenSocket, NULL, NULL);

            if (acceptedSocket == INVALID_SOCKET)
            {
                printf("accept failed with error: %d\n", WSAGetLastError());
                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }

            do
            {

            } while (iResult > 0);

            // here is where server shutdown loguc could be placed

        } while (1);*/


        for (int i = 0; i < trenutniBrojKonekcija; i++)
        {
            // shutdown the connection since we're done
            iResult = shutdown(acceptedSocket[i], SD_SEND);
            if (iResult == SOCKET_ERROR)
            {
                printf("shutdown failed with error: %d\n", WSAGetLastError());
                closesocket(acceptedSocket[i]);
                WSACleanup();
                return 1;
            }
            closesocket(acceptedSocket[i]);
        }


        // cleanup
        closesocket(listenSocket);

        WSACleanup();

        return 0;
    }


bool InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
	return true;
}
