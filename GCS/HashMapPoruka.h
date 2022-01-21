#pragma once
#ifndef HASHMAPPORUKA_H
#define HASHMAPPORUKA_H

#define MAX_GROUP_LEN 20
#define MAX_GROUPS 20
#define MAX_MSG 100

typedef struct Poruka
{
	unsigned char grupa[MAX_GROUP_LEN];
	unsigned int listen_port;
	unsigned char tekst[MAX_MSG];

} Poruka;

struct ElementP
{
	Poruka* poruka;
	struct ElementP* nextElement;
};

static ElementP* HashMapP[MAX_GROUPS];

unsigned long GenerateHashValueP(unsigned char* str);
void HashMap_InitializeP();
bool HashMap_AddMSG(Poruka* poruka);
bool HashMap_DeleteMSG(unsigned char* group);

#endif
#pragma once
