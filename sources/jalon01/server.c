#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MSG_MAXLEN 30

void error(const char *msg);
int do_socket();
void init_serv_addr(struct sockaddr_in *serv_addr, int port);
void do_bind(int socket, struct sockaddr_in addr_in);
void do_listen(int socket, int nb_max);
int do_accept(int socket, struct sockaddr *addr, socklen_t* addrlen);
ssize_t readline(int file_des, void *str, size_t maxlen);
void sendline(int file_des, const void *str, size_t maxlen);

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if(port <= 1024) {
      fprintf(stderr, "Please use a non reserved port number.");
      exit(EXIT_FAILURE);
    }

    int sock;
    struct sockaddr_in serv_addr;

    //create the socket, check for validity!
    sock = do_socket();

    //init the serv_add structure
    init_serv_addr(&serv_addr, port);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(sock, serv_addr);

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(sock, 20);

    char message[MSG_MAXLEN];
    for (;;)
    {
        //accept connection from client
        socklen_t addrlen = sizeof(struct sockaddr);
        int connection_fd = do_accept(sock, (struct sockaddr*)&serv_addr, &addrlen);

        //read what the client has to say
        memset(message, '\0', MSG_MAXLEN);
        int read_length;
        while((read_length = readline(connection_fd, message, MSG_MAXLEN)) > 0)
        {
          printf("< Received : %s\n", message);
          //sendline(connection_fd, message, strlen(message));
          sendline(connection_fd, message, MSG_MAXLEN);
          printf("> Sending : %s\n", message);
        }

        // check if /quit
        if(strncmp("/quit", message, 5) == 0) {
          printf("=== Quiting. ===\n");
          break;
        }
    }

    //clean up server socket
    close(sock);

    return 0;
}

void error(const char *msg)
{
  /* Interrupt program because of an error */
    perror(msg);
    exit(EXIT_FAILURE);
}

int do_socket()
{
  /* Create a socket and return the associated file descriptor */
  int file_des = socket(AF_INET, SOCK_STREAM, 0);
  if (file_des == -1) {
    error("Error during socket creation");
  }

  return file_des;
}

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

 ssize_t readline(int file_des, void *str, size_t maxlen)
 {
   /* Read a line from the file descriptor with a maximum length in the specified buffer */
   return recv(file_des, str, maxlen, 0);
 }

 void sendline(int file_des, const void *str, size_t maxlen)
 {
   /* Write a line in the file descriptor with a maximum length with the given buffer */
   int sent_length=0;
   do {
     sent_length += write(file_des, str + sent_length, maxlen - sent_length);
   } while (sent_length != maxlen);
 }
