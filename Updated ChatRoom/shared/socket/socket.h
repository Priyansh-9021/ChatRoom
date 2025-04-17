// Filename: socket.h
// Description: This file wraps the socket API
// Author: Aaron Kelsey (Updated for cross-platform support)

#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <memory>

#ifdef _WIN32
  #include <WinSock2.h>
  #include <Ws2tcpip.h>
  #define SIMPLESOCKET_API __declspec(dllimport)  // or __declspec(dllexport) during build
#else
  // POSIX / macOS
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <cstring>
  #define SOCKET int
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define closesocket close
  #define SIMPLESOCKET_API
#endif

class SIMPLESOCKET_API Socket {
public:
    // Default constructor initializes the socket (and on Windows, calls WSAStartup)
    Socket();

    // Destructor: closes the socket and, on Windows, calls WSACleanup if owned.
    ~Socket();

    int connect(const int iPort, const std::string& sAddress);
    int bind(const int iPort);
    int listen(const int iBacklog);
    std::shared_ptr<Socket> accept();
    int send(const std::string& sMessage);
    int receive(std::string& sMessage);
    SOCKET getSocket() { return oSocket; }
    void close();

private:
    void setSocket(SOCKET socket) { oSocket = socket; }
    // Private constructor used for accepted sockets. No socket reinitialization.
    Socket(SOCKET sock);

#ifdef _WIN32
    WSADATA     oWsaData;
#endif
    sockaddr_in oServerInfo;
    SOCKET      oSocket;

    bool bConnected;
    bool bClosed;
#ifdef _WIN32
    bool ownsWSA;  // True if this instance called WSAStartup.
#endif

    static const int iBufferSize = 4000;
};

#endif // SOCKET_H
