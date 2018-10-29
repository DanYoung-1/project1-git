#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

char quitString[] = "\\quit";

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    char hostname[16];
    char sep[] = "> ";
    char buf[99];
    char message[999];
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    printf("Enter hostname:");
    memset(hostname, '\0', sizeof(hostname)); // Clear out the buffer array
    scanf("%s", hostname);
    
    printf("Enter portnumber:");
    scanf("%d", &portno);
    
    do {
        printf("Enter user handle:"); fflush(stdout);
        memset(buf, '\0', sizeof(buf)); // Clear out the buffer array
        scanf("%s", buf);
    } while(strlen(buf) > 10);
    
    char *user = malloc(strlen(buf) + 1); // +1 for the null-terminator
    strcpy(user, buf);

    // get connection variables
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        close(sockfd);
        error("ERROR opening socket");
    }
    
    server = gethostbyname("localhost");
    if (server == NULL) {
        close(sockfd);
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }
    
    // build server address struct
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    
    // try to connect
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        close(sockfd);
        error("ERROR connecting");
    }
    
    // build the initial message
    char buffer[70070];
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof buffer,"%s%s%d", user, sep, portno);
    // write the initial message
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) {
        close(sockfd);
        error("ERROR writing to socket");
    }
    
    do { // chat loop
        memset(buffer, '\0',sizeof(buffer));
        // wait for and read the server message
        n = read(sockfd,buffer,70069);
        if (n < 0) {
            close(sockfd);
            error("ERROR reading from socket");
        }
        printf("%s\n", buffer);
        memset(buffer, '\0', sizeof(buffer));
        
        // build a message with handle, < 500 characters
        do {
            printf("%s",user);
            printf("> ");
            memset(message, '\0', sizeof(buf)); // Clear out the buffer array
            scanf("%s", message);
        } while(strlen(message) > 500);
        if (strcmp(message,quitString) == 0) { // if the message is quit string
            close(sockfd);
            break; // break from chat loop
        }
        snprintf(buffer, sizeof(buffer),"%s%s%s", user, sep, message); // buffer is user + sep + message
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) {
            close(sockfd);
            error("ERROR writing to socket");
        }
        
    } while (1);
    return 0;
}
