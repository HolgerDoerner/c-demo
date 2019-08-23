#include "jsmn.h"

// application constants
const char HOSTNAME[] = "api.openweathermap.org";
const int SERVERPORT = 80;
const size_t MAX_TOKENS = 128;

// function declarations
void print_usage(char *appName);
int socket_connect();
char *request_data(int socketfd, char *request_uri);
char *get_value(char *key, jsmntok_t *t, char *json);
