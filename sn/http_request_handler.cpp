// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "http_request_handler.h"
#include "http_mime_types.h"
#include "http_reply.h"
#include "http_request.h"
#include "sni_frame.h"

#include <fstream>
#include <sstream>
#include <string>

#include "sn_pch.h"
using namespace std;

namespace HTTP
{
	namespace server
	{

		request_handler::request_handler(const std::string& doc_root)
			: doc_root_(doc_root)
		{
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
			string gotostepcount = "/gotostepcount";
			string maxstackframes = "/maxstackframes";
			if (request_path == "/skynet")
			{
				rep.content = SN::SN_Manager::GetTopManager().Skynet();
			}
			else if (request_path == "/run")
			{
				rep.content = SN::SN_Manager::GetTopManager().Run();
			}
			else if (request_path == "/runtoend")
			{
				rep.content = SN::SN_Manager::GetTopManager().RunToEnd();
			}
			else if (request_path == "/debugbreak")
			{
				rep.content = SN::SN_Manager::GetTopManager().DebugBreak();
			}
			else if (request_path == "/stepover")
			{
				rep.content = SN::SN_Manager::GetTopManager().StepOver();
			}
			else if (request_path == "/stepinto")
			{
				rep.content = SN::SN_Manager::GetTopManager().StepInto();
			}
			else if (request_path == "/stepout")
			{
				rep.content = SN::SN_Manager::GetTopManager().StepOut();
			}
			else if (request_path == "/stepparam")
			{
				rep.content = SN::SN_Manager::GetTopManager().StepParam();
			}
			else if (request_path.substr(0, gotostepcount.size()) == gotostepcount)
			{
				string data = request_path.substr(gotostepcount.size());
				long stepCount = 0;
				long threadNum = 0;
				vector<string> assignmentList;
				Split(data, "?", assignmentList);
				for (const string &element : assignmentList)
				{
					vector<string> tokenList;
					Split(element, "=", tokenList);
					if (tokenList[0] == "stepcount")
					{
						stepCount = atol(tokenList[1].c_str());
					}
					else if (tokenList[0] == "threadnum")
					{
						threadNum = atol(tokenList[1].c_str());
					}
				}
				rep.content = SN::SN_Manager::GetTopManager().GotoStepCount(stepCount, threadNum);
			}
			else if (request_path.substr(0, maxstackframes.size()) == maxstackframes)
			{
				string data = request_path.substr(gotostepcount.size());
				long stackDepth = 0;
				vector<string> assignmentList;
				Split(data, "?", assignmentList);
				for (const string &element : assignmentList)
				{
					vector<string> tokenList;
					Split(element, "=", tokenList);
					if (tokenList[0] == "stackdepth")
					{
						stackDepth = atol(tokenList[1].c_str());
					}
				}
				rep.content = SN::SN_Manager::GetTopManager().SetMaxStackFrames(stackDepth);
			}
			else if (request_path == "/quit")
			{
				rep.content = SN::SN_Manager::GetTopManager().Quit();
			}
			else
			{
				// Determine the file extension.
				std::size_t last_slash_pos = request_path.find_last_of("/");
				std::size_t last_dot_pos = request_path.find_last_of(".");
				if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
				{
					extension = request_path.substr(last_dot_pos + 1);
				}

				// Open the file to send back.
				std::string full_path = doc_root_ + request_path;
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
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = std::to_string(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = mime_types::extension_to_type(extension);
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
