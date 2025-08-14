#pragma once

#pragma once
#include <string>

namespace skynet::http::server::sync::detail {
    struct Config {
        std::string address{ "0.0.0.0" };
        unsigned short port{ 8080 };
        std::size_t threads{ 1 };
        std::string doc_root{ "." };
    };

    int run_sync(const Config& cfg); // blocks
}
