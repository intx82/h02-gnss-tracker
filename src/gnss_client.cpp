#include "gnss_client.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <thread>
#include <chrono>
#include <netdb.h>
#include <arpa/inet.h>

// Helper: Check if a string is a valid IPv4 or IPv6 address using inet_pton.
static bool is_valid_ip(const std::string &ip) {
    struct in_addr addr4;
    if (inet_pton(AF_INET, ip.c_str(), &addr4) == 1)
        return true;
    struct in6_addr addr6;
    if (inet_pton(AF_INET6, ip.c_str(), &addr6) == 1)
        return true;
    return false;
}

std::string gnss_client_t::resolve_domain(const std::string &host) {
    if (is_valid_ip(host))
        return host;
    // For simplicity, use gethostbyname (IPv4 only). In production, consider getaddrinfo.
    struct hostent *he = gethostbyname(host.c_str());
    if (!he) {
        std::cerr << "DNS resolution failed for " << host << std::endl;
        return "";
    }
    struct in_addr **addr_list = reinterpret_cast<struct in_addr **>(he->h_addr_list);
    if (addr_list[0] != nullptr) {
        return std::string(inet_ntoa(*addr_list[0]));
    }
    std::cerr << "DNS resolution failed: no valid IP addresses for " << host << std::endl;
    return "";
}

void gnss_client_t::alloc_cb(uv_handle_t* /*handle*/, size_t suggested_size, uv_buf_t* buf) {
    buf->base = static_cast<char*>(malloc(suggested_size));
    buf->len = suggested_size;
}

void gnss_client_t::on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    gnss_client_t* client = static_cast<gnss_client_t*>(stream->data);
    if (nread > 0) {
        if (client->on_read_callback_) {
            client->on_read_callback_(client, reinterpret_cast<uint8_t*>(buf->base), nread);
        }
    } else if (nread < 0) {
        std::cerr << "Read error or EOF." << std::endl;
        client->reconnect();
    }
    free(buf->base);
}

void gnss_client_t::on_write(uv_write_t* req, int status) {
    if (status) {
        std::cerr << "uv_write error: " << uv_strerror(status) << std::endl;
        static_cast<gnss_client_t*>(req->handle->data)->reconnect();
    }
    free(req);
}

void gnss_client_t::on_connect(uv_connect_t* req, int status) {
    gnss_client_t* client = static_cast<gnss_client_t*>(req->handle->data);
    if (status < 0) {
        std::cerr << "Connection failed: " << uv_strerror(status) << std::endl;
        client->reconnect();
    } else {
        uv_read_start(reinterpret_cast<uv_stream_t*>(client->tcp_), alloc_cb, on_read);
        client->reconnect_in_ = 1;
    }
    free(req);
}

void gnss_client_t::on_close(uv_handle_t* handle) {
}

bool gnss_client_t::connect_to_server() {
    // Prepare destination address.
    struct sockaddr_storage dest;
    if (server_ip_.find(':') != std::string::npos) {
        struct sockaddr_in6 *addr6 = reinterpret_cast<struct sockaddr_in6*>(&dest);
        uv_ip6_addr(server_ip_.c_str(), server_port_, addr6);
    } else {
        struct sockaddr_in *addr4 = reinterpret_cast<struct sockaddr_in*>(&dest);
        uv_ip4_addr(server_ip_.c_str(), server_port_, addr4);
    }

    connect_req_ = new uv_connect_t;
    int r = uv_tcp_connect(connect_req_, tcp_,
                           reinterpret_cast<const struct sockaddr*>(&dest),
                           on_connect);
    if (r) {
        std::cerr << "uv_tcp_connect error: " << uv_strerror(r) << std::endl;
        free(connect_req_);
        connect_req_ = nullptr;
        return false;
    }
    return true;
}

gnss_client_t::gnss_client_t(const std::string &server_addr)
    : loop_(uv_default_loop()), tcp_(nullptr), connect_req_(nullptr),
      server_port_(default_server_port), reconnect_in_(1), reconnect_timeout(128)
{
    // Parse server_addr.
    size_t colon_pos = server_addr.find(':');
    if (colon_pos != std::string::npos) {
        server_hostname_ = server_addr.substr(0, colon_pos);
        std::string port_str = server_addr.substr(colon_pos + 1);
        try {
            server_port_ = std::stoi(port_str);
        } catch (const std::exception &e) {
            std::cerr << "Invalid port in server address: " << server_addr
                      << "\nUsing default port " << default_server_port << std::endl;
            server_port_ = default_server_port;
        }
    } else {
        server_hostname_ = server_addr;
        server_port_ = default_server_port;
    }

    server_ip_ = resolve_domain(server_hostname_);
    if (server_ip_.empty()) {
        throw std::runtime_error("Could not resolve server: " + server_hostname_);
    }

    // Initialize TCP handle.
    tcp_ = new uv_tcp_t;
    uv_tcp_init(loop_, tcp_);
    tcp_->data = this;
    uv_tcp_keepalive(tcp_, 1, 60);

    if (!connect_to_server()) {
        std::cerr << "Initial connection attempt failed." << std::endl;
    }
}

gnss_client_t::~gnss_client_t() {
    // Let the libuv loop clean up the handle asynchronously.
    if (tcp_) {
        uv_close(reinterpret_cast<uv_handle_t*>(tcp_), on_close);
    }
}

bool gnss_client_t::send_msg(const std::string &packet) {
    if (!tcp_) {
        std::cerr << "TCP handle not initialized." << std::endl;
        reconnect();
        return false;
    }
    uv_write_t *req = new uv_write_t;
    uv_buf_t buf = uv_buf_init(const_cast<char*>(packet.c_str()), packet.size());
    int r = uv_write(req, reinterpret_cast<uv_stream_t*>(tcp_), &buf, 1, on_write);
    if (r) {
        std::cerr << "uv_write error: " << uv_strerror(r) << std::endl;
        reconnect();
        return false;
    }
    return true;
}

bool gnss_client_t::reconnect() {
    reconnect_in_ *= 2;
    if (reconnect_in_ > reconnect_timeout) {
        throw std::runtime_error("Can't connect to the server");
    }
    std::this_thread::sleep_for(std::chrono::seconds(reconnect_in_));

    if (tcp_) {
        delete tcp_;
        tcp_ = new uv_tcp_t;
        uv_tcp_init(loop_, tcp_);
        tcp_->data = this;
        uv_tcp_keepalive(tcp_, 1, 60);
    }
    // Re-resolve the host.
    server_ip_ = resolve_domain(server_hostname_);
    if (server_ip_.empty()) {
        std::cerr << "Could not resolve server: " << server_hostname_ << std::endl;
        return false;
    }
    return connect_to_server();
}

void gnss_client_t::run() {
    uv_run(loop_, UV_RUN_DEFAULT);
}

void gnss_client_t::set_on_read(const std::function<void(gnss_client_t*, uint8_t*, size_t)> &callback) {
    on_read_callback_ = callback;
}

void gnss_client_t::close() {
    if (tcp_) {
        uv_close(reinterpret_cast<uv_handle_t*>(tcp_), on_close);
    }
    uv_stop(loop_);
}
