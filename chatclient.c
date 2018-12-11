/* 
Program Name: chatclient
Description: Simple chat client.
Author: Ryan Ellis
Class: CS372 - Intro to Networks
Citations: Used 'client.c' and 'server.c' sample code from 'Beej's Guide to
Network Programming for to help in setting up connections/sending/receiving.
http://beej.us/guide/bgnet/html/multi/clientserver.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define MAXHANDLELENGTH 10 // max number of characters the handle can be
#define MAXMESSAGELENGTH 488 // max number of character in message

// Description: get sockaddr, IPv4 or IPv6
// Argument: socket address struct
// Returns: pointer to the address of the socket
void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Description: handles all preliminary connection setup
// Argument: string with destination host and port
// Returns: integer with sockfd needed for connection
int connection_setup(char *host, char *port){

	// for connection setup
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints); // clear the contents of the addrinfo struct
	hints.ai_family = AF_UNSPEC; // make the struct IP agnostic
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // fill in my IP for me

	// check results of call to getaddrinfo(), if not 0 then error.
	// if 0, then the required structs setup successfully and 
	// returns a pointer to a linked-lists of results
	if((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next){
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("client: socket");
			continue;
		}

		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	// checks connections
	if(p == NULL){
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // free up memory allocated to servinfo	

	return sockfd;
}

// Description: get handle from user
// Argument: pointer to variable holding user handle
// Returns: nothing
void get_handle(char *handle){
	// variable to clear buffer after scanf call
	int clear;
	// prompt
	printf("Hey there! Welcome to chatclient!\n");
	printf("What handle do you want to use? *MAX LENGTH 10 CHARACTERS*\n");
	scanf("%10s", handle);
	// clear buffer
	while((clear = getchar()) != '\n' && clear != EOF){};
}

// Description: get message from user
// Arguments: pointer to variables users handle and message to send
// Returns: nothing
char *get_message(char *handle){
	// message length in correct datatype to work with getline
	size_t message_length = MAXMESSAGELENGTH;
	// variable to hold status of get_line
	int input_status;
	// allocate memory for message
	char *send_message = (char *) malloc (message_length);
	// check to ensure allocation successful
	if(send_message == NULL){
		puts("Error allocating memory for message!");
	}
	printf("%s> ", handle);
	input_status = getline(&send_message, &message_length, stdin);

	while(input_status == -1 || input_status > MAXMESSAGELENGTH){
		if(input_status == -1){
			puts("Oppps! Looks like there was an error. Please re-enter message.");
		}
		else{
			puts("Oppps! Message is too long. Please limit message to under 488 characters");
		}
		printf("%s> ", handle);
		input_status = getline(&send_message, &message_length, stdin);
	}

	return send_message;
}

// Description: combines user handle and message together in preparation 
// for sending out
// Arguments: handle, message strings
// Returns: pointer to combined string
void build_handle_message(char *handle, char *send_message, char *send_handle_message){
	char *tick_mark = "> ";
	// start building...
	strncpy(send_handle_message, handle, 10);
	strncat(send_handle_message, tick_mark, 2);
	strncat(send_handle_message, send_message, 488);

}

// Description: sends out the message to server
// Arugments: socket designator, and the message to be send
// Returns: nothing
void sending_message(int sockfd, char *send_handle_message){
	// setup local variables
	int handle_message_length = strlen(send_handle_message);
	int bytes_sent; // check to ensure entire message sent

	bytes_sent = send(sockfd, send_handle_message, handle_message_length, 0);
	
	// check to ensure whole message sent
	if (bytes_sent != handle_message_length)
	{
		fprintf(stderr, "Entire message not sent.");
	}
}

int receiving_message(int sockfd){
	// hold status of receiving message
	int receiving_status;
	// holds receiving message
	char receiving_message[MAXMESSAGELENGTH + MAXHANDLELENGTH + 3];

	receiving_status = recv(sockfd, receiving_message, (MAXMESSAGELENGTH + MAXHANDLELENGTH + 2), 0);
	printf("%s\n", receiving_message);
	memset(&receiving_message, 0, sizeof(receiving_message));

	if(receiving_status <= 0)
		return 0;
	else
		return 1;
}

// Description: main function where everything is implemented
// Argument: strings for server and port passed from command line
int main(int argc, char *argv[]){
	
	// holds socket descriptor setup by connection_setup()
	int sockfd = connection_setup(argv[1], argv[2]);
	
	// for messaging
	char handle[MAXHANDLELENGTH + 1];
	// char rec_message[MAXMESSAGELENGTH + 1];
	char *send_message;
	// variable to hold combined message
	char *send_handle_message  = malloc((MAXMESSAGELENGTH + MAXHANDLELENGTH + 3) *sizeof(char));
	// variable to keep track of connection status
	int connection_flag = 1;

	// check to ensure 3 arguments passed in thru command line
	// if not, throws error
	if(argc != 3){
		fprintf(stderr, "usage: client hostname\n");
		exit(1);
	}

	// get user's handle
	get_handle(handle);

	// loop for send/receive functionality
	while(connection_flag){
		// get message to send
		send_message = get_message(handle);

		// close connection when specified by user
		if(!(strncmp(send_message, "\\quit", 5))){
			close(sockfd);
			break;
		}

		// build handle + message to send out
		build_handle_message(handle, send_message, send_handle_message);

		// freeing up mem allocated to send message
		free(send_message);

		sending_message(sockfd, send_handle_message);

		connection_flag = receiving_message(sockfd);
	}

	close(sockfd);

	free(send_handle_message);

	return 0;

}