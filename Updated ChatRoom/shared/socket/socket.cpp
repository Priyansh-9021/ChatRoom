// Filename: socket.cpp
// Description: Cross-platform socket API wrapper (Updated)
// Author: Aaron Kelsey

#include "socket.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
  // Windows-specific initialization
  Socket::Socket() : ownsWSA(true)
  {
      bClosed = false;
      bConnected = false;
      if (WSAStartup(MAKEWORD(2, 2), &oWsaData) != 0) {
          std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
      } else {
          std::cout << "Winsock successfully initialized" << std::endl;
      }
      oSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (oSocket == INVALID_SOCKET) {
          std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
          close();
      }
  }
#else
  // POSIX / macOS initialization: no WSAStartup needed
  Socket::Socket() 
  {
      bClosed = false;
      bConnected = false;
      oSocket = socket(AF_INET, SOCK_STREAM, 0);
      if (oSocket == INVALID_SOCKET) {
          std::cerr << "Error creating socket." << std::endl;
          close();
      }
  }
#endif

// Private constructor for accepted sockets
Socket::Socket(SOCKET sock) : oSocket(sock), bConnected(true), bClosed(false)
{
#ifdef _WIN32
    ownsWSA = false;
#endif
}

Socket::~Socket() {
    close();
#ifdef _WIN32
    if (ownsWSA) {
        if (WSACleanup() == SOCKET_ERROR) {
            std::cerr << "WSACleanup failed with error: " << WSAGetLastError() << std::endl;
        } else {
            std::cout << "Winsock successfully cleaned up" << std::endl;
        }
    }
#endif
}

int Socket::connect(const int iPort, const std::string& sAddress) {
#ifdef _WIN32
    InetPtonA(AF_INET, sAddress.c_str(), &oServerInfo.sin_addr.s_addr);
#else
    inet_pton(AF_INET, sAddress.c_str(), &oServerInfo.sin_addr);
#endif
    oServerInfo.sin_family = AF_INET;
    oServerInfo.sin_port = htons(iPort);

    int iReturnValue = ::connect(oSocket, reinterpret_cast<sockaddr*>(&oServerInfo), sizeof(oServerInfo));
    if (iReturnValue == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "Error connecting to server: " << WSAGetLastError() << std::endl;
#else
        std::perror("Error connecting to server");
#endif
        close();
    } else {
        bConnected = true;
    }
    return iReturnValue;
}

int Socket::bind(const int iPort) {
    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(iPort);

    int iReturnValue = ::bind(oSocket, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr));
    if (iReturnValue == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "Error binding socket: " << WSAGetLastError() << std::endl;
#else
        std::perror("Error binding socket");
#endif
        close();
    }
    return iReturnValue;
}

int Socket::listen(const int iBacklog) {
    int iReturnValue = ::listen(oSocket, iBacklog);
    if (iReturnValue == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "Error enabling socket for listening: " << WSAGetLastError() << std::endl;
#else
        std::perror("Error enabling socket for listening");
#endif
        close();
    }
    return iReturnValue;
}

std::shared_ptr<Socket> Socket::accept() {
    SOCKET oClientSocket = ::accept(oSocket, nullptr, nullptr);
    if (oClientSocket == INVALID_SOCKET) {
#ifdef _WIN32
        std::cerr << "Error waiting for client: " << WSAGetLastError() << std::endl;
#else
        std::perror("Error waiting for client");
#endif
        close();
        return nullptr;
    }
    return std::shared_ptr<Socket>(new Socket(oClientSocket));
}

int Socket::send(const std::string& sMessage) {
    int iBytesSent = ::send(oSocket, sMessage.c_str(), static_cast<int>(sMessage.length()), 0);
    if (iBytesSent == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "Error sending message: " << WSAGetLastError() << std::endl;
#else
        std::perror("Error sending message");
#endif
    }
    return iBytesSent;
}

int Socket::receive(std::string& sMessage) {
    char cBuffer[iBufferSize];
    std::memset(cBuffer, 0, sizeof(cBuffer));
    int iBytesReceived = ::recv(oSocket, cBuffer, sizeof(cBuffer), 0);
    if (iBytesReceived == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
#else
        std::perror("Error receiving data");
#endif
    }
    sMessage = std::string(cBuffer, iBytesReceived);
    return iBytesReceived;
}

void Socket::close() {
    if (!bClosed) {
        closesocket(oSocket);
        bClosed = true;
    }
}
