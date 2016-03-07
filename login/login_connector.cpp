#include "login_connector.hpp"
#include "protobuf.hpp"
#include "main/virtual_client_fwd.hpp"
#include "main/virtual_client.hpp"
#include "login/login_msg_handler.hpp"

USING_NS_NETWORK;
USING_NS_COMMON;

void login_connector_t::on_connect(const tcp_socket_ptr& s)
{
    if (NULL == s)
    {
        log_error("socket is NULL!");
        return;
    }
    m_login_socket = s;
}

void login_connector_t::on_new_msg(const tcp_socket_ptr& s, const message_t& msg)
{
    if (s != m_login_socket)
    {
        log_error("s != m_login_socket");
        return;
    }
    env::server->get_ios().post(boost::bind(&login_msg_handler_t::handle_server_msg, msg.head.cmd, msg.body));
}

void login_connector_t::on_close(const tcp_socket_ptr& s)
{
    log_info("login disconnected!");

    m_login_socket.reset();
}

void login_connector_t::send_msg_to_login( const msg_buf_ptr& msg )
{
    if (NULL == m_login_socket)
    {
        log_error("m_login_socket is NULL!");
        return;
    }
    env::server->network_send_msg(m_login_socket, msg);
}
