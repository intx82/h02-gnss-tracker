project('h02_serializer', 'cpp',
    version: '1.0.0',
    default_options: ['cpp_std=c++17']
)

add_global_arguments('-fPIC', language : 'cpp')
libuv_dep = dependency('libuv', required: true)

executable('h02_serializer',
    'main.cpp',
    'TinyGPS++.cpp',
    'Arduino.cpp',
    'task_loop.cpp',
    'gnss_client.cpp',
    dependencies: libuv_dep,
    install: true
)

gtest_dep = dependency('gtest', required: true)

test_exe = executable('h02_serializer_tests',
    'TinyGPS++.cpp',
    'Arduino.cpp',
    'task_loop.cpp',
    'tests/h02.test.cpp',
    'tests/main.test.cpp',
    'tests/tinygps.test.cpp',
    'tests/arg_parser.test.cpp',
    'tests/task_loop.test.cpp',
    dependencies: [gtest_dep, libuv_dep],
    install: false
)

test('h02_serializer_tests', test_exe)
