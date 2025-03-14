#ifndef GNSS_CLIENT_H
#define GNSS_CLIENT_H

#include <string>
#include <functional>
#include <uv.h>

class gnss_client_t {
public:
    // Default server port if not specified.
    static constexpr int default_server_port = 5013;

    // Constructor: accepts a server address in the form "host[:port]".
    // The host part may be a hostname (which requires DNS resolution)
    // or a direct IP address (IPv4 or IPv6).
    gnss_client_t(const std::string &server_addr);

    // Destructor: closes the connection and cleans up resources.
    ~gnss_client_t();

    // Sends the given message packet to the server.
    bool send_msg(const std::string &packet);

    // Attempts to re-establish the connection.
    bool reconnect();

    // Runs the libuv event loop.
    void run();

    // Registers a callback that is invoked when data is read from the socket.
    // The callback receives a pointer to the instance, a pointer to the received data, and its length.
    void set_on_read(const std::function<void(gnss_client_t*, uint8_t*, size_t)> &callback);

    // Closes the connection and terminates the libuv loop.
    void close();
    
    bool is_connected() const { return connected; }

private:
    // Resolves the given host to an IP address.
    // If the host is already a valid IP (IPv4 or IPv6), returns it unchanged.
    static std::string resolve_domain(const std::string &host);

    // Initiates a connection to the server using libuv.
    bool connect_to_server();

    // --- Libuv Callback functions ---
    static void on_connect(uv_connect_t* req, int status);
    static void on_write(uv_write_t* req, int status);
    static void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
    static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
    static void on_close(uv_handle_t* handle);

    uv_loop_t    *loop_;
    uv_tcp_t     *tcp_;
    uv_connect_t *connect_req_;

    std::string server_ip_;       // Resolved IP address.
    std::string server_hostname_; // Hostname (or provided IP) part.
    int         server_port_;             // Parsed port (or default).
    uint64_t    reconnect_in_;
    const int   reconnect_timeout;
    bool        connected;

    // Callback invoked when data is read.
    std::function<void(gnss_client_t*, uint8_t*, size_t)> on_read_callback_;
};

#endif // GNSS_CLIENT_H
