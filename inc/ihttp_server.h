#pragma once

#include "sn_factory.h"

class IUser;  // forward declare
class IHTTP_Handler;  // forward declare

class IHTTP_Server
{
    DEFINE_INTERFACE(IHTTP_Server);

public:
    virtual ~IHTTP_Server() noexcept = default;

    IHTTP_Server() = default;
    IHTTP_Server(const IHTTP_Server&) = delete;
    IHTTP_Server& operator=(const IHTTP_Server&) = delete;
    IHTTP_Server(IHTTP_Server&&) = delete;
    IHTTP_Server& operator=(IHTTP_Server&&) = delete;

    // Setup the server. 
    // 'guest' is a non-owning pointer: caller retains ownership & lifetime.
    // nullptr is valid if the server can run without a guest.
    virtual int setup(const char* address,
        const char* port,
        const char* doc_root,
        IHTTP_Handler* handler,
        IUser* guest) = 0;

    // Start server asynchronously in a new thread.
    virtual int start() = 0;

    // Run server synchronously until stopped.
    virtual int run() = 0;

    // Stop the server (idempotent).
    virtual int stop() = 0;
};
