#include "pch.h"
#include "HashMapPoruka.h"
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

bool HashMap_AddMSG(Poruka* poruka)
{
	struct Element* newElement = (struct Element*)malloc(sizeof(struct Element));
	newElement->poruka = poruka;
	newElement->nextElement = NULL;

	unsigned int key = GenerateHashValue(poruka->grupa) % MAX_GROUPS;

	if (HashMap[key] == NULL)
	{
		HashMap[key] = newElement;
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

bool HashMap_DeleteMSG(unsigned char* group)
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