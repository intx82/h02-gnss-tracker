#include <sched.h>

#include <csignal>
#include <iostream>
#include <queue>
#include <thread>

#include "Arduino.h"
#include "TinyGPS++.h"
#include "arg_parser.h"
#include "h02.h"
#include "task_loop.h"
#include "tcp_client.h"
#include "unix_tty.h"

// #include "spdlog/spdlog.h"

void gps_handle_task(TinyGPSPlus &gps, unix_tty_t &uart, bool verbose)
{
    int c = 0;
    do {
        c = uart.read_char_nonblocking();
        if ((c != -1) && (gps.encode((char)(c & 0xff)))) {
            if (verbose && gps.location.isValid() && gps.date.isValid() && gps.time.isValid() &&
                        gps.location.FixQuality() != TinyGPSLocation::Quality::Invalid) {
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

void server_handle_task(TinyGPSPlus &gps, arg_parser &args, tcp_client_t &tcp)
{
    static std::queue<location_t> cache;

    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid() 
                && gps.location.FixQuality() != TinyGPSLocation::Quality::Invalid ) {
        std::string location_packet;

        std::tm _tm = {};
        _tm.tm_year = gps.date.year();
        _tm.tm_mon = gps.date.month() - 1;
        _tm.tm_mday = gps.date.day();
        _tm.tm_hour = gps.time.hour();
        _tm.tm_min = gps.time.minute();
        _tm.tm_sec = gps.time.second();

        location_t data(args.get_id().c_str(),
                        gps.location.lat(), gps.location.lng(),
                        gps.speed.kmph(),
                        gps.course.deg(), _tm, 0xFFFFFFFF);

        if (tcp.is_connected()) {
            while (!cache.empty()) {
                location_packet = cache.front().serialize();
                if (args.get_verbose()) {
                    LOG(INFO, "Sending cached: %s", location_packet.c_str());
                }
                tcp.send_msg(location_packet.c_str());
                cache.pop();
            }

            location_packet = data.serialize();
            if (args.get_verbose()) {
                LOG(INFO, "Sending: %s", location_packet.c_str());
            }

            tcp.send_msg(location_packet.c_str());
        } else {
            if (args.get_verbose()) {
                location_packet = data.serialize();
                LOG(INFO, "Server is unavailable, save to cache: %s", location_packet.c_str());
            }
            cache.push(data);
        }
    }
}

static bool is_running;

void _kill_app(int sig)
{
    LOG(WARN, "Catch SIGKILL/SIGINT signal. Exiting..");
    is_running = false;
}

int main(int argc, char **argv)
{
    arg_parser args;
    is_running = true;

    args.parse(argc, argv);
    if (args.get_help_flag()) {
        args.print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    TinyGPSPlus gps;
    unix_tty_t uart(args.get_port().c_str(), args.get_baud());
    tcp_client_t tcp(args.get_server());
    task_loop_t loop;
    task_t<TinyGPSPlus &, unix_tty_t &, bool> gps_task(gps_handle_task, gps, uart, args.get_verbose());
    task_t<TinyGPSPlus &, arg_parser &, tcp_client_t &> server_task(server_handle_task, gps, args, tcp);

    uart.set_nonblocking(true);
    signal(SIGINT, _kill_app);
    signal(SIGQUIT, _kill_app);

    tcp.set_on_read([](tcp_client_t *client, uint8_t *data, size_t sz) {
        LOG(AUDIT, "Received: %.*s", (int)sz, data);
    });

    loop.add(&gps_task, 1);
    loop.add(&tcp, 10);
    loop.add(&server_task, args.get_interval() * 1000);

    while (is_running) {
        int sleep_until = loop.scheduler();
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_until));
    }

    tcp.close();
    return 0;
}
