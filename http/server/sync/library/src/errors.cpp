// Portions of this file are derived from Boost.Beast example "http_server_sync.cpp".
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE_1_0.txt or https://www.boost.org/LICENSE_1_0.txt
// Modifications (c) 2025 Peter Driscoll

#include "skynet/http/server/sync/detail/errors.hpp"
#include <boost/system/error_code.hpp>   // boost::system::error_code
#include <iostream>

#include "skynet_http_server_sync_lib_pch.h"

namespace skynet::http::server::sync::detail {
    void fail(const boost::system::error_code& ec, const char* what) noexcept {
        std::cerr << what << ": " << ec.message() << "\n";
    }
}
