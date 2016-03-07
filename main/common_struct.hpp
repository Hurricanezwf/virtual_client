#ifndef _VIRTUAL_CLIENT_COMMON_STRUCT_HPP_
#define _VIRTUAL_CLIENT_COMMON_STRUCT_HPP_

#include "common/common_fwd.hpp"

#include <string>

// enum ================================================================
// virtual_client与服务器的连接状态
enum connect_state_em : uint8_t
{
    connect_state_idle = 0,
    connect_state_login_connected = 1,
    connect_state_gate_connected = 1 << 1,
};

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
