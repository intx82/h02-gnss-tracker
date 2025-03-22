#include <gtest/gtest.h>
#include <cstdint>
#include <chrono>
#include <thread>

// Include your task loop header and the sys_time header.
#include "task_loop.h"
#include "Arduino.h"
#include <cinttypes>

// ---------------------------------------------------------------------
// Callback Tests
// ---------------------------------------------------------------------
TEST(callback_Test, cb_test)
{
    int val = 0;
    shared_cb_t<int&> cbt(val);

    cbt.push_back([](int& v){
        v += 1;
    });

    cbt.push_back([](int& v){
        v += 2;
    });

    cbt.push_back([](int& v){
        v += 7;
    });

    // Call all stored callbacks.
    cbt();

    EXPECT_EQ(val, 10);
}

TEST(callback_Test, shared_cb_test)
{
    int val = 0;
    cb_t<int&> cbt;

    cbt.push_back([](int& v){
        v += 1;
    });

    cbt.push_back([](int& v){
        v += 2;
    });

    cbt.push_back([](int& v){
        v += 12;
    });

    // Call all callbacks by passing the argument.
    cbt(val);

    EXPECT_EQ(val, 15);
}

TEST(task_loop_Test, task_t_test) {
    int val = 0;
    task_t<int&> cbt([](int& v) { v += 1; }, val);

    cbt();
    cbt();
    cbt();

    EXPECT_EQ(val, 3);
}

TEST(task_loop_Test, methods_test) {
    int val = 0;
    task_loop_t loop;

    // Task 1: Increments by 1 every 10ms.
    loop.add(
        [](int32_t tid, void* args) {
            (void)tid;
            *static_cast<int*>(args) += 1;
        },
        10, static_cast<void*>(&val));

    // Task 2: Increments by 100 every 100ms.
    loop.add(
        [](int32_t tid, void* args) {
            LOG(TEST, "Task: %d", tid);
            *static_cast<int*>(args) += 100;
        },
        100, static_cast<void*>(&val));

    uint64_t start = sys_t::get_ticks();
    LOG(TEST, "Start %lu", start);
    while (sys_t::get_ticks() <= (start + 255)) {
        loop.scheduler();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EXPECT_EQ(val, 225);
}

TEST(task_loop_Test, functors_test) {
    int val = 0;
    task_loop_t loop;

    // Task added as a functor using new task_t.
    loop.add(new task_t<int&>([](int& val) { val += 1; }, val), 10);
    loop.add(new task_t<int&>([](int& val) { val += 100; }, val), 100);

    uint64_t start = sys_t::get_ticks();
    LOG(TEST, "Start %lu", start);
    while (sys_t::get_ticks() <= (start + 255)) {
        loop.scheduler();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    LOG(TEST, "val %u", val);
    EXPECT_EQ(val, 225);
}

TEST(task_loop_Test, functor_rm_test) {
    int val = 0;
    task_loop_t loop;

    // Create a task that would add 100 and then trigger an assertion if executed.
    task_t<int&>* t2 = new task_t<int&>(
        [](int& val) {
            val += 100;
            ASSERT_TRUE(false) << "This task should have been removed";
        },
        val);

    // Add a task that increments by 1.
    loop.add(new task_t<int&>([](int& val) { val += 1; }, val), 10);
    loop.add(t2, 100);

    // Remove the problematic task.
    loop.remove(t2);

    uint64_t start = sys_t::get_ticks();
    LOG(TEST, "Start %lu", start);
    while (sys_t::get_ticks() <= (start + 257)) {
        loop.scheduler();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Only the +1 task should have run (~25 times).
    EXPECT_EQ(val, 25);
}

TEST(task_loop_Test, method_rm_test) {
    int val = 0;
    task_loop_t loop;

    // Task that would add 100 and assert if executed.
    auto t2 = [](int32_t tid, void* args) {
        LOG(TEST, "Task: %d", tid);
        *static_cast<int*>(args) += 100;
        ASSERT_TRUE(false) << "This task should have been removed";
    };

    // Add a recurring task that increments by 1.
    loop.add(
        [](int32_t tid, void* args) {
            (void)tid;
            *static_cast<int*>(args) += 1;
            LOG(TEST, "Arg: %d", *static_cast<int*>(args));
        },
        10, static_cast<void*>(&val));

    // Add the task that should be removed.
    loop.add(t2, 100, static_cast<void*>(&val));

    // Remove the task by its method pointer.
    loop.remove(t2);

    uint64_t start = sys_t::get_ticks();
    LOG(TEST, "Start %lu", start);
    while (sys_t::get_ticks() <= (start + 257)) {
        int sleep_until = loop.scheduler();
        LOG(TEST, "Sleep until: %d", sleep_until);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_until));
    }

    EXPECT_EQ(val, 25);
}

TEST(task_loop_Test, shared_cb_test) {
    int val = 0;
    task_loop_t loop;
    shared_cb_t<int&> cbt(val);

    cbt.push_back([](int& v){
        v += 1;
    });

    cbt.push_back([](int& v){
        v += 2;
    });

    cbt.push_back([](int& v){
        v += 7;
    });
    
    // Add the shared callback as a task to the loop.
    loop.add(&cbt, 100);

    uint64_t start = sys_t::get_ticks();
    LOG(TEST, "Start %lu", start);
    while (sys_t::get_ticks() <= (start + 250)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(loop.scheduler()));
    }

    // The shared callback sums to 10 per call and should be executed twice.
    EXPECT_EQ(val, 20);
}
