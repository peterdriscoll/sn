// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HANDLER_H_INCLUDED
#define HTTP_REQUEST_HANDLER_H_INCLUDED

#include <string>
#include <unordered_map>
using namespace std;

namespace HTTP {
	namespace server {

		struct reply;
		struct request;

		typedef unordered_map<string, string> string_umap;

		/// The common handler for all incoming requests.
		class request_handler
		{
		public:
			request_handler(const request_handler&) = delete;
			request_handler& operator=(const request_handler&) = delete;

			/// Construct with a directory containing files to be served.
			explicit request_handler(const std::string& doc_root);

			/// Handle a request and produce a reply.
			void handle_request(const request& req, reply& rep);

		private:
			/// The directory containing the files to be served.
			std::string doc_root_;

			/// Perform URL-decoding on a string. Returns false if the encoding was
			/// invalid.
			static bool url_decode(const std::string& in, std::string& out);
			static string extract_values(const string &p_URL, string_umap &p_Map);
		};

	}
}

#endif // HTTP_REQUEST_HANDLER_H_INCLUDED
