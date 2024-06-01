#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


static void communicate_with_client(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        perror("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "I would remember this time.";
    write(connfd, wbuf, strlen(wbuf));
}


int main() {

    struct sockaddr_in server_addr = {};

    // Create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use appropriate IP address
    server_addr.sin_port = htons(1234); 

    int rv = bind(fd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rv) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
        if (connfd < 0) {
            continue;   // error
        }

        communicate_with_client(connfd);
        close(connfd);
    }




}