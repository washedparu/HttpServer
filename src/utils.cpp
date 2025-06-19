#include "utils.h"
#include <string>

bool Server::endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
            str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Server::getClientIPFromHeaders(const std::string& request) {
    std::istringstream stream(request);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.rfind("X-Forwarded-For:", 0) == 0) {
            return line.substr(strlen("X-Forwarded-For: "));
        }
    }
    return "unknown";
}


std::string Server::getMimeType(const std::string& path) {
        if (endsWith(path, ".html") || endsWith(path, ".htm")) return "text/html";
        if (endsWith(path, ".css")) return "text/css";
        if (endsWith(path, ".js")) return "application/javascript";
        if (endsWith(path, ".json")) return "application/json";
        if (endsWith(path, ".xml")) return "application/xml";
        if (endsWith(path, ".txt")) return "text/plain";

        // Images
        if (endsWith(path, ".jpg") || endsWith(path, ".jpeg")) return "image/jpeg";
        if (endsWith(path, ".png")) return "image/png";
        if (endsWith(path, ".gif")) return "image/gif";
        if (endsWith(path, ".bmp")) return "image/bmp";
        if (endsWith(path, ".webp")) return "image/webp";
        if (endsWith(path, ".svg")) return "image/svg+xml";
        if (endsWith(path, ".ico")) return "image/x-icon";

        // Audio
        if (endsWith(path, ".mp3")) return "audio/mpeg";
        if (endsWith(path, ".wav")) return "audio/wav";
        if (endsWith(path, ".ogg")) return "audio/ogg";

        // Video
        if (endsWith(path, ".mp4")) return "video/mp4";
        if (endsWith(path, ".webm")) return "video/webm";
        if (endsWith(path, ".avi")) return "video/x-msvideo";
        if (endsWith(path, ".mov")) return "video/quicktime";

        // Fonts
        if (endsWith(path, ".ttf")) return "font/ttf";
        if (endsWith(path, ".otf")) return "font/otf";
        if (endsWith(path, ".woff")) return "font/woff";
        if (endsWith(path, ".woff2")) return "font/woff2";

        // Default fallback
        return "application/octet-stream";
    }