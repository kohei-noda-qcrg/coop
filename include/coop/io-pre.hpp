#pragma once
#include <windows.h>

#include "cohandle.hpp"

namespace coop {
struct Runner;

struct IOWaitResult {
    bool read;
    bool write;
    bool error;
};

struct [[nodiscard]] IOAWaiter {
    Runner*      runner;
    SOCKET       file;
    bool         read;
    bool         write;
    IOWaitResult result;

    auto await_ready() const -> bool;
    template <CoHandleLike CoHandle>
    auto await_suspend(CoHandle caller_task) -> void;
    auto await_resume() const -> IOWaitResult;
    IOAWaiter(const SOCKET file, const bool read, const bool write);
};

using wait_for_file = IOAWaiter;
} // namespace coop
