// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_HEADER_H_INCLUDED
#define HTTP_HEADER_H_INCLUDED

#include <string>

namespace HTTP {
	namespace server {

		struct header
		{
			std::string name;
			std::string value;
		};

	}
}

#endif // HTTP_HEADER_H_INCLUDED
