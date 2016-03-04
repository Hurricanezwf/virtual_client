#include "virtual_client.hpp"
#include "virtual_client_fwd.hpp"
#include "http/http_msg_handler.hpp"

#include <boost/property_tree/ini_parser.hpp>

USING_NS_NETWORK;
USING_NS_COMMON;

static const std::string config_ini_path("./config.ini");

virtual_client_t::virtual_client_t()
{
}

virtual_client_t::~virtual_client_t()
{
}

bool virtual_client_t::load_ini()
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(config_ini_path, pt);

        env::cfg->listen_http.ip        = pt.get<std::string>("listen.ip");
        env::cfg->listen_http.port      = pt.get<uint16_t>("listen.port");
        env::cfg->connect_to_login.ip   = pt.get<std::string>("login.ip");
        env::cfg->connect_to_login.port = pt.get<uint16_t>("login.port");
    }
    catch (boost::property_tree::ptree_error& e)
    {
        log_error("read [%s] failed, error[%s]\n", config_ini_path.c_str(), e.what());
        return false;
    }

    return true;
}

bool virtual_client_t::on_init_server()
{
    // log
    if (!g_log.open("./logs/", "virtual_client", true, true, true))
    {
        COMMON_ASSERT(false, "init log failed!");
        return false;
    }
    log_info("init log success!");

    // ini
    if (!load_ini())
    {
        COMMON_ASSERT(false);
        return false;
    }

    // http msg handler
    if (!http_msg_handler_t::init_msg_handler())
    {
        COMMON_ASSERT(false, "init http msg handler failed!");
        return false;
    }
    log_info("init http msg handler success!");

    // network
    if (!m_network.init(4))
    {
        COMMON_ASSERT(false, "init network failed!");
        return false;
    }
    log_info("init network success!");

    // listen for http
    if (!m_network.listen_at(env::cfg->listen_http.ip, env::cfg->listen_http.port, &m_http_listener))
    {
        COMMON_ASSERT(false);
        return false;
    }
    log_info("virtual_client listen at [%s:%d] success!", env::cfg->listen_http.ip.c_str(), env::cfg->listen_http.port);

    log_info("init virtual client success!");

    return true;
}

bool virtual_client_t::on_close_server()
{
    if (!m_network.close())
    {
        log_error("close network failed");
        return false;
    }
    log_info("close network success!");

    return true;
}

void virtual_client_t::on_run_server()
{
    log_info("virtual client running...");
}


bool virtual_client_t::connect_to_login()
{
    return m_network.connect_to(env::cfg->connect_to_login.ip.c_str(), env::cfg->connect_to_login.port, &m_login_connector);
}

void virtual_client_t::disconnect_with_login()
{
    m_network.close_socket(m_login_connector.get_socket());
    log_debug("virtual client disconnect with login");
}


void virtual_client_t::network_send_msg(const network::tcp_socket_ptr& s, const network::msg_buf_ptr& buf, bool is_sync)
{
    m_network.post_send_msg(s, buf, is_sync);
}


void virtual_client_t::send_msg_to_http(const std::string& msg)
{
    m_http_listener.reply_http_msg(msg); 
}
