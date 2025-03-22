#ifndef __TASK_LOOP_HPP
#define __TASK_LOOP_HPP

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <list>
#include <chrono>

#include "Arduino.h"
#include "callback.h"


/**
 * Task loop class
 *
 * @remark
 * How to use:
 *
 * 1. At first, write own task:
 *   Task example:
 *   //
 *   //  * Simple task for test
 *   //  * @param tid Task id
 *   //  * @param args Pointer to task arguments
 *   // static void SimpleTask(int32_t tid, void* args)
 *   // {
 *   //     int32_t task_period = loop->get_period(tid);
 *   //     LOG(FINE,"This is: {task: %ld; current time: %"PRIu64", period: %"PRIi32";", tid, sys_t::get_ticks(),
 * task_period);
 *   //     if(task_period > 0)
 *   //     {
 *   //         loop->set_period(tid,task_period+1000);
 *   //     }
 *   // }
 *
 * 2. Create task class:
 *   // loop = new task_t();
 * 3. Add task to scheduler list:
 *   // loop->add(SimpleTask,1000); //Means run SimpleTask() at every 1000ms (at least 1000ms, @see warnings)
 * 4. Run scheduler in main thread loop:
 *   // loop->scheduler();
 * 5. Clear after all
 *   // delete loop;
 *
 * @warning: If all tasks using more time than period time, period time could be violated
 * @warning: Task arguments, after passing it into structure might be immutable
 * @warning: If MCU goes to sleep, task period time also could be violated
 * @warning: For correctly working 'remove', 'find', 'add_once' methods - functor should have correctly written operator ==
 */
class task_loop_t {
   public:
    /**
     * @brief Typedef task - defines task method signature
     * @param #1 task ID
     * @param #2 task arguments
     */
    typedef void (*func_t)(int32_t, void*);

    /**
     * @brief Add callback to task loop with two arguments - himself ID and pointer to task arguments (which will be
     * called when his time come) If task already in list - will be another task
     *
     * @param method Pointer to task function
     * @param period Period in which task function will be called, if provided -1 then task called only once and then
     * deleted from callback list
     * @param args Arguments for task
     * @return Task ID
     */
    int32_t add(func_t method, int32_t period, void* args);

    /**
     * @brief Add callback to task loop with functor ( @see task_t @see functor_t)
     *
     * @param functor Pointer to functor
     * @param period Period in which task function will be called, if provided -1 then task called only once and then
     * deleted from callback list
     * @return Task ID
     */
    int32_t add(functor_t* functor, int32_t period);

    /**
     * Add callback for task loop with two arguments - himself ID and pointer to task arguments (which will be called
     * when his time come) If task already in list - will returned -1 and new task will not added
     *
     * @param method Pointer to task function
     * @param period Period in which task function will be called, if provided -1 then task called only once and then
     * deleted from callback list
     * @param args Arguments for task
     * @return task ID
     */
    int32_t add_once(func_t method, int32_t period, void* args);

    /**
     * @brief Add callback for task loop with functor
     * @remark If task already in list - will returned -1 and new task will not added
     * 
     * @param functor Pointer to functor
     * @param period Period in which task function will be called, if provided -1 then task called only once and then
     * deleted from callback list
     * @return Task ID
     */
    int32_t add_once(functor_t* functor, int32_t period);

    /**
     * Get task period
     * @param tid task id
     * @return -1 if task not found, or something else goes wrong, otherwise contains task period time in mS
     */
    int32_t get_period(int32_t tid);

    /**
     * @brief Set task period (by tid)
     * @param tid Task ID
     * @param interval Task period
     * @return true - if new period was set
     */
    bool set_period(int32_t tid, int32_t interval);

    /**
     * @brief Removes task from list by ID
     * @param tid task ID
     * @return True if remove successful
     */
    bool remove(int32_t tid);

    /**
     * @brief Removes task from list by pointer to method
     * @param task Pointer to task
     * @return True if remove successful
     */
    bool remove(func_t task);

    /**
     * @brief Removes task from list by pointer to method
     * @param task Pointer to task
     * @return True if remove successful
     */
    bool remove(functor_t* task);

    /**
     * @brief Search Task ID by method pointer
     * @param method Pointer to method
     * @return -1 if task not found
     */
    int32_t find(func_t method);

    /**
     * @brief Search Task ID by functor pointer
     * @param method Pointer to method
     * @return -1 if task not found
     */
    int32_t find(functor_t* functor);

    /**
     * Scheduler function.
     * Should called from `while(true)`
     */
    uint64_t scheduler();

    /**
     * @brief Конструктор
     */
    task_loop_t();

    /**
     * @brief Деструктор
     *
     */
    ~task_loop_t();

   private:
    /**
     * Task structure
     */
    class cb_t {
       public:
        int32_t tid;         /**< Task ID */
        functor_t* callable; /**< Callback function itself */
        int32_t period;      /**< Task period */
        uint64_t next_time;  /**< Task previous tick time */
        bool is_static;      /**< Show, should be functor destroyed after freeing memory */

        /**
         * @brief task structure
         * @param tid Task ID
         * @param cb Callback itself
         * @param period Interval in which task should be called
         * @param args Task arguments
         */
        cb_t(int32_t tid, functor_t* cb, int32_t period)
            : tid(tid),
              callable(cb),
              period(period),
              next_time(period > 0 ? sys_t::get_ticks() + period : 0),
              is_static(true) {}

        /**
         * @brief task structure
         * @param tid Task ID
         * @param cb Callback itself
         * @param period Interval in which task should be called
         * @param args Task arguments
         */
        cb_t(int32_t tid, func_t _cb, int32_t period, void* args)
            : tid(tid), period(period), next_time(period > 0 ? sys_t::get_ticks() + period : 0), is_static(false) {
            task_t<int32_t, void*>* t = new task_t<int32_t, void*>(_cb, tid, args);
            callable = t->get_base();
        }

        ~cb_t() {
            if (!is_static) {
                delete callable;
            }
        }
    };

    /**
     * @brief Retrieve minimal unused TID
     *
     * @return int32_t task id
     */
    int32_t find_unused_tid();

    /**
     * @brief Search task by task-id
     * @param tid Pointer to task method
     * @return cb_t* Task definition
     */
    cb_t* _find(int32_t tid);

    /**
     * @brief Search task by pointer to task method
     *
     * @param cb Pointer to task method
     * @return cb_t* Task definition
     */
    cb_t* _find(func_t cb);

    /**
     * @brief Search task by functor
     *  @param cb Pointer to task method
     * @return cb_t* Task definition
     */
    cb_t* _find(functor_t* cb);

    /**
     * @brief Internal remove method
     *
     * @param cb
     */
    void _remove(cb_t* cb);

    /**
     * @brief Internal implementation of add
     *
     * @param cb
     */
    int32_t _add(cb_t* cb);

    void* mtx;
    bool _break_sch;
    uint64_t minimal_time_to_next = UINT32_MAX;
    std::list<cb_t*> cb_list; /**< Tasks list	*/
};

#endif /* __TASK_LOOP_HPP */
