#include "jsmn.h"

// application constants
const char HOSTNAME[] = "api.openweathermap.org";
const int SERVERPORT = 80;
const size_t MAX_TOKENS = 128;
char HEADER_TEMPLATE[] = "GET https://%s/data/2.5/weather?q=%s&APPID=2aa9d2b9844b929c5faca06067a9fe33&units=metric HTTP/1.0\r\nHost: %s\r\n\r\n";

// function declarations
void print_usage(char *appName);
void print_output(jsmntok_t t[][MAX_TOKENS], char **d, size_t length);
int socket_connect();
char *request_data(int socketfd, char *city);
char *get_value(char *key, jsmntok_t *token, char *json);
