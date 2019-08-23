const char HOSTNAME[] = "api.openweathermap.org";
const int SERVERPORT = 80;

void print_usage(char *appName);
int socket_connect();
char *request_data(int socketfd, char *request_uri);
char *get_value(const char *key, const jsmntok_t *t, const char *json);
