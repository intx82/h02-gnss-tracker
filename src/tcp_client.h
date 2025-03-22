#ifndef GNSS_CLIENT_H
#define GNSS_CLIENT_H

#include <functional>
#include <string>
#include <cstdint>

#include "callback.h"

class tcp_client_t : public functor_t
{
   public:
    static constexpr int default_server_port = 5013;     /**< Default server port if not specified. */
    static constexpr int default_reconnect_timeout = 10; /**< Default reconnect timeout  */

    /**
     * @brief Constructor: accepts a server address in the form "host[:port]".
     * The host part may be a hostname (which requires DNS resolution)
     * or a direct IP address (IPv4 or IPv6).
     */
    tcp_client_t(const std::string& server_addr);

    /**
     * @brief Destructor: closes the connection and cleans up resources.
     */
    ~tcp_client_t();

    /**
     * @brief Sends the given message packet to the server.
     */
    bool send_msg(const std::string& packet);

    /**
     * @brief sched iface
     */
    void operator()();

    /**
     * @brief Registers a callback that is invoked when data is read from the
     * socket. The callback receives a pointer to the instance, a pointer to the
     * received data, and its length.
     */
    void set_on_read(
        const std::function<void(tcp_client_t*, uint8_t*, size_t)>& callback);

    /**
     * @brief Closes the connection
     */
    void close();

    /**
     * @brief Is host right now connected or not
     */
    bool is_connected() const { return _connected; }

   private:
    using _read_cb_t = std::function<void(tcp_client_t*, uint8_t*, size_t)>;

    bool _connected;              /**< Shows is client connected to server or not */
    std::string _server;          /**< Server host */
    uint16_t _port;               /**< Server port */
    uint64_t _reconnect_in;       /**< When happens next reconnection */
    uint16_t _reconnect_timeout;  /**< Reconnection interval */
    int _sockfd;                  /**< Socket itself */
    _read_cb_t _on_read_callback; /**< Read callback */
    static bool sigpipe_received; /**< Show that SIGPIPE was received recently (then try to re-establish connection, if possible) */

    /**
     * @brief Connects to the remote server
     * @return True if connected
     */
    bool connect();

    /**
     * @brief Resolves the given host to an IP address.
     * @return If the host is already a valid IP (IPv4 or IPv6), returns it
     * unchanged.
     */
    static std::string resolve_domain(const std::string& host);

    /**
     * @brief Checks is IP valid or not
     */
    static bool is_valid_ip(const std::string& ip);

    /**
     * @brief Signal handler to catch SIGPIPE (on broke connection)
     */
    static void sigpipe_cb(int signum);
};

#endif  // GNSS_CLIENT_H
