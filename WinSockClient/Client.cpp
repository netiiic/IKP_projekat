#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "../GCS/HashMap.cpp"


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016
#define MAX_MSG 100

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();

char* read_message()
{
    int c;
    char* input = (char*)malloc(1);
    size_t length = 0;

    while ((c = getchar()) != '\n' && c != EOF)
    {
        char* t = (char*)realloc(input, length + 1);
        input = t;
        input[length++] = c;
    }
    return input;
}

int __cdecl main(int argc, char **argv) 
{
    typedef struct ToSendInfo
    {
        unsigned char group[MAX_GROUP_LEN];
        unsigned int port;
        unsigned char listen_address[MAX_ADDRESS];
        unsigned int listen_port;
        unsigned int flag = 1;
        unsigned char message[MAX_MSG];
    }ToSendInfo;

    typedef struct Poruka
    {
        unsigned char grupa[MAX_GROUP_LEN];
        unsigned int listen_port;
        char poruka[];
    }Poruka;

    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    char *messageToSend = "this is a test";
    
    // Validate the parameters
    /*if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }*/

    if(InitializeWindowsSockets() == false)
    {
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
    }

    //to get socket info
    
    // create a socket
    connectSocket = socket(AF_INET,
                           SOCK_STREAM,
                           IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(DEFAULT_PORT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
    }

    struct sockaddr_in socketAddress;
    int socketAddress_len = sizeof(socketAddress);

    if (getsockname(connectSocket, (sockaddr*)&socketAddress, &socketAddress_len) == -1)
    {
        return -1;
    }
    
    char *message = "this is a test";
    //printf("What would you like to send? ");
    //fgets(message, sizeof(message), stdin);
    //scanf("%ms", &message);
    
    ToSendInfo packet;

    //-------------------KLIJENT BIRA GRUPU ZA KONEKCIJU----------------------------
    
    printf("Hello, what group would you like to connect to? \n");
    unsigned char group[MAX_GROUP_LEN];
    scanf("%s", &group);
    strcpy((char*)packet.group, (char*)group);
    strcpy((char*)packet.listen_address, inet_ntoa(socketAddress.sin_addr));
    packet.listen_port = (int)ntohs(socketAddress.sin_port);
    strcpy((char*)packet.message, "");
    packet.port = 2;

    iResult = send(connectSocket, (char*)&packet, sizeof(packet), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    printf("My port: %d\n\n", (int)ntohs(socketAddress.sin_port));

    while (true)
    {
        //primanje poruke
        char recvbuf[DEFAULT_BUFLEN];
        // Receive data until the client shuts down the connection
        iResult = recv(connectSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult > 0)
        {
            recvbuf[iResult] = '\0';
            if (strcmp(&recvbuf[0], "1") == 0) {
                printf("Successfully connected!\n");
                strcpy(recvbuf, "");
            }
            
            printf("\nRecieved message: %s\n", recvbuf);
            printf("Would you like to send a message ? (yes / no)");
            char yn[4];
            scanf("%s", yn);
            char* yes = "yes";
            char* no = "no";
            
           if (strcmp(yn, yes) == 0)
           {
                printf("What would you like to send? ");
                char* message = (char*)malloc(100 * sizeof(char));
                //scanf("%s", message);
                getchar();

                fgets(message, MAX_MSG, stdin);

                int i = 0;
                for (i = 0; message[i] != '\0'; i++);
                int len = i;

                strncpy((char*)packet.message, message, len);
                packet.message[len] = '\0';
                free(message);
                packet.flag = 0;
                iResult = send(connectSocket, (char*)&packet, sizeof(packet), 0);

           }
           else if (strcmp(yn, no) == 0)
           {
               printf("Waiting for messages.\n");
           }
           else
           {
                printf("bullshit");
           }
            
            

        }
        else if (iResult == 0)  //connection was closed gracefully
        {
            //printf("Connection with server closed.\n");
            printf("Server vise nije dostupan!\n");
            return 0;

        }
        else  // there was an error during recv
        {
            //printf("recv failed with error: %d\n", WSAGetLastError());
            printf("Server vise nije dostupan!\n");
            return 0;

        }
    }
    
    //printf("Bytes Sent: %ld\n", iResult);
    _getch();
    // cleanup
    closesocket(connectSocket);
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
