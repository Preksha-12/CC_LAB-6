#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>

int main() {

    // Get backend name from environment variable
    const char* name = getenv("BACKEND_NAME");
    std::string backend_name = name ? std::string(name) : "unknown";

    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "ERROR: Failed to create socket" << std::endl;
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to port 8080
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "ERROR: Failed to bind to port 8080" << std::endl;
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "ERROR: Failed to listen on port 8080" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8080 (Backend: " 
              << backend_name << ")" << std::endl;

    while (true) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        // Prepare response body
        std::string body = "Served by backend: " + backend_name + "\n";

        // Proper HTTP response
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n\r\n" +
            body;

        send(client_fd, response.c_str(), response.size(), 0);
        shutdown(client_fd, SHUT_WR);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
