/* Author: Derek Yang
 * Course: CS372 Intro to Networks
 * Program Name: ftserver.c
 * Description:
 * ftserver waits on determined port for data
 * connection from client.  ftserver then receives commands
 * from client, either -l to list files or -g to get file.
 * If file is not found using -g, ftserver sends a fail key
 * to the client.
 * Last Modified: 3/7/2019 at 11PM PT
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>

// Error function used for reporting issues
void error(const char *msg) { perror(msg); exit(1); }

/******************************************************************************************
 * Summary:
 *      Concatenates two strings together.
 * Description:
 *      Allocates memory for a new string and concatenates
 *      str1 and str2 together, then returns the result
 * Parameters:
 *      str1 (const char *string): start of new string
 *      str2 (const char *string): end of new string
 * Return:
 *      res (char*)
 * cited:
 *      https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c
 */
char* concatenate(const char *str1, const char *str2){
    char* res = malloc(strlen(str1) + strlen(str2)+1);
    strcpy(res, str1);
    strcat(res, str2);
    return res;
}


/******************************************************************************************
 * Summary:
 *      Sets the connection
 * Description:
 *      Uses the socket api to set a connection with the received sockfd and addressinfo.
 *      exits the program if the connection failed with status code 1.
 * Parameters:
 *      sockfd (int): socket file descriptor
 *      res (struct addrinfo* res): the address information
 * Return:
 *      void
 * cited:
 *      Beej's guide
 */
void setConnection(int sockfd, struct addrinfo* res){
    if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0){
        printf("connection failed\n");
        exit(1);
    }else {
        printf("connection success\n");
    };
}

/******************************************************************************************
 * Summary:
 *      Creates and binds a new socket
 * Description:
 *      Sets a new socket using the address information in res.  Then returns the new socket
 *      upon successful creation.  If the socket creation fails, the program will exit
 *      with status code 1.
 * Parameters:
 *      res (struct addrinfo* res): the address information
 * Return:
 *      sockfd (int)
 * cited:
 *      Beej's guide
 */
int setSocket(struct addrinfo * res){
    //create client socket and verify
    int sockfd;
    sockfd = socket((struct addrinfo *)res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd == -1){
        printf("Socket creation failed.\n");
        exit(1);
    }
    return sockfd;
}

/******************************************************************************************
 * Summary:
 *      Bind the socket
 * Description:
 *      Binds the newly created socket using the address information in res.  Closes socket
 *      and exits the program with exit status 1 if binding failed.
 * Parameters:
 *      sockfd (int): socket file descriptor
 * Return:
 *      void
 * cited:
 *      Beej's guide
 */
void bindSocket(int sockfd, struct addrinfo* res){
    //bind the socket
    if(bind(sockfd, res->ai_addr, res->ai_addrlen) == -1){
        printf("binding didn't work\n");
        close(sockfd);
        exit(1);
    } else {
        printf("binding success\n");
    }
}

/******************************************************************************************
 * Summary:
 *      Starts listening for connection
 * Description:
 *      Uses the newly created and bound socket to start listening for an incoming
 *      connection from the client.
 * Parameters:
 *      sockfd (int): socket file descriptor
 * Return:
 *      void
 * cited:
 *      Beej's guide
 */
void listenSocket(int sockfd){
    //start listening
    if(listen(sockfd, 5) == -1){
        close(sockfd);
        printf("listening didn't work\n");
        exit(1);
    } else {
        printf("now listening....\n\n");
    }
}


/******************************************************************************************
 * Summary:
 *      Check if there is a space in the string
 * Description:
 *      Helper function to validate if the string has spaces.  The helper function is
 *      used to assist the tokenizer function with the deliminator ' '.  Returns
 *      1 on success and 0 on failure.
 * Parameters:
 *      string (char*): Source string to check for spaces
 * Return:
 *      1 (int)
 *      0 (int)
 * cited:
 *      None
 */
int stringSpace(char* string){
    int i;
    for(i = 0; i<strlen(string); i++){
        if(string[i] == ' '){
            return 1;
        }
    }
    return 0;
}

/******************************************************************************************
 * Summary:
 *      Tokenize a string with deliminator
 * Description:
 *      Tokenizes an incoming string using the declared deliminator.  This allows each
 *      piece of data to be separated into individual strings for use later on in the
 *      program.  The program will return an array of strings with each command.
 * Parameters:
 *      str (char*): string to be tokenized
 *      delim (const char): deliminator
 * return:
 *      result (char**)
 * cited:
 *      https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
 */
char** tokenizestr(char* str, const char delim){
    char** result = 0;
    size_t count = 0;
    char* temp = str;
    char* last = 0;
    char deliminator[2];
    deliminator[0] = delim;
    deliminator[1] = 0;

    while(*temp){
        if(delim == *temp){
            count++;
            last = temp;
        }
        temp++;
    }

    count += last < (str + strlen(str)-1);
    count++;

    result = malloc(sizeof(char*) * count);

    if(result){
        size_t i = 0;
        char* tkn = strtok(str, deliminator);

        while(tkn){
            *(result + i++) = strdup(tkn);
            tkn = strtok(0, deliminator);
        }
        *(result + i) = 0;
    }
    return result;
}


/******************************************************************************************
 * Summary:
 *      Frees array of strings
 * Description:
 *      Frees strings within our declared array of strings
 * Parameters:
 *      str (char*): string to be cleared
 *      size (int): size of array
 * return:
 *      void
 * cited:
 *      https://stackoverflow.com/questions/4733649/freeing-memory-which-has-been-allocated-to-an-array-of-char-pointers-strings
 */
void freeStringArray(char** arr, int size){
    int i;
    for (i = 0; i<size; i++){
        free(arr[i]);
    }
    free(arr);
}

/******************************************************************************************
 * Summary:
 *      Gathers address information
 * Description:
 *      Gathers address information needed to create a new socket.  If the destination
 *      is set to NULL, the function will set addres info for local so that it can
 *      accept connections.  Otherwise, it sets the address information so that it can
 *      send information to an external client.
 * Parameters:
 *      port (char*): initial port string from argv
 *      dest (char*): The destination, if set to NULL, then use local address information
 * Return:
 *      res (struct addrinfo*)
 * cited:
 *      Beej's guide System Calls or Bust
 */
struct addrinfo* initAddr(char* port, char* dest){
    // get address information, taken from beej's guide
    int getaddrinfoflag;
    struct addrinfo addrinfo;
    struct addrinfo *res;
    memset(&addrinfo, '\0', sizeof(addrinfo));

    //Create a network capable socket
    addrinfo.ai_family = AF_INET;
    addrinfo.ai_socktype = SOCK_STREAM;

    if(dest == NULL){
        addrinfo.ai_flags = AI_PASSIVE;  //bind and accept connections
        getaddrinfoflag = getaddrinfo(NULL, port, &addrinfo, &res)< 0;
    } else {
        getaddrinfoflag = getaddrinfo(dest, port, &addrinfo, &res)< 0;
    }

    if(getaddrinfoflag){
        printf("Port creation failed\n");
        exit(1);
    }

    return res;
}

/******************************************************************************************
 * Summary:
 *      Handles the -l command
 * Description:
 *      Function to handle the -l command.  Sets up a new connection for the dataport.
 *      Reads each item from the server's directory and concatenates it to a buffer.
 *      Sends the buffer over to the client in the established dataport.
 * Parameters:
 *      sockfd (int): socket file descriptor
 *      dataConnection (char**): array of strings for each of the commands and data
 *                               port information.
 * Return:
 *      void
 * cited:
 *      https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 *      Beej's guide
 */
void listCommand(int sockfd, char** dataConnection){

    DIR *d;
    struct dirent *dir;
    char buffer[500000];

    //set up connection, taken from Beej's guide
    struct addrinfo addrinfo;
    memset(&addrinfo, '\0', sizeof(addrinfo));
    memset(&buffer, '\0', sizeof(buffer));
    printf("Connection from: %s\n", dataConnection[2]);
    printf("List directory requested on port: %s\n", dataConnection[1]);
    struct addrinfo* res = initAddr(dataConnection[1], dataConnection[2]);
    int datasockfd = setSocket(res);
    setConnection(datasockfd, res);

    d = opendir(".");
    if (d){
        while((dir = readdir(d)) != NULL) {
            char* s = concatenate(buffer, dir->d_name);
            s = concatenate(s, "\n");
            strcpy(buffer,s);
            free(s);
        }
        printf("Sending directory contents to %s:%s\n", dataConnection[2], dataConnection[1]);
        send(datasockfd, buffer, strlen(buffer)+1, 0);

        closedir(d);
        close(datasockfd);
    }
}


/******************************************************************************************
 * Summary:
 *      Handles the -g command
 * Description:
 *      Function to handle the -g command.  Sets up a new connection for the dataport.
 *      Receives a command to find a file.  If the file exists, read the contents of the
 *      file into the stream.  Client then receives the contents of the file.
 *      If the file does not exist, send a fail key to the server to notify that the
 *      file does not exist.
 * Parameters:
 *      sockfd (int): socket file descriptor
 *      dataConnection (char**): array of strings for each of the commands and data
 *                               port information.
 * Return:
 *      void
 * cited:
 *      https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 *      https://stackoverflow.com/questions/9482770/checking-a-file-exists-in-c
 *      Beej's guide
 */
void getFileCommand(int sockfd, char** dataConnection){
    DIR *d;
    struct dirent *dir;
    char failKey[30] = "6B1ACA4C28E2B1AE3F8C23D62D26B";

    //set up connection, taken from Beej's guide
    struct addrinfo addrinfo;
    memset(&addrinfo, '\0', sizeof(addrinfo));

    printf("Connection from: %s\n", dataConnection[3]);
    printf("File \"%s\" requested on port: %s\n", dataConnection[1], dataConnection[2]);
    struct addrinfo* res = initAddr(dataConnection[2], dataConnection[3]);
    int datasockfd = setSocket(res);
    setConnection(datasockfd, res);

    //If the file exists, open the file
    FILE* f;
    f = fopen(dataConnection[1], "r");
    if (f){
        //file exists, send contents of file
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *buffer = malloc(fsize+1);
        fread(buffer, fsize, 1, f);
        fclose(f);
        buffer[fsize] = 0;
        printf("Sending \"%s\" to: %s:%s\n", dataConnection[1], dataConnection[3], dataConnection[2]);
        send(datasockfd, buffer, strlen(buffer)+1, 0);
    } else {
        //file doesn't exist, sent fail key
        printf("File not found. Sending error message to: %s:%s\n", dataConnection[3], dataConnection[2]);
        send(datasockfd, failKey, strlen(failKey)+1, 0);

    }
    close(datasockfd);
    }

/******************************************************************************************
 * Summary:
 *     Routes the commands to their appropriate helper functions
 * Description:
 *     Receives the commands from client.  Then tokenize the commands so that the
 *     helper functions can parse the information cleanly.  Validates if the command
 *     is either a -l or -g.  Dual validation is used where client and server validate
 *     commands -l and -g.  Returns a 0 if command is invalid and 1 on success.
 * Parameters:
 *     sockfd (int): socket file descriptor
 *     command (char*): long string of commands from the client
 * Return:
 *      1 (int)
 *      0 (int)
 * cited:
 *      None
 */
int routeCommand(int sockfd, char* command){
    char** tokens;
    memset(command, 0, sizeof(command));

    // checks if command is deliminated by spaces and if recv failed
    if((recv(sockfd, command, 500, 0) < 0)){
        printf("Receive failed. Please check connection or command\n");
        return 0;
    }

    //tokenizes command
    tokens = tokenizestr(command, ' ');

    //routes command to appropriate command helper function
    if(strncmp(command, "-l", 2) == 0){
        listCommand(sockfd, tokens);
    } else if ((strncmp(command, "-g", 2) == 0)
            || (stringSpace(command) != 0)){
        getFileCommand(sockfd, tokens);
    } else {
        printf("%s is an invalid command\n", command);
        return 0;
    }
    freeStringArray(tokens,5);
    printf("Data port connection closed\n\n");
    return 1;

}



int main(int argc, char *argv[]) {
    char buffer[500000];
    int pid;
    printf("==ftserve==\n");


    // check correct # of arguments
    if(argc != 2){
        printf("Required arg: <port>\n");
        exit(1);
    }

    // display server info to user
    printf("Server open on port: %s\n",argv[1]);

    // get address information, taken from beej's guide
    struct addrinfo addrinfo;
    memset(&addrinfo, '\0', sizeof(addrinfo));
    struct addrinfo* res = initAddr(argv[1], NULL);

    int sockfd;
    sockfd = setSocket(res);
    bindSocket(sockfd, res);

    listenSocket(sockfd);

    //start waiting from server
    struct sockaddr_storage clientaddr;
    socklen_t clientsize = sizeof(clientaddr);

    while(1){
        int childfd = accept(sockfd, (struct addrinfo *) &clientaddr, &clientsize);
        if (childfd < 0){
            continue;
        } else {
            int command_routed = routeCommand(childfd, buffer);
        }
        close(childfd);
    }

    // per assignment instructions, send ctrl+c for SIGINT to terminate program
    close(sockfd);
    printf("connection closed...\n");
    return 0;
}