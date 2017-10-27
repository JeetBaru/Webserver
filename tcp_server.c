/*
Project : HTTP/1.1 protocol based webserver

Author : Jeet Baru

This project supports GET and POST methods for the webserver and also supports multiple TCP connections to the webserver using fork()
*/
#define _GNU_SOURCE

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define CONNMAX 1000
#define BYTES 1024
char * R;
char ROOT[100];
char * P;
int listenfd, clients[CONNMAX];
void startServer(char *);
void respond(int);
char header[100];
int httpflag;

char * getitem(char * anything);

//This function although not called can be used to modify the POST requested file by addind data as the header to the html file
void editpostfile(char * filename, char * data)
{
	char * fileread;
	FILE * fp;
	fp = fopen(filename,"r"); //This is used to
	fseek(fp,0,SEEK_END);	  //Find the length of file
	int size = ftell(fp);
	rewind(fp);
	fread(fileread, 1, size, fp);	
	fclose(fp);
	char * towrite;
	sprintf(towrite,"<http><h1>%s</h1></http>",data); //Write data to the
	printf("%s\n\n%s\n\n",towrite,fileread);	  //top of html file
	fp = fopen(filename,"w");
	fwrite(towrite,1,strlen(towrite),fp);
	fseek(fp,0,strlen(towrite));
	fwrite(fileread,1,size,fp);
	fclose(fp);
}

//This function is used to detect and send the POST data back to calling function
char * returndata(char * message)
{
	while(*message != '\0')
	{
		if(*message == '\n' && *(message+1) == '\n')  //detect two consecutive /ns
			break;
		message++;
		printf("%c\n",*message);
	}
	*(message+strlen(message)) = '\0';	
	printf("%s\n",message+2);

	return message+2;
}

//This function creates header in the required format of content type and content length
char * createheader(char * filename)
{
	httpflag = 1;

	FILE * fp;
	fp =fopen(filename,"r");  	//This finds length of file
	fseek(fp,0L, SEEK_END);
	int size = ftell(fp);
	
	rewind(fp);
	fclose(fp);
	
	char * extension;
	char * a = strchr(filename,'.');	//searches for extension of file
	strcpy(extension,a);

	sprintf(header,"HTTP/1.%d 200 Document Follows\nContent-Type : %s\nContent-Length : %d\n\n",httpflag,getitem(extension),size);

	return header;
}

//This function extracts the second word from config file search
char * getsecondword(char * str)
{
	while( *str !=' ')	//search for space
	{
		if(*str == '\0')
			return NULL;
		str++;
	}
	
	str++;	

	*(str+strlen(str)-1) = '\0';

	return str;		//return second word
}

//This function searches for item in the ws.conf file and returns coressponding value
char * getitem(char * word)
{
	char line[100];
	char * rv;

	FILE * fp;
	fp = fopen("ws.conf","r");
	
	while(fgets(line,sizeof(line),fp) != NULL)	//searches the file line by line
	{	
		if(line[0] == '#')
			continue;
		if(strstr(line,word)!=NULL)
		{
			rv = getsecondword(line);	//find second word corresspond to item searched
			fclose(fp);
			return rv;
		}
		bzero(line,sizeof(line));
	}
	fclose(fp);
	return '\0';			
}

int main(int argc, char* argv[])
{
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	char c;    
	char PORT[100];
	FILE * f2;
	int flag = 0;

	f2 = fopen("ws.conf","r");
	if(f2 == NULL)
	{
		P = "10000";
		R = "/home/jeet/netsys/pa2/Final/";
		strcpy(PORT,P);
		strcpy(ROOT,R);
		flag = 1;
	}
	else
	{
		//Get info from conf file
		P = getitem("listen");
		strcpy(PORT,P);		//extract port number
		R = getitem("root");
		strcpy(ROOT,R);		//extract root address
	}
	int slot=0;

	printf("Server started at port no. %s with root directory as %s\n",PORT,ROOT);
	// Setting all elements to -1: signifies there is no client connected
	int i;
	for (i=0; i<CONNMAX; i++)
		clients[i]=-1;
	startServer(PORT);
	// ACCEPT connections
	while (1)
	{
		addrlen = sizeof(clientaddr);
		clients[slot] = accept(listenfd, (struct sockaddr *) &clientaddr, &addrlen);
		if (clients[slot]<0)
			perror ("accept() error");
		else
		{
			if ( fork()==0 && flag == 0 )
			{
				respond(slot);
				exit(0);
			}
			else if(flag == 1)
			{
                               	bzero(header, sizeof(header));
                               	sprintf(header,"HTTP/1.0 500 Internal Server Error\n\n<html><body>500 Internal Server Error </body></html>");
                               	send(clients[slot], header, strlen(header),0);
				shutdown (clients[slot], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
        			close(clients[slot]);
        			clients[slot]=-1;
			}
		}
		//increment socket number
		while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
	}

	return 0;
}
//I have referred this module from an online source
//start server
void startServer(char *port)
{
	struct addrinfo hints, *res, *p;

	// getaddrinfo for host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo( NULL, port, &hints, &res) != 0)
	{
		perror ("getaddrinfo() error");
		exit(1);
	}
	// socket and bind
	for (p = res; p!=NULL; p=p->ai_next)
	{
		listenfd = socket (p->ai_family, p->ai_socktype, 0);
		if (listenfd == -1) continue;
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
	}
	if (p==NULL)
	{
		perror ("socket() or bind()");
		exit(1);
	}

	freeaddrinfo(res);

	// listen for incoming connections
	if ( listen (listenfd, 1000000) != 0 )
	{
		perror("listen() error");
		exit(1);
	}
}

//client connection
void respond(int n)
{
	char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999], header[100];
	int rcvd, bytes_read;
	char * head;
	char messagecopy[10000];
	char * data;
	char h1head[1000];
	FILE * fd;

	memset( (void*)mesg, (int)'\0', 99999 );

	rcvd=recv(clients[n], mesg, 99999, 0);

	if (rcvd<0)    // receive error
		fprintf(stderr,("recv() error\n"));
	else if (rcvd==0)    // receive socket closed
		fprintf(stderr,"Client disconnected upexpectedly.\n");
	else    // message received
	{
		//create message copy
		strncpy(messagecopy,(void *)mesg,rcvd);
		printf("Message:\n%s\n",mesg);
		
		//parse first word
		reqline[0] = strtok (mesg, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
		{
			reqline[1] = strtok (NULL, " \t");	//parse addr
			reqline[2] = strtok (NULL, " \t\r\n");	//parse http
			if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				bzero(header, sizeof(header));
				sprintf(header,"HTTP/1.0 400 Bad Request\n\n<html><body>400 Bad Request %s</body></html>",reqline[2]);
				send(clients[n], header, strlen(header),0);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 )
					reqline[1] = getitem("defaultwebpage");        //Because if no file is specified, index.html will be opened by default 
				strcpy(path, ROOT);
				strcpy(&path[strlen(ROOT)], reqline[1]);
				printf("file: %s\n", path);
				if ( (fd=fopen(path, "r"))!=NULL )    //FILE FOUND
				{
					fclose(fd);
					head = createheader(path);	
					printf("Reply Header : \n%s\n",head);
					fd=fopen(path,"r");
					send(clients[n], head, strlen(head), 0);
					while ( (bytes_read=fread(data_to_send,1, BYTES,fd))>0 )
						write (clients[n], data_to_send, bytes_read);
				}
				else    
				{
					printf("\n\nPage not found\n\n");
					bzero(header, sizeof(header));
					sprintf(header,"HTTP/1.1 404 Not Found\n\n<html><body>404 Not Found : %s</body></html>",reqline[1]);
					send(clients[n], header, strlen(header),0); //FILE NOT FOUND	
				}	
			}
		}
		else if( strncmp(reqline[0], "POST\0", 5)==0) // for post
		{	
			reqline[1] = strtok (NULL, " \t"); // parse add
			reqline[2] = strtok (NULL, " \t\r\n");	//parse http

			data = returndata(messagecopy);
			if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				bzero(header, sizeof(header));
				sprintf(header,"HTTP/1.0 400 Bad Request\n\n<html><body>400 Bad Request %s</body></html>",reqline[2]);
				send(clients[n], header, strlen(header),0);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 )
					reqline[1] = getitem("defaultwebpage");        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

				strcpy(path, ROOT);
				strcpy(&path[strlen(ROOT)], reqline[1]);
				printf("file: %s\n", path);

				if ( (fd=fopen(path, "r"))!=NULL )    //FILE FOUND
				{
					fclose(fd);
					head = createheader(path);
					fd=fopen(path,"r");
//					bzero(header, sizeof(header));
//					strcpy(header,"HTTP/1.0 200 OK\n\n");
					send(clients[n], head,strlen(head), 0);
					sprintf(h1head,"<html><h1>%s</h1></html>",data);
					write(clients[n],h1head,strlen(h1head));
					while ( (bytes_read=fread(data_to_send,1, BYTES,fd))>0 )
						write (clients[n], data_to_send, strlen(data_to_send));
				}
				else    
				{
					printf("\n\nPage not found\n\n");
					bzero(header, sizeof(header));
					strcpy(header,"HTTP/1.1 404 Not Found\n\n<html><body>404 Not Found : </body></html>");
					send(clients[n], header, strlen(header),0); //FILE NOT FOUND	
				}
			}
		}
		else if(strncmp(reqline[0], "DELETE\0", 7)==0 || strncmp(reqline[0], "OPTIONS\0", 8)==0 || strncmp(reqline[0], "HEAD\0", 5)==0)
		{
			bzero(header,sizeof(header));
			sprintf(header,"HTTP/1.1 501 Not Implemented\n\n<html><body>501 Not Implemented %s</body></html>",reqline[0]);
			send(clients[n], header, strlen(header), 0);
		}
		else
		{
//			*(reqline[0]+strlen(reqline[0])-2)='\0';
			bzero(header,sizeof(header));
            sprintf(header,"HTTP/1.1 400 Bad Request\n\n<html><body>400 Bad Request %s</body></html>",reqline[0]);
            send(clients[n], header, strlen(header), 0);
		}
	}

	//Closing SOCKET
	shutdown (clients[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
	close(clients[n]);
	clients[n]=-1;
}
