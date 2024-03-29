#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "main.h"
#include "jsmn.h"

int main(int argc, char **argv) {
	// check argument count, print usage info if no args are provided
	if (argc == 1) {
		print_usage(argv[0]);
		return EXIT_SUCCESS;
    }

	size_t num_args = argc - 1;
	char **data = (char **) malloc(sizeof(char *) * num_args);

	// jsmn json-parser
	jsmn_parser p;
	jsmntok_t **tokens = (jsmntok_t **) malloc(sizeof(jsmntok_t *) * num_args * MAX_TOKENS);

	// query the api for each argument, take the json response from the body.
	for (int i = 0; i < num_args; i++) {
		int socketfd = socket_connect();
		char *msg = request_data(socketfd, argv[i + 1]);
		char *token;

		// tokenize the json-string with jsmn and store both in arrays.
		while ((token = strsep(&msg, "\r\n")) != NULL) {
			if (strncmp("{", token, strlen("{")) == 0) {
				tokens[i] = (jsmntok_t *) malloc(sizeof(jsmntok_t) * MAX_TOKENS);
				data[i] = token;
				jsmn_init(&p);
				jsmn_parse(&p, data[i], strlen(data[i]), tokens[i], MAX_TOKENS);
			}
		}

		free(msg);
		close(socketfd);
	}

	// print the data to stdout.
	print_output(tokens, data, num_args);

    return EXIT_SUCCESS;
}


/*
 * prints usage info for the application when
 * called without arguments.
 *
 * args: the name of the application
 * returns: void
 */
void print_usage(char *appName) {
	printf("%s <city> <citiys ...>\r\n", appName);
	return;
}

/*
 * Prints the final output to stdout.
 *
 * Args: an 2-d array containing the tokens,
 *			an array containing the json-strings
 * Returns: nothing (prints directly to stdout)
 */
void print_output(jsmntok_t **t, char **d, size_t length) {
	printf("%-10s %-10s %-10s %-10s\r\n", "CITY", "COUNTRY", "SKY", "TEMP");
	printf("%-10s %-10s %-10s %-10s\r\n", "----", "-------", "---", "----");
	
	// get the desired values from the json and print them to stdout.
	for (int i = 0; i < length; i++) {
		if (!strcmp(get_value("cod", t[i], d[i]), "404")) continue;

		char *name = get_value("name", t[i], d[i]);
		char *main = get_value("main", t[i], d[i]);
		char *country = get_value("country", t[i], d[i]);
		char *temp = get_value("temp", t[i], d[i]);

		printf("%-10s %-10s %-10s %-10s\r\n", name, country, main, temp);
	}

	return;
}

/*
 * opens a http socket-connection to remote endpoint.
 *
 * args: none
 * returns: the file-discriptor for the socket
 */
int socket_connect() {
	struct hostent *server;
	struct sockaddr_in server_addr;
	int socketfd;

	server = gethostbyname(HOSTNAME);
	if (server == NULL) {
		fprintf(stderr, "Host '%s' unknown!", HOSTNAME);
		exit(EXIT_FAILURE);
	}

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 3) {
		fprintf(stderr, "Error opening socket!");
		exit(EXIT_FAILURE);
	}

	// settings for remote endpoint.
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

	if (connect(socketfd,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		fprintf(stderr, "Error connecting to host!");
		exit(EXIT_FAILURE);
	}

	return socketfd;
}

/*
 * makes a HTTP call to the remote endpoint and
 * send a GET request for querying data.
 *
 * args: file-discriptor of an open socket,
 *			name of a city to get weather data for
 * returns: the raw response
 */
char *request_data(int socketfd, char *city) {
	int request_size = strlen(HEADER_TEMPLATE)
		+ strlen(city)
		+ strlen(HOSTNAME)*2;

	char *request = (char *) malloc(request_size);

	// create final header based on parameters
	snprintf(request, request_size, HEADER_TEMPLATE, HOSTNAME, city, HOSTNAME);

	// write request to socket (= send it to endpoint)
	if (write(socketfd, request, request_size) < 0) {
		fprintf(stderr, "Error sending request!");
		exit(EXIT_FAILURE);
	}

	free(request);
	request = NULL;
	
	int buffer_size = sizeof(char)*256;
	char buffer[buffer_size];
	char *output = (char *) malloc(buffer_size*4);

	// read the response from socket
	while (read(socketfd, buffer, buffer_size) > 1) {
		strcat(output, buffer);
		bzero(buffer, buffer_size);
	}

	return output;
}

/*
 * gets a value by key from a json-string by using tokens.
 *
 * args: the key to search for,
 *			the tokens for the matching json-string,
 *			the json-string
 * returns: the value stored under the given key
 */
char *get_value(char *key, jsmntok_t *token, char *json) {
	char *value = "";

	// iterate over the tokens
	for (jsmntok_t *i = token; i < token + MAX_TOKENS; ++i) {
		// in this case only string-tokens are of interrest
		if (i->type == JSMN_STRING) {
			jsmntok_t tok = *i;
			char tmp_key[tok.end - tok.start + 1];
			memcpy(tmp_key, &json[tok.start], tok.end-tok.start);
			tmp_key[tok.end-tok.start] = '\0';

			// check for the requested key
			if (strcmp(tmp_key, key) == 0){
				tok = *(i + 1);
				value = (char *) malloc(tok.end - tok.start + 1);
				memcpy(value, &json[tok.start], tok.end-tok.start);
				value[tok.end-tok.start] = '\0';
				
				return value;
			}
		}
	}

	// if key is not found, return an empty string
	return value;
}
