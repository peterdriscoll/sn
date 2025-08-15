#include "server.hpp"
#include <boost/asio/io_context.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    try {
        std::string address = "0.0.0.0";
        std::string port    = "8080";
        std::string docroot = argc > 1 ? argv[1] : "."; // pass doc_root as argv[1]

        boost::asio::io_context io;
        skynet::http::server::sync::server srv(io, address, port, docroot);
        return srv.run();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal: " << ex.what() << std::endl;
        return 1;
    }
}
