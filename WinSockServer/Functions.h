#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int Connect(char* queueName);
int Disconnect(char* queueName);
void SendMessage(void* message, int messageSize);

#endif FUNCTIONS_H
