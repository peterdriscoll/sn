//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include "../inc/ihttp_handler.h"

IHTTP_Handler *LoadLibrary();

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (false && argc != 4)
    {
      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }
	IHTTP_Handler *handler = LoadLibrary();
	http::server::request_handler::SetIHTTP_Handler(handler);

    // Initialise the server.
	//http::server::server s(argv[1], argv[2], argv[3]);
	char *doc_root = "C:/Users/peter_driscoll/Documents/Source/Repos/skynet2/html";

	http::server::server s("0.0.0.0", "80", doc_root);

	http::server::request_handler::SetIHTTP_Handler(LoadLibrary());
    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
