#include "skynet/http/server/syncoo/request_handler.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <fstream>
#include <utility>

// Include your factory header (adjust path)
#include "inc/sn_factory.h"  // SN::SN_Factory<IHTTP_Handler>::CreateObject()

#include "skynet_http_server_syncoo_lib_pch.h"

namespace skynet::http::server::syncoo
{
    request_handler::request_handler(const std::string& doc_root, IHTTP_Handler* HTTP_Handler, IUser* guest)
    : m_doc_root(doc_root)
	, m_HTTP_Handler(HTTP_Handler)
	, m_guest(guest)    
{
}

const char* request_handler::extension_to_type(const std::string& ext)
{
    using boost::beast::iequals;
    if (iequals(ext, "htm")  || iequals(ext, "html")) return "text/html; charset=utf-8";
    if (iequals(ext, "css"))                           return "text/css";
    if (iequals(ext, "txt"))                           return "text/plain; charset=utf-8";
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

void request_handler::normalize_target(const boost::beast::string_view& target_sv,
                                       std::string& path_out,
                                       std::string& query_out,
                                       std::string& ext_out)
{
    boost::beast::string_view t = target_sv;
    std::size_t qpos = t.find('?');
    if (qpos != boost::beast::string_view::npos) {
        query_out.assign(t.data() + qpos + 1, t.size() - qpos - 1);
        t = t.substr(0, qpos);
    } else {
        query_out.clear();
    }

    path_out.assign(t.data(), t.size());
    if (path_out.empty() || path_out[0] != '/') path_out = "/";
    if (!path_out.empty() && path_out.back() == '/') path_out += "index.html";

    std::size_t dot = path_out.rfind('.');
    if (dot != std::string::npos && dot + 1 < path_out.size())
        ext_out = path_out.substr(dot + 1); // no leading '.'
    else
        ext_out.clear();
}

boost::beast::http::message_generator
request_handler::handle(boost::beast::http::request<boost::beast::http::string_body>&& req)
{
    std::string path, query, ext;
    normalize_target(req.target(), path, query, ext);

	// Is the server running?
    if (path == "/healthz" && req.method() == boost::beast::http::verb::get)
    {
        boost::beast::http::response<boost::beast::http::string_body> res{ boost::beast::http::status::ok, req.version() };
        res.set(boost::beast::http::field::cache_control, "no-store");                 // avoid caches
        res.set(boost::beast::http::field::content_type, "text/plain; charset=utf-8");
        res.body() = "OK";
        res.prepare_payload();                                           // sets Content-Length
        res.keep_alive(req.keep_alive());                                 // or false while debugging
        return res;  // if your handler returns message_generator, this will convert implicitly
    }

    // 1) Your plugin first (exact port of your old behavior)
    if (m_HTTP_Handler &&
        m_HTTP_Handler->handle_response(path.c_str(), query.c_str(), ext.c_str(), m_guest))
    {
        // Prefer the plugin's declared extension; fall back to the URL's ext if empty.
        std::string plugin_ext = m_HTTP_Handler->extension();
        if (plugin_ext.empty()) plugin_ext = ext;
        boost::beast::http::response<boost::beast::http::string_body> res{
            boost::beast::http::status::ok, req.version()
        };
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, extension_to_type(plugin_ext));
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.body() = m_HTTP_Handler->response_data();
        res.prepare_payload();
        return res; // implicit to message_generator
    }

    // 2) Static files: allow only GET/HEAD
    if (req.method() != boost::beast::http::verb::get && req.method() != boost::beast::http::verb::head) {
        boost::beast::http::response<boost::beast::http::string_body> bad{ boost::beast::http::status::bad_request, req.version() };
        bad.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        bad.set(boost::beast::http::field::content_type, "text/plain; charset=utf-8");
        bad.set(boost::beast::http::field::access_control_allow_origin, "*");
        bad.keep_alive(req.keep_alive());
        bad.body() = "Unsupported method";
        bad.prepare_payload();
        return bad;
    }

    std::string full_path(m_doc_root.data(), m_doc_root.size());
    full_path += path;

    boost::beast::error_code ec;
    boost::beast::http::file_body::value_type body;
    body.open(full_path.c_str(), boost::beast::file_mode::scan, ec);

    if (ec == boost::beast::errc::no_such_file_or_directory) {
        boost::beast::http::response<boost::beast::http::string_body> not_found{ boost::beast::http::status::not_found, req.version() };
        not_found.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        not_found.set(boost::beast::http::field::content_type, "text/plain; charset=utf-8");
        not_found.set(boost::beast::http::field::access_control_allow_origin, "*");
        not_found.keep_alive(req.keep_alive());
        not_found.body() = "File not found";
        not_found.prepare_payload();
        return not_found;
    }
    if (ec) {
        boost::beast::http::response<boost::beast::http::string_body> bad{ boost::beast::http::status::internal_server_error, req.version() };
        bad.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        bad.set(boost::beast::http::field::content_type, "text/plain; charset=utf-8");
        bad.set(boost::beast::http::field::access_control_allow_origin, "*");
        bad.keep_alive(req.keep_alive());
        bad.body() = "Error opening file";
        bad.prepare_payload();
        return bad;
    }

    std::uint64_t size = body.size();

    if (req.method() == boost::beast::http::verb::head) {
        boost::beast::http::response<boost::beast::http::empty_body> res{ boost::beast::http::status::ok, req.version() };
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, extension_to_type(ext));
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.content_length(size);
        return res;
    }

    boost::beast::http::response<boost::beast::http::file_body> res{ boost::beast::http::status::ok, req.version() };
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, extension_to_type(ext));
    res.set(boost::beast::http::field::access_control_allow_origin, "*");
    res.keep_alive(false /*req.keep_alive()*/);
    res.content_length(size);
    res.body() = std::move(body);
    return res;
}

} // namespace skynet::http::server::syncoo
