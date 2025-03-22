project('h02-gps-tracker', 'cpp',
    version: '1.0.0',
    default_options: ['cpp_std=c++17']
)

add_global_arguments('-fPIC', language : 'cpp')

executable('h02cl',
    'main.cpp',
    'TinyGPS++.cpp',
    'Arduino.cpp',
    'task_loop.cpp',
    'tcp_client.cpp',
    install: true
)

gtest_dep = dependency('gtest', required: true)

test_exe = executable('h02cl_tests',
    'TinyGPS++.cpp',
    'Arduino.cpp',
    'task_loop.cpp',
    'tests/h02.test.cpp',
    'tests/main.test.cpp',
    'tests/tinygps.test.cpp',
    'tests/arg_parser.test.cpp',
    'tests/task_loop.test.cpp',
    dependencies: [gtest_dep],
    install: false
)

test('h02cl_tests', test_exe)
