#include "login_msg_handler.hpp"
#include "main/virtual_client_fwd.hpp"
#include "main/virtual_client.hpp"
#include "common/common_struct.hpp"

#include <boost/lexical_cast.hpp>

USING_NS_NETWORK;
USING_NS_COMMON;

server_msg_handler_t<op_cmd::LC_BEGIN, op_cmd::LC_END, login_msg_handler_t::msg_handler_func> login_msg_handler_t::m_login_msg_handle;

bool login_msg_handler_t::init_msg_handler()
{
    m_login_msg_handle.register_func(op_cmd::lc_login_reply,                  handle_lc_login_reply);

    return true;
}

bool login_msg_handler_t::handle_lc_login_reply(const msg_buf_ptr& msg_buf)
{
    // disconnect with login
    env::server->disconnect_with_login();

    PRE_S2C_MSG(proto::client::lc_login_reply);
    if (msg.reply_code() != 0)
    {
        log_error("client login to login_server failed! ErrorCode=%d", msg.reply_code());
        std::string reply("virtual client login to login_server failed! ErrorCode=%d", msg.reply_code());
        env::server->send_msg_to_http(reply);
        return false;
    }
    
    // connect to gate
    std::string uid = msg.uid();
    std::string guid = msg.general().guid();
    std::string gate_ip = msg.gate_ip();
    uint32_t gate_port = msg.gate_port();
    if (false == env::server->connect_to_gate(gate_ip, gate_port))
    {
        std::string reply("vitual client connect to gate with socket failed!"); 
        env::server->send_msg_to_http(reply);
        return false;
    }
    env::server->set_guid(guid);
    env::server->set_uid(boost::lexical_cast<uint64_t>(uid));
    env::server->set_gate_ip(gate_ip);
    env::server->set_gate_port(gate_port);

    proto::client::ca_connect_request req;
    req.set_uid(uid);
    req.set_guid(guid);
    req.set_reconnect(1);
    env::server->send_msg_to_gate(op_cmd::ca_connect_request, req);

    return true;
}
