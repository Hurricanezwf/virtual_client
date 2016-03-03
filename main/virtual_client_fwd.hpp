#ifndef __VIRTUAL_CLIENT_FWD_HPP__
#define __VIRTUAL_CLIENT_FWD_HPP__

#include "log.hpp"
#include "common/common_fwd.hpp"
#include "common_struct.hpp"
#include <boost/asio.hpp>

class virtual_client_t;
struct env
{
    static virtual_client_t* server;
    static boost::asio::signal_set* signals;
    static virtual_client_config_t* cfg;
};

#endif
