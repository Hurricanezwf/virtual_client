#include "gate_connector.hpp"
#include "protobuf.hpp"
#include "main/virtual_client_fwd.hpp"
#include "main/virtual_client.hpp"
#include "gate/gate_msg_handler.hpp"

USING_NS_NETWORK;
USING_NS_COMMON;

void gate_connector_t::on_connect(const tcp_socket_ptr& s)
{
    if (NULL == s)
    {
        log_error("socket is NULL!");
        return;
    }
    m_gate_socket = s;
}

void gate_connector_t::on_new_msg(const tcp_socket_ptr& s, const message_t& msg)
{
    if (s != m_gate_socket)
    {
        log_error("s != m_gate_socket");
        return;
    }
    env::server->get_ios().post(boost::bind(&gate_msg_handler_t::handle_server_msg, msg.head.cmd, msg.body));
}

void gate_connector_t::on_close(const tcp_socket_ptr& s)
{
    log_info("gate disconnected!");

    m_gate_socket.reset();
}

void gate_connector_t::send_msg_to_gate( const msg_buf_ptr& msg )
{
    if (NULL == m_gate_socket)
    {
        log_error("m_gate_socket is NULL!");
        return;
    }
    env::server->network_send_msg(m_gate_socket, msg);
}
