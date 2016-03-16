#include "gate_msg_handler.hpp"
#include "main/virtual_client_fwd.hpp"
#include "main/virtual_client.hpp"
#include "common/common_struct.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

USING_NS_NETWORK;
USING_NS_COMMON;

server_msg_handler_t<op_cmd::AC_BEGIN, op_cmd::AC_END, gate_msg_handler_t::msg_handler_func> gate_msg_handler_t::m_gate_msg_handle;
server_msg_handler_t<op_cmd::GC_BEGIN, op_cmd::GC_END, gate_msg_handler_t::msg_handler_func> gate_msg_handler_t::m_logic_msg_handle;

bool gate_msg_handler_t::init_msg_handler()
{
    // gate msg
    m_gate_msg_handle.register_func(op_cmd::ac_connect_reply,           handle_ac_connect_reply);
    m_gate_msg_handle.register_func(op_cmd::ac_kick_user_notify,        handle_ac_kick_user_notify);

    // logic msg
    m_logic_msg_handle.register_func(op_cmd::gc_enter_game_reply,       handle_gc_enter_game_reply);

    // item relevant
    m_logic_msg_handle.register_func(op_cmd::gc_create_item_reply,      handle_gc_create_item_reply);
    m_logic_msg_handle.register_func(op_cmd::gc_cost_item_reply,        handle_gc_cost_item_reply);

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


bool gate_msg_handler_t::handle_ac_kick_user_notify(const msg_buf_ptr& msg_buf)
{
    env::server->disconnect_with_gate();
    return true;
}


bool gate_msg_handler_t::handle_gc_enter_game_reply(const msg_buf_ptr& msg_buf)
{
    PRE_S2C_MSG(proto::client::gc_enter_game_reply);
    if (msg.reply_code() != 0)
    {
        log_warn("enter game failed! need to create new user");
        proto::client::cg_create_user_request req;
        req.set_nickname(env::server->get_nickname());
        env::server->send_msg_to_gate(op_cmd::cg_create_user_request, req);
    }
    else
    {
        boost::property_tree::ptree pt;
        try 
        {
            // user data
            proto::common::user_data user = msg.user();
            boost::property_tree::ptree user_data;
            user_data.put("uid",           user.uid());
            user_data.put("name",          user.name());
            user_data.put("did",           user.did());
            user_data.put("create_time",   user.create_time());
            user_data.put("last_login_time", user.last_login_time());
            user_data.put("last_logout_time", user.last_logout_time());
            pt.put_child("user_data", user_data);

            // item data
            proto::common::item_data items = msg.item();
            boost::property_tree::ptree item_data;
            for (int32_t i = 0; i < items.item_list_size(); ++i)
            {
                proto::common::item_single item = items.item_list(i);
                boost::property_tree::ptree item_single;
                item_single.put("tid", item.tid());
                item_single.put("num", item.num());
                item_single.put("uid", item.uid());
                item_data.push_back(std::make_pair("", item_single));
            }
            pt.put_child("item_data", item_data);
        }
        catch (boost::property_tree::ptree_error& ec)
        {
            log_error("construct json failed! Message:%s", ec.what()); 
            return false;
        }

        env::server->send_reply_to_http("gc_enter_game_reply", pt);
    }

    return true;
}


// item relevant
bool gate_msg_handler_t::handle_gc_create_item_reply(const network::msg_buf_ptr& msg_buf)
{
    PRE_S2C_MSG(proto::client::gc_create_item_reply);    
    if (msg.reply_code() > 0)
    {
        env::server->send_err_to_http("");
    }
    else
    {
        if (msg.has_create_item_data())
        {
            proto::common::item_single single = msg.create_item_data();
            boost::property_tree::ptree pt;
            try
            {
                pt.put("uid", single.uid());
                pt.put("tid", single.tid());
                pt.put("num", single.num());
            }
            catch (boost::property_tree::ptree_error& ec)
            {
                log_error("construct json failed! Message: %s", ec.what());
                env::server->send_err_to_http("construct json failed!");
                return false;
            }
            
            env::server->send_reply_to_http("gc_create_item_reply", pt);
        }
    }
    
    return true;
}

bool gate_msg_handler_t::handle_gc_cost_item_reply(const network::msg_buf_ptr& msg_buf)
{
    PRE_S2C_MSG(proto::client::gc_cost_item_reply);
    if (msg.reply_code() > 0)
    {
        env::server->send_err_to_http("");
    }
    else
    {
        if (msg.has_cost_item_data())
        {
            proto::common::item_single single = msg.cost_item_data();
            boost::property_tree::ptree pt;
            try
            {
                pt.put("uid", single.uid());
                pt.put("tid", single.tid());
                pt.put("num", single.num());
            }
            catch (boost::property_tree::ptree_error& ec)
            {
                log_error("construct json failed! Message: %s", ec.what());
                env::server->send_err_to_http("construct json failed!");
                return false;
            }

            env::server->send_reply_to_http("gc_cost_item_reply", pt);
        }
    }

    return true;
}
