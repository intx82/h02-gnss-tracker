#include "tcp_client.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>

#include "Arduino.h"

bool tcp_client_t::sigpipe_received = false;

void tcp_client_t::sigpipe_cb(int signum)
{
    LOG(WARN, "SIGPIPE received, ignoring");
    tcp_client_t::sigpipe_received = true;
}

bool tcp_client_t::is_valid_ip(const std::string& ip)
{
    struct in_addr addr4;
    struct in6_addr addr6;

    if (inet_pton(AF_INET, ip.c_str(), &addr4) == 1) {
        return true;
    }

    if (inet_pton(AF_INET6, ip.c_str(), &addr6) == 1) {
        return true;
    }
    return false;
}

std::string tcp_client_t::resolve_domain(const std::string& host)
{
    if (is_valid_ip(host)) {
        return host;
    }

    struct hostent* he = gethostbyname(host.c_str());
    if (!he) {
        std::cerr << "DNS resolution failed for " << host << std::endl;
        return "";
    }

    struct in_addr** addr_list = reinterpret_cast<struct in_addr**>(he->h_addr_list);
    if (addr_list[0] != nullptr) {
        return std::string(inet_ntoa(*addr_list[0]));
    }

    std::cerr << "DNS resolution failed: no valid IP addresses for " << host
              << std::endl;
    return "";
}

tcp_client_t::tcp_client_t(const std::string& server_addr)
    : _connected(false), _port(default_server_port), _reconnect_in(0), _reconnect_timeout(default_reconnect_timeout)
{
    // Parse server_addr.
    size_t colon_pos = server_addr.find(':');
    if (colon_pos != std::string::npos) {
        _server = server_addr.substr(0, colon_pos);
        std::string port_str = server_addr.substr(colon_pos + 1);
        try {
            _port = std::stoi(port_str);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port in server address: " << server_addr
                      << "\nUsing default port " << default_server_port
                      << std::endl;
            _port = default_server_port;
        }
    } else {
        _server = server_addr;
        _port = default_server_port;
    }

    _server = resolve_domain(_server);
    if (_server.empty()) {
        throw std::runtime_error("Could not resolve server: " + _server);
    }

    signal(SIGPIPE, sigpipe_cb);
    _connected = connect();
}

tcp_client_t::~tcp_client_t()
{
    this->close();
}

bool tcp_client_t::send_msg(const std::string& packet)
{
    if (_connected && _sockfd > 0 && !sigpipe_received) {
        size_t sz = packet.length();
        while (sz) {
            ssize_t written = write(_sockfd, packet.c_str(), packet.length());
            if (written < 0) {
                _connected = false;
                _sockfd = -1;
                LOG(INFO, "Write failed: %ld -> %s", written, strerror(errno));
                break;
            }
            sz -= written;
        }

        if (sz == 0) {
            return true;
        }
    }
    return false;
}

void tcp_client_t::operator()()
{
    bool reestablish = false;
    if (sigpipe_received) {
        sigpipe_received = false;
        reestablish = true;
    }

    reestablish |= !_connected || (_sockfd < 0);

    if (reestablish) {
        if (_reconnect_in > sys_t::get_ticks()) {
            return;
        }

        if (!connect()) {
            LOG(WARN, "Trying to reconnect in: %d sec", _reconnect_timeout);
            _reconnect_in = sys_t::get_ticks() + (_reconnect_timeout * 1000);
            return;
        }
    }

    if (_on_read_callback) {
        std::vector<uint8_t> _tmp;
        size_t total = 0;
        ssize_t sz = 0;
        do {
            uint8_t d = 0;
            sz = ::read(_sockfd, &d, sizeof(d));
            if ((sz <= 0) && (total == 0)) {
                return;
            }

            _tmp.push_back(d);
            total += sz;
        } while (sz > 0);

        _on_read_callback(this, _tmp.data(), total);
    }
}

void tcp_client_t::set_on_read(
    const std::function<void(tcp_client_t*, uint8_t*, size_t)>& callback)
{
    _on_read_callback = callback;
}

void tcp_client_t::close()
{
    if (_sockfd) {
        ::close(_sockfd);
    }
}

bool tcp_client_t::connect()
{
    struct sockaddr_in serv_addr;

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
        throw std::runtime_error("Can't open socket" + _server);
    }

    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);
    inet_pton(AF_INET, _server.c_str(), &serv_addr.sin_addr.s_addr);

    int c = ::connect(_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (c < 0) {
        LOG(INFO, "Connection failed: %d -> %s", c, strerror(errno));
        _connected = false;
    } else {
        fcntl(_sockfd, F_SETFL, O_NONBLOCK);
        LOG(INFO, "connected: %d", c);
        int flags = 1;
        if (setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&flags, sizeof(flags))) {
            LOG(INFO, "Keepalive turning on failed: %s", strerror(errno));
        };
        _connected = true;
    }

    return _connected;
}