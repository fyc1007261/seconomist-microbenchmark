
#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <assert.h>

const int PORT = 8080;
const int MAX_CLIENTS = 1024;

int main(int argc, char **argv)
{
    // Create the file to store the results
    assert(argc == 2);
    int fd = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0755);
    const int buf_size = 500000000;
    char *file_buf = new char[buf_size];
    memset(file_buf, 0, buf_size);
    int ret = write(fd, file_buf, buf_size);
    if (ret < 0)
    {
        std::cout << strerror(errno) << std::endl;
    }
    assert(ret == buf_size);
    delete[] file_buf;
    std::cout << "file created " << ret << std::endl;
    int offset = 0;

    file_buf = (char *)mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_buf == NULL || (long)file_buf == -1)
    {
        std::cerr << strerror(errno) << std::endl;
        exit(1);
    }
    std::cout << "memory mapped\n";

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::vector<pollfd> poll_fds;
    poll_fds.push_back({server_fd, POLLIN, 0});

    while (true)
    {
        int ret = poll(poll_fds.data(), poll_fds.size(), -1);

        if (ret < 0)
        {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < poll_fds.size(); i++)
        {
            if (poll_fds[i].revents == 0)
                continue;

            if (poll_fds[i].fd == server_fd)
            {
                // Accept new connection
                if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                std::cout << "New client connected: " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << std::endl;

                // Add the new client to poll_fds
                poll_fds.push_back({new_socket, POLLIN, 0});

                if (poll_fds.size() > MAX_CLIENTS)
                {
                    std::cerr << "Too many clients connected. Closing the new connection." << std::endl;
                    close(new_socket);
                    poll_fds.pop_back();
                }
            }
            else
            {
                // Read and echo data from the client
                char buffer[1024] = {0};
                int bytes_read = read(poll_fds[i].fd, buffer, sizeof(buffer));
                timeval tv;
                gettimeofday(&tv, NULL);
                if (bytes_read <= 0)
                {
                    // Remove the client from poll_fds and close the connection
                    std::cout << "Client disconnected." << std::endl;
                    close(poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                    i--;
                }
                else
                {
                    // Echo the data back to the client
                    // std::cout << buffer << " " << tv.tv_sec << "." << tv.tv_usec << std::endl;

                    offset += sprintf(file_buf + offset, "%s %ld %ld\n", buffer, tv.tv_sec, tv.tv_usec);
                }
            }
        }
    }

    // Close all connections
    for (auto &poll_fd : poll_fds)
    {
        close(poll_fd.fd);
    }
}
