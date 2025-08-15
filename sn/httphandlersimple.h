#pragma once

// New handler using a per-request RequestAdapter with a static route table.
// This file is intended to replace sni_http_handler.h (kept separate for diffing).

#include "ihttp_handler.h"
#include <string>
#include <unordered_map>

// Project-wide alias
typedef std::unordered_map<std::string, std::string> string_umap;

namespace SNI {

    class HttpHandlerSimple : public IHTTP_Handler {
    public:
        HttpHandlerSimple();
        ~HttpHandlerSimple() override;

        bool        handle_response(const char* path, const char* query, const char* url_ext) override;
        const char* response_data() override;
        const char* extension()     override;

    private:
        // (your RequestAdapter use lives inside handle_response)
        std::string m_response;
        std::string m_ext;
    };
} // namespace SNI
