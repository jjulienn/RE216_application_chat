#ifndef SERVER_TOOLS_H_
#define SERVER_TOOLS_H_

#define NB_MAX_CLIENT 2

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "../general_tools.h"

struct users{
  int user_id;
  pthread_t linked_thread;
  char *pseudo;
  char *IP_addr;
  unsigned short port;
  char * date;
  struct users* next;
};

typedef struct thread_arg {
  int thread_fd_connection;
  int linked_user_id;
  int * pt_nb_conn;
  struct users * users;
  char * IP_addr;
  unsigned short port_number;
} thread_arg;

void init_serv_addr(struct sockaddr_in *serv_addr, int port);
void do_bind(int socket, struct sockaddr_in addr_in);
void do_listen(int socket, int nb_max);
int do_accept(int socket, struct sockaddr *addr, socklen_t* addrlen);
void *connection_handler(void* sock);
struct users* users_add_user(struct users * list, int user_id, char* pseudo, char* IP_addr, unsigned short port, char* date);
struct users* users_delete_user(struct users * list, int user_id_to_delete);
char * users_get_user_pseudo(struct users * users, int user_id);
void user_set_pseudo(struct users * users, int user_id, char * pseudo);
char *users_get_pseudo_list(struct users *users);
char *users_get_info_user(struct users * users, char *pseudo);
#endif
