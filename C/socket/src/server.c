#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080


int 
main(int argc, char **argv)
{
	int 		server_fd, new_socket, valread;
	struct sockaddr_in address;
	int 		opt = 1;
	int 		addrlen = sizeof(address);
	char 		buffer   [BUFSIZ] = {0};
	char           *hello = "Hello from server";

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	/*
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))  {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        */

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *) & address, sizeof(address)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	printf("before listen\n");
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	for (;;) {
		printf("waiting for client\n");
		new_socket = accept(server_fd, (struct sockaddr *) & address, (socklen_t *) & addrlen);
		if (fork() == 0) {
			for (;;) {
				valread = read(new_socket, buffer, BUFSIZ);
				if (valread < 0) {
					printf("client ended\n");
					break;
				}
                buffer[valread] = '\0';
				printf("from client: [%s]\n", buffer);
				send(new_socket, hello, strlen(hello), 0);
				printf("sent %s\n", hello);
			}
			close(new_socket);
			exit(0);
		}
	}
	return 0;
}
