#include "include/server/server_tools.h"
#include "include/general_tools.h"

#define MSG_MAXLEN 30

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
    pthread_t thread_id;
    thread_arg * thread_input;





    for (;;)
    {
        //accept connection from client
        socklen_t addrlen = sizeof(struct sockaddr);
        int connection_fd = do_accept(sock, (struct sockaddr*)&serv_addr, &addrlen);

        //initialisation thread
        thread_input = (thread_arg*)malloc(sizeof *thread_input);
        thread_input.thread_fd_connection = connection_fd;
        thread_input.thread_sock = sock;

        //création thread
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &thread_input) != 0)
        {
            error("Erreur création du tread.");
        }
        //pthread_join( thread_id , NULL);


    }

    //clean up server socket
    close(sock);

    return 0;
}
