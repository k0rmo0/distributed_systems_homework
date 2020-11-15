#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void writeFile(int socketDescriptor);
void DieWithError(char *errorMessage);
void *ThreadMain(void *threadArgs);

struct ThreadArgs{
    int clientSocketDescriptor;
};

int main(int argc, char const *argv[])
{
    int serverSocket, clientSocket, addr_size;
    pthread_t threadID;
    struct sockaddr_in server, client;
    struct ThreadArgs *threadArgs;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError(" socket () failed") ;

    printf("[+]Server socket created successfully.\n");

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;    
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
        DieWithError ("bind () failed");

    printf("[+]Binding successfull.\n");

    if (listen(serverSocket, 5) < 0)
        DieWithError("listen() failed") ;

    printf("[+]Listening....\n");

    addr_size = sizeof(struct sockaddr_in);
    
    for (;;) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr *) &client, (socklen_t*)&addr_size)) < 0)
            DieWithError( "accept () failed");
    
        printf("Handling client %s\n", inet_ntoa(client.sin_addr));

        if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
            DieWithError("malloc() failed");
        threadArgs -> clientSocketDescriptor = clientSocket;

        if (pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs) != 0)
            DieWithError("pthread_create() failed");
        printf("with thread %ld\n", (long int) threadID);
    }
    exit(0);
}


void DieWithError(char *errorMessage)
{
  perror (errorMessage);
  exit(1);
}

void *ThreadMain(void *threadArgs)
{
  int clientSocket;        /* Socket descriptor for client connection */
  
  /* Guarantees that thread resources are deallocated upon return */
  pthread_detach(pthread_self());

  /* Extract socket file descriptor from argument */
  clientSocket = ((struct ThreadArgs *) threadArgs) -> clientSocketDescriptor;
  free(threadArgs); /* Deallocate memory for argument */ 

  writeFile(clientSocket);

  return (NULL);
}

void writeFile(int socketDescriptor){
  int bytesReceived = 0;
  FILE *uploadedFile;
  char buffer[512];

if (recv(socketDescriptor, buffer, sizeof(buffer), 0) < 0)
  DieWithError ("Error receiving name");

int fileNameSize = buffer[0];
char fileName[fileNameSize];

strcpy(fileName, buffer+1);

uploadedFile = fopen(fileName, "wb"); //overwrite if exists and create if not
 if(NULL == uploadedFile){
    DieWithError("Error creating file");
  }
  
  printf("Created filename: %s\n", fileName);
  puts("waiting");
  sleep(10);

  while((bytesReceived=recv(socketDescriptor, buffer, sizeof(buffer),0)) > 0){
    printf("Received %d bytes\n", bytesReceived);
    fwrite(buffer, 1, bytesReceived, uploadedFile);
  }

  if(bytesReceived == 0) {
    puts("Client disconnected");
  } else if(bytesReceived == -1) {
    DieWithError("Error recieving file");
  }
  close(socketDescriptor);
  fclose(uploadedFile);
  return;
}