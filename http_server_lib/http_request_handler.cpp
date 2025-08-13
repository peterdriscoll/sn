// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <fstream>
#include <sstream>
#include <string>

#include "http_request_handler.h"
#include "http_mime_types.h"
#include "http_reply.h"
#include "http_request.h"

#include "http_server_lib_pch.h"

#pragma warning (disable: 4244)

namespace HTTP
{
	namespace server
	{
		request_handler::request_handler()
			: m_handler(0)
		{
		}

		request_handler::request_handler(const std::string& doc_root)
			: doc_root_(doc_root)
			, m_handler(0)
		{
		}

		request_handler::~request_handler()
		{
			delete m_handler;
		}

		void request_handler::setup(const std::string & doc_root)
		{
			doc_root_ = doc_root;
		}

		void request_handler::handle_request(const request& req, reply& rep)
		{
			// Decode url to path.
			std::string request_path;
			if (!url_decode(req.uri, request_path))
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			// Request path must be absolute and not contain "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos)
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			// If path ends in slash (i.e. is a directory) then add "index.html".
			if (request_path[request_path.size() - 1] == '/')
			{
				request_path += "index.html";
			}

			std::string extension = "html";

			// Break up the path.
			std::size_t first_question_pos = request_path.find_first_of("?");
			string path;
			string paramString;
			if (first_question_pos == std::string::npos)
			{
				path = request_path;
			}
			else
			{
				path = request_path.substr(0, first_question_pos);
				paramString = request_path.substr(first_question_pos+1);
			}

			std::size_t last_slash_pos = path.find_last_of("/");
			std::size_t last_dot_pos = path.find_last_of(".");
			if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
			{
				extension = path.substr(last_dot_pos + 1);
			}

			// Open the file to send back.
			std::string full_path = doc_root_ + path;

			if (!m_handler)
			{
				m_handler = SN::SN_Factory<IHTTP_Handler>::CreateObject();
			}

			if (m_handler->handle_response(path.data(), paramString.data(), extension.data()))
			{
				rep.content.append(m_handler->response_data());
				extension = m_handler->extension();
			}
			else
			{
				std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
				if (!is)
				{
					rep = reply::stock_reply(reply::not_found);
					return;
				}
				char buf[512];
				while (is.read(buf, sizeof(buf)).gcount() > 0)
					rep.content.append(buf, is.gcount());
			}
			// Fill out the reply to be sent to the client.
			rep.status = reply::ok;
			rep.headers.resize(3);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = std::to_string(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = mime_types::extension_to_type(extension);
			rep.headers[2].name = "Access-Control-Allow-Origin";
			rep.headers[2].value = "*";
		}

		bool request_handler::url_decode(const std::string& in, std::string& out)
		{
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				if (in[i] == '%')
				{
					if (i + 3 <= in.size())
					{
						int value = 0;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							out += static_cast<char>(value);
							i += 2;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (in[i] == '+')
				{
					out += ' ';
				}
				else
				{
					out += in[i];
				}
			}
			return true;
		}
	} // namespace server
} // namespace http
