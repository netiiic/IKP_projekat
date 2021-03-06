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
	for (int i = 0; i < MAX_GROUPS; i++)
	{
		HashMap[i] = NULL;
	}
}

bool HashMap_AddValue(ClientData* client)
{
	struct Element* newElement = (struct Element*)malloc(sizeof(struct Element));
	newElement->client = client;
	newElement->nextElement = NULL;

	unsigned int key = GenerateHashValue(client->group) % MAX_GROUPS;

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

bool HashMap_DeleteValue(unsigned char* group, unsigned int listen_port)
{
	unsigned int key = GenerateHashValue(group) % MAX_GROUPS;
	struct Element* tempElement = HashMap[key], *prev = NULL;
	if (tempElement != NULL)
	{
		if (listen_port == tempElement->client->listen_port)
		{
			if (tempElement->nextElement)
			{
				HashMap[key]->client = tempElement->nextElement->client;
				HashMap[key]->nextElement = tempElement->nextElement->nextElement;
				return true;
			}
			else
			{
				HashMap[key] = NULL;
				return true;
			}
		}
		else
		{
			while (tempElement->nextElement)
			{
				prev = tempElement;										//prethodni
				tempElement = tempElement->nextElement;					//trenutni

				if (tempElement->client->listen_port == listen_port)
				{
					if (tempElement->nextElement)
					{
						prev->nextElement = tempElement->nextElement;
						free(tempElement);
						return true;
					}
					else
					{						
						prev->nextElement = NULL;
						tempElement == NULL;
						return true;
					}
				}
			} 
		}
	}
	else
		printf("Group not found!\n");
	return false;
}

bool HashMap_DeleteGroup(unsigned char* group)
{
	unsigned int key = GenerateHashValue(group) % MAX_GROUPS;
	struct Element* tempElement = HashMap[key];

	if (tempElement != NULL)
	{
		HashMap[key] = NULL;
		printf("Group succesfully deleted.\n");
		free(tempElement);
		return true;
	}	

	printf("Group not found.\n");
	return false;
}

void HashMap_Show()
{
	printf("\n---- START_GRUPE ----\n");
	for (int i = 0; i < MAX_GROUPS; i++)
	{
		struct Element* tempElement = HashMap[i];
		printf("[%d] --->", i);
		while (tempElement)
		{
			printf(" %s, %d, %s, %d, SOCKET:%d |", tempElement->client->group, tempElement->client->port,
				tempElement->client->listen_address, tempElement->client->listen_port, tempElement->client->socket);
			tempElement = tempElement->nextElement;
		}
		printf(" NULL\n");
	}
	printf("---- END_GRUPE ----\n");
}
