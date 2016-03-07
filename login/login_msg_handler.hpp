#ifndef _VIRTUAL_CLIENT_LOGIN_MSG_HANDLER_HPP__
#define _VIRTUAL_CLIENT_LOGIN_MSG_HANDLER_HPP__

#include "msg_handler.hpp"
#include "protos_fwd.hpp"

class login_msg_handler_t
{
public:
    typedef bool (*msg_handler_func)(const network::msg_buf_ptr& msg_buf);

    static bool init_msg_handler();

    static bool handle_server_msg(uint16_t cmd, const network::msg_buf_ptr msg_buf)
    {
        log_trace("recv cmd[%d] message from login", cmd);

        msg_handler_func func = m_login_msg_handle.find_func(cmd);
        if ( NULL == func )
        {
            log_error("no handler func for msg cmd:%d", cmd);
            return false;
        }
        func(msg_buf);

        return true;
    };

public:
    static bool handle_lc_login_reply(const network::msg_buf_ptr& msg_buf);

private:
    static network::server_msg_handler_t<op_cmd::LC_BEGIN, op_cmd::LC_END, msg_handler_func> m_login_msg_handle;
};

#endif
