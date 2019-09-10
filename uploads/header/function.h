#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define BUFSIZE 1024
#define BUFDEG 10

#define HOME_DIR "home"

#define CMD_BASE 1000
#define CMD_LS CMD_BASE + 1
#define CMD_CD CMD_BASE + 2
#define CMD_CP CMD_BASE + 3
#define CMD_MKDIR CMD_BASE + 4
#define CMD_RM CMD_BASE + 5
#define CMD_FILE CMD_BASE + 6

#define PERM_SIZE 10
#define TIME_SIZE 16
#define PATHNAME_SIZE 1024

/*
   char** parsing(char* msg);
   void command_rm(char* cur, char* file);
   void command_cp(char* cur, char* origin, char* copy);
   void command_mkdir(char* cur, char* dirname);
   char* command_cd(char* cur, char* next);
   int command_chk(char* msg);
   char* command_ls(char* cur, int argc, char** argv);
 */

char** parsing(char* msg){
	char **ret, *tmp;
	int i;
	ret = (char**)calloc(sizeof(char*), BUFDEG);
	for (i = 0; i < BUFDEG; i++){
		ret[i] = (char*)calloc(sizeof(char), BUFSIZE);
	}

	tmp = strtok(msg, " ");
	for (i = 0; i < BUFDEG; i++){
		//		printf("%d : %s\n", i, tmp);
		strcpy(ret[i], tmp);
		tmp = strtok(NULL, " ");
		if (tmp == NULL)
			break;
	}
	return ret;
}

char* command_file(char *cur, char **arg, char *file_content){
	char *ret = (char*)calloc(sizeof(char), BUFSIZE);
	char path[BUFSIZE];
	FILE* fp;

	if (cur[strlen(cur) - 1] == '/'){
		sprintf(path, "%s%s", cur, arg[1]);
	}
	else {
		sprintf(path, "%s/%s", cur, arg[1]);
	}


	if (strncmp(arg[0], "-r", 2) == 0){
		fp = fopen(path, "r");
		fread(ret, BUFSIZE, 1, fp);

		fclose(fp);
		return ret;
	}
	else if (strncmp(arg[0], "-w", 2) == 0){
		fp = fopen(path, "w");
	}
	else if (strncmp(arg[0], "-a", 2) == 0){
		fp = fopen(path, "a");
	}

	fprintf(fp, "%s\n", file_content);
	fclose(fp);

	return NULL;
}

char * getperm (char *perm_c, const mode_t mode){
	char *ref = "xwr";
	int i;

	memset (perm_c, '-', PERM_SIZE);

	if ((mode & S_IFMT) == S_IFDIR)
		perm_c[0] = 'd';

	for (i = 0; i < 9; i++)
	{
		if (mode & (1 << i))
			perm_c[PERM_SIZE - i - 1] = ref[i % 3];
	}

	return perm_c;
}

char * gettime (char *time_c, const struct tm *tinfo){
	snprintf (time_c, TIME_SIZE + 1, "%04d-%02d-%02d %02d:%02d", \
			tinfo->tm_year + 1900, \
			tinfo->tm_mon + 1, \
			tinfo->tm_mday, \
			tinfo->tm_hour, \
			tinfo->tm_min
			);

	return time_c;
}
void command_rm(char* cur, char* file){
	char path[BUFSIZE];
	if (cur[strlen(cur) - 1] == '/')
		sprintf(path, "%s%s", cur, file);
	else
		sprintf(path, "%s/%s", cur, file);

	if (remove(path) == -1){
		perror("remove");
		return;
	}
	return;
}

void command_cp(char* cur, char* origin, char* copy){
	char path_orig[BUFSIZE], path_copy[BUFSIZE], buf[BUFSIZE];
	FILE* fp1, *fp2;
	struct stat sbuf;
	if (cur[strlen(cur) - 1] == '/'){
		sprintf(path_orig, "%s%s", cur, origin);
		sprintf(path_copy, "%s%s", cur, copy);
	}
	else{
		sprintf(path_orig, "%s/%s", cur, origin);
		sprintf(path_copy, "%s/%s", cur, copy);
	}

	if (access(path_orig, F_OK) == -1){
		printf("No File or Directory\n");
		return;
	}
	if (stat(path_orig, &sbuf) == -1){
		perror("stat error ");
		return;
	}
	if (S_ISDIR(sbuf.st_mode)){
		printf("Can't copy directory\n");
		return;
	}
	fp1 = fopen(path_orig, "r");
	fp2 = fopen(path_copy, "w");

	while (fgets(buf, BUFSIZE, fp1)){
		fprintf(fp2, "%s", buf);	
	}

	fclose(fp1);
	fclose(fp2);
	return;
}

void command_mkdir(char* cur, char* dirname){
	char path[BUFSIZE];
	if (cur[strlen(cur) - 1] == '/')
		sprintf(path, "%s%s", cur, dirname);
	else
		sprintf(path, "%s/%s", cur, dirname);

	if (mkdir(path, 0766) == -1){
		perror("mkdir error ");
		return;
	}
}

char* command_cd(char* cur, char* next){
	char path[BUFSIZE], *ret;
	int i;
	ret = (char*)calloc(sizeof(char), BUFSIZE);

	if (strlen(next) == 0 || next == NULL){
		strcpy(ret, HOME_DIR);
	}
	else if (strlen(next) == 1 && strcmp(next, ".") == 0){
		strcpy(ret, cur);
	}
	else if (strlen(next) == 2 && strcmp(next, "..") == 0){
		for (i = strlen(cur) - 2; i >= 0; i--){
			if (cur[i] == '/'){
				cur[i] = '\0';
				break;
			}
		}
		strcpy(ret, cur);
	}
	else {
		if (cur[strlen(cur) - 1] == '/')
			sprintf(path, "%s%s", cur, next);
		else
			sprintf(path, "%s/%s", cur, next);
		strcpy(ret, path);
	}
	return ret;
}

char* command_ls(char* cur, int argc, char** argv){
	DIR* dp;
	struct dirent *dent;
	struct stat buf;
	char ino[100],lsl[100];
	char path[BUFSIZE], tmp[BUFSIZE],*ret;
	char *order = argv[1];
	char pathname[PATHNAME_SIZE + 1] = {0,};
	char perm_c[PERM_SIZE + 1]={0,};
	char time_c[TIME_SIZE + 1] = {0,};


	if(order[0] == '-' && order[1]== 'l') {
		strcpy(path, cur);
	}else if(order[0] == '-' && order[1]== 'a') {
		strcpy(path, cur);
	}else if(order[0] == '-' && order[1]== 'i') {
		strcpy(path, cur);
	}else if (argv[1] != NULL && strlen(argv[1])) {
		sprintf(path, "home/%s", argv[1]);
	}else{
		strcpy(path, cur);
	}

	if ((dp = opendir(path)) == NULL) {
		perror("opendir error ");
		return NULL;
	}
	ret = (char*)calloc(sizeof(char), BUFSIZE);

	printf("DIR <%s> : \n", path);
	strcat(path,"/");
	strcpy(tmp,path);
	int isa = 0;
	while ((dent = readdir(dp))!=NULL) {

		strcat(path,dent->d_name);
		printf("path : %s\n",path);
		stat(path,&buf);

		for(int i=1; i<strlen(argv[1]); i++) {
			if(order[i] == 'a') {
				isa = 0;
			}else{
				isa = 1;
			}
		}
		if (strlen(argv[1]) == 0 ) {
			if (dent->d_name[0] == '.') {
				continue;
			}
			printf("%s ", dent->d_name);
			strcat(ret, dent->d_name);
			strcat(ret, " ");
		}else if( strlen(argv[1])==2 && order[1] == 'a') {
			printf("%s ", dent->d_name);
			strcat(ret, dent->d_name);
			strcat(ret, " ");
		}else{
			if(isa == 0) {
				for(int i=1; i<strlen(argv[1]); i++) {
					if(order[i] == 'i') {
						sprintf(ino,"%lu",buf.st_ino);
						strcat(ret, ino);
						strcat(ret," ");
					}
				}
				for(int i=1; i<strlen(argv[1]); i++) {
					if(order[i] == 'l') {
						sprintf(lsl," %10s %2ld %8s %8s %5d %10s %5s",
								getperm(perm_c,buf.st_mode),
								buf.st_nlink,
								getpwuid(buf.st_uid)->pw_name,
								getgrgid(buf.st_gid)->gr_name,
								(int)buf.st_size,
								gettime(time_c,localtime(&buf.st_mtime)),
								pathname);
						strcat(ret,lsl);
					}
				}
				printf("%s ", dent->d_name);
				strcat(ret, dent->d_name);
			}else{
				if (dent->d_name[0] == '.') {
					continue;
				}
				for(int i=1; i<strlen(argv[1]); i++) {
					if(order[i] == 'i') {
						sprintf(ino,"%lu",buf.st_ino);
						strcat(ret, ino);
						strcat(ret," ");
					}
				}
				for(int i=1; i<strlen(argv[1]); i++) {
					if(order[i] == 'l') {
						sprintf(lsl," %10s %2ld %8s %8s %5d %10s %5s",
								getperm(perm_c,buf.st_mode),
								buf.st_nlink,
								getpwuid(buf.st_uid)->pw_name,
								getgrgid(buf.st_gid)->gr_name,
								(int)buf.st_size,
								gettime(time_c,localtime(&buf.st_mtime)),
								pathname);
						strcat(ret,lsl);
					}
				}
				printf("%s ", dent->d_name);
				strcat(ret, dent->d_name);
			}
			strcpy(path,tmp);
			strcat(ret, "\n");

		}
	}
	printf("\n");
	closedir(dp);
	return ret;
}

int command_chk(char* msg){
	if (strncmp(msg, "ls", 2) == 0){
		return CMD_LS;
	}
	if (strncmp(msg, "cd", 2) == 0){
		return CMD_CD;
	}
	if (strncmp(msg, "cp", 2) == 0){
		return CMD_CP;
	}
	if (strncmp(msg, "mkdir", 5) == 0){
		return CMD_MKDIR;
	}
	if (strncmp(msg, "rm", 2) == 0){
		return CMD_RM;
	}
	if (strncmp(msg, "file", 4) == 0){
		return CMD_FILE;
	}
	return -1;
}
