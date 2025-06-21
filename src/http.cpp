#include "http.h"

HttpServer::Http* g_HttpInstance = nullptr;


HttpServer::Http::Http(int PORT) : m_PORT(PORT), m_ServerSocket(-1), m_ClientSocket(-1), m_IsRunning(true) {}


HttpServer::Http::~Http() {
    std::cout << "Finished!\n";
    close(m_ServerSocket);
}


void HttpServer::Http::Init() {
    g_HttpInstance = this;
    m_ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_ServerSocket == -1) {
        std::cerr << "Failed to create the socket.\n";
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(m_PORT);

    int enable = 1;
    setsockopt(m_ServerSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    setsockopt(m_ServerSocket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));
    if (bind(m_ServerSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind the socket to the address\n";
        return;
    }

    listen(m_ServerSocket, 1);
    std::cout << "Server is running on http://localhost:" << m_PORT << '\n';
    std::signal(SIGINT, HttpServer::Http::SignalHandler);
}


void HttpServer::Http::Shutdown() {
    m_IsRunning = false;

    if (m_ServerSocket != -1) {
        shutdown(m_ServerSocket, SHUT_RDWR);
        close(m_ServerSocket);
        m_ServerSocket = -1;
    }
}


void HttpServer::Http::SignalHandler(int signum) {
    std::cout << "\nSignal (" << signum << ") received. Shutting down...\n";
    if(g_HttpInstance) {
        g_HttpInstance->Shutdown();
    }
}



void HttpServer::Http::OnUpdate() {
    while (m_IsRunning) {
        socklen_t clientLength = sizeof(m_ClientAddress);
        m_ClientSocket = accept(m_ServerSocket, (sockaddr*)&m_ClientAddress, &clientLength);
        if (m_ClientSocket == -1) {
            if(errno == EINTR) { 
                std::cout << "Interrupted accept() by a signal\n";
                break;
            } 
            std::cerr << "Failed to accept connection: " << strerror(errno) << '\n';
            continue;
        }

      
    
        char rawIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(m_ClientAddress.sin_addr), rawIp, INET_ADDRSTRLEN);

        char requestBuffer[4096] = {0};
        ssize_t bytesRead = read(m_ClientSocket, requestBuffer, sizeof(requestBuffer) - 1);
        if (bytesRead > 0) {
            requestBuffer[bytesRead] = '\0'; 

            std::string_view clientIP = getClientIPFromHeaders(std::string(requestBuffer, bytesRead));
            if(clientIP.empty()) {
                std::cout << "Client IP(raw): " << rawIp << '\n';
            } 
            else {
                std::cout << "Client IP: " << clientIP << '\n';
            }

        } else {
            std::cerr << "Failed to read client request or connection closed\n";
            close(m_ClientSocket);
            continue;
        }


        // Parse HTTP request line
        std::istringstream reqStream(requestBuffer);
        std::string method, requestPath, httpVersion;
        reqStream >> method >> requestPath >> httpVersion;

        if (method != "GET") {
            // Only GET supported
            std::string response = "HTTP/1.1 405 Method Not Allowed\r\n"
                                   "Content-Length: 0\r\n\r\n";
            write(m_ClientSocket, response.c_str(), response.size());
            close(m_ClientSocket);
            continue;
        }

        if (requestPath == "/") {
            requestPath = "/index.html";
        }

        // Security: prevent path traversal (remove '..')
        if (requestPath.find("..") != std::string::npos) {
            std::string forbidden = "HTTP/1.1 403 Forbidden\r\n"
                                    "Content-Length: 9\r\n\r\n"
                                    "Forbidden";
            write(m_ClientSocket, forbidden.c_str(), forbidden.size());
            close(m_ClientSocket);
            continue;
        }

        std::string indexHtmlPath = "../html" + requestPath;

        std::ifstream indexHtml(indexHtmlPath, std::ios::binary);
        if (!indexHtml.is_open()) {
            std::string notFound = "HTTP/1.1 404 Not Found\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Content-Length: 13\r\n\r\n"
                                   "404 Not Found";
            write(m_ClientSocket, notFound.c_str(), notFound.size());
            close(m_ClientSocket);
            continue;
        }

        std::stringstream buffer;
        buffer << indexHtml.rdbuf();
        std::string body = buffer.str();

        std::string mimeType = getMimeType(indexHtmlPath);
        std::string header = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: " + mimeType + "\r\n"
                             "Content-Length: " + std::to_string(body.size()) + "\r\n"
                             "\r\n";

        std::string httpResponse = header + body;
        if (write(m_ClientSocket, httpResponse.c_str(), httpResponse.size()) == -1) {
            std::cerr << "Failed to write to client socket\n";
        }
        
        close(m_ClientSocket);
        indexHtml.close();
    }
}