// Filename: server.cpp
// Description: Multi-user chatroom server with broadcasting and cross-platform compatibility
// Author: Updated by Priyansh

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <atomic>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <cstring>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

// Global container for all connected clients
std::vector<SOCKET> clients;
std::mutex clientsMutex;
std::atomic<bool> serverRunning(true);

// Function to broadcast a message to all clients except the sender
void broadcastMessage(const std::string &message, SOCKET excludeSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto client : clients) {
        if (client != excludeSocket) {
            send(client, message.c_str(), static_cast<int>(message.length()), 0);
        }
    }
}

// Thread function to handle an individual client
void handleClient(SOCKET clientSocket) {
    char buffer[1024];

    while (serverRunning) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            std::cout << "A client disconnected.\n";
            // Remove the client from the global list
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            }
            closesocket(clientSocket);
            break;
        }

        // Create message string from received data
        std::string message(buffer, bytes);
        std::cout << message << std::endl; // Optionally print on the server console

        // Broadcast the received message to all other connected clients
        broadcastMessage(message, clientSocket);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <Port>\n";
        return 1;
    }
    int port = std::stoi(argv[1]);

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << "\n";
        return 1;
    }
#endif

    // Create the listening socket
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create listening socket.\n";
        return 1;
    }

    // Setup the server address structure
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(listeningSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        closesocket(listeningSocket);
        return 1;
    }

    // Put the socket in listening mode
    if (listen(listeningSocket, 10) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        closesocket(listeningSocket);
        return 1;
    }

    std::cout << "Server running on port " << port << ". Waiting for connections...\n";

    // Vector to keep track of client threads
    std::vector<std::thread> clientThreads;

    while (serverRunning) {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(listeningSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept error.\n";
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(clientSocket);
        }

        std::cout << "New client connected from " << inet_ntoa(clientAddr.sin_addr) << "\n";

        // Launch a new thread for the connected client
        clientThreads.emplace_back(std::thread(handleClient, clientSocket));
    }

    // Clean up: join all client handling threads.
    for (auto &t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }

    closesocket(listeningSocket);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
