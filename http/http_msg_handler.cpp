#include "http_msg_handler.hpp"
#include "main/virtual_client_fwd.hpp"
#include "main/virtual_client.hpp"
#include "common/common_fwd.hpp"
#include "common/log.hpp"
#include "common/channel_enum.hpp"
#include "protos/op_cmd.hpp"
#include "protos/client.pb.h"
#include "protos/common.pb.h"

#include <boost/property_tree/ptree.hpp>


http_msg_handler_t::http_msg_map http_msg_handler_t::m_register; 

bool http_msg_handler_t::init_msg_handler()
{
    bool ret = true;
    ret &= regist_msg_handler("cl_login_request",       handle_cl_login_request);
    ret &= regist_msg_handler("ca_logout_request",      handle_ca_logout_request);

    return ret;
}

void http_msg_handler_t::handle_http_msg(const boost::property_tree::ptree& pt)
{
    try
    {
        std::string cmd = pt.get<std::string>("cmd"); 
        msg_handler_func handler = find_func(cmd);
        if (NULL == handler)
        {
            log_error("no cmd[%s]'s handler founded!", cmd.c_str()); 
            std::string reply("Server Internal Error!");
            env::server->send_err_to_http(reply);
            return;
        }

        handler(pt);
    }
    catch (boost::property_tree::ptree_error& ec)
    {
        log_error("resolve property_tree error! Message:%s", ec.what()); 
        std::string reply = std::string(ec.what());
        env::server->send_err_to_http(reply);
    }
}


// client->login  =================================================================
void http_msg_handler_t::handle_cl_login_request(const boost::property_tree::ptree& pt)
{
    std::string guid = pt.get<std::string>("guid");

    std::string reply;
    // connect to login
    if (false == env::server->connect_to_login())
    {
        reply = "virtual client connect to login_server failed!<br>"; 
        env::server->send_err_to_http(reply);
        return;
    }

    env::server->set_nickname(guid);

    // send cl_login_request to login
    proto::client::cl_login_request login_req;
    login_req.set_ch_type((uint32_t)channel::CHANNEL_TYPE_TEST);
    login_req.mutable_general()->set_session_id(guid);
    env::server->send_msg_to_login(op_cmd::cl_login_request, login_req);
}


// client->gate =================================================================
void http_msg_handler_t::handle_ca_logout_request(const boost::property_tree::ptree& pt)
{
    env::server->disconnect_with_gate();

    env::server->send_reply_to_http("", pt); 
}


// msg register =================================================================
bool http_msg_handler_t::regist_msg_handler(std::string cmd, msg_handler_func func)
{
   if (!is_msg_handler_exist(cmd))
   {
       m_register[cmd] = func;
       return true;
   }

   log_error("http msg map has existed cmd[%s]!", cmd.c_str());
   return false;
}

http_msg_handler_t::msg_handler_func http_msg_handler_t::find_func(const std::string cmd)
{
    if (m_register.end() != m_register.find(cmd))
    {
        return m_register[cmd];
    }

    return NULL;
}

bool http_msg_handler_t::is_msg_handler_exist(const std::string cmd)
{
    if (m_register.end() != m_register.find(cmd))
    {
        return true;
    }

    return false;
}
