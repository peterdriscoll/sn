#pragma once
#include <memory>
#include <string>
#include <boost/asio/ip/tcp.hpp>

namespace skynet::http::server::syncoo
{
    class request_handler;

    class session {
    public:
        session(boost::asio::ip::tcp::socket &&socket, const std::string& doc_root);

        int run(); // returns 0 on normal close

    private:
        boost::asio::ip::tcp::socket m_socket;
        std::string m_doc_root;
    };

} // namespace
