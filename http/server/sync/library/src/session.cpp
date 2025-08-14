// Portions of this file are derived from Boost.Beast example "http_server_sync.cpp".
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE_1_0.txt or https://www.boost.org/LICENSE_1_0.txt
// Modifications (c) 2025 Peter Driscoll

#include "skynet/http/server/sync/detail/session.hpp"
#include "skynet/http/server/sync/detail/errors.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>

#include "skynet_http_server_sync_lib_pch.h"

// --- keep these as internal-linkage helpers in this TU ---
static boost::beast::string_view mime_type(boost::beast::string_view path)
{ /* ... (example code) ... */ }
static std::string path_cat(boost::beast::string_view base, boost::beast::string_view path) { /* ... */ }

template<class Body, class Allocator>
static boost::beast::http::message_generator handle_request(
    boost::beast::string_view doc_root,
    boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req)
{
    // Accept only GET/HEAD for now
    if (req.method() != boost::beast::http::verb::get &&
        req.method() != boost::beast::http::verb::head)
    {
        boost::beast::http::response<boost::beast::http::string_body> res{
            boost::beast::http::status::bad_request, req.version() };
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "Unsupported method";
        res.prepare_payload();
        return res;
    }

    if (req.method() == boost::beast::http::verb::head)
    {
        boost::beast::http::response<boost::beast::http::empty_body> res{
            boost::beast::http::status::ok, req.version() };
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.keep_alive(req.keep_alive());
        return res;
    }

    // GET: simple text stub
    boost::beast::http::response<boost::beast::http::string_body> res{
        boost::beast::http::status::ok, req.version() };
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/plain");
    res.keep_alive(req.keep_alive());
    res.body() = "ok";
    res.prepare_payload();
    return res;
}
// ---------------------------------------------------------

namespace skynet::http::server::sync::detail {
    int do_session(boost::asio::ip::tcp::socket socket, std::shared_ptr<const std::string> doc_root) {
        boost::beast::error_code ec;
        boost::beast::flat_buffer buffer;

        for (;;) {
            boost::beast::http::request<boost::beast::http::string_body> req; /// <--- compile error
            boost::beast::http::read(socket, buffer, req, ec);
            if (ec == boost::beast::http::error::end_of_stream) break;
            if (ec) { fail(ec, "read"); return -1; }

            auto msg = handle_request(*doc_root, std::move(req));
            bool keep = msg.keep_alive();
            boost::beast::write(socket, std::move(msg), ec);
            if (ec) { fail(ec, "write"); return -1; }
            if (!keep) break;
        }

        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        return 0;
    }
}
