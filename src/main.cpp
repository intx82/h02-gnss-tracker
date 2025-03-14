#include "Arduino.h"
#include "TinyGPS++.h"
#include "arg_parser.h"
#include "gnss_client.h"
#include "h02.h"
#include "task_loop.h"
#include "unix_tty.h"
#include <iostream>
#include <thread>

// #include "spdlog/spdlog.h"

void gps_handle_task(TinyGPSPlus &gps, unix_tty_t &uart, bool verbose)
{
    int c = 0;
    do {
        c = uart.read_char_nonblocking();
        if ((c != -1) && (gps.encode((char)(c & 0xff)))) {
            if (verbose && gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
                LOG(INFO, "Location: %.6f %.6f @ %02d/%02d/%d %02d:%02d:%02d sat: %d proceed: %d",
                    gps.location.lat(), gps.location.lng(),
                    gps.date.day(), gps.date.month(), gps.date.year(),
                    gps.time.hour(), gps.time.minute(), gps.time.second(),
                    gps.satellites.value(),
                    gps.sentencesWithFix());
            }
        }
    } while (c != -1);
}

void server_handle_task(TinyGPSPlus &gps, std::string &uid, gnss_client_t& tcp, bool verbose)
{
    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) {
        std::tm _tm = {};
        _tm.tm_year = gps.date.year();
        _tm.tm_mon = gps.date.month() - 1;
        _tm.tm_mday = gps.date.day();
        _tm.tm_hour = gps.time.hour();
        _tm.tm_min = gps.time.minute();
        _tm.tm_sec = gps.time.second();

        location_t data(uid.c_str(),
                        gps.location.lat(), gps.location.lng(),
                        gps.speed.kmph(),
                        gps.course.deg(), _tm, 0xFFFFFFFF);

        std::string location_packet = data.serialize();
        if (verbose) {
            LOG(INFO, "Sending: %s", location_packet.c_str());
        }
        tcp.send_msg(location_packet.c_str());
    }
}

int main(int argc, char **argv)
{
    arg_parser args;

    args.parse(argc, argv);
    if (args.get_help_flag()) {
        args.print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    std::string uid = args.get_id();
    TinyGPSPlus gps;
    unix_tty_t uart(args.get_port().c_str(), args.get_baud());
    gnss_client_t tcp(args.get_server());
    uart.set_nonblocking(true);
    std::thread uv_thread([&tcp]() { tcp.run(); });

    task_loop_t loop;
    tcp.set_on_read([](gnss_client_t *client, uint8_t *data, size_t sz) {
        LOG(AUDIT, "Received: %.*s", (int)sz, data);
    });

    loop.add(new task_t<TinyGPSPlus &, unix_tty_t &, bool>(gps_handle_task, gps, uart, args.get_verbose()), 0);
    loop.add(new task_t<TinyGPSPlus &, std::string &, gnss_client_t &, bool>(server_handle_task, gps, uid, tcp, args.get_verbose()), args.get_interval() * 1000);

    while (true) {
        loop.scheduler();
    }

    tcp.close();
    uv_thread.join();
    return 0;
}
