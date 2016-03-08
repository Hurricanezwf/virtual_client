#include "gate_msg_handler.hpp"
#include "main/virtual_client_fwd.hpp"
#include "main/virtual_client.hpp"
#include "common/common_struct.hpp"

#include <boost/lexical_cast.hpp>

USING_NS_NETWORK;
USING_NS_COMMON;

server_msg_handler_t<op_cmd::AC_BEGIN, op_cmd::AC_END, gate_msg_handler_t::msg_handler_func> gate_msg_handler_t::m_gate_msg_handle;
server_msg_handler_t<op_cmd::GC_BEGIN, op_cmd::GC_END, gate_msg_handler_t::msg_handler_func> gate_msg_handler_t::m_logic_msg_handle;

bool gate_msg_handler_t::init_msg_handler()
{
    // gate msg
    m_gate_msg_handle.register_func(op_cmd::ac_connect_reply,           handle_ac_connect_reply);

    // logic msg
    m_logic_msg_handle.register_func(op_cmd::gc_enter_game_reply,       handle_gc_enter_game_reply);

    return true;
}

bool gate_msg_handler_t::handle_ac_connect_reply(const msg_buf_ptr& msg_buf)
{
    std::string reply;

    PRE_S2C_MSG(proto::client::ac_connect_reply);
    if (msg.reply_code() != 0)
    {
        log_error("virtual client connect to gate failed!");
        reply = "virtual client connect to gate failed!";    
        env::server->send_err_to_http(reply);
        return false;
    }

    proto::client::cg_enter_game_request req;
    env::server->send_msg_to_gate(op_cmd::cg_enter_game_request, req);
    
    return true;
}


bool gate_msg_handler_t::handle_gc_enter_game_reply(const msg_buf_ptr& msg_buf)
{
    PRE_S2C_MSG(proto::client::gc_enter_game_reply);
    if (msg.reply_code() != 0)
    {
        log_warn("enter game failed! need to create new user");
        proto::client::cg_create_user_request req;
        req.set_nickname(boost::lexical_cast<std::string>(env::server->get_uid()));
        env::server->send_msg_to_gate(op_cmd::cg_create_user_request, req);
    }
    else
    {
        char buf[256];
        sprintf(buf, "{'code':0, 'msg':'nihao'}");
        //sprintf(buf, "enter game success!<br> \
                //`uid`=%s<br>", 
                //msg.user().uid().c_str());
        std::string reply(buf);
        env::server->send_reply_to_http("gc_enter_game_reply", reply);
    }

    return true;
}
