#pragma once

#include <memory>
#include <string>

namespace boost { namespace asio { class io_context; } }
namespace boost { namespace asio { namespace ip { class address; } } }

namespace skynet::http::server::sync::detail {
    int run_accept_loop(
        boost::asio::io_context& ioc,
        const boost::asio::ip::address& address,
        unsigned short port,
        std::shared_ptr<const std::string> doc_root);
}
