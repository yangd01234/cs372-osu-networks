/****************************************************************
AUTHOR:     Derek Yang
COURSE:     CS 344
TERM:       Fall 2018
FILENAME:   otp_dec_d.c


DESCRIPTION:  Server side, connects to otp_enc and performs
a one-time pad style decryption.  Uses modular addition/subtraction
to decrypt the incoming text file and sends back the decrypted text.
*****************************************************************/

/*
Notes:
set up particular server to run in the background at port using & so you can keep command line
gcc -o client client.c
gcc -o server server.c
server 51717 &
client localhost 51717

NOTE: we may need to keep the server into the loop so that it lives forever.
look at the commented out section below
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

/***********************************
NAME: convIntChar(int intIn)
DESCRIPTION: Converst an integer to a
char from position of char_list[]
***********************************/
char convIntChar(int intIn){
	char* char_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	return char_list[intIn];
}

/***********************************
NAME: convCharInt(char charIn)
DESCRIPTION: Converst a character to an
integer from position of char_list[]
***********************************/
int convCharInt(char charIn){
	int i;
	char* char_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	for(i = 0; i<27; i++){
		if(charIn == char_list[i]){
			return i;
		}
	}
}

/***********************************************
NAME: dencryption(char plainText[], char key[])
DESCRIPTION: decrypts plainText using key and
modular subtraction.  Edits plainText directly.
***********************************************/
void dencryption(char plainText[], char key[]){
	int len, i, keyConv, plainTextConv, outputEnc;

	len = strlen(plainText)-1;

	for(i = 0; i<len; i++){
		keyConv = convCharInt(key[i]);
		plainTextConv = convCharInt(plainText[i]);
		outputEnc = (plainTextConv - keyConv) % 27; //modulo 27 because of space
		if(outputEnc < 0){outputEnc += 27;}
		plainText[i] = convIntChar(outputEnc);
	}
	plainText[i] = '\0';

}

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten, keyRead, terminalLocation, delivered;
	int handshakeWrite = 0, handshakeReceive = 1;
	socklen_t sizeOfClientInfo;
	char buffer[80000];
	char buffer_text[80000];
	char completeBuffer[80000];
	char completeBufferText[80000];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket, specify what kind of socket
	serverAddress.sin_port = htons(portNumber); // Store the port number into something the computer can use
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process from any ip address

	// Set up the socket
	/*same as client side*/
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	//NEXT STEP: bind the socket, enables system to listen and respond from port
	// Enable the socket to begin listening
	/*listenSocketFD = socket we have previously created
	(struct sockaddr *)&serverAddress = the network address struct which identifies which port to use
	sizeof(serverAddress)) < 0 = length of the address structure
	9/10 times if it doesn't suceed is it because the port is in use!
	*/
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");

	/*
	Note: data won't start flowing until accept is called.
	Listen will queue up the particular socket.
	5 = how many different connections should the socket be able to connect at once.
	*/
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	int pid;
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	//////////////START OF WHILE LOOP TO KEEP SERVER ON FOREVER//////////////////////////
while (1) {
	// Accept a connection, blocking if one is not available until one connects
	
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) {error("ERROR on accept");}

	pid = fork();
//testing out switch statement...otp_enc_d has non switch to see if it is faster...
	switch(pid) {
		case -1:
		error("Error with fork...");
		break;
		case 0:
					//while loop
			memset(completeBuffer, '\0', sizeof(buffer_text));
			memset(buffer, '\0', sizeof(buffer));


			close(listenSocketFD);

		// note: Use these two to complete the text.  From lec 4.2
		// char completeBuffer[80000];
		// char completeBufferText[80000];
			//key
		while(strstr(buffer, "@@") == NULL){
			memset(buffer, '\0', sizeof(buffer));
			charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer),0);
			strcat(completeBuffer,buffer);
			if (charsRead == -1) { printf("charsRead == -1\n"); break; }
			if (charsRead == 0) { printf("charsRead == 0\n"); break; }
		}
												//perror("Works here server\n");
												//printf("buffer %s\n", buffer);
			terminalLocation = strstr(completeBuffer, "@@") - completeBuffer; // Where is the terminal (taken from lecture 4.2)
			completeBuffer[terminalLocation] = '\0'; // End the string early to wipe out the terminal (take from lecture 4.2)
			//printf("Receive completeBuffer: %s\n", completeBuffer);
			if(charsRead < 0){error("SOCKET READ ERROR");}

		// note: Use these two to complete the text.  From lec 4.2
		// char completeBuffer[80000];
		// char completeBufferText[80000];
			//text
		while(strstr(buffer_text, "@@") == NULL){
			memset(buffer_text, '\0', sizeof(buffer_text));
			keyRead = recv(establishedConnectionFD, buffer_text, sizeof(buffer_text), 0);
			strcat(completeBufferText, buffer_text);
			if (keyRead == -1) { printf("keyRead == -1\n"); break; }
			if (keyRead == 0) { printf("keyRead == 0\n"); break; }
		}
			terminalLocation = strstr(completeBufferText, "@@") - completeBufferText; // Where is the terminal (taken from lecture 4.2)
			completeBufferText[terminalLocation] = '\0'; // End the string early to wipe out the terminal (take from lecture 4.2)
			//printf("Receive buffer_text: %s\n", completeBufferText);
			if(keyRead < 0){error("SOCKET READ ERROR");}


			int i, count;
			for (i = 0; i<sizeof(buffer)+1; i++){
				if(buffer[i] == '\n'){
					count++;
				}
			}

			//printf("Number of newlines: %d\n", count);
			//printf("buffer Server: %s len: %d\n", buffer, strlen(buffer));
			//printf("buffer_text Server: %s len: %d\n", buffer_text, strlen(buffer_text));
			dencryption(buffer_text, buffer);
				//append control code per lect 4.2 slide 18
		//COPY
			buffer_text[strcspn(buffer_text, "\n")] = 0;
			strcat(buffer_text, "@@");
			//printf("Encrypted: %s\n", buffer_text);

			delivered = 0;
		while(delivered < sizeof(buffer_text)){
			charsWritten = send(establishedConnectionFD, buffer_text, sizeof(buffer_text)-delivered, 0);
			delivered = delivered+charsWritten;
		}
			//printf("Num delivered: %d\n", delivered);
			if (charsWritten < 0) error("Error reading server");
			//printf("size: %d\n", sizeof(buffer_text));




			if(charsWritten < 0){error("SOCKET READ ERROR");}

			close(establishedConnectionFD);
			close(listenSocketFD);
			exit(0);
		break;

		default:
			close(establishedConnectionFD);
		break;
		}

			}
		return 0;
}


/* Two methods to give multiple clients at a time

Concurrent Servers
Apparent concurrency (fake): single thread of execution using select() command and non-blocking I/O
You switch to another connection whenver an I/O request would block.
Maximizes CPU utilization
Increases throughout

Real concurrency: multiple threads of execution, or multiple processes, each with one thread
four ways to create real concurrency:
-create one process per client connection
Fork Solution #1
for a new process to handle every connection

-create a pool of available processes before clients connect
Create a set of processes that are re-usable and re-purposable
This will get rid of the fork issue.

-use only one process, but create one thread per client connection
-use only one process, but create a pool of available threads before clients connect
Threads allow multiple concurrent execution contexts within a single process




select() - used for server like applications that have many communication channels open at once
will return when any file descriptor is readable or writable

int select(
	int nfds, //highest # file descriptor + 1
	fd_set* readfds,    //bitmasks for FD of interest
	fd_set* writefds,
	fd_set* errorfds,
	struct timeval* timeout     //
)
*/