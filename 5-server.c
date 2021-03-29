#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <stdbool.h>

#define MAXWRITE 80
#define MAXREAD 80

int create_listener(int listen_port)
{
    struct sockaddr_in6 servaddr6;
    int listenfd;

    // luodaan kuunteleva pistoke
    if ((listenfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    memset(&servaddr6, 0, sizeof(servaddr6));
    servaddr6.sin6_family = AF_INET6;
    servaddr6.sin6_addr = in6addr_any;
    servaddr6.sin6_port = htons(listen_port);

    if (bind(listenfd, (struct sockaddr *) &servaddr6,
        sizeof(servaddr6)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(listenfd, 1) < 0) {
        perror("listen");
        return -1;
    }
    return listenfd;
}

int write_block(int sockfd, uint32_t blockSize)
{
    char* buf = (char*)malloc(blockSize);
    size_t n = 0;
    size_t total = 0;

    while((n = write(sockfd, &buf[total], blockSize-total)) > 0) {
        if(n < 0){
            perror("read error");
            return -1;
        }
        total += n;
    }
    printf("wrote %d bytes\n", total);

    free(buf);
    return 1;
}

int initial()
{
    int sockfd;
    struct sockaddr_in servaddr;
    // Open socket and check for errors
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }

    // Define address to connect to and check for format errors
    const char *address = "130.233.154.208";

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(5000);

    if(inet_pton(AF_INET, address, &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s\n", address);
        return 1;
    }

    // Connect to defined address and check for connection errors
    if(connect(sockfd,
                (struct sockaddr *) &servaddr,
                sizeof(servaddr)) < 0) {
        perror("connect error");
        return 1;
    }


    int n = 0;
    char str[] = "729637\n5-server\n";

    printf("WRITING INITIAL MESSAGE TO A\n");
    if((n = write(sockfd, str, strlen(str))) < 0) {
        perror("write error");
        return 1;
    }
    printf("wrote %d bytes\n", n);

    return sockfd;
}


int main()
{
    signal(SIGPIPE, SIG_IGN);

    int connfd;
    int sockfd = initial();

    char sendline[MAXWRITE];    // Write buffer
    char recvline[MAXREAD + 1]; // Read buffer

    int listen_port = 6451;

    struct sockaddr_in6 cliaddr;
    struct sockaddr_in own;

    socklen_t ownlen = sizeof(struct sockaddr_in);

    if (getsockname(sockfd, (struct sockaddr *)&own, &ownlen) < 0) {
        perror("getsockname error: ");
    }
    char outbuf[16];
    inet_ntop(AF_INET, &(own.sin_addr), outbuf, sizeof(outbuf));
    sprintf(sendline, "SERV %s %d\n", outbuf, listen_port);

    int n = 0;

    while(1) {
        memset(&cliaddr, 0, sizeof(cliaddr));

        if((n = read(sockfd, recvline, MAXREAD)) < 0) {
            perror("write error");
            return 1;
        }
        recvline[n] = 0;
        // Check if complete
        if(strncmp(recvline, "OK", 2) == 0) break;
        if(strncmp(recvline, "FAIL", 4) == 0) break;

        int listenfd = create_listener(listen_port);

        printf("WRITING OWN ADDR AND PORT\n");
        if((n = write(sockfd, sendline, strlen(sendline))) < 0) {
            perror("write error");
            return 1;
        }
        printf("wrote: %s %d bytes\n", sendline, n);
        memset(sendline, 0, MAXWRITE);

        socklen_t len = sizeof(cliaddr);

        printf("LISTENING...\n");
        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr,
                             &len)) < 0) {
            perror("accept");
            return -1;
        }
        printf("CONNECTED\n");
        
        while(1) {
            uint32_t num;
            printf("READING BLOCK SIZE\n");
            if((n = read(connfd, &num, sizeof(uint32_t))) <= 0) {
                break;
            }
            printf("read %d bytes\n", n);
            printf("WRITING BLOCK OF %d BYTES\n", ntohl(num));
            write_block(connfd, ntohl(num));
        }

        close(connfd);
        close(listenfd);
    }

    // Done
    printf("%s\n", recvline);
 
    close(sockfd);
    return 0;
}
