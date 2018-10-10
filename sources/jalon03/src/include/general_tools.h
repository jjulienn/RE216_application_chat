#ifndef GENERAL_TOOLS_H_
#define GENERAL_TOOLS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#define MSG_MAXLEN 100

void error(const char *msg);
int do_socket();
void readline(int file_des, void *str);
void sendline(int file_des, const void *str);
users users_add_user(users list, int thread_id, char* pseudo, char* IP_addr, char* port);
users users_delete_user(users list, users user);

#endif
