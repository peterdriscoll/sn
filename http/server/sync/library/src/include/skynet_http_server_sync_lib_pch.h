#pragma once

#include "inc/sn_factory.h"
#include "inc/ihttp_handler.h"

#include "skynet/http/server/sync/server.h"

#include "skynet/http/server/sync/detail/listener.hpp"
#include "skynet/http/server/sync/detail/session.hpp"
#include "skynet/http/server/sync/detail/server_sync.hpp"
#include "skynet/http/server/sync/detail/errors.hpp"

#include <boost/system/error_code.hpp>   // boost::system::error_code
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>      // for BOOST_BEAST_VERSION_STRING
#include <boost/asio/ip/tcp.hpp>

#include <thread>
#include <iostream>
#include <string>
#include <iostream>
#include <memory>
#include <cstdlib>
