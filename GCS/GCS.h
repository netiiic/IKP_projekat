#pragma once
#ifndef GCS_H
#define GCS_H

int Connect(char *queueName);
int Disconnect(char* queueName);
void SendMessage(void* message, int messageSize);

#endif GCS_H
