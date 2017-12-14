/*****************************************
* Author: Kyle De Laurell
* Date: 10/19/2017
* Description: This is the source file
* that sends messages back and forth with
* another host.
*******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int main(int argc, char *argv[]){
  //intializes variables
  int socketFD;
  int portNumber;
  int charsWritten;
  int messageLength;
  int charsRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverHostInfo;
  char buffer[1000];
  char username[10];
  char message[501];
  char clearChar;
  char Totalmessage[515];
  int result = 0;
  int result2 = 1;

  //Checks to ensure that program usage is correct
  if(argc < 3){
    fprintf(stderr, "USAGE: %s host port\n", argv[0]);
    exit(0);
  }


  //get clients username
  printf("Please enter your username: ");
  scanf ("%s", username);
  if (strlen(username) > 10){
    fprintf(stderr, "username can only be up to 10 characters long\n");
    exit(0);
  }

  //get initial message
  printf("\n%s > ", username);
  clearChar = getchar();
  scanf ("%[^\n]%*c", message);
  if (strlen(username) > 10){
    fprintf(stderr, "message can only be up to 500 characters long\n");
    exit(0);
  }

  //combines username and message
  strcat(Totalmessage, username);
  strcat(Totalmessage, " > ");
  strcat(Totalmessage, message);
  messageLength = strlen(Totalmessage);


  // Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[2]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverHostInfo = gethostbyname(argv[1]);
	if (serverHostInfo == NULL) {
    fprintf(stderr, "CLIENT: ERROR, no such host\n");
    exit(0);
  }
	memcpy((char*)serverHostInfo->h_addr_list[0], (char*)&serverAddress.sin_addr.s_addr, serverHostInfo->h_length);

  //sets up socket
  if((socketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("socket");
    exit(2);
  }

  //connects to socket
  if(connect(socketFD, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
    perror("connect");
    exit(1);
  }

  //sends initial message
  charsWritten = send(socketFD, Totalmessage, strlen(Totalmessage), 0); // Write to the server
  if (charsWritten < 0) {
    perror("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(username)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  //checks to see if terminate message was sent
  result = strcmp(message, "\\quit");
  //printf("%d\n", result);

  //loops until terminate message is sent or received
  while (result != 0 && result2 != 0){

    // Get return message from server
    memset(buffer, '\0', sizeof(buffer));
    charsRead = recv(socketFD, buffer, sizeof(buffer) , 0);
    if (charsRead < 0) {
      perror("CLIENT: ERROR reading from socket");
    }

    //checks to see if quit message was received
    result2 = strcmp(buffer, "\\quit");
    //printf("RESULT 2: %d\n", result2);
    if (result2 != 0){
      printf("%s\n", buffer);

      //combines username and message
      memset(message, '\0', sizeof(message));
      memset(Totalmessage, '\0', sizeof(Totalmessage));


      printf("%s > ", username);
      scanf ("%[^\n]%*c", message);
      //printf("DONE\n");

      if (strlen(message) > 500){
        fprintf(stderr, "message can only be up to 500 characters long\n");
        exit(0);
      }

      //checks to see if quit message was sent
      result = strcmp(message, "\\quit");
      //printf("%d\n", result);
      if (result != 0){
        strcat(Totalmessage, username);
        strcat(Totalmessage, " > ");
        strcat(Totalmessage, message);
        messageLength = strlen(Totalmessage);


        charsWritten = send(socketFD, Totalmessage, messageLength, 0); // Write to the server
        if (charsWritten < 0) {
          perror("CLIENT: ERROR writing to socket");
        }
        if (charsWritten < messageLength){
          printf("CLIENT: WARNING: Not all data written to socket!\n");
        }
        // Send message to server
        memset(Totalmessage, '\0', sizeof(Totalmessage));
        memset(message, '\0', sizeof(message));
      }
    }
  }

  //sends final quit message if user was the one to quit chat session
  charsWritten = send(socketFD, message, strlen(message), 0); // Write to the server
  if (charsWritten < 0) {
    perror("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(Totalmessage)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

  //notifies user who ended chat session
  if(result == 0){
    printf("You have ended the chat session\n");
  } else{
    printf("Your partner has ended the chat session\n");
  }

  // Closes the socket
  close(socketFD);

  return 0;
}
