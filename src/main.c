#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "main.h"
#include "jsmn.h"

// application constants
//const char HOSTNAME[] = "goweather.herokuapp.com";
//const int SERVERPORT = 80;

// foreward declaration of functions
//void printUsage(char *appName);
//int socket_connect();
//char *request_data(int socketfd, char *request_uri);

int main(int argc, char **argv) {
	// check argument count, print usage info if no args are provided
	if (argc == 1) {
		print_usage(argv[0]);
		return 0;
    }

	jsmn_parser p;
	jsmntok_t t[128];
	jsmn_init(&p);
	int r;

	size_t num_args = argc - 1;
	char *data[num_args];
	jsmntok_t tokens[num_args][num_args];

	for (int i = 0; i < num_args; i++) {
		int socketfd = socket_connect();
		char *msg = request_data(socketfd, argv[i + 1]);
		char *token;

		while ((token = strsep(&msg, "\r\n")) != NULL) {
			if (strncmp("{", token, strlen("{")) == 0) {
				data[i] = token;
				//printf("%s\r\n\r\n", data[i-1]);
			}
		}

		close(socketfd);
	}

	r = jsmn_parse(&p, data[0], strlen(data[0]), t, 128);
	printf("%s\r\n", get_value("description", &t, data[0]));

    return 0;
}

void print_usage(char *appName) {
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
	char request_template[] = "GET https://%s/data/2.5/weather?q=%s&APPID=2aa9d2b9844b929c5faca06067a9fe33 HTTP/1.0\r\nHost: %s\r\n\r\n";
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

char *get_value(const char *key, const jsmntok_t *t, const char *json) {
	size_t length = sizeof(json) / sizeof(json[0]);

	for (int i = 0; i < length; i++) {
		if (t[i].type == JSMN_STRING) {
			jsmntok_t tok = t[i];
			char tmp_key[tok.end - tok.start + 1];
			memcpy(tmp_key, &json[tok.start], tok.end-tok.start);
			tmp_key[tok.end-tok.start] = '\0';

			if (strcmp(tmp_key, key) == 0){
				tok = t[i + 1];
				char value[tok.end - tok.start + 1];
				memcpy(value, &json[tok.start], tok.end-tok.start);
				value[tok.end-tok.start] = '\0';
				
				return value;
			}
		}
	}

	return "";
}
