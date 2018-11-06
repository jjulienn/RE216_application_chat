#include "client_tools.h"

void init_client_addr(struct sockaddr_in *serv_addr, char *ip, int port) {
   // clean structure
   memset(serv_addr, '\0', sizeof(*serv_addr));
   serv_addr->sin_family = AF_INET; // IP V4
   serv_addr->sin_port = htons(port); // specified port in args
   serv_addr->sin_addr.s_addr = inet_addr(ip); // specified server IP in args
 }

 void do_connect(int sock, struct sockaddr_in host_addr) {
   int connect_result;

   do {
     connect_result = connect(sock, (struct sockaddr *) &host_addr, sizeof(host_addr));
   } while ((connect_result == -1) && (errno == EAGAIN || errno == EINTR));

   if (connect_result == -1)
     error("connect");
 }

 void auth_user(int sock) {
  char message[MSG_MAXLEN];
  int finished = 0;

  do {
     printf("![System] : Please identify yourself by using '/nick <Your Name>'  ");
     //get user input
     memset(message, 0, MSG_MAXLEN);
     fgets(message, MSG_MAXLEN-1, stdin);
     if(parser(message) == FUNC_NICK && is_pseudo_correct(message+strlen("/nick "))) {
       send_line(sock, message);
       printf(">(me) : %s", message);
       fflush(stdout);
       finished=1;
     }
   } while(!finished);
}

 int is_pseudo_correct(char * pseudo) {
  // TODO : check avec une regex sur la consistance du pseudo (pas urgent)
  if (strlen(pseudo) > 1 && *pseudo != ' ')
    return 1;
  else
    return 0;
}
// TODO differencier mutex en lecture des mutex en écriture
// TODO passer le pseudo de l'emetteur du message pour l'affiche à la réception
void * reception_handler(void * arg) {
  /* INITS */
  reception_arg * input = (reception_arg *) arg;
  char message[MSG_MAXLEN];

  /* RECEPTION AND DISPLAY OF MESSAGES */
  while(input->status != CLIENT_QUITTING) {
    memset(message, 0, MSG_MAXLEN);
    pthread_mutex_lock(&input->sock_mutex);
    read_line(input->sock, message);
    printf("<[?] : %s", message);
    fflush(stdout);
    pthread_mutex_unlock(&input->sock_mutex);

    // check if /quit
    if(strncmp("/quit", message, 5) == 0)
      input->status = CLIENT_QUITTING;
  }
  return NULL;
}

void * communication_handler(void * arg) {
  /* INITS */
  communication_arg * input = (communication_arg *) arg;
  char message[MSG_MAXLEN];

  /* EMISSION OF MESSAGES */
  while(input->status != CLIENT_QUITTING) {
    memset(message, 0, MSG_MAXLEN);
    fgets(message, MSG_MAXLEN-1, stdin);
    printf(">(me) : %s", message);
    fflush(stdout);
    pthread_mutex_lock(&input->sock_mutex);
    send_line(input->sock, message);
    pthread_mutex_unlock(&input->sock_mutex);

    // check if /quit
    if(strncmp("/quit\n", message, 6) == 0)
      input->status = CLIENT_QUITTING;
  }
  return NULL;
}