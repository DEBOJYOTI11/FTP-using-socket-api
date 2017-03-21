#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
#include<string.h>
#include <arpa/inet.h>

#define SERVER_PORT 11088
#define MAX 2000
#define MAX_MSG 200
char a[10];

void ls_request(int);
char *trim (char *s)
{
    int i;
    while (isspace (*s)) s++;   // skip left side white spaces
    for (i = strlen (s) - 1; (isspace (s[i])); i--) ;   // skip right side white spaces
    s[i + 1] = '\0';
     return s;
}


char *command(char *s){
strcpy(s,trim(s));
a[0]=s[0];a[1]=s[1];a[2]=s[2];a[3]='\0';
//strcpy(a,s);
return a;
}


void get_request(char *comm,int sd){
	int len;FILE *fp;
	char info[MAX];
	char response[200];
	char filename[200];char type[4];
	sscanf(comm,"%s %s",type , filename);
	send(sd , comm ,strlen(comm),0);
	len  =  recv(sd , response ,sizeof(response) , 0);
	if(strlen(response) <=3)
		{
		fp = fopen(filename , "w+");

		recv(sd , info,MAX,0);
		fwrite(&info , 1,strlen(info),fp);
		printf("\n%s is downloaded succesfully\n",filename);
		fclose(fp);
		}
	else{
	 puts(response);
	}
	bzero(info,MAX);
	bzero(response,200);
	bzero(filename,200);
	return;
}

void put_request(char *comm,int sd){
	char filename[20],type[4],info[MAX];
	FILE *fp ;  char response[200];
	int len;
		
	sscanf(comm , "%s %s", type ,filename);
	send(sd , comm , strlen(comm), 0);
	len = recv(sd , response , sizeof(response) , 0);
	if(strlen(response)<=3)
		{
		fp = fopen(filename , "r+");
		if(fp==NULL)
			{
			printf( "Requested file %s is not present",filename);
			send(sd, "no",4,0);		
			return;}
		fread(&info , sizeof(char),MAX,fp);
		send(sd , info , strlen(info),0);
		fclose(fp);
		printf("\nFile : %s is succesfully uploaded\n",filename);	
		}
	else
		puts(response);
        bzero(info,MAX);
        bzero(response,200);
        bzero(filename,200);

	return;
}

int main(int argc , char *argv[])
{
	int sd , rc , len,ch;
	struct sockaddr_in localAddr , servAddr;
	struct hostent *h;
	char buf[MAX_MSG];
	char comm[20];
	if(argc < 2){
		fprintf(stderr , "usage : %s <server IP> <server port>\n" ,argv[0]);
		exit(1);
	}
	bzero((char *)&servAddr , sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	servAddr.sin_port = htons(SERVER_PORT);
	
	sd = socket(AF_INET, SOCK_STREAM, 0);
  	if(sd<0) {
    		perror("cannot open socket ");
    		exit(1);
  	
	}
	printf("Welcome..\n");
	
	rc = connect(sd , (struct sockaddr *)&servAddr , sizeof(servAddr));
	if(rc<0){
		perror("cannot connect");
		exit(1);
	}
	printf("You are connected to %s\n" , argv[1]);

	while (1){
	printf("\nEnter command (get/put <filename>) or list Directory (ls) : ");
	fgets(comm,sizeof(comm),stdin);
	printf("\nss = %s",command(comm));
	if(strcmp(command(comm) , "put")==0)
		ch =1;
	if(strcmp(command(comm) , "get")==0)
		ch=2;
	if(strcmp(command(comm),"exi")==0)
		ch=3;
	if(strcmp(command(comm),"ls")==0)
		ch=4;
	if(strcmp(command(comm),"cd")==0)
		ch=5;
	
	switch(ch){
	case 1:
		put_request(comm ,sd);			
		break;	
	case 2:
		get_request(comm,sd);
		break;
	case 3:
		exit(1);
	case 4:
		ls_request(sd);
		break;
	case 5:
		send(sd,comm,strlen(comm),0);
		break;
	default:
		printf("Invalid Command %s ..Enter again\n",comm);
	}
	ch=100;
	bzero(comm , sizeof(comm));
	}
	printf("Ok.. Connection closing");
	close(sd);
	
	
}

void ls_request(int sd){
	char dir[1000];int len;
	send(sd,"ls",3,0 );
	while( recv(sd , dir, sizeof(dir),0)){
		if(strcmp(dir,"no")==0)break;
		fputs(dir,stdout);printf("     ");
		bzero(dir,sizeof(dir));
		send(sd , "d",2,0);
		}
return;
}
