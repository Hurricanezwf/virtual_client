#ifndef __VIRTUAL_CLIENT_HPP__
#define __VIRTUAL_CLIENT_HPP__

#include "server_base.hpp"
#include "protobuf.hpp"
#include "network.hpp"
#include "http/http_listener.hpp"
#include "login/login_connector.hpp"
#include "gate/gate_connector.hpp"
#include "common_struct.hpp"

#include <string>
#include <zdb.h>

#include <boost/property_tree/ptree.hpp>

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
    bool is_connected_with_login();

    bool connect_to_gate(std::string& gate_ip, uint32_t gate_port);
    void disconnect_with_gate();
    bool is_connected_with_gate();
    
    void network_send_msg(const network::tcp_socket_ptr& s, const network::msg_buf_ptr& buf, bool is_sync = false);

    // 无错时用send_reply, 有错误消息时用send_err
    void send_reply_to_http(const std::string& cmd, const boost::property_tree::ptree& data);
    void send_err_to_http(const std::string& err_msg);

    template<typename T_MSG>
    void send_msg_to_login(uint16_t cmd, const T_MSG& msg)
    {
        if (false == is_connected_with_login())
        {
            log_error("send msg to login failed! Message: not connect with login server");
            return;
        }

        network::msg_buf_ptr buf = network::wrap_msg<T_MSG>(cmd, msg);
        m_login_connector.send_msg_to_login(buf);
        log_trace("send cmd[%d] to login", cmd);
    }

    template<typename T_MSG>
    void send_msg_to_gate(uint16_t cmd, const T_MSG& msg)
    {
        if (false == is_connected_with_gate())
        {
            log_error("send msg to gate failed! Message: not connect with gate server");
            return;
        }

        network::msg_buf_ptr buf = network::wrap_msg<T_MSG>(cmd, msg);
        m_gate_connector.send_msg_to_gate(buf);
        log_trace("send cmd[%d] to gate", cmd);
    }
public:
    void set_guid(const std::string val) { m_guid = val; }
    std::string get_guid() const { return m_guid; }

    void set_uid(const uint64_t val) { m_uid = val; }
    uint64_t get_uid() const { return m_uid; }

    void set_nickname(const std::string val) { m_nickname = val; }
    std::string get_nickname() const { return m_nickname; }

    void set_gate_ip(const std::string val) { m_gate_ip = val; }
    std::string get_gate_ip() const { return m_gate_ip; }

    void set_gate_port(const uint32_t val) { m_gate_port = val; }
    uint32_t get_gate_port() const { return m_gate_port; }
private:
    network::network_t  m_network;
    http_listener_t     m_http_listener;
    login_connector_t   m_login_connector;
    gate_connector_t    m_gate_connector;
    uint8_t             m_connect_state; // virtual_client与服务器的连接状态

    std::string         m_guid;
    uint64_t            m_uid;
    std::string         m_nickname;
    std::string         m_gate_ip;
    uint32_t            m_gate_port;
};

#endif
