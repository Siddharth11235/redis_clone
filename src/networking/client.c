#include <sys/socket.h>
#include <sys/errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

const size_t k_max_msg = 4096;
static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}


static int32_t read_full(int connfd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(connfd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or unexpected EOF
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(int connfd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(connfd, buf, n);
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t query(int connfd, const char *text) {
    uint32_t len = (uint32_t)strlen(text);
    if (len > k_max_msg) {
        return -1;
    }

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4);  // assume little endian
    memcpy(&wbuf[4], text, len);
    if (int32_t err = write_all(connfd, wbuf, 4 + len)) {
        return err;
    }

    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read() error");
        }
        return err;
    }

    memcpy(&len, rbuf, 4);  // assume little endian
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // reply body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    // do something
    rbuf[4 + len] = '\0';
    printf("server says: %s\n", &rbuf[4]);
    return 0;
}


int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        msg("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        msg("Socket connection failed");
        exit(EXIT_FAILURE);
    }

    int32_t err = query(fd, "It rains where I am.");
    if (err) {
        close(fd);
        return 0;
    }
    err = query(fd, "The gloom stays where I am.");
    if (err) {
       close(fd);
        return 0;
    }
    err = query(fd, "These nights get colder.");
    if (err) {
        close(fd);
        return 0;
    }

    close(fd);
    return 0;

}