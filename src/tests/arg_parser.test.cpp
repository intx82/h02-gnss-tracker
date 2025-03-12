#include <gtest/gtest.h>
#include "arg_parser.h"
#include <termios.h>
#include <vector>

// Helper function to create a vector of char* from a vector of std::string.
static std::vector<char*> create_argv(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    for (const auto &arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    return argv;
}

TEST(ArgParserTest, ValidInputDefault) {
    // Only required server parameter provided.
    std::vector<std::string> args = {"test_program", "-s", "localhost:1234"};
    auto argv = create_argv(args);
    arg_parser parser;
    EXPECT_NO_THROW(parser.parse(static_cast<int>(argv.size()), argv.data()));
    EXPECT_EQ(parser.get_port(), "/dev/ttyUSB0");
    EXPECT_EQ(parser.get_baud(), (speed_t)B9600);
    // "localhost" is typically resolved to "127.0.0.1".
    EXPECT_EQ(parser.get_server_ip(), "127.0.0.1");
    EXPECT_EQ(parser.get_server_port(), 1234);
    EXPECT_FALSE(parser.get_verbose());
}

TEST(ArgParserTest, ValidInputAllParameters) {
    std::vector<std::string> args = {"test_program", "-p", "/dev/ttyS1", "-b", "115200", "-s", "localhost:8080", "-v"};
    auto argv = create_argv(args);
    arg_parser parser;
    EXPECT_NO_THROW(parser.parse(static_cast<int>(argv.size()), argv.data()));
    EXPECT_EQ(parser.get_port(), "/dev/ttyS1");
    EXPECT_EQ(parser.get_baud(), (speed_t)B115200);
    EXPECT_EQ(parser.get_server_ip(), "127.0.0.1");
    EXPECT_EQ(parser.get_server_port(), 8080);
    EXPECT_TRUE(parser.get_verbose());
}

TEST(ArgParserTest, MissingServerArgument) {
    // Server is required; missing -s should throw.
    std::vector<std::string> args = {"test_program", "-p", "/dev/ttyS1", "-b", "115200"};
    auto argv = create_argv(args);
    arg_parser parser;
    EXPECT_THROW({
        parser.parse(static_cast<int>(argv.size()), argv.data());
    }, std::runtime_error);
}

TEST(ArgParserTest, InvalidServerFormat) {
    // Server argument without colon should throw.
    std::vector<std::string> args = {"test_program", "-s", "localhost"};
    auto argv = create_argv(args);
    arg_parser parser;
    EXPECT_THROW({
        parser.parse(static_cast<int>(argv.size()), argv.data());
    }, std::runtime_error);
}

TEST(ArgParserTest, InvalidBaudValue) {
    // Non-numeric baud value should throw.
    std::vector<std::string> args = {"test_program", "-b", "notanumber", "-s", "localhost:8080"};
    auto argv = create_argv(args);
    arg_parser parser;
    EXPECT_THROW({
        parser.parse(static_cast<int>(argv.size()), argv.data());
    }, std::runtime_error);
}

TEST(ArgParserTest, UnsupportedBaudRate) {
    // Baud rate that is not in the baud_mappings should throw.
    std::vector<std::string> args = {"test_program", "-b", "12345", "-s", "localhost:8080"};
    auto argv = create_argv(args);
    arg_parser parser;
    EXPECT_THROW({
        parser.parse(static_cast<int>(argv.size()), argv.data());
    }, std::runtime_error);
}

TEST(ArgParserTest, UnknownArgument) {
    // Unknown arguments should be ignored (printed to stderr) but not throw.
    std::vector<std::string> args = {"test_program", "-s", "localhost:8080", "--unknown", "-v"};
    auto argv = create_argv(args);
    arg_parser parser;
    EXPECT_NO_THROW(parser.parse(static_cast<int>(argv.size()), argv.data()));
    // The verbose flag should be set.
    EXPECT_TRUE(parser.get_verbose());
    // Server information should still be parsed correctly.
    EXPECT_EQ(parser.get_server_ip(), "127.0.0.1");
    EXPECT_EQ(parser.get_server_port(), 8080);
}

