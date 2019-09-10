#include <stdio.h>

#include "../header/m_socket.h"
#include "../header/function.h"


int main(int argc, char** argv){
	char **parsed;
	if (argc != 3){
		printf("Usage : %s <IP> <Port> \n", argv[0]);
		exit(1);
	}
	char *buf_rcv, *buf_snd;
	buf_snd = (char*)calloc(sizeof(char), BUFSIZE);

	init_client(argv[1], atoi(argv[2]));

	while (1){
		buf_rcv = clnt_read();
		printf("%s$ ", buf_rcv);
		fgets(buf_snd, BUFSIZE, stdin);
		buf_snd[strlen(buf_snd) - 1] = '\0';
		clnt_write(buf_snd);
		if (strcmp(buf_snd, "exit") == 0){
			break;
		}
		if (strncmp(buf_snd, "file", 4) == 0 && strncmp(buf_snd + 5, "-r", 2) != 0){
			parsed = parsing(buf_snd);
			printf("FILE %s writing...\n", parsed[2]);
			while (1){
				fgets(buf_snd, BUFSIZE, stdin);
				buf_snd[strlen(buf_snd) - 1] = '\0';
				clnt_write(buf_snd);
				if (strncmp(buf_snd, "EOF", 3) == 0){
					break;
				}
			}
		}
	}

	exit_client();
}
