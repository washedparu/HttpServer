#pragma once

#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstring>
#include <csignal>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define _RELEASE


namespace HttpServer {

    class Http { 
    public:
        Http(int PORT);
        ~Http();

        void Init();

        void OnUpdate();
        void Shutdown();
        static void SignalHandler(int signum);

    private:
        bool m_IsRunning; 

        const int m_PORT;

        int m_ServerSocket;
        sockaddr_in m_ServerAddress;

        int m_ClientSocket;
        sockaddr_in m_ClientAddress;

    };
    
  
}