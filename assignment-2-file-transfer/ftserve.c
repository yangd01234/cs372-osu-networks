/* Author: Derek Yang
 * Program Name: chatclient.c
 * Description: chatclient.c creates a client to connect with
 * chatserve.py.  In conjunction, both programs work to
 * create a client and server for a chat room.  Chat client will
 * terminate if either server or client sends the command \quit.
 * The user will still be able to connect to the server using another
 * instance of the client.
 *
 * Last Modified: 2/10/2019 at 4PM PT
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/* verifies input string is less than max, loops until correct len is reached
Pre-condition:  Needs a string to modify and check
Post-condition: Modifies string within limit.
 */
void verifyInputLen(char* input, int len, int limit, char* subject){
    while(1){
        // clear out handle
        memset(input,0, sizeof(input));
        fgets(input, len, stdin);

        if(strlen(input) > limit+1){
            printf("Please enter a(n) %s under %d chars:\n", subject, limit);
            continue;
        } else {
            // clear out newline that fgets adds
            input[strcspn(input, "\n")] = 0;
            break;
        }
    }
}

/* helper function to send messages
Pre-condition:  Client is ready to send information and
                server is ready to receive.
Post-condition: Client sends information and
                server receives information from client.
 */
void sendMessage(int sockfd, char* handle, char* sendMsg){
    char handleFormat[2] = "> ";
    memset(sendMsg, 0, sizeof(sendMsg));

    printf("Input message to send-> ");
    verifyInputLen(sendMsg, 500, 500,"message");

    char *new_str;
    new_str = malloc(strlen(sendMsg)+strlen(handle)+strlen(handleFormat)+1);
    new_str[0] = '\0';
    strcat(new_str, handle);
    strcat(new_str,handleFormat);
    strcat(new_str, sendMsg);

    send(sockfd, new_str, strlen(new_str), 0);
}

/* helper function to receive messages and clears recvmsg
Receives messages from the server and closes connection if \quit is triggered.
Pre-condition:  Client is ready to receive information and
                server sends out information to client.
Post-condition: Client receives information and prints info.
*/
void receiveMessage(int sockfd, char* recvMsg){
    memset(recvMsg, 0, sizeof(recvMsg));
    if(recv(sockfd, recvMsg, 500, 0)) {
        printf("%s\n", recvMsg);
    }

}


/*
chat loop to go through and keep sending/receiving information.
 This function will first initiate contact with the server
 and send a message dicated by the user.
Pre-condition:  Server is ready to receive initial message.
Post-condition: Server and client exchange messages.
*/
void chatLoop(int sockfd, char* handle, struct addrinfo* res){
    char buffer[500];

    while(1){
        //Check to make sure socket connected
        if(connect(sockfd, res->ai_addr, res->ai_addrlen)){
            // sends the initial message
            sendMessage(sockfd, handle, buffer);
            if(strncmp(buffer, "\\quit", 5) == 0){
                printf("CLIENT \\quit command was triggered\n");
                break;
            }

            // receives message from server
            receiveMessage(sockfd, buffer);

            //cited: https://stackoverflow.com/questions/19724450/c-language-how-to-get-the-remaining-string-after-using-strtok-once
            char * const sep = strchr(buffer,' ');
            *sep = '\0';
            if(strncmp(sep+1, "\\quit", 5) == 0){
                printf("SERVER \\quit command was triggered\n");
                break;
            }
        }
    }
    close(sockfd);
}

int main(int argc, char *argv[]) {
    int handShakeFlag = 0;
    //argv[1] = hostname, argv[2] = port
    printf("==Welcome to Chat Client==\n");
    char handle[2000];


    // check correct # of arguments
    if(argc != 3){
        printf("Required args: server port\n");
        exit(1);
    }

    // display server info to user
    printf("Server A host name: %s\n",argv[1]);
    printf("Server A port: %s\n", argv[2]);

    // get address information, taken from beej's guide
    struct addrinfo addrinfo;
    struct addrinfo *res;
    memset(&addrinfo, '\0', sizeof(addrinfo));

    //Create a network capable socket
    addrinfo.ai_family = AF_INET;
    addrinfo.ai_socktype = SOCK_STREAM;

    // CITED: Beej's guide section 5 system calls or bust http://beej.us/guide/bgnet/examples/showip.c
    int status = getaddrinfo(argv[1], argv[2], &addrinfo, &res);
    int sockfd;

    //create client socket and verify
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        printf("Socket creation failed.\n\n");
        exit(1);
    } else {
        printf("Socket created ready to connect.\n\n");
    }

    // get user handle and loops if exceeded number of chars
    printf("Please input a user handle under 10 chars-> ");
    verifyInputLen(handle, 2000, 10,"handle");

    chatLoop(sockfd, handle, res);
    return 0;
}