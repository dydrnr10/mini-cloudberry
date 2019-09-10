#include <stdio.h>


#include "../header/function.h"
#include "../header/m_socket.h"

int main(int argc, char *argv[]){
	pid_t pid = 0;
	char *buf_rcv, *buf_snd, **buf_parsed, cur[BUFSIZE] = HOME_DIR, *buf_ret;
	int arg_cnt, i = 0;
	struct stat sbuf;

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	if (argc != 2){
		printf("Usage : %s <Port>\n", argv[0]);
		exit(1);
	}

	init_server(atoi(argv[1]));

	clnt_addr_size = sizeof(clnt_addr);
	while (1){
		sock_client = accept(sock_server, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if (fork() == 0)
			break;
	}
	printf("Connected\n");

	buf_snd = (char*)calloc(sizeof(char), BUFSIZE);

	while (1){
		strcat(buf_snd, "\n");
		strcat(buf_snd, cur);
		serv_write(buf_snd);

//		memset(buf_rcv, 0, sizeof(buf_rcv));
//		memset(buf_parsed, 0, sizeof(buf_parsed));
		memset(buf_snd, 0, BUFSIZE);

		printf("CUR : %s\n", cur);
		buf_rcv = serv_read();
		if (strncmp(buf_rcv, "exit", 4) == 0)
			break;
		buf_parsed = parsing(buf_rcv);
		arg_cnt = 0;
		for (i = 0; i < BUFDEG; i++){
			if (buf_parsed[i] == NULL || strlen(buf_parsed[i]) == 0)
				break;
			arg_cnt++;
		}
		switch(command_chk(buf_parsed[0])){
		case CMD_LS:
			printf("<LS>\n");
			buf_ret = command_ls(cur, arg_cnt, buf_parsed);
			strcpy(buf_snd, buf_ret);
			break;
		case CMD_CD:
			printf("<CD>\n");
			buf_ret = command_cd(cur, buf_parsed[1]);
			if (access(buf_ret, F_OK) == -1)
				break;
			stat(buf_ret, &sbuf);
			if (!S_ISDIR(sbuf.st_mode))
				break;
			strcpy(cur, buf_ret);
			break;
		case CMD_CP:
			printf("<CP>\n");
			if (arg_cnt != 3){
				strcpy(buf_snd, "Usage : cp <ORIGIN> <COPY>");
				break;
			}
			command_cp(cur, buf_parsed[1], buf_parsed[2]);
			break;
		case CMD_MKDIR:
			printf("<MKDIR>\n");
			if (arg_cnt != 2){
				strcpy(buf_snd, "Usage : mkdir <DIRNAME>");
				break;
			}
			command_mkdir(cur, buf_parsed[1]);
			break;
		case CMD_RM:
			printf("<RM>\n");
			if (arg_cnt != 2){
				strcpy(buf_snd, "Usage : rm <filename>");
				break;
			}
			command_rm(cur, buf_parsed[1]);
			break;
		case CMD_FILE:
			printf("<FILE>\n");
			if (arg_cnt != 3){
				strcpy(buf_snd, "Usage : file <opt> <filename>");
				break;
			}
			if (strncmp(buf_parsed[1], "-r", 2) == 0){
				printf("where.... are you?\n");
				buf_rcv = command_file(cur, buf_parsed + 1, NULL);
				strcpy(buf_snd, buf_rcv);
				break;
			}
			//read_file_content();
			while (1){
				buf_rcv = serv_read();
				if (strncmp(buf_rcv, "EOF", 3) == 0){
					break;
				}
				command_file(cur, buf_parsed + 1, buf_rcv);
				buf_parsed[1][1] = 'a';
			}
			break;
		default:
			printf("Unknown Command. %s\n", buf_parsed[0]);
			break;
		}
	}
	
	exit_server();
}
