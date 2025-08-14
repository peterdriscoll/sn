// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "skynet/http/server/sync/server.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <cstdlib>

#include "skynet_http_server_sync_lib_pch.h"

#include "inc/exp_ctrl_skynet_http_server_sync_lib.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace skynet::http::server::sync
{
	void Server::setup(const std::string& address, const std::string& port, const std::string& doc_root)
	{
        m_address = boost::asio::ip::make_address(address);
        m_port = static_cast<unsigned short>(std::atoi(port.c_str()));
        m_doc_root = std::make_shared<std::string>(doc_root);
	}
	void Server::run()
	{
        try
        {
            // The io_context is required for all I/O
            boost::asio::io_context ioc{ 1 };

            // The acceptor receives incoming connections
            boost::asio::ip::tcp::acceptor acceptor{ ioc, {m_address, m_port} };
            for (;;)
            {
                // This will receive the new connection
                boost::asio::ip::tcp::socket socket{ ioc };

                // Block until we get a connection
                acceptor.accept(socket);

                // Launch the session, transferring ownership of the socket
                std::thread(
                    [s = std::move(socket), root = m_doc_root]() mutable {
                        (void)skynet::http::server::sync::detail::do_session(std::move(s), root);
                    }
                ).detach();
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
	void Server::stop()
	{
	}
}