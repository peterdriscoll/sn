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
//static boost::beast::string_view mime_type(boost::beast::string_view path)
//{ /* ... (example code) ... */ }

// Minimal MIME lookup (full namespaces)
static const char* extension_to_type(const std::string& ext_in)
{
    boost::beast::string_view ext(ext_in.data(), ext_in.size());
    if (!ext.empty() && ext.front() == '.') ext.remove_prefix(1);

    using boost::beast::iequals;

    if (iequals(ext, "htm") || iequals(ext, "html")) return "text/html";
    if (iequals(ext, "css"))                           return "text/css";
    if (iequals(ext, "txt"))                           return "text/plain";
    if (iequals(ext, "js"))                            return "application/javascript";
    if (iequals(ext, "json"))                          return "application/json";
    if (iequals(ext, "xml"))                           return "application/xml";
    if (iequals(ext, "png"))                           return "image/png";
    if (iequals(ext, "jpg") || iequals(ext, "jpeg"))   return "image/jpeg";
    if (iequals(ext, "gif"))                           return "image/gif";
    if (iequals(ext, "bmp"))                           return "image/bmp";
    if (iequals(ext, "ico"))                           return "image/vnd.microsoft.icon";
    if (iequals(ext, "tif") || iequals(ext, "tiff"))   return "image/tiff";
    if (iequals(ext, "svg"))                           return "image/svg+xml";
    if (iequals(ext, "mp4"))                           return "video/mp4";
    if (iequals(ext, "woff"))                          return "font/woff";
    if (iequals(ext, "woff2"))                         return "font/woff2";
    if (iequals(ext, "ttf"))                           return "font/ttf";
    if (iequals(ext, "wasm"))                          return "application/wasm";
    return "application/octet-stream";
}

static std::string path_cat(boost::beast::string_view base, boost::beast::string_view path) { /* ... */ }

template<class Body, class Allocator>
static boost::beast::http::message_generator handle_request(
    boost::beast::string_view doc_root,
    boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req,
    IHTTP_Handler *handler)
{
    // Split path and query from the target
    boost::beast::string_view target = req.target();
    boost::beast::string_view query_sv{};
    std::size_t qpos = target.find('?');
    if (qpos != boost::beast::string_view::npos) {
        query_sv = target.substr(qpos + 1);
        target = target.substr(0, qpos);
    }

    // Make owned strings (don't append to string_view)
    std::string path(target.data(), target.size());
    std::string query(query_sv.data(), query_sv.size());

    // Normalize directories: map "/" -> "/index.html" like the old server
    if (path.empty() || path[0] != '/')
        path = "/";
    if (!path.empty() && path.back() == '/')
        path += "index.html";

    // Extract extension for MIME type
    std::string ext;
    std::size_t dot = path.rfind('.');
    if (dot != std::string::npos)
        ext = path.substr(dot);

    // Example handler call (unchanged signature)
    if (handler && handler->handle_response(path.c_str(), query.c_str(), ext.c_str())) {
        boost::beast::http::response<boost::beast::http::string_body> res{
            boost::beast::http::status::ok, req.version()
        };
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, extension_to_type(ext));
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.body() = handler->response_data();
        res.prepare_payload();
        return res; // implicit to message_generator
    }

    // Static file fallback: build full path using doc_root + path
    std::string full_path(doc_root.data(), doc_root.size());
    full_path += path;

    boost::beast::error_code ec;
    boost::beast::http::file_body::value_type body;
    body.open(full_path.c_str(), boost::beast::file_mode::scan, ec);

    if (ec == boost::beast::errc::no_such_file_or_directory) {
        boost::beast::http::response<boost::beast::http::string_body> not_found{ boost::beast::http::status::not_found, req.version() };
        not_found.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        not_found.set(boost::beast::http::field::content_type, "text/plain");
        not_found.set(boost::beast::http::field::access_control_allow_origin, "*");
        not_found.keep_alive(req.keep_alive());
        not_found.body() = "File not found";
        not_found.prepare_payload();
        return not_found;
    }
    if (ec) {
        boost::beast::http::response<boost::beast::http::string_body> bad{ boost::beast::http::status::internal_server_error, req.version() };
        bad.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        bad.set(boost::beast::http::field::content_type, "text/plain");
        bad.set(boost::beast::http::field::access_control_allow_origin, "*");
        bad.keep_alive(req.keep_alive());
        bad.body() = "Error opening file";
        bad.prepare_payload();
        return bad;
    }

    auto const size = body.size();

    if (req.method() == boost::beast::http::verb::head) {
        boost::beast::http::response<boost::beast::http::empty_body> res{ boost::beast::http::status::ok, req.version() };
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, extension_to_type(target));
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.content_length(size);
        return res;
    }

    boost::beast::http::response<boost::beast::http::file_body> res{ boost::beast::http::status::ok, req.version() };
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, extension_to_type(target));
    res.set(boost::beast::http::field::access_control_allow_origin, "*");
    res.keep_alive(req.keep_alive());
    res.content_length(size);
    res.body() = std::move(body);
    return res; // message_generator from file_body
}
// ---------------------------------------------------------

namespace skynet::http::server::sync::detail {
    int do_session(boost::asio::ip::tcp::socket socket, std::shared_ptr<const std::string> doc_root) {
        boost::beast::error_code ec;
        boost::beast::flat_buffer buffer;

        IHTTP_Handler *handler = SN::SN_Factory<IHTTP_Handler>::CreateObject();



        for (;;) {
            boost::beast::http::request<boost::beast::http::string_body> req; /// <--- compile error
            boost::beast::http::read(socket, buffer, req, ec);
            if (ec == boost::beast::http::error::end_of_stream) break;
            if (ec) { fail(ec, "read"); return -1; }

            auto msg = handle_request(*doc_root, std::move(req), handler);
            bool keep = msg.keep_alive();
            boost::beast::write(socket, std::move(msg), ec);
            if (ec) { fail(ec, "write"); return -1; }
            if (!keep) break;
        }

        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        return 0;
    }
}
