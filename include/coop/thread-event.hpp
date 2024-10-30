#pragma once

#include <winsock2.h>

#include "thread-event-pre.hpp"

namespace coop {

inline auto ThreadEvent::await_ready() const -> bool {
    return false;
}

template <CoHandleLike CoHandle>
inline auto ThreadEvent::await_suspend(CoHandle caller_task) -> void {
    const auto runner = caller_task.promise().runner;
    runner->io_wait(fd.fd, true, false, result);
}

inline auto ThreadEvent::await_resume() -> void {
    auto count = eventfd_t();
    eventfd_read(fd, &count);
}

inline auto ThreadEvent::notify() -> void {
    if(!fd.write("", 1)) {
        std::terminate();
    }
}

inline ThreadEvent::ThreadEvent(ThreadEvent&& o)
    : fd(std::move(o.fd)) {
}

inline ThreadEvent::ThreadEvent() {
}

inline ThreadEvent::~ThreadEvent() {
}
} // namespace coop
