#include "http.h"


#ifdef _DEBUG
void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    std::cout << "[new] " << size << " bytes at " << ptr << '\n';
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete(void* ptr) noexcept {
    std::cout << "[delete] at " << ptr << '\n';
    std::free(ptr);
}

#elid defined(_RELEASE)
// nothing
#endif


int main() {
    HttpServer::Http http(8080);
    http.Init();
    http.OnUpdate();

    return 0;
}