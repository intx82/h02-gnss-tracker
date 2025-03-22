
# H02 GNSS Tracker

H02 GNSS Tracker is a C++ application that send GNSS data from serial port (NMEA) to remote GNSS (H02) server (for example traccar [https://github.com/traccar/traccar](https://github.com/traccar/traccar)). The application also includes unit tests built with Google Test ([https://github.com/google/googletest](https://github.com/google/googletest)).

## Usage

Run the application (named h02cl) with the following options:

```
Usage: ./h02cl [options]  
Options:  
 -h, --help                    Show this help  
 -p, --port [tty]              Use next tty, by default /dev/ttyUSB0 (optional)  
 -b, --baud [baudrate]         Use next baudrate for port, by default 9600 (optional)  
 -s, --server [host:port]      h02 remote server (required). If host is not an IP, DNS resolution will be performed.  
 -i, --id [device-id]          h02 device ID, 15 symbols.    
 -t, --interval [interval]     Interval in seconds between server notifications  
 -v, --verbose                 Enable verbose output
```

## Building the Application

This project uses Meson as its build system and requires gtest for tests
Prerequisites:

- Meson and Ninja
- A C++17 compiler
- Google Test (development files) – for running tests
- TinyGPS++ is used as a submodule. Before building, update the submodules by running: `git submodule update --init --recursive`

For example, after cloning, your submodules should include: `0a205759da22a1c54c5f5285480fe6132592a4e2 lib/TinyGPSPlus (Version-1.0.2.1-5-g0a20575)`

### Build Instructions:

1. Configure the build directory: `meson setup build --buildtype=release`
    This configures the project in release mode (with -O2 optimizations and without debuginfo).
2. Compile the project: `meson compile -C build`
3. Install the executable (optional): `meson install -C build`
4. Running Tests: After building, run tests with: `meson test -C build`

## Running the Application

After a successful build, run the main executable from the build directory: `./build/h02cl [options]`
Example: 

```
./build/h02cl --port /dev/ttyUSB0 --baud 57600 --server free-gps.ru:5013 --id 123451234567890 --interval 10 --verbose
```

