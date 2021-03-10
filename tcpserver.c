// Compile with: gcc tcpserver.c -o tcpserver
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

// Sockets library and internet protocol library
#include <sys/socket.h>
#include <netinet/in.h>

#define MESSAGEBUFFER 128

int main() {
	// creation with a socket() 
	int server_socket;
	int socket_read;
	// domain = AF_INET(IPv4) or AF_INET6
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	// define the server address format
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8888);
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	
	// once server is set up we continuously listen for new connections

	// bind() the server to an address
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	printf("server successfully bound to address\n");

	while(1) {
        printf("now awaiting incoming connections\n\n");
        // listen() for client connections
        listen(server_socket, SOMAXCONN);

        // Integer to hold a potential client socket
        int client_socket;
        client_socket = accept(server_socket, NULL, NULL);

        // announce the client arrival
        printf("Client has joined: %d\n", client_socket);

        // make first contact with and greet client
        char client_message[MESSAGEBUFFER];
        char client_message_buffer[MESSAGEBUFFER];
        char client_username[MESSAGEBUFFER];
        char server_message[MESSAGEBUFFER] = "You have reached the server";

        recv(client_socket, &client_username, sizeof(client_username), 0);
        send(client_socket, &server_message, sizeof(server_message), 0);
        printf("welcome, %s\n", client_username);


        while (1) {
            // check if received data differs from what we've already got
            strcpy(client_message_buffer, client_message);
            // Receive data from the client
            recv(client_socket, &client_message, sizeof(client_message), 0);
            if ((strncmp(client_message_buffer, client_message, sizeof(client_message)) != 0)) {
                // Print out the client's command
                printf("%s: %s\n", client_username, client_message);
                // if the command is substantial, pass it to a forked proccess making asystem call
                if (strlen(client_message) > 0) {
                    pid_t pid;
                    pid = fork();
                    if (pid == -1) {
                        printf("error encountered while attempting fork\n");
                        exit(1);
                    } else if (pid == 0) {
                        system(client_message);
                        exit(0);
                    }
                }
                // if the command is exit, break the loop, and terminate the connection
                if (strncmp("exit", client_message, 4) == 0) {
                    break;
                }
            }
        }
        close(client_socket);
        printf("connection with client %d has been terminated\n", client_socket);
	}
}
