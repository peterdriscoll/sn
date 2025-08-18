#pragma once
#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "inc/ihttp_server.h"
#include "inc/ihttp_handler.h"
#include "inc/iuser.h"

#include "session.hpp"

#include "inc/exp_ctrl_skynet_http_server_syncoo_lib.h"

namespace skynet::http::server::syncoo
{
    class SKYNET_HTTP_SERVER_SYNCOO_EXPORT server : public IHTTP_Server
    {
    public:
        server();
        virtual ~server() noexcept;

        virtual int setup(const char* address,
            const char* port,
            const char* doc_root,
            IHTTP_Handler* handler,
            IUser* guest);

        int start() override;

        int run() override; // blocking accept loop (sync)

        int stop() override;

        void on_session_start();
        void on_session_end();
    private:
        std::atomic<bool>                               m_stopping{ false };
        boost::asio::io_context                         m_io;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
        std::string                                     m_doc_root;
        std::thread m_thread;
        IHTTP_Handler* m_handler;
        IUser* m_guest; // non-owning pointer to the user, if any

        std::shared_ptr<session> m_session;    // single live session
        std::atomic<int> session_cnt{ 0 };
        std::mutex m;
        std::mutex mtx;
        std::condition_variable cv;
    };

} // namespace
