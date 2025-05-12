#ifndef MYFUNC_H
#define MYFUNC_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <memory.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

// Error handling

int Socket(int domain, int type, int protocol) {
    int n;
    if((n = socket(domain, type, protocol)) < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // 服务器可立即重启，无需等待 TIME_WAIT 超时
    int reuse = 1;
    if (setsockopt(n, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    return n;
}


int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int n;
    if((n = bind(sockfd, addr, addrlen)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    return n;
}

int Listen(int sockfd, int backlog) {
    int n;
    if((n = listen(sockfd, backlog)) < 0) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }
    return n;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int n;
    if((n = accept(sockfd, addr, addrlen)) < 0) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }
    return n;
}

int Recv(int sockfd, void *buf, size_t len, int flags) {
    int n;
    if((n = recv(sockfd, buf, len, flags)) < 0) {
        perror("Error receiving message");
        exit(EXIT_FAILURE);
    }
    return n;
}

int Send(int sockfd, const void *buf, size_t len, int flags) {
    int n;
    if((n = send(sockfd, buf, len, flags)) < 0) {
        perror("Error sending message");
        exit(EXIT_FAILURE);
    }
    return n;
}

int Sendmsg(int sockfd, const msghdr *msg, int flags) {
    int n;
    if((n = sendmsg(sockfd, msg, flags)) < 0) {
        perror("Error sending message");
        exit(EXIT_FAILURE);
    }
    return n;
}

FILE* Fopen(const char *path, const char *mode) {
    FILE *file = fopen(path, mode);
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int n;
    if((n = connect(sockfd, addr, addrlen)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    return n;
}

int Open(const char *pathname, int flags, mode_t mode) {
    int fd = open(pathname, flags, mode);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int Write(int fd, const void *buf, size_t count) {
    ssize_t n = write(fd, buf, count);
    if (n == -1) {
        perror("Error writing to file");
        exit(EXIT_FAILURE);
    }
    return n;
}

size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t n = fread(ptr, size, nmemb, stream);
    if (n == 0 && ferror(stream)) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    return n;
}



#endif

