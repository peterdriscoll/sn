#pragma once
#include <memory>
#include <string>
#include <boost/asio/ip/tcp.hpp>

namespace skynet::http::server::syncoo
{
    class request_handler;
    class server;

    class session {
    public:
        session(server* srv, boost::asio::ip::tcp::socket &&socket,
            const std::string& doc_root,
            IUser* guest,
            IHTTP_Handler* handler);
        ~session();

        int run(); // returns 0 on normal close

        void stop();

    private:
		server* m_server; // non-owning pointer to the server
        std::atomic<bool> stopping_{ false };

        boost::asio::ip::tcp::socket m_socket;
        std::string m_doc_root;
        IUser* m_guest;
        IHTTP_Handler* m_HTTP_Handler;
    };

} // namespace
