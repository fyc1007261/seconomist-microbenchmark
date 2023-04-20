#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/signal.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: ./client <serv_addr> <message>" << std::endl;
        exit(1);
    }
    // create a TCP socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // connect to server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address" << std::endl;
        return 1;
    }
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }

    // send data to server
    while (1)
    {
        timeval tv;
        gettimeofday(&tv, NULL);
        if (tv.tv_usec < 50)
            break;
    }
    while (1)
    {
        while (1)
        {
            timeval tv;
            gettimeofday(&tv, NULL);
            if (tv.tv_usec < 50)
                break;
        }
        ssize_t num_bytes = send(client_fd, argv[2], strlen(argv[2]), 0);
        if (num_bytes == -1)
        {
            std::cerr << "Failed to send data to server" << std::endl;
            return 1;
        }
        usleep(900000);
    }

    // close socket
    close(client_fd);

    return 0;
}