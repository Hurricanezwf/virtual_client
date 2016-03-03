#ifndef __VIRTUAL_CLIENT_HTTP_LISTENER_HPP__
#define __VIRTUAL_CLIENT_HTTP_LISTENER_HPP__

#include "network.hpp"

class http_listener_t
	: public network::acceptor_callback_t
{
public:
	http_listener_t() {}
	virtual ~http_listener_t() {}

	virtual const char* name() { return "virtual_client"; }
	virtual void on_new_accept(const network::tcp_socket_ptr& s);
    virtual void on_new_msg(const network::tcp_socket_ptr& s, const network::message_t& msg){}
	virtual void on_close(const network::tcp_socket_ptr& s);
    virtual bool on_read(const network::tcp_socket_ptr& s, char* data, size_t bytes);

    void close();

    void reply_http_msg(const std::string& msg);

private:
	network::tcp_socket_ptr m_socket = NULL;
};


#endif
