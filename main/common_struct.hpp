#ifndef _VIRTUAL_CLIENT_COMMON_STRUCT_HPP_
#define _VIRTUAL_CLIENT_COMMON_STRUCT_HPP_

#include "common/common_fwd.hpp"

#include <string>

// struct ===============================================================
struct address_s
{
    std::string ip;
    uint16_t    port;
};
typedef struct address_s address_t;


struct virtual_client_config_s
{
    address_t listen_http;
    address_t connect_to_login;
};
typedef struct virtual_client_config_s virtual_client_config_t;


#endif // _VIRTUAL_CLIENT_COMMON_STRUCT_HPP_
