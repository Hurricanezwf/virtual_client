#ifndef _VIRTUAL_CLIENT_GATE_MSG_HANDLER_HPP__
#define _VIRTUAL_CLIENT_GATE_MSG_HANDLER_HPP__

#include "msg_handler.hpp"
#include "protos_fwd.hpp"

class gate_msg_handler_t
{
public:
    typedef bool (*msg_handler_func)(const network::msg_buf_ptr& msg_buf);

    static bool init_msg_handler();

    static bool handle_server_msg(uint16_t cmd, const network::msg_buf_ptr msg_buf)
    {
        log_trace("recv cmd[%d] message from gate", cmd);

        msg_handler_func func;
        if (cmd > op_cmd::AC_BEGIN && cmd < op_cmd::AC_END)
        {
            func = m_gate_msg_handle.find_func(cmd);
        }
        else if (cmd > op_cmd::GC_BEGIN && cmd < op_cmd::GC_END)
        {
            func = m_logic_msg_handle.find_func(cmd);
        }
        else
        {
            func = NULL;
        }

        if ( NULL == func )
        {
            log_error("no handler func for msg cmd:%d", cmd);
            return false;
        }
        func(msg_buf);

        return true;
    };

private:
    // gate msg
    static bool handle_ac_connect_reply(const network::msg_buf_ptr& msg_buf);
    static bool handle_ac_kick_user_notify(const network::msg_buf_ptr& msg_buf);

    // logic msg
    static bool handle_gc_enter_game_reply(const network::msg_buf_ptr& msg_buf);

    // item relevant
    static bool handle_gc_create_item_reply(const network::msg_buf_ptr& msg_buf);
    static bool handle_gc_cost_item_reply(const network::msg_buf_ptr& msg_buf);


private:
    static network::server_msg_handler_t<op_cmd::AC_BEGIN, op_cmd::AC_END, msg_handler_func> m_gate_msg_handle;
    static network::server_msg_handler_t<op_cmd::GC_BEGIN, op_cmd::GC_END, msg_handler_func> m_logic_msg_handle;
};

#endif
