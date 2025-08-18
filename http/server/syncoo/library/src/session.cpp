#include "skynet/http/server/syncoo/session.hpp"

#include "skynet/http/server/syncoo/request_handler.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <utility>

#include "skynet_http_server_syncoo_lib_pch.h"

namespace skynet::http::server::syncoo
{
    session::session(
        server* srv,
        boost::asio::ip::tcp::socket &&socket,
        const std::string &doc_root,
        IUser* guest,
        IHTTP_Handler* handler)
        : m_socket(std::move(socket))
        , m_doc_root(doc_root)
		, m_guest(guest)
		, m_HTTP_Handler(handler)
        , m_server(srv)
    {
        m_server->on_session_start();
    }

    session::~session()
    {
        m_server->on_session_end();
    }

    int skynet::http::server::syncoo::session::run()
    {
        boost::beast::error_code ec;
        boost::beast::flat_buffer buffer;

        skynet::http::server::syncoo::request_handler handler(m_doc_root, m_HTTP_Handler, m_guest);

        int  status = 0;
        bool running = true;

        while (running)
        {
            boost::beast::http::request<boost::beast::http::string_body> req;
            boost::beast::http::read(m_socket, buffer, req, ec);

            const bool eof = (ec == boost::beast::http::error::end_of_stream);
            const bool read_ok = !ec;

            if (read_ok)
            {
                auto msg = handler.handle(std::move(req));
                const bool keep = msg.keep_alive();

                boost::beast::write(m_socket, std::move(msg), ec);
                const bool write_ok = !ec;

                // next iteration:
                running = (write_ok && keep);
                status = write_ok ? 0 : -1;
            }
            else
            {
                // graceful close on EOF, error otherwise
                running = false;
                status = eof ? 0 : -1;
            }
        }

        // half-close send side; ignore shutdown errors
        boost::beast::error_code ignore;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignore);
        return status;
    }

    void skynet::http::server::syncoo::session::stop() {
        bool expected = false;
        if (!stopping_.compare_exchange_strong(expected, true)) return;
        boost::system::error_code ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);  // unblocks http::read(...)
        m_socket.close(ec);
    }
} // namespace
