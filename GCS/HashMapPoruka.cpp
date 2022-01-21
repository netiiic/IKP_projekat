#include "pch.h"
#include "HashMapPoruka.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void HashMap_ShowP()
{
	printf("\n---- START ----\n");
	for (int i = 0; i < MAX_GROUPS; i++)
	{
		struct ElementP* tempElement = HashMapP[i];
		printf("[%d] --->", i);
		while (tempElement)
		{
			printf(" %s, %d, %s |", tempElement->poruka->grupa, tempElement->poruka->listen_port,
				tempElement->poruka->tekst);
			tempElement = tempElement->nextElement;
		}
		printf(" NULL\n");
	}
	printf("---- END ----\n");
}

unsigned long GenerateHashValueP(unsigned char* str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}

void HashMap_InitializeP()
{
	for (int i = 0; i < MAX_GROUPS; i++)
	{
		HashMapP[i] = NULL;
	}
}

bool HashMap_AddMSG(Poruka* poruka)
{
	struct ElementP* newElement = (struct ElementP*)malloc(sizeof(struct ElementP));
	newElement->poruka = poruka;
	newElement->nextElement = NULL;

	unsigned int key = GenerateHashValueP(poruka->grupa) % MAX_GROUPS;

	if (HashMapP[key] == NULL)
	{
		HashMapP[key] = newElement;
		return true;
	}
	else
	{
		struct ElementP* tempElement = HashMapP[key];
		while (tempElement->nextElement)
		{
			tempElement = tempElement->nextElement;
		}
		tempElement->nextElement = newElement;
		return true;
	}
	return false;
}

bool HashMap_DeleteMSG(unsigned char* group)
{
	unsigned int key = GenerateHashValueP(group) % MAX_GROUPS;
	struct ElementP* tempElement = HashMapP[key];

	if (tempElement != NULL)
	{
		HashMapP[key] = NULL;
		printf("Group succesfully deleted.\n");
		free(tempElement);
		return true;
	}

	printf("Group not found.\n");
	return false;
}