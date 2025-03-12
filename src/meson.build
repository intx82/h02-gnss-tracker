project('h02_serializer', 'cpp',
    version: '1.0.0',
    default_options: ['cpp_std=c++17']
)

add_global_arguments('-fPIC', language : 'cpp')
# add_project_arguments('-Ofast', '-g3',  language : 'cpp')

#lfmt_dep = dependency('fmt', required: true)

executable('h02_serializer',
    'main.cpp',
    'TinyGPS++.cpp',
    'Arduino.cpp',
    'task_loop.cpp',
#    dependencies: lfmt_dep,
    install: true
)

dependency_gtest = dependency('gtest', required: true)

test_exe = executable('h02_serializer_tests',
    'TinyGPS++.cpp',
    'Arduino.cpp',
    'task_loop.cpp',
    'tests/h02.test.cpp',
    'tests/main.test.cpp',
    'tests/tinygps.test.cpp',
    'tests/arg_parser.test.cpp',
    'tests/task_loop.test.cpp',
    dependencies: dependency_gtest, 
    install: false
)

test('h02_serializer_tests', test_exe)
