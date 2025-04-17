// Filename: client.cpp
// Description: Multi-user chatroom client with username, real-time sound notification,
// and timestamp, with improved display formatting.
// Author: Updated by Priyansh and modified by [Your Name]

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
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


std::atomic<bool> running(true);

// Helper function to get the current timestamp in the format "YYYY-MM-DD HH:MM:SS"
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void receiveMessages(SOCKET socket_fd) {
    char buffer[1024];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            std::cout << "\nDisconnected from server or error occurred.\n";
            running = false;
            break;
        }

        // Trigger a real-time sound notification.
#ifdef _WIN32
        // On Windows, use the Beep function (frequency: 750 Hz, duration: 300 ms)
        Beep(750, 300);
#else
        // On other platforms, output the bell character
        std::cout << "\a";
#endif

        // Retrieve the current timestamp.
        std::string timestamp = getCurrentTimestamp();

        // Print the timestamp and the received message on a new line.
        std::cout << "\n[" << timestamp << "] " << buffer << std::endl;
        std::cout.flush();
    }
}   

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: client <IP Address> <Port>\n";
        return 1;
    }
  
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << "\n";
        return 1;
    }
#endif

    std::string serverIP = argv[1];
    int port = std::stoi(argv[2]);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed.\n";
        closesocket(clientSocket);
        return 1;
    }

    std::wcout << "Connected to server.\n";

    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);

    // Start the receiver thread.
    std::thread recvThread(receiveMessages, clientSocket);

    std::string input;
    while (running) {
        // Print the prompt for user input.
       // std::cout << ">: ";
        std::getline(std::cin, input);

        if (input == "exit") {
            running = false;
            break;
        }

        std::string message = name + ": " + input;
        int sent = send(clientSocket, message.c_str(), message.length(), 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "Send failed.\n";
            running = false;
            break;
        }
    }

    closesocket(clientSocket);
    if (recvThread.joinable()) 
        recvThread.join();

#ifdef _WIN32
    WSACleanup();
#endif

    std::cout << "Disconnected from server.\n";
    return 0;
}
