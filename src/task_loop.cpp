
#include "task_loop.h"
#include <algorithm>

#ifndef MTX_LOCK
#define MTX_LOCK(mtx)
#endif

#ifndef MXT_UNLOCK
#define MTX_UNLOCK(mtx)
#endif

task_loop_t::task_loop_t() : mtx(nullptr), _break_sch(false), cb_list() {}

task_loop_t::~task_loop_t() {
    if (mtx) {
        // delete mtx
        free(mtx);
    }

    for (cb_t *cb : cb_list) {
        delete cb;
    }
}

int32_t task_loop_t::find_unused_tid() {
    int32_t ret = -1;
    for (int32_t tid = 0; tid < INT32_MAX; tid++) {
        bool _found = false;
        MTX_LOCK(mtx);
        for (cb_t *_cb : cb_list) {
            if (_cb->tid == tid) {
                _found = true;
                break;
            }
        }
        MTX_UNLOCK(mtx);

        if (!_found) {
            ret = tid;
            break;
        }
    }

    return ret;
}

int32_t task_loop_t::_add(cb_t* cb) {

    MTX_LOCK(mtx);
    _break_sch = true;
    cb_list.push_front(cb);
    MTX_UNLOCK(mtx);

    return cb->tid;
}

int32_t task_loop_t::add(task_loop_t::func_t method, int32_t period, void *args) {
    return _add(new cb_t(find_unused_tid(), method, period, args));
}

int32_t task_loop_t::add(functor_t *functor, int32_t period) {
    return _add(new cb_t(find_unused_tid(), functor, period));
}

int32_t task_loop_t::add_once(task_loop_t::func_t method, int32_t period, void *args) {
    cb_t *_cb = _find(method);
    if (_cb != nullptr) {
        return _cb->tid;
    }
    return add(method, period, args);
}

int32_t task_loop_t::add_once(functor_t *functor, int32_t period) {
    cb_t *_cb = _find(functor);
    if (_cb != nullptr) {
        return _cb->tid;
    }
    return add(functor, period);
}

int32_t task_loop_t::get_period(int32_t tid) {
    if (tid < 0) {
        return -1;
    }

    cb_t *_cb = _find(tid);
    if (_cb == nullptr) {
        return -1;
    }

    return _cb->period;
}

bool task_loop_t::set_period(int32_t tid, int32_t period) {
    if (tid < 0) {
        return false;
    }

    cb_t *_cb = _find(tid);
    if (_cb == nullptr) {
        return false;
    }

    MTX_LOCK(mtx);

    _cb->period = period;
    _cb->next_time = 0;

    MTX_UNLOCK(mtx);

    return true;
}

bool task_loop_t::remove(func_t method) {
    cb_t *cb = _find(method);
    if (cb == nullptr) {
        return false;
    }

    _remove(cb);
    return true;
}

bool task_loop_t::remove(functor_t *functor) {
    cb_t *cb = _find(functor);
    if (cb == nullptr) {
        return false;
    }

    _remove(cb);
    return true;
}

bool task_loop_t::remove(int32_t tid) {
    if (tid <= -1) {
        return false;
    }

    cb_t *cb = _find(tid);
    if (cb == nullptr) {
        return false;
    }
    _remove(cb);
    return true;
}

void task_loop_t::_remove(cb_t *cb) {
    MTX_LOCK(mtx);
    _break_sch = true;
    cb_list.remove(cb);
    delete cb;
    MTX_UNLOCK(mtx);
}

int32_t task_loop_t::find(func_t method) {
    cb_t *cb = _find(method);
    if (cb != nullptr) {
        return cb->tid;
    }

    return -1;
}

int32_t task_loop_t::find(functor_t *functor) {
    cb_t *cb = _find(functor);
    if (cb != nullptr) {
        return cb->tid;
    }

    return -1;
}

uint64_t task_loop_t::scheduler() {
    MTX_LOCK(mtx);
    _break_sch = false;
    for (cb_t *cb : cb_list) {
        if (static_cast<uint64_t>(cb->period) < minimal_time_to_next) {
            minimal_time_to_next = cb->period;
        }

        if ((cb->next_time <= sys_t::get_ticks()) || (cb->period <= 0)) {
            cb->next_time = sys_t::get_ticks() + cb->period;
            (*cb->callable)();

            if (cb->period == -1) {
                remove(cb->tid);
            }
        }

        if (_break_sch) {
            minimal_time_to_next = 0;
            break;
        }
    }

    MTX_UNLOCK(mtx);
    return minimal_time_to_next > 0 ? minimal_time_to_next : 0;
}

task_loop_t::cb_t *task_loop_t::_find(int32_t tid) {
    MTX_LOCK(mtx);
    std::list<cb_t *>::iterator found =
        std::find_if(begin(cb_list), end(cb_list), [tid](cb_t *cb) { return cb->tid == tid; });
    MTX_UNLOCK(mtx);

    cb_t *_task = *found;
    return found != std::end(cb_list) ? _task : nullptr;
}

task_loop_t::cb_t *task_loop_t::_find(func_t method) {
    MTX_LOCK(mtx);
    std::list<cb_t *>::iterator found = std::find_if(begin(cb_list), end(cb_list), [method](cb_t *cb) {
        return (*cb->callable) == reinterpret_cast<void *>(method);
    });
    MTX_UNLOCK(mtx);

    cb_t *_task = *found;
    return found != std::end(cb_list) ? _task : nullptr;
}

task_loop_t::cb_t *task_loop_t::_find(functor_t *method) {
    MTX_LOCK(mtx);
    std::list<cb_t *>::iterator found = std::find_if(
        begin(cb_list), end(cb_list), [method](cb_t *cb) { return cb->callable == reinterpret_cast<void *>(method); });
    MTX_UNLOCK(mtx);

    cb_t *_task = *found;
    return found != std::end(cb_list) ? _task : nullptr;
}
