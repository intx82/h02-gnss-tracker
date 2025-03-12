#ifndef __ARDUINO_H
#define __ARDUINO_H

#include <chrono>
#include <math.h>
#include <cinttypes>

#define TWO_PI (2 * M_PI)

typedef uint8_t byte;

/**
 * @brief
 * @return
 */
unsigned long millis(void);

/**
 * @brief
 * @param radians
 * @return
 */
double degrees(double radians);
/**
 * @brief
 * @param in
 * @return
 */
double sq(double in);

/**
 * @brief
 * @param degrees
 * @return
 */
double radians(double degrees);

#define nameof(x) #x

/**
 * @brief Log levels
 *
 */
enum log_level_t {
    LOG_DBG,
    LOG_AUDIT,
    LOG_INFO,
    LOG_WARN,
    LOG_TEST,
    LOG_ERR,
    LOG_FATAL,
};

#ifndef LOG_LVL
#define LOG_LVL log_level_t::LOG_AUDIT
#endif

#define LOG(lvl, args...)                                                                            \
    do {                                                                                             \
        if (LOG_##lvl >= LOG_LVL) {                                                                  \
            const char *const p = __FILE__;                                                          \
            const char *fname = nullptr;                                                             \
            for (fname = p + sizeof(__FILE__) - 1; fname > p; fname--)                               \
                if (*fname == '/') {                                                                 \
                    fname++;                                                                         \
                    break;                                                                           \
                }                                                                                    \
            printf("%" PRIu64 " %s [%s:%d]: ", sys_t::get_ticks(), nameof(lvl), __FILE__, __LINE__); \
            printf(args);                                                                            \
            printf("\r\n");                                                                          \
        }                                                                                            \
    } while (0)


/**
 * @brief System time class, with ticks in ms and timestamp in sec
 */
struct tim_reg_t;

/**
 * @brief System class, which controls IRQ, Time's, etc
 */
class sys_t {
   public:
    /**
     * @brief Get 1ms ticks after system start
     */
    static uint64_t get_ticks();

    /**
     * @brief Retrieve pointer to ticks variable (for mem-if)
     *
     * @return struct time_t*
     */
    static tim_reg_t* ticks_ptr();

    /**
     * @brief waits while cb returns 'true' up to timeout.
     * @param timeout timeout while callback should return true
     * @param arg Additional argument for callback
     * @param cb callback
     * @return true - if callback will return true until timeout is out.
     */
    static bool waits_for(uint64_t timeout, void* arg, bool (*cb)(void*));

    /**
     * @brief delay in ms
     * @param ms Delay time in mS
     */
    static void delay(uint32_t ms);

    /**
     * @brief Sleeps until timeout is not running out
     *
     * @param timeout Wake up timeout (in mS)
     */
    static void sleep_until(uint32_t timeout);

};

#endif