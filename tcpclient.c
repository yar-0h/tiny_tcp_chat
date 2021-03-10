//
// Compile with: gcc tcpclient.c -o tcpclient
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netinet/in.h>

#define MAXBUFFERSIZE 128 // should probably be the same size as in the server code

int main() {
	
	// Creating a socket
	int client_socket;
	if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("error creating socket\n");
 	}
	// establishing username
	int i = 0;
	char client_username[MAXBUFFERSIZE];
	char server_status[MAXBUFFERSIZE];
	char welcome_message[MAXBUFFERSIZE];

	while(1) {
		printf("please enter username: ");
		if (fgets(client_username, sizeof(client_username), stdin) != NULL) {
			size_t username_length = strlen(client_username);
			if (username_length > 0) {
				client_username[--username_length] = '\0';
				break;
			}
		}
	}
	printf("username: %s\n\n", client_username);

	// attempting connection to server
	printf("attempting to establish connection with server\n\n");
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET; 
	server_address.sin_port = htons(8888);
	server_address.sin_addr.s_addr = htons(INADDR_ANY); 

	int connection_status;

	// Report back to user if our connection was successful
	if ((connection_status = connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address))) <0 ){
		printf("connection unsuccessful: error code %d\n", connection_status);
		close(client_socket);
		exit(0);	
	}
	else{
		printf("connection successful\n");
	}

    // Integer to hold a potential client socket
    int server_socket;
    server_socket = accept(client_socket, NULL, NULL);
    // send a message from the server
    send(client_socket, &client_username, sizeof(client_username), 0);
    recv(client_socket, &welcome_message, sizeof(welcome_message), 0);
    printf("%s\n\n", welcome_message);

    // continuously taking user input and sending to server
    char outgoing_message[MAXBUFFERSIZE];
    while(1) {
        printf("%s:", client_username);
        if (fgets(outgoing_message, sizeof(outgoing_message), stdin) != NULL) {
            size_t outgoing_length = strlen(outgoing_message);
            if (outgoing_length > 0) {
                outgoing_message[--outgoing_length] = '\0';
                send(client_socket, &outgoing_message, sizeof(outgoing_message), 0);
                // exit and disconnect on 'exit' command
                if (strncmp("exit", outgoing_message, 4) == 0) {
                    break;
                }
            }
        }
    }
    // Close the socket and exit
    close(client_socket);
    printf("bye bye\n");
    exit(1);
}

