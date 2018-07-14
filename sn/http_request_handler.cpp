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

#pragma warning (disable: 4244)

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

			PGC::PGC_Transaction::RegisterInWebServer();

			// If path ends in slash (i.e. is a directory) then add "index.html".
			if (request_path[request_path.size() - 1] == '/')
			{
				request_path += "index.html";
			}
			std::string extension = "html";
			string_umap umap;
			string path = extract_values(request_path, umap);
			long threadNum = atol(umap["threadnum"].c_str());
			long stackdepth = atol(umap["stackdepth"].c_str());
			SNI_Thread::ThreadListLock();
			SNI::SNI_Thread *l_thread = SNI::SNI_Thread::GetThreadByNumber(threadNum);
			if (l_thread)
			{
				if (path == "/skynet")
				{
					rep.content = l_thread->Skynet();
				}
				else if (path == "/run")
				{
					rep.content = l_thread->RunWeb();
				}
				else if (path == "/runtoend")
				{
					rep.content = l_thread->RunToEndWeb();
				}
				else if (path == "/debug")
				{
					rep.content = l_thread->DebugWeb();
				}
				else if (path == "/debugbreak")
				{
					rep.content = l_thread->DebugBreakWeb();
				}
				else if (path == "/stepover")
				{
					rep.content = l_thread->StepOverWeb(stackdepth);
				}
				else if (path == "/stepinto")
				{
					rep.content = l_thread->StepIntoWeb();
				}
				else if (path == "/stepout")
				{
					rep.content = l_thread->StepOutWeb(stackdepth);
				}
				else if (path == "/stepparam")
				{
					rep.content = l_thread->StepParamWeb();
				}
				else if (path == "/gotostepcount")
				{
					long stepCount = atol(umap["stepcount"].c_str());
					rep.content = l_thread->GotoStepCountWeb(stepCount);
				}
				else if (path == "/maxstackframes")
				{
					long stackDepth = atol(umap["stackdepth"].c_str());
					rep.content = l_thread->SetMaxStackFramesWeb(stackDepth);
				}
				else if (path == "/thread")
				{
					rep.content = l_thread->Skynet();
				}
				else if (path == "/quit")
				{
					rep.content = l_thread->QuitWeb();
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
						SNI_Thread::ThreadListUnlock();
						return;
					}
					char buf[512];
					while (is.read(buf, sizeof(buf)).gcount() > 0)
						rep.content.append(buf, is.gcount());
				}
				SNI_Thread::ThreadListUnlock();
			}
			else
			{
				rep.content = SNI_Thread::ThreadEnded(threadNum);
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


		/*static*/ string request_handler::extract_values(const string &p_URL, string_umap &p_Map)
		{
			string result;
			vector<string> partList;
			Split(p_URL, "?", partList);
			vector<string> assignmentList;
			result = partList[0];
			if (partList.size() == 2)
			{
				Split(partList[1], "&", assignmentList);
				for (const string &element : assignmentList)
				{
					vector<string> tokenList;
					Split(element, "=", tokenList);
					string t0 = tokenList[0];
					string t1 = tokenList[1];
					p_Map[tokenList[0]] = tokenList[1];
					string r1 = p_Map[tokenList[0]];
				}
			}
			return result;
		}
	} // namespace server
} // namespace http
