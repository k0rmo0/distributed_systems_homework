#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <fcntl.h>

void DieWithError(char *errorMessage)
{
  perror (errorMessage);
  exit(1);
}

int main(int argc, char* argv[]) {
    int socketDescriptor, bytesRead = 0;
    struct sockaddr_in servAddr;
    char *fileNameString;
    char fileBuffer[512];
    FILE* fileToUpload;

    if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
            argv[0]);
        exit(1);
    }

    if ((socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("socket () failed");

    printf("[+]Client socket created successfully.\n");

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(8888);

    if (connect(socketDescriptor, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        DieWithError("connect () failed");

    printf("[+]Connected successfully.\n");

    fileBuffer[0] = strlen(argv[1]);
    // strcat(fileBuffer, argv[1]);
    strcpy(fileBuffer + 1, argv[1]);

    int response = send(socketDescriptor, fileBuffer, sizeof(fileBuffer), 0);
    if(response < 0)
		printf("Error sending file name information\n");
    
    fileToUpload = fopen(argv[1], "rb");
    if (fileToUpload == NULL) {
        printf("Error opening file");
        exit(1);
    }
    printf("File opened successfully!\n");

    while(!feof(fileToUpload)) {
        bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), fileToUpload);

        if (send(socketDescriptor, fileBuffer, bytesRead, 0) < 0)
            DieWithError("Error sending file");
    }
    fclose(fileToUpload);
    close(socketDescriptor);
    exit(0);
}