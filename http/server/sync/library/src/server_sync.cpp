// Portions of this file are derived from Boost.Beast example "http_server_sync.cpp".
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE_1_0.txt or https://www.boost.org/LICENSE_1_0.txt
// Modifications (c) 2025 Peter Driscoll

#include "skynet/http/server/sync/detail/server_sync.hpp"
#include "skynet/http/server/sync/detail/listener.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

#include "skynet_http_server_sync_lib_pch.h"

namespace skynet::http::server::sync::detail {
    int run_sync(const Config& cfg) {
        try {
            auto addr = boost::asio::ip::make_address(cfg.address);
            boost::asio::io_context ioc(1);
            auto root = std::make_shared<const std::string>(cfg.doc_root);
            return run_accept_loop(ioc, addr, cfg.port, root);
        }
        catch (const std::exception& e) {
            // your logging
            return EXIT_FAILURE;
        }
    }
}
