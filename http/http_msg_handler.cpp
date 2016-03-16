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

    // item relevant
    ret &= regist_msg_handler("cg_create_item_request",    handle_cg_create_item_request);
    ret &= regist_msg_handler("cg_cost_item_request",      handle_cg_cost_item_request);

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
    std::string guid;
    try
    {
        guid = pt.get<std::string>("guid");
    }
    catch (boost::property_tree::ptree_error& ec)
    {
        log_error("Get guid error! Message:%s", ec.what());
        env::server->send_err_to_http("Server Internal Error!<br>");
        return;
    }

    std::string reply;
    // connect to login
    if (false == env::server->connect_to_login())
    {
        env::server->send_err_to_http("virtual client connect to login_server failed!<br>");
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

    boost::property_tree::ptree tmp_pt;
    env::server->send_reply_to_http("", tmp_pt); 
}


// item relevant
void http_msg_handler_t::handle_cg_create_item_request(const boost::property_tree::ptree& pt)
{
    uint32_t item_tid = 0;
    uint32_t add_num  = 0;
    try
    {
        item_tid = pt.get<uint32_t>("item_tid"); 
        add_num  = pt.get<uint32_t>("add_num");
    }
    catch (boost::property_tree::ptree_error& ec)
    {
        log_error("Resolve data error! Message:%s", ec.what());
        env::server->send_err_to_http("Server Internal Error!");
        return;
    }

    proto::client::cg_create_item_request req;
    req.set_item_tid(item_tid);
    req.set_add_num(add_num);
    env::server->send_msg_to_gate(op_cmd::cg_create_item_request, req);
}

void http_msg_handler_t::handle_cg_cost_item_request(const boost::property_tree::ptree& pt)
{
    std::string item_uid;
    uint32_t    cost_num = 0;
    try
    {
        item_uid = pt.get<std::string>("item_uid"); 
        cost_num = pt.get<uint32_t>("cost_num");
    }
    catch (boost::property_tree::ptree_error& ec)
    {
        log_error("Resolve data error! Message:%s", ec.what());
        env::server->send_err_to_http("Server Internal Error!");
        return;
    }

    proto::client::cg_cost_item_request req;
    req.set_item_uid(item_uid);
    req.set_cost_num(cost_num);
    env::server->send_msg_to_gate(op_cmd::cg_cost_item_request, req);
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
