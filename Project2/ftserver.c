/*****************************************
* Author: Kyle De Laurell
* Date: 11/26/2017
* Description: This is the source file for
* a program that acts as an ftp server and accepts
* two commands and transfers files
*******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>
#define LIMIT 5


int main(int argc, char *argv[])
{
  //connection P/server variables
  struct sockaddr_storage clientAddress;
  socklen_t sizeOfClientInfo;
  struct sockaddr_in serverAddress;
  int socketFD, connectionP, numConn, charsRead, portNumber;
	char buffer[256];
  char service[200];
  char clientName[200];
  char fullClientName[200];
  char sendData[256];
  pid_t forkedID;

  //connectionQ variables
  struct sockaddr_in dataServerAddress;
  struct hostent* dataServerHostInfo;
  int connectionQ;

  //file variables
  DIR *d;
  struct dirent *dir;
  char fileRequested[200];
  char requestedPort[10];
  int fileFound, charsWritten;
  FILE* reqFile;

  //validates
	if (argc != 2) {
    fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1);
  }

  // Set up the address struct for the server
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));
	portNumber = atoi(argv[1]);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0) perror("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		perror("ERROR on binding");
  numConn = 0;
	listen(socketFD, LIMIT);
  printf("Server open on %d\n", portNumber);

  //accepts connections to LIMIT
while(numConn <= LIMIT){

	sizeOfClientInfo = sizeof(clientAddress);
	connectionP = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
	if (connectionP < 0) perror("ERROR on accept");
  numConn++;
  forkedID = fork();
  //creates new process for accepted connection
  if(forkedID == 0){

    //gets client information and displays it
    memset(buffer, '\0', sizeof(buffer));
    getnameinfo((struct sockaddr *)&clientAddress,
		sizeof(clientAddress),
		clientName,
		sizeof clientName,
		service,
		sizeof service,
		NI_NOFQDN);
    getnameinfo((struct sockaddr *)&clientAddress,
		sizeof(clientAddress),
		fullClientName,
		sizeof fullClientName,
		service,
		sizeof service,
		0);

    printf("Connection from %s\n", clientName);
  	charsRead = recv(connectionP, buffer, sizeof(buffer) - 1, 0);
  	if (charsRead < 0) perror("ERROR reading from socket");

    //executes based on commmand sent
    if(strcmp(buffer, "-1") == 0){
      charsRead = send(connectionP, "accept", 6, 0); // Send success back
    	if (charsRead < 0) perror("ERROR writing to socket");

      charsRead = recv(connectionP, buffer, sizeof(buffer) - 1, 0); // Read the client's message from the socket
    	if (charsRead < 0) perror("ERROR reading from socket");

      printf("List directory requested on port %s.\n", buffer);
      printf("Sending directory contents to %s:%s\n", clientName, buffer);


      // Set up the server address struct
      memset((char*)&dataServerAddress, '\0', sizeof(dataServerAddress));
      dataServerAddress.sin_family = AF_INET;
      dataServerAddress.sin_port = htons(atoi(buffer));
      dataServerHostInfo = gethostbyname(fullClientName);
      if (dataServerHostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
      }
      memcpy((char*)dataServerHostInfo->h_addr_list[0], (char*)&dataServerAddress.sin_addr.s_addr, dataServerHostInfo->h_length);

      //sets up socket
      if((connectionQ = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(2);
      }

      //connects to socket
      if(connect(connectionQ, (struct sockaddr *) &dataServerAddress, sizeof(dataServerAddress)) == -1){
        perror("connect fail");
        exit(1);
      }

      d = opendir(".");
      if(d){
        while ((dir = readdir(d)) != NULL)
        {
          strcpy(sendData, dir->d_name);
          charsRead = send(connectionQ, sendData, sizeof(sendData), 0); // Send success back
        	if (charsRead < 0) perror("ERROR writing to socket");

          charsRead = recv(connectionQ, buffer, sizeof(buffer) - 1, 0); // Read the client's message from the socket
          if (charsRead < 0) perror("ERROR reading from socket");
        }
        closedir(d);
      }

      charsRead = send(connectionP, "complete", 8, 0); // Send success back
      if (charsRead < 0) perror("ERROR writing to socket");

      charsRead = send(connectionQ, "complete", 8, 0); // Send success back
      if (charsRead < 0) perror("ERROR writing to socket");

      charsRead = recv(connectionP, buffer, sizeof(buffer) - 1, 0); // Read the client's message from the socket
      if (charsRead < 0) perror("ERROR reading from socket");

      if(strcmp(buffer, "OK")){
        close(connectionQ);
      }
      else{
        printf("ERROR in Data...Exiting\n");
        close(connectionQ);
      }
    }
    else if(strcmp(buffer, "-g") == 0) {
      charsRead = send(connectionP, "accept", 6, 0); // Send success back
    	if (charsRead < 0) perror("ERROR writing to socket");

      charsRead = recv(connectionP, buffer, sizeof(buffer) - 1, 0); // Read the client's message from the socket
    	if (charsRead < 0) perror("ERROR reading from socket");

      strcpy(requestedPort, buffer);

      // Set up the server address struct to connect to
      memset((char*)&dataServerAddress, '\0', sizeof(dataServerAddress));
      dataServerAddress.sin_family = AF_INET;
      dataServerAddress.sin_port = htons(atoi(buffer));
      dataServerHostInfo = gethostbyname(fullClientName);
      if (dataServerHostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
      }
      memcpy((char*)dataServerHostInfo->h_addr_list[0], (char*)&dataServerAddress.sin_addr.s_addr, dataServerHostInfo->h_length);

      charsRead = send(connectionP, "accept", 6, 0);
      if (charsRead < 0) perror("ERROR writing to socket");

      charsRead = recv(connectionP, buffer, sizeof(buffer) - 1, 0);
      if (charsRead < 0) perror("ERROR reading from socket");

      //checks if file is in directory and send maessage if not found
      strcpy(fileRequested, buffer);
      fileFound = 0;
      d = opendir(".");
      if(d){
        while ((dir = readdir(d)) != NULL)
        {
          strcpy(sendData, dir->d_name);
          if(strcmp(sendData, fileRequested) == 0){
            fileFound = 1;
          }
        }
        closedir(d);
      }

      if(fileFound == 0){
        printf("File not found.\n");
        printf("Sending error message to %s:%s\n", clientName, requestedPort);
        charsRead = send(connectionP, "ERROR! Requested File Not Found", 31, 0);
      }else{
        printf("File \"%s\" requested on port %s.\n", fileRequested, requestedPort);
        printf("Sending \"%s\" contents to %s:%s\n", fileRequested, clientName, requestedPort);
      }


      //sets up socket
      if((connectionQ = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(2);
      }

      //connects to socket
      if(connect(connectionQ, (struct sockaddr *) &dataServerAddress, sizeof(dataServerAddress)) == -1){
        perror("connect fail");
        exit(1);
      }

      if(fileFound == 0){

        charsRead = send(connectionQ, "complete", 8, 0);
        charsRead = send(connectionP, "complete", 8, 0); // Send success back
      }else{
        //opens file and sends to client
        reqFile = fopen(fileRequested, "r+");
        while(fgets(buffer, sizeof(buffer), reqFile) != NULL){
          strcpy(sendData, buffer);
          charsWritten = send(connectionQ, sendData, strlen(sendData), 0);
          if (charsWritten < 0){
            fprintf(stderr, "CLIENT: ERROR writing to socket");
          }
          if (charsWritten < strlen(buffer)){
            fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
          }

          charsRead = recv(connectionQ, buffer, sizeof(buffer) - 1, 0);
          if (charsRead < 0) perror("ERROR reading from socket");
        }
        charsWritten = send(connectionQ, "zyxvw", 5, 0);
        fclose(reqFile);
      }
      close(connectionQ);
      charsWritten = send(connectionP, "complete", 8, 0);
    	close(connectionP);
    }
    else{
      charsRead = send(connectionP, "ERROR! Invalid Command", 25, 0);
      charsRead = recv(connectionQ, buffer, sizeof(buffer), 0);
      charsRead = send(connectionP, "ERROR! Invalid Command", 25, 0);
    	if (charsRead < 0) perror("ERROR writing to socket");
    	close(connectionP);
    }
  }
}
close(socketFD);
  return 0;
}
