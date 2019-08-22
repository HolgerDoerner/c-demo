#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// application constants
const char HOSTNAME[] = "goweather.herokuapp.com";
const int SERVERPORT = 80;

// foreward declaration of functions
void printUsage(char *appName);
int socket_connect();
char *request_data(int socketfd, char *request_uri);

int main(int argc, char **argv) {
	// check argument count, print usage info if no args are provided
   if (argc == 1) {
		printUsage(argv[0]);
       return 0;
    }

	system("clear");

	char *data[argc - 1];

	for (int i = 1; i < argc; i++) {
		int socketfd = socket_connect();
		char *msg = request_data(socketfd, argv[i]);
		char *token;

		while ((token = strsep(&msg, "\r\n")) != NULL) {
			if (strncmp("{", token, strlen("{")) == 0) {
				data[i-1] = token;
				printf("%s\r\n\r\n", data[i-1]);
			}
		}

		close(socketfd);
	}

    return 0;
}

void printUsage(char *appName) {
	printf("%s <city> <citiys ...>\r\n", appName);
	return;
}

int socket_connect() {
	struct hostent *server;
	struct sockaddr_in server_addr;
	int socketfd;

	server = gethostbyname(HOSTNAME);
	if (server == NULL) {
		fprintf(stderr, "Host '%s' unknown!", HOSTNAME);
		exit(1);
	}

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 3) {
		fprintf(stderr, "Error opening socket!");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

	if (connect(socketfd,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error connecting to host!");
		exit(1);
	}

	return socketfd;
}

char *request_data(int socketfd, char *request_uri) {
	char request_template[] = "GET https://%s/weather/%s HTTP/1.0\r\nHost: %s\r\n\r\n";
	int request_size = strlen(request_template)
		+ strlen(request_uri)
		+ strlen(HOSTNAME)*2;

	char *request = (char *) malloc(request_size);

	snprintf(request, request_size, request_template, HOSTNAME, request_uri, HOSTNAME);

	if (write(socketfd, request, request_size) < 0) {
		fprintf(stderr, "Error sending request!");
		exit(1);
	}

	free(request);
	
	int buffer_size = sizeof(char)*256;
	char buffer[buffer_size];
	char *output = (char *) malloc(buffer_size*4);

	while (read(socketfd, buffer, buffer_size) > 1) {
		strcat(output, buffer);
		bzero(buffer, buffer_size);
	}

	return output;
}
