#pragma once
#include <boost/system/error_code.hpp>

namespace skynet::http::server::sync::detail {
	void fail(const boost::system::error_code& ec, const char* what) noexcept;
}
