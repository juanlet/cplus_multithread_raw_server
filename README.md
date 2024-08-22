# C++ HTTP Server Project

## Description
This is an advanced C++ HTTP server that serves static files, provides a simple JSON API, and handles multiple clients concurrently using multithreading.

## Dependencies

- g++-13
- jsoncpp (libjsoncpp-dev)

## Setup Instructions

1. **Install dependencies**:
    ```bash
    sudo apt update
    sudo apt install -y build-essential g++-13 libjsoncpp-dev
    ```

2. **Clone the repository**:
    ```bash
    git clone https://github.com/yourusername/your-repo-name.git
    cd your-repo-name
    ```

3. **Compile the server**:
    ```bash
    g++-13 -std=c++11 -pthread -ljsoncpp http_server.cpp -o http_server
    ```

4. **Run the server**:
    ```bash
    ./http_server
    ```

## Usage

- Visit `http://localhost:8080/` to view the homepage.
- Visit `http://localhost:8080/api` to get a JSON response.
