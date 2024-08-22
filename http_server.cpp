#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <ctime>
#include <filesystem>

#define PORT 8083

// Get the absolute path to the project root
const std::string PROJECT_ROOT = std::filesystem::current_path().string();

void handleClient(int client_socket);
std::string getCurrentTimestamp();
std::string serveFile(const std::string& path);
std::string getContentType(const std::string& path);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Binding to the specified port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " << PORT << "\n";

    // Main server loop
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Handle each client in a new thread
        std::thread clientThread(handleClient, new_socket);
        clientThread.detach();
    }

    return 0;
}

// Handle individual client connections
void handleClient(int client_socket) {
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);

    // Log request with timestamp and client info
    std::string request(buffer);
    std::cout << "[" << getCurrentTimestamp() << "] Received request: " << request << std::endl;

    // Parse request and route
    std::istringstream requestStream(request);
    std::string method, url;
    requestStream >> method >> url;

    // If the request is for '/', serve 'index.html'
    if (url == "/") {
        url = "/index.html";  // Redirect root requests to 'index.html'
    }

    // Serve the requested file
    std::string response = serveFile(url.substr(1));  // Remove leading '/' to get file path

    if (response.empty()) {
        // 404 Not Found if the file is not found
        response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\nContent-Length: 97\n\n"
                   "<html><body><h1>404 Not Found</h1><p>The page you are looking for does not exist.</p></body></html>";
    }

    // Send the response
    send(client_socket, response.c_str(), response.size(), 0);
    close(client_socket);
}

// Serve static files (HTML, CSS, JS, Images)
std::string serveFile(const std::string& path) {
    // We assume that PROJECT_ROOT already contains the current directory,
    // so no need to concatenate it again
    std::string fullPath = std::filesystem::absolute(path).string();
    std::cout << "Serving file from: " << fullPath << std::endl;  // Debug print

    std::ifstream file(fullPath, std::ios::binary);  // Open the file in binary mode for images
    if (!file.is_open()) {
        std::cout << "File not found: " << fullPath << std::endl;  // Debug print for file not found
        return "";
    }

    // Read the file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // Determine the content type based on the file extension
    std::string contentType = getContentType(path);

    // Create the HTTP response
    std::string fileContents = buffer.str();
    std::string response = "HTTP/1.1 200 OK\nContent-Type: " + contentType + "\nContent-Length: " +
                           std::to_string(fileContents.size()) + "\n\n" + fileContents;
    return response;
}

// Get the content type based on the file extension
std::string getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".ico") != std::string::npos) return "image/x-icon";
    return "text/plain";  // Default to plain text
}

// Get current timestamp for logging
std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localTime);
    return std::string(buffer);
}
