#include "login_msg_handler.hpp"
#include "main/gate_server.hpp"
#include "main/gate_server_fwd.hpp"
#include "network.hpp"
#include "user/user_manager.hpp"
#include "common/common_struct.hpp"

USING_NS_NETWORK;
USING_NS_COMMON;

server_msg_handler_t<op_cmd::LA_BEGIN, op_cmd::LA_END, login_msg_handler_t::msg_handler_func> login_msg_handler_t::m_login_msg_handle;

bool login_msg_handler_t::init_msg_handler()
{
    m_login_msg_handle.register_func(op_cmd::la_login_request,                  handle_la_login_request);
    m_login_msg_handle.register_func(op_cmd::la_kick_user_by_othe_user_notify,  handle_la_kick_user_by_othe_user_notify);

    return true;
}

bool login_msg_handler_t::handle_la_login_request(const msg_buf_ptr& msg_buf)
{
    PRE_S2S_MSG(proto::server::la_login_request);

    user_ptr p_new_user;
    user_ptr p_old_user;
    bool rlt = false;
    do 
    {
        p_old_user = user_manager_t::get_user(uid);
        if (NULL != p_old_user)
        {
            if (p_old_user->is_wait_to_close())
            {
                user_manager_t::del_user(uid);
            }
            else
            {
                if (NULL != p_old_user->get_socket())
                {
                    log_error("user[%lu: %d] was kicked!", uid, p_old_user->get_socket()->socket().native());
                    user_manager_t::notify_game_by_kick_user(p_old_user, KICK_USER_REASON_BY_OTHER_USER);
                    break;
                }
                else
                {
                    log_error("user[%lu] is old user and it's socket is NULL!", uid);
                    user_manager_t::del_user(uid);
                }
            }
        }

        p_new_user = user_manager_t::add_user(uid, msg.guid(), msg.gate_index());
        if (NULL == p_new_user)
        {
            log_error("p_new_user is NULL!");
            break;
        }

        rlt = true;

    } while (0);

    proto::server::al_login_reply rep;
    if (rlt)
    {
        rep.set_reply_code(0);
        rep.set_guid(p_new_user->get_guid());
        rep.set_gate_ip(env::xml->listen_at_client.out_ip);
        rep.set_gate_port(env::xml->listen_at_client.out_port);
    }
    else
    {
        rep.set_reply_code(1);
    }
    env::server->send_msg_to_login(op_cmd::al_login_reply, uid, rep);

    return true;
}

bool login_msg_handler_t::handle_la_kick_user_by_othe_user_notify(const msg_buf_ptr& msg_buf)
{
    PRE_S2S_MSG(proto::server::la_kick_user_by_othe_user_notify);

    /*user_ptr p_user = user_manager_t::get_user(uid);
    if (NULL == p_user)
    {
        log_error("NULL == p_user");
        return true;
    }
    user_manager_t::notify_game_by_kick_user(p_user, KICK_USER_REASON_BY_OTHER_USER);*/

    return true;
}