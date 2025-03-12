#ifndef __CALLBACK_HPP_
#define __CALLBACK_HPP_

#include <functional>
#include <list>
#include <tuple>
#include <type_traits>

/**
 * @brief Functor class. Atomic callable object
 */
class functor_t {
   public:
    /**
     * @brief Callable part of functor
     */
    virtual void operator()(){};

    /**
     * @brief Overloaded destructor
     */
    virtual ~functor_t(){};

    /**
     * @brief Comparing methods in functor and outside functor
     * @remark used for determine which is
     * @param left
     * @param _right
     * @return true
     * @return false
     */
    virtual bool operator==(void* right) {
        (void)right;
        return false;
    }

    functor_t* get_base() { return this; }
};

/**
 * @brief Codition functor
 */
class condition_functor_t : public functor_t {
   public:
    explicit condition_functor_t(
        functor_t& functor, std::function<bool()> condition = []() { return true; })
        : functor(functor), condition(condition) {}
    /**
     * @brief Callable part of functor
     */
    virtual void operator()() final {
        if (condition) functor();
    };

    /**
     * @brief Overloaded destructor
     */
    virtual ~condition_functor_t(){};

   private:
    functor_t& functor;
    std::function<bool()> condition;
};

/**
 * @brief Task class
 * @tparam T Args params
 */
template <typename... T>
class task_t : public functor_t {
   public:
    task_t(void (*task)(T...), T... args) : _task(task), _args(args...) {}

    virtual void operator()() {
        if (_task != nullptr) {
            (*_task)(std::get<T>(_args)...);
        }
    }

    virtual ~task_t(){};

    /**
     * @brief Comparing methods in functor and outside functor
     * @remark used for determine which is
     * @param right Pointer to method
     * @return true if method and internal task is the same
     */
    virtual bool operator==(void* right) { return reinterpret_cast<void*>(_task) == right; }

   private:
    void (*_task)(T...);    /**< Stored task */
    std::tuple<T...> _args; /**< Shared argument */
};

/**
 * Shared callbacks class
 */
template <typename... T>
class shared_cb_t : public std::list<void (*)(T...)>, public functor_t {
   public:
    /**
     * call all stored callbacks
     */
    virtual void operator()() final {
        typename shared_cb_t<T...>::iterator it;
        for (it = this->begin(); it != this->end(); ++it) {
            (*it)(std::get<T>(args)...);
        }
    }

    /**
     * call all stored callbacks with separate arguments list
     */
    void operator()(T... _args) {
        typename shared_cb_t<T...>::iterator it;
        for (it = this->begin(); it != this->end(); ++it) {
            (*it)(_args...);
        }
    }

    /**
     * @brief Comparing methods in functor and outside functor
     * @remark used for determine which is
     * @param left
     * @param _right
     * @return true
     * @return false
     */
    virtual bool operator==(void* right) {
        typename shared_cb_t<T...>::iterator it;
        for (it = this->begin(); it != this->end(); ++it) {
            if (reinterpret_cast<void*>(*it) == right) {
                return true;
            }
        }
        return false;
    }

    /**
     * Shared callback argument
     */
    shared_cb_t(T... args) : args(args...) {}

    /**
     * @brief Destroy the shared cb object
     */
    ~shared_cb_t() final {}

   private:
    std::tuple<T...> args; /**< Shared argument */
};

/**
 * Callbacks list class
 */
template <typename... T>
class cb_t : public std::list<void (*)(T...)> {
   public:
    /**
     * call all stored callbacks with separate arguments list
     */
    void operator()(T... _args) {
        typename cb_t<T...>::iterator it;
        for (it = this->begin(); it != this->end(); ++it) {
            (*it)(_args...);
        }
    }
};

#endif