#ifndef __H02_H
#define __H02_H
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <vector>

class location_t {
public:
    std::string device_id;
    double latitude;
    double longitude;
    double speed;
    double direction;
    std::tm date_time;
    uint32_t status;

    location_t(const std::string& dev_id, double lat, double lon, double spd, double dir, const std::tm& dt, uint32_t stat = 0xffffffff)
        : device_id(dev_id), latitude(lat), longitude(lon), speed(spd), direction(dir), date_time(dt), status(stat) {}

    location_t(const std::string& message) {
        deserialize(message);
    }
    
    static double convert_lat(double in) {
        int degrees = static_cast<int>(in);
        double fractional = in - degrees;
        double minutes = fractional * 60;
        return degrees * 100 + minutes;
    }
    
    static double convert_lng(double in) {
        int degrees = static_cast<int>(in);
        double fractional = in - degrees;
        double minutes = fractional * 60;
        return degrees * 100 + minutes;
    }

    std::string serialize() const {
        std::ostringstream oss;
        oss << "*HQ," << device_id << ",V1," 
            << std::setw(2) << std::setfill('0') << date_time.tm_hour
            << std::setw(2) << std::setfill('0') << date_time.tm_min
            << std::setw(2) << std::setfill('0') << date_time.tm_sec
            << ",A," 
            << std::fixed << std::setprecision(4) << convert_lat(latitude) << ",N," 
            << std::fixed << std::setprecision(4) << convert_lng(longitude) << ",E," 
            << std::setprecision(2) << speed << "," 
            << std::setprecision(2) << direction << ","
            << std::setw(2) << std::setfill('0') << date_time.tm_mday
            << std::setw(2) << std::setfill('0') << (date_time.tm_mon + 1)
            << std::setw(2) << std::setfill('0') << (date_time.tm_year % 100)
            << "," << std::hex << std::uppercase << status << ",000000,000000,000000,000000#";
        return oss.str();
    }

    bool deserialize(const std::string& message) {
        std::vector<std::string> parts;
        std::stringstream ss(message);
        std::string segment;
        while (std::getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        if (parts.size() < 16 || parts[0] != "*HQ") {
            return false;
        }

        try {
            device_id = parts[1];
            date_time.tm_hour = std::stoi(parts[3].substr(0, 2));
            date_time.tm_min = std::stoi(parts[3].substr(2, 2));
            date_time.tm_sec = std::stoi(parts[3].substr(4, 2));
            latitude = std::stod(parts[5]) / 100;
            longitude = std::stod(parts[7]) / 100;
            speed = std::stod(parts[9]);
            direction = std::stod(parts[10]);
            date_time.tm_mday = std::stoi(parts[11].substr(0, 2));
            date_time.tm_mon = std::stoi(parts[11].substr(2, 2)) - 1;
            date_time.tm_year = std::stoi(parts[11].substr(4, 2)) + 100;
            status = std::stoul(parts[12], nullptr, 16);
        } catch (const std::exception& e) {
            std::cerr << "Deserialization error: " << e.what() << std::endl;
            return false;
        }
        return true;
    }
};

#endif
