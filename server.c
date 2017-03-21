#include <stdio.h>
#include <stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#include<string.h>
#define MAX_CLIENT 5
#define SERVER_PORT 11088
#define MAX_MSG 100
#define MAX 2000
#include <arpa/inet.h>
#include <dirent.h>

void get_request(char * , int);
void put_request(char * , int);
void ls_request(int );
void cd_request(char *,int );

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {

	int sd , newSd , cliLen , len;
	struct sockaddr_in cliAddr , servAddr;
	
	char buf[MAX_MSG];
	int optval;
	//end

	//variables for tfpt
	char type[4],filename[20];	
	//end
	
	bzero((char *)&servAddr , sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(SERVER_PORT);
	
	sd = socket(AF_INET , SOCK_STREAM,0);
	if(sd < 0){
		perror("cannot open socket");
		exit(1);
	}
	setsockopt(sd, SOL_SOCKET , SO_REUSEADDR , &optval , sizeof(int));
	
	if (bind(sd , (struct sockaddr *)&servAddr  , sizeof(servAddr)) < 0){
		perror("cannot bind port");
		exit(1);
	}
	
	listen(sd , MAX_CLIENT);
	
	while(1){
		printf("%s :: waiting for data on port TCP %u\n " , argv[0],SERVER_PORT);
		
		cliLen = sizeof(cliAddr);
		
		newSd = accept(sd , (struct sockaddr *)&cliAddr , &cliLen);
		if( newSd < 0){
			perror("cannot accept connection");
			exit(1);
		}
		else{
		printf("----------\nConnected to child with PID : %d ....\n",newSd);
		}
		memset(buf , 0x0 , MAX_MSG);
		
		printf("waiting...\n");
		while(len = recv( newSd  , buf ,sizeof(buf) , 0)){
			sscanf(buf , "%s %s",type , filename);
			if(strcmp(type,"get")==0)
				get_request(filename , newSd);
			else if(strcmp(type , "put")==0)
				put_request(filename,newSd);
			else if(strcmp(type,"ls")==0)
				ls_request(newSd);
			else if(strcmp(type , "cd"))
				cd_request(filename,newSd);
				
			else{
				printf("Unlnown Command %s!\n",buf);
		}
		bzero(buf,sizeof(buf));
		printf("waiting...\n");
		}
	
	printf("connection ended !\n");
	}
		
	return 0;
}

void get_request(char *filename ,int newSd){
	printf("A get requested is requested\n");
	FILE *fp ;
	char res[200], buf[100];
	char info[MAX];
	
	fp = fopen(filename, "r+");
	if(!(fp ==NULL))
		send(newSd , "ok", 3 , 0);
	else {
		strcpy(res , "File Not present or you dont have permission to access this file");
		send(newSd , res , strlen(res),0);
		return;
		}
	bzero(res,sizeof(res));

	fread(&info , sizeof(char),MAX,fp);
	send(newSd , info , strlen(info),0);
	fclose(fp);
	bzero(info,sizeof(info));
	printf("File %s transferred succesfully\n",filename);
	return;
}
	
void put_request(char *filename , int newSd){
	printf("A put requested is requested\n");
	FILE *fp;
	char info[MAX];
	char response[10];

	fp =fopen(filename,"w+");
	if(fp==NULL){
		send(newSd ,"You dont have access to put files here",100,0);
		return;}
	else
		send(newSd , "ok",4,0);
	recv(newSd , info ,MAX,0);
	if(strcmp(info,"no")==0){
		remove(filename);
		return;}
	fwrite(&info , 1,strlen(info),fp);
	printf("File %s is uploaded here\n",filename);
	fclose(fp);
	bzero(info,MAX);
	return;
}

void ls_request(int newSd){
printf("An list directory request is requested\n");
 char res[5];
  DIR           *d;
  struct dirent *dir;
  d = opendir("./");
 
 if(d) {
    while ((dir = readdir(d)) != NULL)
    {
	if (dir->d_type == DT_REG){
  		send(newSd ,dir->d_name, strlen(dir->d_name),0);
		recv(newSd , res , sizeof(res),0);
		bzero(res,sizeof(res));

	//	bzero(dir->d_name , sizeof(dir->d_name));
	 }
      }
    
send(newSd , "no",5,0);
closedir(d);
}
return;
}

void cd_request(char *dir,int newSd){
	char reply[50];
	if(chdir(dir)!=0){
		sprintf(reply,"\nDirectory %s not present",dir);
		send(newSd , dir , strlen(dir),0);
	}
}
