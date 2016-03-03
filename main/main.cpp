#include "virtual_client.hpp"
#include "virtual_client_fwd.hpp"

virtual_client_t* env::server = NULL;
boost::asio::signal_set* env::signals = NULL;
virtual_client_config_t* env::cfg = NULL;

USING_NS_NETWORK;
USING_NS_COMMON;

static void handle_signal(int32_t signal)
{
    log_trace("recv signal[%d]", signal);
    if (signal == SIGHUP)
    {
        log_error("error recv sighup");
        env::signals->async_wait(boost::bind(handle_signal, boost::asio::placeholders::signal_number));
    }
    else if (signal == SIGINT || signal == SIGTERM || signal == SIGQUIT)
    {
        log_info("gm server stopping...");
        env::server->close_server();
    }
}

int main(int argc, char** argv)
{
    virtual_client_t server;
    env::server = &server;
        
    virtual_client_config_t config;
    env::cfg = &config;

    boost::asio::signal_set signals(env::server->get_ios());
    signals.add(SIGINT);
    signals.add(SIGTERM);
    signals.add(SIGQUIT);
    signals.add(SIGHUP);
    signals.async_wait(boost::bind(handle_signal, boost::asio::placeholders::signal_number));
    env::signals = &signals;
        
    server.run_server();

    printf("virtual client stop!\n");
    return 0;
}
