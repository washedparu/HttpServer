#pragma once

#include "utils.h"

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <sstream>

namespace HttpServer {

    class Http { 
    public:
        Http(int PORT);
        ~Http();

        void Init();

        void onUpdate();
    private:
        const int m_PORT;
        int m_ServerSocket;
        sockaddr_in m_ServerAddress;

        int m_ClientSocket;
        sockaddr_in m_ClientAddress;
    };
    
  
}