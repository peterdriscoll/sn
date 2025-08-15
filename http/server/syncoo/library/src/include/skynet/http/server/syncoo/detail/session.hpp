#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <memory>
#include <string>

namespace boost { namespace system { class error_code; } }
namespace boost { namespace asio { namespace ip { class tcp; } } }

namespace skynet::http::server::sync::detail {
    // Own the socket—pass by value so the thread takes ownership.
    int do_session(
        boost::asio::ip::tcp::socket socket,
        std::shared_ptr<const std::string> doc_root);
}
