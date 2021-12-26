#pragma once

int Connect(char *queueName);
int Disconnect(char* queueName);
void SendMessage(void* message, int messageSize);
