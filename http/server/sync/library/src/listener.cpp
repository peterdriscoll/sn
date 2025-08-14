// Portions of this file are derived from Boost.Beast example "http_server_sync.cpp".
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE_1_0.txt or https://www.boost.org/LICENSE_1_0.txt
// Modifications (c) 2025 Peter Driscoll

#include "skynet/http/server/sync/detail/listener.hpp"
#include "skynet/http/server/sync/detail/session.hpp"
#include "skynet/http/server/sync/detail/errors.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <thread>

#include "skynet_http_server_sync_lib_pch.h"

namespace skynet::http::server::sync::detail {
    int run_accept_loop(boost::asio::io_context& ioc,
        const boost::asio::ip::address& address,
        unsigned short port,
        std::shared_ptr<const std::string> doc_root) {
        boost::asio::ip::tcp::acceptor acceptor{ ioc, boost::asio::ip::tcp::endpoint{address, port} };
        for (;;) {
            boost::asio::ip::tcp::socket socket{ ioc };
            boost::system::error_code ec;
            acceptor.accept(socket, ec);
            if (ec) { fail(ec, "accept"); continue; }

            std::thread{ [s = std::move(socket), root = doc_root]() mutable {
              skynet::http::server::sync::detail::do_session(std::move(s), root);
            } }.detach();
        }
    }
}
