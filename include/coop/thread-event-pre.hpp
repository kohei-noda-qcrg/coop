#pragma once
#include <exception>
#include <iostream>
#include <utility>

#include <winsock2.h>

#include "cohandle.hpp"
#include "io-pre.hpp"

namespace coop {

struct SockPipe {
    static constexpr auto ip = 0x7f000001; // 127.0.0.1

    SOCKET   fd = INVALID_SOCKET;
    uint16_t port;

    auto read(void* buf, size_t size) -> int {
        auto result = recvfrom(fd, (char*)buf, size, 0, NULL, NULL);
        if(result < 0) {
            std::cerr << "recvfrom failed, errno=" << WSAGetLastError() << std::endl;
        }
        return result;
    }

    auto write(const void* buf, size_t size) -> int {
        auto fd              = socket(AF_INET, SOCK_DGRAM, 0);
        auto addr            = sockaddr_in();
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = htonl(ip);
        addr.sin_port        = port;
        auto result          = sendto(fd, (char*)buf, size, 0, (sockaddr*)(&addr), sizeof(addr));
        if(result < 0) {
            std::cerr << "sendto failed, errno=" << WSAGetLastError() << std::endl;
        }
        closesocket(fd);
        return result;
    }

    SockPipe(SockPipe&& o)
        : fd(std::exchange(o.fd, INVALID_SOCKET)) {
    }

    SockPipe() {
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        auto addr            = sockaddr_in();
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = htonl(ip);
        addr.sin_port        = htons(0);
        if(bind(fd, (sockaddr*)(&addr), sizeof(addr)) != 0) {
            std::cerr << "bind failed, errno=" << WSAGetLastError() << std::endl;
            std::terminate();
        }
        auto sock_name = sockaddr_in{};
        auto sock_len  = int(sizeof(sock_name));
        if(getsockname(fd, (sockaddr*)(&sock_name), &sock_len) != 0) {
            std::cerr << "getsockname failed, errno=" << WSAGetLastError() << std::endl;
            std::terminate();
        }
        if(sock_name.sin_addr.s_addr != htonl(ip)) {
            std::cerr << "getsockname failed, addr=" << sock_name.sin_addr.s_addr << std::endl;
            std::terminate();
        }
        port = sock_name.sin_port;
    }

    ~SockPipe() {
        if(fd != INVALID_SOCKET) {
            closesocket(fd);
        }
    }
};
} // namespace coop

namespace coop {
struct ThreadEvent {
    SockPipe     fd;
    IOWaitResult result;

    auto await_ready() const -> bool;
    template <CoHandleLike CoHandle>
    auto await_suspend(CoHandle caller_task) -> void;
    auto await_resume() -> void;

    auto notify() -> void;

    ThreadEvent(ThreadEvent&& o);
    ThreadEvent();
    ~ThreadEvent();
};
} // namespace coop
