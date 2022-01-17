#include "pch.h"
#include "HashMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long GenerateHashValue(unsigned char* str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}

void HashMap_Initialize()
{
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		HashMap[i] = NULL;
	}
}

bool HashMap_AddValue(ClientData* client)
{
	struct Element* newElement = (struct Element*)malloc(sizeof(struct Element));
	newElement->client = client;
	newElement->nextElement = NULL;

	unsigned int key = GenerateHashValue(client->group) % MAX_CLIENT;

	if (HashMap[key] == NULL)
	{
		HashMap[key] = newElement;
		printf("Element uspesno dodeljen.\n");
		return true;
	}
	else
	{
		struct Element* tempElement = HashMap[key];
		while (tempElement->nextElement)
		{
			tempElement = tempElement->nextElement;
		}
		tempElement->nextElement = newElement;
		return true;
	}
	return false;
}

void HashMap_Show()
{
	printf("\n---- START ----\n");
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		struct Element* tempElement = HashMap[i];
		printf("[%d] --->", i);
		while (tempElement)
		{
			printf(" %s, %d, %s, %d |", tempElement->client->group, tempElement->client->port,
				tempElement->client->listen_address, tempElement->client->listen_port);
			tempElement = tempElement->nextElement;
		}
		printf(" NULL\n");
	}
	printf("---- END ----\n");
}
