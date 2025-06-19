#include "http.h"

using namespace Server;
int main() {
    Http http(8080);
    http.Init();
    http.onUpdate();
    return 0;
}