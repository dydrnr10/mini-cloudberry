#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFSIZE 1024

int sock_server;
int sock_client;
/*
char* clnt_read();
char* serv_read();

void clnt_write(char* msg);
void serv_write(char* msg);

void init_server();
void init_client();

void exit_server();
void exit_client();
*/
char* clnt_read(){
	char *buf = (char*)calloc(sizeof(char), BUFSIZE);
	if (read(sock_server, buf, BUFSIZE) <= 0){
		perror("read error");
		return NULL;
	}
	return buf;
}

char* serv_read(){
	char *buf = (char*)calloc(sizeof(char), BUFSIZE);
	if (read(sock_client, buf, BUFSIZE) <= 0){
		perror("read error");
		return NULL;
	}
	return buf;
}

void clnt_write(char* msg){
	if (write(sock_server, msg, BUFSIZE) <= 0){
		perror("write error");
		return;
	}
	return;
}

void serv_write(char* msg){
	if (write(sock_client, msg, BUFSIZE) <= 0){
		perror("write error");
		return;
	}
	return;
}

void init_client(char * addr, int port){
	struct sockaddr_in server_addr;

	if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("server sock error");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(addr);
	server_addr.sin_port = htons(port);

	if (connect(sock_server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
		perror("connect error");
		exit(1);
	}
	printf("Client Init Complete\n");
	return;
}

void init_server(int port){
	struct sockaddr_in server_addr, client_addr;
	int client_addr_size;

	if ((sock_server = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("server sock error");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock_server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
		perror("bind error");
		exit(1);
	}
	if (listen(sock_server, 10) == -1){
		perror("listen error");
		exit(1);
	}

/*	client_addr_size = sizeof(client_addr);
	sock_client = accept(sock_server, (struct sockaddr*)&client_addr, &client_addr_size);
	if (sock_client == -1){
		perror("Connect client error\n");
		exit(1);
	}*/

	printf("Server Init Complete\n");
	return;
}

void exit_server(){
	close(sock_client);
	close(sock_server);
}
void exit_client(){
	close(sock_server);
}
