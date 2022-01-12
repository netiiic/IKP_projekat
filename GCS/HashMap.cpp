#include "HashMap.h"

unsigned long GenerateHashFunction(unsigned char* str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}

void InitializeHashMap()
{
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		HashMap[i] = NULL;
	}
}