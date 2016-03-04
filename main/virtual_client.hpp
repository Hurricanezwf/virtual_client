#ifndef __VIRTUAL_CLIENT_HPP__
#define __VIRTUAL_CLIENT_HPP__

#include "server_base.hpp"
#include "protobuf.hpp"
#include "network.hpp"
#include "http/http_listener.hpp"
#include "login/login_connector.hpp"

#include <string>
#include <zdb.h>

class http_listener_t;
class virtual_client_t
    : public common::server_base_t
{
public:
    virtual_client_t();
    virtual ~virtual_client_t();

    virtual const char* server_name() { return "virtual_client"; }
    virtual bool on_init_server();
    virtual bool on_close_server();
    virtual void on_run_server();

    bool load_ini();

    bool connect_to_login();
    void disconnect_with_login();
    
    void network_send_msg(const network::tcp_socket_ptr& s, const network::msg_buf_ptr& buf, bool is_sync = false);
    void send_msg_to_http(const std::string& msg);
public:

private:
    network::network_t m_network;
    http_listener_t  m_http_listener;
    login_connector_t m_login_connector;
};

#endif
