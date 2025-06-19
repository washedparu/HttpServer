#include "http.h"

HttpServer::Http::Http(int PORT) : m_PORT(PORT) {}


HttpServer::Http::~Http() {
    std::cout << "Finished!\n";
    close(m_ServerSocket);
}
void HttpServer::Http::Init() {
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

    if (bind(m_ServerSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind the socket to the address\n";
        return;
    }

    listen(m_ServerSocket, 1);
    std::cout << "Server is running on http://localhost:" << m_PORT << '\n';
}

void HttpServer::Http::onUpdate() {
    while (true) {
        socklen_t clientLength = sizeof(m_ClientAddress);
        int clientSocket = accept(m_ServerSocket, (sockaddr*)&m_ClientAddress, &clientLength);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept connection: " << strerror(errno) << '\n';
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(m_ClientAddress.sin_addr), clientIp, INET_ADDRSTRLEN);

        char requestBuffer[4096] = {0};
        ssize_t bytesRead = read(clientSocket, requestBuffer, sizeof(requestBuffer) - 1);
        if (bytesRead > 0) {
            requestBuffer[bytesRead] = '\0'; // Null-terminate BEFORE using as string

            std::cout << "Client IP (socket): " << clientIp << '\n';

            std::string requestStr(requestBuffer, bytesRead);
            std::cout << "Client IP (x-forwarded-for): " << getClientIPFromHeaders(requestStr) << '\n';
        } else {
            std::cerr << "Failed to read client request or connection closed\n";
            close(clientSocket);
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
            write(clientSocket, response.c_str(), response.size());
            close(clientSocket);
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
            write(clientSocket, forbidden.c_str(), forbidden.size());
            close(clientSocket);
            continue;
        }

        std::string indexHtmlPath = "../html" + requestPath;

        std::ifstream indexHtml(indexHtmlPath, std::ios::binary);
        if (!indexHtml.is_open()) {
            std::string notFound = "HTTP/1.1 404 Not Found\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Content-Length: 13\r\n\r\n"
                                   "404 Not Found";
            write(clientSocket, notFound.c_str(), notFound.size());
            close(clientSocket);
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
        if (write(clientSocket, httpResponse.c_str(), httpResponse.size()) == -1) {
            std::cerr << "Failed to write to client socket\n";
        }

        close(clientSocket);
        indexHtml.close();
    }
}