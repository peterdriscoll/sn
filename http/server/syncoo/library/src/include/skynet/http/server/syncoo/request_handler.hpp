#pragma once

#include <memory>
#include <string>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>

#include "inc/ihttp_handler.h"  // The IHTTP_Handler interface

namespace skynet::http::server::syncoo
{
    class request_handler
    {
    public:
        explicit request_handler(const std::string &doc_root);

        // Central choke point: turn a Request into a message_generator
        boost::beast::http::message_generator
        handle(boost::beast::http::request<boost::beast::http::string_body>&& req);

    private:
        std::string m_doc_root;
        std::unique_ptr<IHTTP_Handler>     m_plugin;

        static const char* extension_to_type(const std::string& ext);
        static void normalize_target(const boost::beast::string_view& target_sv,
                                     std::string& path_out,
                                     std::string& query_out,
                                     std::string& ext_out);
    };

} // namespace skynet::http::server::syncoo
