#include "http.h"


int main() {
    Server::Http http(8080);
    http.Init();
    http.onUpdate();
    return 0;
}