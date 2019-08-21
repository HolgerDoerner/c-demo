#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv) {
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			printf("%s\r\n", argv[i]);
		}
	}

	char* msg;
	msg = "test";

	puts(msg);

	pid_t pid = fork();
	if (pid != 0) printf("%d\r\n", getpid());

	return 0;
}
