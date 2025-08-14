#pragma once

#include <string>
#include "inc/ihttp_server.h"
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "inc/exp_ctrl_skynet_http_server_sync_lib.h"

namespace skynet::http::server::sync
{
    // Implement IHTTP_Server.
    class SKYNET_HTTP_SERVER_SYNC_EXPORT Server : public IHTTP_Server
    {
    public:
        Server() = default;
        ~Server() override = default;

        void setup(const std::string& address, const std::string& port, const std::string& doc_root) override;

        void run() override;

        void stop() override;

    private:
        IHTTP_Handler* m_handler = nullptr;
        boost::asio::ip::address m_address;
        boost::asio::ip::port_type m_port = 0;
        std::shared_ptr<std::string> m_doc_root;
    };
} // namespace
