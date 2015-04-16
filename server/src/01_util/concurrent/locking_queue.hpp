/**
 * @file locking_queue.hpp Thread-safe queue-like container template class.
 * @author Adam Romanek <a.romanek -at- no.pl>
 */

// Copyright (c) Adam Romanek 2011. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(BOOST_LOCKING_QUEUE_HPP)
#define BOOST_LOCKING_QUEUE_HPP

#include <stdexcept>                                       // for std::logic_error
#include <queue>
#include <boost/thread/mutex.hpp>                          // for boost::mutex
#include <boost/thread/condition_variable.hpp>             // for boost::condition_variable
#include <boost/date_time/posix_time/posix_time_types.hpp> // for boost::posix_time
#include <boost/utility.hpp>                               // for boost::noncopyable

namespace boost {

/**
 * Generic thread-safe queue class template using boost's mutex and condition_variable.
 *
 * @tparam T         type that is to be stored in locking queue
 * @tparam Container container type that will serve as underlying container
 *                   for queue values
 */
template<
    typename T,
    typename Container = std::queue<T>
>
class locking_queue : boost::noncopyable {
private:
    /**
     * Lock guard type.
     */
    typedef boost::lock_guard<boost::mutex> lock_guard;

public:
    /**
     * Empty queue exception type.
     */
    class queue_empty {};

    /**
     * Container type.
     */
    typedef Container container_type;

    /**
     * Value type.
     */
    typedef typename Container::value_type value_type;

    /**
     * Size type.
     */
    typedef typename Container::size_type size_type;

    /**
     * Constructs new locking queue.
     */
    explicit locking_queue()
        : unfinished_tasks(0)
    {}

    /**
     * Constructs new locking queue with the copy of the contents of
     * other container.
     *
     * @param[in] other other container to copy the contents from
     *                  when constructing new locking queue.
     */
    explicit locking_queue(const container_type& other)
        : container(other), unfinished_tasks(container.size())
    {}

    /**
     * Checks whether the queue is empty.
     * @return true if the queue is empty, false otherwise
     */
    bool empty() const {
        lock_guard guard(mutex);
        return container.empty();
    }

    /**
     * Returns the number of elements in the queue.
     * @return the number of elements in the queue.
     */
    size_type size() const {
        lock_guard guard(mutex);
        return container.size();
    }

    /**
     * Pops an element from the front of the queue and returns it to the
     * caller.
     *
     * If block is true and timeout is a positive integral value then wait for
     * the element to be available in the queue for at most timeout seconds
     * and then throw @a empty exception.
     *
     * This method is the preferred way of popping elements from the queue when
     * the T's assignment operator is guaranteed not to throw any exceptions.
     * In the other case one should use locking_queue::pop_safe().
     *
     * @param[in] block if true, then blocks until an element is available
     * @param[in] timeout number of seconds to wait for the element to be
     *                    available
     * @throws locking_queue::empty in case no elements were available
     * @return the first element of the queue
     */
    value_type pop(bool block = false, int timeout = 0) {
        boost::mutex::scoped_lock lock(mutex);

        pop_common(lock, block, timeout);

        value_type element(container.front());
        container.pop();

        return element;
    }

    /**
     * Pops an element from the front of the queue and returns it to the
     * caller in a safe manner.
     *
     * If block is true and timeout is a positive integral value then wait for
     * the element to be available in the queue for at most timeout seconds
     * and then throw @a empty exception.
     *
     * This method is safer than locking_queue#pop() in the way it returns the
     * value to the caller as when an exception is thrown in T's assignment
     * operator the element stays on the queue and is not lost.
     *
     * @param[out] element placeholder for the element that is to be taken from
     *                     the queue
     * @param[in] block if true then blocks until an element is available
     * @param[in] timeout number of seconds to wait for the element to be
     *                    available
     *
     * @sa locking_queue#pop()
     */
    void pop_safe(value_type& element, bool block = false, int timeout = 0) {
        boost::mutex::scoped_lock lock(mutex);

        pop_common(lock, block, timeout);

        element = container.front();
        container.pop();
    }

    /**
     * Pushes a new element to the back of the queue.
     * @param[in] element element to be pushed to the back of the queue
     */
    void push(const value_type& element) {
        {
            lock_guard guard(mutex);
            container.push(element);
            unfinished_tasks++;
        }
        non_empty.notify_one();
    }

    /**
     * Reports a previously enqueued task completion.
     *
     * Used by consumer threads to indicate task completion.
     */
    void task_done() {
        lock_guard guard(mutex);

        unsigned long unfinished = unfinished_tasks - 1;
        if (unfinished < 0) {
            throw std::logic_error("Task done reported more times than the number of elements in the queue");
        }

        if (unfinished == 0) {
            all_tasks_done.notify_all();
        }

        unfinished_tasks = unfinished;
    }

    /**
     * Blocks until all the elements in the queue have been taken from the
     * queue and processed.
     *
     * When an element is pushed onto the queue by means of
     * locking_queue#push(), an internal counter of unfinished tasks is
     * incremented. Then when the element gets popped from the queue and
     * processed one may indicate task completion by means of
     * locking_queue#task_done().
     *
     * When all tasks are done this method unblocks and returns to the caller.
     */
    void join() const {
        boost::mutex::scoped_lock lock(mutex);
        while (unfinished_tasks) {
            all_tasks_done.wait(lock);
        }
    }

private:
    void pop_common(boost::mutex::scoped_lock& lock, bool block, int timeout) {
        if (block) {
            while (container.empty()) {
                if (timeout > 0) {
                    if (!non_empty.timed_wait(
                                lock, boost::posix_time::seconds(timeout))) {
                        throw queue_empty();
                    }
                } else {
                    non_empty.wait(lock);
                }
            }
        } else {
            if (container.empty()) {
                throw queue_empty();
            }
        }
    }

private:
    /**
     * Underlying container that serves as a storage for the queue elements.
     */
    container_type container;

    /**
     * Container access mutex.
     */
    mutable boost::mutex mutex;

    /**
     * Non empty condition variable.
     */
    mutable boost::condition_variable non_empty;

    /**
     * Unfinished tasks counter.
     */
    unsigned long unfinished_tasks;

    /**
     * All tasks done condition variable.
     */
    mutable boost::condition_variable all_tasks_done;
};

} // namespace boost

#endif /* BOOST_LOCKING_QUEUE_HPP */
