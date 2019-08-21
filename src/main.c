#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit, atoi, malloc, free */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

int main(int argc, char** argv) {
   if (argc == 1) {
       printf("%s <city>\r\n", argv[0]);
       return 0;
    }

    int port = 80;
    char *host = "https://goweather.herokuapp.com";
    size_t strsize = strlen(host) + strlen("/weather/") + strlen(argv[1]);
    char *url = (char *)malloc(sizeof(char) * strsize+1);
    snprintf(url, strsize+1, "%s/weather/%s", host, argv[1]);
    puts("...");

    puts(url);
    
    return 0;
}
