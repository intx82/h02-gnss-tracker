#include <gtest/gtest.h>
#include <stdio.h>
#include "TinyGPS++.h"

// A sample NMEA stream.

// The TinyGPSPlus object.


TEST(TinyGpsTest, TinyGpsSimpleTest) {
    const char *gpsStream =
    "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
    "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
    "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
    "$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n"
    "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
    "$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";

    
    TinyGPSPlus gps;
    printf("Testing TinyGPSPlus library v. %s\n", TinyGPSPlus::libraryVersion());
    
    while (*gpsStream) {
        if (gps.encode(*gpsStream++)) {
            break;
        }
    }
    // Location: 30.236640,-97.821453  Date/Time: 9/3/2013 04:51:03.00
    printf("Location: ");
    EXPECT_EQ(gps.location.isValid(), true);
    // Print latitude and longitude with six decimal places.
    printf("%.6f,%.6f", gps.location.lat(), gps.location.lng());

    printf("  Date/Time: ");
    EXPECT_EQ(gps.date.isValid(), true);
    printf("%d/%d/%d", gps.date.month(), gps.date.day(), gps.date.year());

    EXPECT_EQ(gps.date.month(), 9);
    EXPECT_EQ(gps.date.day(), 3);
    EXPECT_EQ(gps.date.year(), 2013);

    printf(" ");
    EXPECT_EQ(gps.time.isValid(), true);
    
    printf("%02d:", gps.time.hour());
    printf("%02d:", gps.time.minute());
    printf("%02d.", gps.time.second());
    EXPECT_EQ(gps.time.hour(), 4);
    EXPECT_EQ(gps.time.minute(), 51);
    EXPECT_EQ(gps.time.second(), 3);

    printf("\n");
}
