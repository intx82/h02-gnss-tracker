#include <iostream>
#include "TinyGPS++.h"
#include "arg_parser.h"
#include "h02.h"
#include "unix_tty.h"
#include "task_loop.h"

//#include "spdlog/spdlog.h"

void gps_handle_task(TinyGPSPlus& gps, unix_tty_t& uart) {

    if (gps.encode( uart.read_char())) {
        if (gps.location.isValid()) {
            printf("Location: ");
            printf("%.6f,%.6f ", gps.location.lat(), gps.location.lng());
        }

        if (gps.date.isValid()) {
            printf("Date/Time: ");
            printf("%d/%d/%d ", gps.date.day(), gps.date.month(), gps.date.year());
        }

        if (gps.time.isValid()) {
            printf("%02d:", gps.time.hour());
            printf("%02d:", gps.time.minute());
            printf("%02d - ", gps.time.second());
        }
        printf("Sat: %d ", gps.satellites.value());
        printf("Proceed: %d \r\n", gps.sentencesWithFix());
    }
}

void server_handle_task() {
    
}

int main(int argc, char **argv)
{
    arg_parser args;
    args.parse(argc, argv);
    if (args.get_help_flag()) {
        args.print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    TinyGPSPlus gps;
    unix_tty_t  uart(args.get_port().c_str(), args.get_baud());
    task_loop_t loop;
    loop.add(new task_t<TinyGPSPlus&, unix_tty_t&>(gps_handle_task, gps, uart), 0);

    while (true) {
        loop.scheduler();
    }

    /*
        std::tm _tm = {};
        _tm.tm_year = 2025;
        _tm.tm_mon = 2;
        _tm.tm_mday = 8;
        _tm.tm_hour = 15;
        _tm.tm_min = 46;
        _tm.tm_sec = 32;
        location_t data("1234567890", 22.3942, 114.0088, 0.00, 348, _tm, 0xFFFFFFFF);
        std::string location_packet = data.serialize();
        std::cout << "Serialized Location Packet: " << location_packet << std::endl;

        //std::string received_message = "*HQ,1234567890,V1,250224,A,2239.4210,N,11400.8825,E,0.00,348.00,123045,FFFFFFFF,000000,000000,000000,000000#";
        location_t received_data(location_packet);

        std::cout << "Deserialized Location Data:" << std::endl;
        std::cout << "Device ID: " << received_data.device_id << std::endl;
        std::cout << "Latitude: " << received_data.latitude << std::endl;
        std::cout << "Longitude: " << received_data.longitude << std::endl;
        std::cout << "Speed: " << received_data.speed << std::endl;
        std::cout << "Direction: " << received_data.direction << std::endl;
        std::cout << "Date: " << received_data.date_time.tm_mday << "/" << received_data.date_time.tm_mon + 1 << "/" << received_data.date_time.tm_year + 1900 << std::endl;
        std::cout << "Status: " << received_data.status << std::endl;
        return 0;
    */
    return 0;
}
