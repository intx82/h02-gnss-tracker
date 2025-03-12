#ifndef __UNIX_TTY_HPP
#define __UNIX_TTY_HPP

#include <cstdint> // For uint8_t
#include <cstring> // For std::strlen()
#include <fcntl.h> // For open()
#include <iostream>
#include <stdexcept> // For std::runtime_error
#include <string>
#include <termios.h> // For termios functions and types
#include <unistd.h>  // For read(), write(), and close()

class unix_tty_t
{
public:
    // Constructor: open the specified tty device with the given baud rate and configure it.
    // device_path: path to the tty device (e.g., "/dev/ttyS0" or "/dev/tty")
    // baud_rate: baud rate (e.g., B9600, B115200, etc.)
    unix_tty_t(const std::string &device_path, speed_t baud_rate = B9600)
    {
        tty_fd = open(device_path.c_str(), O_RDWR | O_NOCTTY);
        if (tty_fd < 0) {
            throw std::runtime_error("Failed to open " + device_path);
        }

        // Get current tty settings.
        struct termios tty_settings;
        if (tcgetattr(tty_fd, &tty_settings) != 0) {
            throw std::runtime_error("Failed to get tty attributes");
        }

        // Set raw mode (disable canonical mode, echo, and signal processing).
        cfmakeraw(&tty_settings);

        // Set the input and output baud rate.
        if (cfsetispeed(&tty_settings, baud_rate) != 0 ||
            cfsetospeed(&tty_settings, baud_rate) != 0) {
            throw std::runtime_error("Failed to set baud rate");
        }

        // Configure for 8 data bits, no parity, 1 stop bit (8N1).
        tty_settings.c_cflag &= ~CSIZE;
        tty_settings.c_cflag |= CS8;
        tty_settings.c_cflag |= CLOCAL | CREAD;

        // Disable software flow control.
        tty_settings.c_iflag &= ~(IXON | IXOFF | IXANY);

        // Apply the settings immediately.
        if (tcsetattr(tty_fd, TCSANOW, &tty_settings) != 0) {
            throw std::runtime_error("Failed to set tty attributes");
        }
    }

    // Destructor: close the file descriptor.
    ~unix_tty_t()
    {
        if (tty_fd >= 0) {
            close(tty_fd);
        }
    }

    // Enable or disable non-blocking mode.
    void set_nonblocking(bool enable)
    {
        int flags = fcntl(tty_fd, F_GETFL, 0);
        if (flags < 0) {
            throw std::runtime_error("Failed to get tty flags");
        }
        if (enable) {
            flags |= O_NONBLOCK;
        } else {
            flags &= ~O_NONBLOCK;
        }
        if (fcntl(tty_fd, F_SETFL, flags) < 0) {
            throw std::runtime_error("Failed to set tty non-blocking flag");
        }
    }

    // Read a full line into a uint8_t buffer.
    // max_len: maximum number of bytes to read (including space for the null terminator).
    // Returns the number of bytes read (excluding the null terminator).
    ssize_t read_line(uint8_t *buffer, size_t max_len)
    {
        if (max_len == 0) {
            return 0;
        }
        size_t i = 0;
        while (i < max_len - 1) { // leave space for null terminator
            uint8_t ch;
            ssize_t bytes_read = read(tty_fd, &ch, 1);
            if (bytes_read <= 0) {
                break;
            }
            buffer[i++] = ch;
            if (ch == '\n') { // stop reading at newline
                break;
            }
        }
        buffer[i] = '\0'; // null-terminate the buffer
        return i;
    }

    // Non-blocking: Read a full line into a uint8_t buffer.
    // Returns immediately with available data, or 0 if no data is available.
    ssize_t read_line_nonblocking(uint8_t *buffer, size_t max_len)
    {
        if (max_len == 0) {
            return 0;
        }
        size_t i = 0;
        while (i < max_len - 1) {
            uint8_t ch;
            ssize_t bytes_read = read(tty_fd, &ch, 1);
            if (bytes_read < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // No data available; return what we have.
                    break;
                } else {
                    throw std::runtime_error("Error reading tty in non-blocking mode");
                }
            } else if (bytes_read == 0) {
                // End-of-file reached.
                break;
            }
            buffer[i++] = ch;
            if (ch == '\n') {
                break;
            }
        }
        buffer[i] = '\0';
        return i;
    }

    // Non-blocking: Read a single character from the tty.
    // Returns the character if available, or -1 if no data is available.
    int read_char_nonblocking()
    {
        uint8_t ch;
        ssize_t bytes_read = read(tty_fd, &ch, 1);
        if (bytes_read < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return -1; // No data available.
            }
            throw std::runtime_error("Error reading tty in non-blocking mode");
        } else if (bytes_read == 0) {
            return -1; // End of file or no data.
        }
        return ch;
    }

    // Read a single character from the tty.
    uint8_t read_char()
    {
        uint8_t ch;
        ssize_t bytes_read = read(tty_fd, &ch, 1);
        if (bytes_read <= 0) {
            throw std::runtime_error("Failed to read a character from tty");
        }
        return ch;
    }

    // Write an array of uint8_t bytes to the tty.
    void write_bytes(const uint8_t *buffer, size_t length)
    {
        ssize_t bytes_written = write(tty_fd, buffer, length);
        if (bytes_written < 0) {
            throw std::runtime_error("Failed to write to tty");
        }
    }

private:
    int tty_fd; // file descriptor for the tty device
};

#endif
