#pragma once
#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "inc/ihttp_server.h"

#include "inc/exp_ctrl_skynet_http_server_syncoo_lib.h"

namespace skynet::http::server::syncoo
{
    class SKYNET_HTTP_SERVER_SYNCOO_EXPORT server : public IHTTP_Server
    {
    public:
        server();
        virtual ~server() noexcept;

        // Inherited via IHTTP_Server
        void setup(
               const std::string& address,
               const std::string& port,
               const std::string& doc_root) override;

        void start();

        int run() override; // blocking accept loop (sync)

        void stop() override;

    private:
        std::atomic<bool>                               m_stopping{ false };
        boost::asio::io_context                         m_io;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
        std::string                                     m_doc_root;
        std::thread m_thread;
    };

} // namespace
