#ifndef SYNTHESE_UTIL_CONCURRENT_SYNCHRONIZED_QUEUE_H
#define SYNTHESE_UTIL_CONCURRENT_SYNCHRONIZED_QUEUE_H


#include <queue>

#include <boost/thread/mutex.hpp>



namespace synthese
{



namespace util
{



/** Thread-safe version of STL queue.
    @ingroup m01
    */
template<class K>
class SynchronizedQueue
{

private:

    mutable boost::mutex _queueMutex;
    std::queue<K> _queue;

public:

    SynchronizedQueue () {};
    ~SynchronizedQueue () {};

    bool empty() const;

    size_t size() const;

    K& front();
    const K& front() const;

    K& back();
    const K& back() const;

    void push (const K&);
    void pop ();


private:

};



template<class K>
bool
SynchronizedQueue<K>::empty() const
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    return _queue.empty ();
}



template<class K>
size_t
SynchronizedQueue<K>::size() const
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    return _queue.size ();
}




template<class K>
K&
SynchronizedQueue<K>::front()
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    return _queue.front ();
}



template<class K>
const K&
SynchronizedQueue<K>::front() const
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    return _queue.front ();
}


template<class K>
K&
SynchronizedQueue<K>::back()
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    return _queue.back ();
}




template<class K>
const K&
SynchronizedQueue<K>::back() const
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    return _queue.back ();
}



template<class K>
void
SynchronizedQueue<K>::push (const K& v)
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    _queue.push (v);

}




template<class K>
void
SynchronizedQueue<K>::pop ()
{
    boost::mutex::scoped_lock queueLock ( _queueMutex);
    return _queue.pop ();
}





}
}


#endif


