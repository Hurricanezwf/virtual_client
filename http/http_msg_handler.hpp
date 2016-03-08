#ifndef __HTTP_MSG_HANDLER_HPP__
#define __HTTP_MSG_HANDLER_HPP__

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>

class http_msg_handler_t
{
public:
    typedef void (*msg_handler_func)(const boost::property_tree::ptree& pt);

    static bool init_msg_handler();
    static void handle_http_msg(const boost::property_tree::ptree& pt);

private:
    static void handle_cl_login_request(const boost::property_tree::ptree& pt);
    static void handle_ca_logout_request(const boost::property_tree::ptree& pt);

private:
    static bool regist_msg_handler(std::string cmd, msg_handler_func func);
    static msg_handler_func find_func(const std::string cmd);
    static bool is_msg_handler_exist(const std::string cmd);

private:
    typedef std::map<std::string, msg_handler_func> http_msg_map;
    static http_msg_map m_register;
};

#endif //__HTTP_MSG_HANDLER_HPP__
