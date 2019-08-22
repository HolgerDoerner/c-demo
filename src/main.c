#include <stdio.h>			/* printf, sprintf */
#include <stdlib.h>			/* exit, atoi, malloc, free */
#include <unistd.h>			/* read, write, close */
#include <string.h>			/* memcpy, memset */

// application constants
const char host[] = "goweather.herokuapp.com";

// foreward declaration of functions
void printUsage(char *appName);
void get_data(char **urls, size_t num_elements);

int main(int argc, char **argv) {
	// check argument count, print usage info if no args are provided
   if (argc == 1) {
       printUsage(argv[0]);
       return 0;
    }

	system("clear");

	char *urls[argc - 1];
	
	for (int i = 1; i < argc; i++) {
		size_t tmp_size = strlen("curl -s -X GET https://")
			+ strlen(host)
			+ strlen("/weather/")
			+ strlen(argv[i]) + 1;

		char tmp_url[tmp_size];
		urls[i] = malloc(tmp_size);
		snprintf(tmp_url,  tmp_size, "curl -s -X GET https://%s/weather/%s", host,  argv[i]);
		strcpy(urls[i], tmp_url);
	}

	get_data(urls, argc);

    return 0;
}

void printUsage(char *appName) {
	printf("%s <city> <citiys ...>\r\n", appName);
	return;
}

void get_data(char **urls, size_t num_elements) {
	int path_max = sizeof(char) * 1024;
	char path[path_max];
	FILE *fp;

	for (int i = 1; i < num_elements; i++) {
		fp = popen(urls[i], "r");
		while (fgets(path, path_max, fp) != NULL) {
			printf("%s\r\n\r\n", path);
		}
		
		pclose(fp);
		bzero(path, path_max);
	}

	return;
}
