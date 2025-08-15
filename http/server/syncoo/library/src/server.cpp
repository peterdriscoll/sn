#include "skynet/http/server/syncoo/server.hpp"

#include "skynet/http/server/syncoo/session.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "skynet_http_server_syncoo_lib_pch.h"

namespace skynet::http::server::syncoo
{
    server::server()
    {
    }

    server::~server()
    {
        stop();
        if (m_thread.joinable()) m_thread.join();
    }

    void server::setup(const std::string& address,
        const std::string& port,
        const std::string& doc_root)
    {
        m_doc_root = doc_root;
        m_acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(m_io);

        boost::asio::ip::tcp::resolver resolver(m_io);
        auto results = resolver.resolve(address, port);
        auto ep = (*results.begin()).endpoint();

        m_acceptor->open(ep.protocol());
        m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor->bind(ep);
        m_acceptor->listen(boost::asio::socket_base::max_listen_connections);
    }

    void server::start() {
        if (m_thread.joinable()) return;    // <-- idempotent
        m_thread = std::thread([this] { this->run(); });
    }

    int skynet::http::server::syncoo::server::run()
    {
        int  status = 0;
        bool running = true;

        if (!m_acceptor) {
            status = -1;          // not set up
            running = false;       // nothing to do; fall through to return
        }
        else {
            m_stopping.store(false, std::memory_order_relaxed);
        }

        while (running)
        {
            boost::asio::ip::tcp::socket socket(m_io);
            boost::system::error_code ec;
            m_acceptor->accept(socket, ec);

            const bool stopping = m_stopping.load(std::memory_order_relaxed);
            const bool aborted = (ec == boost::asio::error::operation_aborted) ||
                (ec == boost::asio::error::bad_descriptor);

            // Only handle a connection when accept() succeeded and we're not stopping.
            const bool handle_connection = (!stopping && !ec);

            if (handle_connection) {
                skynet::http::server::syncoo::session(std::move(socket), m_doc_root).run();
            }

            // Decide next-iteration state:
            // - If stopping or accept was aborted/closed -> exit after this iteration.
            // - If transient error (ec && !aborted)      -> keep looping.
            // - If handled connection                     -> keep looping.
            running = !(stopping || aborted);
        }

        return status;
    }

    void server::stop()
    {
        m_stopping.store(true, std::memory_order_relaxed);
        if (m_acceptor) {
            boost::system::error_code ec;
            m_acceptor->cancel(ec); // unblocks accept()
            m_acceptor->close(ec);
        }
        m_io.stop();
    }

} // namespace
