#ifndef __ARG_PARSER_H
#define __ARG_PARSER_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <termios.h>

// Structure to map integer baud rate to speed_t constant.
struct baud_mapping {
    int rate;
    speed_t speed;
};

static const baud_mapping baud_mappings[] = {
    {50, B50},
    {75, B75},
    {110, B110},
    {134, B134},
    {150, B150},
    {200, B200},
    {300, B300},
    {600, B600},
    {1200, B1200},
    {1800, B1800},
    {2400, B2400},
    {4800, B4800},
    {9600, B9600},
    {19200, B19200},
    {38400, B38400},
    {57600, B57600},
    {115200, B115200},
    {230400, B230400},
};

class arg_parser {
public:
    arg_parser()
        : help_flag(false),
          port("/dev/ttyUSB0"),
          uid("123456789012345"),
          interval(1),
          baud(B9600),  // default baud rate set to B9600
          server(""),
          verbose(false) {}

    // Parse command-line arguments.
    void parse(int argc, char *argv[]) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                help_flag = true;
            } else if (arg == "-p" || arg == "--port") {
                if (i + 1 < argc) {
                    port = argv[++i];
                } else {
                    throw std::runtime_error("Missing value for port");
                }
            } else if (arg == "-b" || arg == "--baud") {
                if (i + 1 < argc) {
                    try {
                        int baud_int = std::stoi(argv[++i]);
                        baud = parse_baudrate(baud_int);
                    } catch (const std::exception &e) {
                        throw std::runtime_error("Invalid baudrate value: " + std::string(e.what()));
                    }
                } else {
                    throw std::runtime_error("Missing value for baudrate");
                }
            } else if (arg == "-i" || arg == "--id") {
                if (i + 1 < argc) {
                    uid = argv[++i];
                } else {
                    throw std::runtime_error("Missing value for port");
                }
            } else if (arg == "-t" || arg == "--interval") {
                if (i + 1 < argc) {
                    interval = std::stoi(argv[++i]);
                    if (interval == 0) {
                        throw std::runtime_error("Interval must higher than 0");
                    }
                }
            } else if (arg == "-s" || arg == "--server") {
                if (i + 1 < argc) {
                    server = argv[++i];
                    size_t colon_pos = server.find(':');
                    if (colon_pos == std::string::npos) {
                        throw std::runtime_error("Missing value for server port");
                    }
                } else {
                    throw std::runtime_error("Missing value for server");
                }
            } else if (arg == "-v" || arg == "--verbose") {
                verbose = true;
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
            }
        }
        // If help was not requested, ensure required arguments are set.
        if (!help_flag && server.empty()) {
            throw std::runtime_error("Server is required. Use -s or --server to specify it (format: host:port).");
        }
    }

    // Print help/usage information.
    void print_help(const std::string &program_name) {
        std::cout << "Usage: " << program_name << " [options]\n"
                  << "Options:\n"
                  << "  -h, --help                    Show this help\n"
                  << "  -p, --port [tty]              Use next tty, by default /dev/ttyUSB0 (optional)\n"
                  << "  -b, --baud [baudrate]         Use next baudrate for port, by default 9600 (optional)\n"
                  << "  -s, --server [host:port]      h02 remote server (required). If host is not an IP, DNS resolution will be performed.\n"
                  << "  -i, --id [device-id]          h02 device ID, 15 symbols. \n"
                  << "  -t, --interval [interval]     Interval in seconds between server notifications\n"
                  << "  -v, --verbose                 Enable verbose output\n";
    }

    // Getters.
    bool get_help_flag() const { return help_flag; }
    std::string get_port() const { return port; }
    std::string get_id() const { return uid; }
    int get_interval() const { return interval; }
    speed_t get_baud() const { return baud; }
    std::string get_server() const { return server; }
    bool get_verbose() const { return verbose; }

private:
    // Helper function: search the baud_mappings array for the matching speed_t.
    speed_t parse_baudrate(int rate) {
        for (const auto &mapping : baud_mappings) {
            if (mapping.rate == rate) {
                return mapping.speed;
            }
        }

        throw std::runtime_error("Unsupported baudrate value");
    }

    bool        help_flag;
    std::string port;
    std::string uid;
    int         interval;
    speed_t     baud;
    std::string server;
    bool        verbose;
};

#endif
