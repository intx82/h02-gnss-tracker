#include <gtest/gtest.h>
#include "h02.h"

TEST(LocationTTest, SerializeDeserialize) {
    std::tm date_time = {0};
    date_time.tm_year = 124; // Year 2024
    date_time.tm_mon = 1;
    date_time.tm_mday = 25;
    date_time.tm_hour = 12;
    date_time.tm_min = 30;
    date_time.tm_sec = 45;

    location_t original("1234567890", 22.2, 114.4, 0.00, 348, date_time, 0xffffffff);
    std::string serialized = original.serialize();

    location_t deserialized(serialized);
    EXPECT_EQ(original.device_id, deserialized.device_id);
    EXPECT_NEAR(22.12, deserialized.latitude, 0.0001);
    EXPECT_NEAR(114.24, deserialized.longitude, 0.0001);
    EXPECT_NEAR(original.speed, deserialized.speed, 0.01);
    EXPECT_NEAR(original.direction, deserialized.direction, 0.01);
    EXPECT_EQ(original.date_time.tm_mday, deserialized.date_time.tm_mday);
    EXPECT_EQ(original.date_time.tm_mon, deserialized.date_time.tm_mon);
    EXPECT_EQ(original.date_time.tm_year, deserialized.date_time.tm_year);
    EXPECT_EQ(original.date_time.tm_hour, deserialized.date_time.tm_hour);
    EXPECT_EQ(original.date_time.tm_min, deserialized.date_time.tm_min);
    EXPECT_EQ(original.date_time.tm_sec, deserialized.date_time.tm_sec);
    EXPECT_EQ(original.status, deserialized.status);
}

