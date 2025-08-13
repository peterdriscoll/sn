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

#include "../inc/ihttp_server.h"
#include "../inc/sn_factory.h"

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

    // Initialise the server.
	const char *doc_root = "C:/Users/peter/Documents/Source/Repos/skynet2/html";

	IHTTP_Server *server = SN::SN_Factory<IHTTP_Server>::CreateObject();
	
	//server->setup(argv[1], argv[2], argv[3]);
	server->setup("0.0.0.0", "80", doc_root);

	// Run the server until stopped.
	server->run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
