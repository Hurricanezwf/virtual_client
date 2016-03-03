#include "http_listener.hpp"
#include "common/http_request_parser.hpp"
#include "main/virtual_client_fwd.hpp"
#include "main/virtual_client.hpp"
#include "common/log.hpp"

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

USING_NS_NETWORK;
USING_NS_COMMON;

void http_listener_t::on_new_accept(const network::tcp_socket_ptr& s)
{
	if (NULL == s)
	{
		log_error("socket is NULL!");
		return;
	}

	m_socket = s;
}

void http_listener_t::on_close(const network::tcp_socket_ptr& s)
{
    if (s == m_socket)
    {
        m_socket = NULL;
    }
    else
    {
        log_error("close socket error! socket didn't equal!");
    }
}

// 这个函数无论如何都返回true
bool http_listener_t::on_read(const network::tcp_socket_ptr& s, char* data, size_t bytes)
{
    std::string msg(data, bytes); 

    http_request_t req;
    http_request_parser_t parser;
    parser.parse(req, msg.begin(), msg.end());
    //log_warn("content:%s", req.m_content.c_str());
    
    try
    {
        boost::property_tree::ptree pt;
        std::stringstream stream(req.m_content);
        boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(stream, pt);
        std::string guid = pt.get<std::string>("guid");
        std::string id   = pt.get<std::string>("id");
        //std::string no   = pt.get<std::string>("no");
        log_warn("guid=%s, id=%s", guid.c_str(), id.c_str());
    }
    catch (boost::property_tree::ptree_error& ec)
    {
        log_error("read json error! Message:%s", ec.what());
        msg = "Server Internal Error";
    }

    // send reply whatever happened
    msg.append("\r\n");
    reply_http_msg(msg);

    return true;
}

void http_listener_t::close()
{
    if (NULL != m_socket)
    {
        m_socket->close();
    }
}

void http_listener_t::reply_http_msg(const std::string& msg)
{
    if (NULL == m_socket)
    {
        log_error("reply http msg failed! socket is null!");
        return;
    }

    m_socket->socket().write_some(boost::asio::buffer(msg));
    close();
}
