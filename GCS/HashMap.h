#pragma once
#ifndef HASHMAP_H
#define HASHMAP_H

#define MAX_GROUP 20
#define MAX_ADDRESS 50
#define MAX_CLIENT 60

typedef struct ClientData
{
	unsigned char group[MAX_GROUP];
	unsigned int port;
	unsigned char listen_address[MAX_ADDRESS];
	unsigned int listen_port;
} ClientData;

struct Element
{
	ClientData* client;
	struct Element* nextElement;
};

static Element* HashMap[MAX_CLIENT];

unsigned long GenerateHashValue(unsigned char* str);
void HashMap_Initialize();
bool HashMap_AddValue(ClientData* client);
ClientData* FindValue(unsigned int* group);
bool DeleteValue(unsigned int* port);
bool DeleteValueGroup(unsigned char* group);

#endif
#pragma once
