#include "httphandlersimple.h"
#include "requestadapter.h"

#include "sni_user.h"
#include "sni_thread.h"

#include <string_view>
#include "sn_pch.h"

namespace SNI
{

    HttpHandlerSimple::HttpHandlerSimple() = default;
    HttpHandlerSimple::~HttpHandlerSimple() = default;

    bool HttpHandlerSimple::handle_response(const char* pathC,
        const char* queryC,
        const char* /*url_ext*/,
        IUser* iuser)
    {
        SN::SN_Transaction::RegisterInWebServer();

        std::string_view path = pathC ? std::string_view{ pathC } : std::string_view{};

		SNI_User* user = dynamic_cast<SNI_User*>(iuser);
        RequestAdapter adapter(queryC);
        if (!user) return false;

        const long tnum = adapter.threadnum(0);
        SNI_Thread* thr = (tnum >= 0) ? user->GetThreadByNumber(tnum) : nullptr;
        if (!thr) return false;

        RequestAdapter::Reply r;
        if (adapter.dispatch(path, *user, *thr, r))
        {
            m_response = std::move(r.body);
            m_ext = r.ext;
            return true;
        }
        return false;
    }

    const char* HttpHandlerSimple::response_data()
    { 
        return m_response.c_str();
    }
    const char* HttpHandlerSimple::extension()
    { 
        return m_ext.c_str();
    }

} // namespace SNI