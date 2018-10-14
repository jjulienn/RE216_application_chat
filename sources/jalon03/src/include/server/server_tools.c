#include "server_tools.h"

void init_serv_addr(struct sockaddr_in *serv_addr, int port)
 {
   /* Modify specified sockaddr_in for the server side with specified port */
   // clean structure
   memset(serv_addr, '\0', sizeof(*serv_addr));
   serv_addr->sin_family = AF_INET; // IP V4
   serv_addr->sin_port = htons(port); // specified port in args
   serv_addr->sin_addr.s_addr = INADDR_ANY;
 }

 void do_bind(int socket, struct sockaddr_in addr_in)
 {
   /* Perform a bind on specified socket */
   int bind_result = bind(socket, (struct sockaddr *) &addr_in, sizeof(addr_in));
   if (bind_result == -1) {
     error("Error during socket binding");
   }
 }

 void do_listen(int socket, int nb_max)
 {
   /* Switch specified socket in the listen state */
   int listen_result = listen(socket, nb_max);
   if (listen_result == -1) {
     error("Error during socket listening");
   }
 }

 int do_accept(int socket, struct sockaddr *addr, socklen_t* addrlen)
 {
   /* Accept a connection with the specified socket and return the file des from accepted socket*/
   int file_des_new = accept(socket, addr, addrlen);
   if(file_des_new == -1) {
     error("Error while accepting a connection");
   }
   return file_des_new;
 }

void *connection_handler(void* thread_input)
{
  /* Function called at the creation of a thread due to a new connection on the server */

  // Get thread args
  thread_arg * thread_args = (thread_arg *)thread_input;
  int thread_fd_connection = thread_args->thread_fd_connection;
  int my_id = thread_args->linked_user_id;
  struct users * users_list = thread_args->users;

  ++ *(thread_args->pt_nb_conn); // increase number of nb_total_connections
  printf("=== Connection %i opened ===\n", *(thread_args->pt_nb_conn));
  free(thread_input); // Free thread_input

  // INITS

  char message[MSG_MAXLEN];
  users_list = users_add_user(users_list, my_id, "Inconnu", "127.0.0.1", 8080);

  while(1) {
    //read what the client has to say
    memset(message, '\0', MSG_MAXLEN);
    readline(thread_fd_connection, message);
    printf("< Received [%s] : %s\n", users_get_user_pseudo(users_list, my_id), message);
    if (strncmp("/nick", message, 5) == 0) {
      char * pseudo = malloc((strlen(message)-6)*sizeof(char));
      strncpy(pseudo, message+6*sizeof(char), strlen(message)-7);
      user_set_pseudo(users_list, my_id, pseudo);
      memset(message, '\0', MSG_MAXLEN);
      strcpy(message, "Hello ");
      strcat (message, users_get_user_pseudo(users_list, my_id));
    }
    else if (strncmp("/who", message, 4) == 0) {
      memset(message, '\0', MSG_MAXLEN);
      strcpy(message, users_get_pseudo_list(users_list));
    }
    sendline(thread_fd_connection, message);
    printf("> Sending [%s] : %s\n", users_get_user_pseudo(users_list, my_id),message);
    // check if /quit
    if(strncmp("/quit", message, 5) == 0)
      break;
  }

  printf("=== Connection stopped ===\n");
  close(thread_fd_connection); // closing the fd associated to the connection
  return NULL; // a thread should return a pointer
}

struct users* users_add_user(struct users * list, int user_id, char* pseudo, char* IP_addr, int port){
  // add a new user at the end of the list users
  struct users * new_user = malloc(sizeof( struct users));

  if (new_user == NULL) {
    error("error creation new user");
  }

  new_user->user_id = user_id;
  new_user->pseudo = pseudo;
  new_user->IP_addr = IP_addr;
  new_user->port = port;
  new_user->next = NULL;

  if (list == NULL) {
    return new_user;
  }

  struct users *temp;
  temp=list;

  while (temp->next!=NULL) {
    temp=temp->next;
  }
  temp->next=new_user;

  return list;
}

struct users* users_delete_user(struct users * list, struct users * user){
  if (user->user_id == list->user_id) {
    return list->next;
  }
  else if (user->user_id == list->next->user_id){
    list->next = NULL;
    return list;
  }
  else{
    while (list->next!=NULL) {
      if (user->user_id==list->next->user_id) {
        list->next=list->next->next;
        return list;
      } else {
        list=list->next;
      }
    }
  }
  return list;
}

char * users_get_user_pseudo(struct users * users, int user_id){
  while (users->user_id!=user_id) {
    users = users->next;
  }
  return users->pseudo;
}

void user_set_pseudo(struct users * users, int user_id, char * pseudo){
  while (users->user_id!=user_id) {
    users = users->next;
  }
  users->pseudo = pseudo;
}

char * users_get_pseudo_list(struct users* users){
  char * list_pseudo = "Online users are \n";
  while (users!=NULL) {
    strcat(list_pseudo, users->pseudo);
    users = users->next;
  }
}
